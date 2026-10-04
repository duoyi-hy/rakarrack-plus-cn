#!/bin/bash
# =====================================================================
# RakArrack-Plus 中文版 deb 打包脚本
#  - 编译 standalone 版本
#  - 自动收集全部运行时依赖库并封装进安装包（glibc 核心除外）
#  - 用 patchelf 设置 RPATH，使程序优先使用自带库
#  - 产出: dist/rakarrack-plus_<version>_<arch>.deb
#
# 用法（在 Debian/Ubuntu/deepin 系统或容器内执行）:
#   bash packaging/build-deb.sh
# =====================================================================
set -euxo pipefail

SRC_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$SRC_DIR"

VERSION="1.4.1"
PKG_VERSION="1.4.1-cn1"
ARCH="$(dpkg --print-architecture)"

echo "==> [1/5] 安装编译依赖"
export DEBIAN_FRONTEND=noninteractive
if [ "$(id -u)" = "0" ]; then APT="apt-get"; SUDO=""; else APT="apt-get"; SUDO="sudo"; fi

# Debian 11 (bullseye) 已 EOL，官方镜像不再提供，改用 archive.debian.org 归档源
APT_OPTS=""
if [ -f /etc/os-release ] && grep -q 'VERSION_ID="11"' /etc/os-release; then
    echo "检测到 Debian 11，切换到 archive.debian.org 归档源"
    $SUDO tee /etc/apt/sources.list >/dev/null <<'EOS'
deb [check-valid-until=no] http://archive.debian.org/debian bullseye main
deb [check-valid-until=no] http://archive.debian.org/debian-security bullseye-security main
EOS
    APT_OPTS="-o Acquire::Check-Valid-Until=false"
fi
$SUDO $APT $APT_OPTS update -qq
$SUDO $APT $APT_OPTS install -y --no-install-recommends \
    build-essential cmake pkg-config patchelf file ca-certificates \
    libasound2-dev libjack-jackd2-dev libsndfile1-dev liblo-dev \
    libx11-dev libxft-dev libxrender-dev libxpm-dev \
    libfreetype-dev libfontconfig1-dev zlib1g-dev \
    libzita-resampler-dev libsamplerate-dev || \
$SUDO $APT $APT_OPTS install -y --no-install-recommends \
    build-essential cmake pkg-config patchelf file ca-certificates \
    libasound2-dev libjack-jackd2-dev libsndfile1-dev liblo-dev \
    libx11-dev libxft-dev libxrender-dev libxpm-dev \
    libfreetype-dev libfontconfig1-dev zlib1g-dev
# FLTK 1.3 (Debian 11/Ubuntu) 或 1.4 (Debian 13+)
$SUDO $APT $APT_OPTS install -y --no-install-recommends libfltk1.3-dev || \
    $SUDO $APT $APT_OPTS install -y --no-install-recommends libfltk1.4-dev || true

echo "==> [2/5] 编译 rakarrack-plus"
rm -rf build dist stage
cmake -B build \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_RPLUS_STANDALONE=ON \
    -DBUILD_RPLUS_LV2=OFF \
    -DBUILD_LV2_EFFECTS=OFF
cmake --build build -j"$(nproc)"

echo "==> [3/5] 安装到暂存目录"
DESTDIR="$SRC_DIR/stage" cmake --install build
BIN="$SRC_DIR/stage/usr/bin/rakarrack-plus"
test -x "$BIN"

echo "==> [4/5] 收集运行时依赖库并封装"
LIBDIR="$SRC_DIR/stage/usr/lib/rakarrack-plus"
mkdir -p "$LIBDIR"
# 排除 glibc 核心与系统关键库（这些必须使用目标机上的版本）
EXCLUDE_RE='(^|/)(ld-linux|libc\.so|libm\.so|libpthread|libdl\.|librt\.|libresolv|libnss_|libnsl|libanl|libBrokenLocale|libcidn|libcrypt\.|libutil\.|libsystemd|libdbus|libcap\.|libselinux|libseccomp|libapparmor)'
collect_deps() {
    local target="$1"
    ldd "$target" 2>/dev/null | awk '/=> \// {print $3} /^\// {print $1}' | sort -u || true
}
# 递归收集（两层足够覆盖传递依赖）
declare -A SEEN
QUEUE=("$BIN")
for round in 1 2 3; do
    NEXT_QUEUE=()
    for f in "${QUEUE[@]}"; do
        [ -f "$f" ] || continue
        for dep in $(collect_deps "$f"); do
            [ -f "$dep" ] || continue
            echo "$dep" | grep -qE "$EXCLUDE_RE" && continue
            base="$(basename "$dep")"
            [ -n "${SEEN[$base]:-}" ] && continue
            SEEN[$base]=1
            cp -L "$dep" "$LIBDIR/$base"
            NEXT_QUEUE+=("$LIBDIR/$base")
        done
    done
    QUEUE=("${NEXT_QUEUE[@]:-}")
    [ ${#QUEUE[@]} -eq 0 ] && break
done

# 设置 RPATH：程序与自带库都优先查找打包目录
patchelf --set-rpath '$ORIGIN/../lib/rakarrack-plus' "$BIN"
for f in "$LIBDIR"/*.so*; do
    [ -f "$f" ] && patchelf --set-rpath '$ORIGIN' "$f" || true
done

echo "封装的依赖库："
ls -1 "$LIBDIR"

echo "==> [5/5] 生成 deb 安装包"
PKGROOT="$SRC_DIR/stage"
mkdir -p "$PKGROOT/DEBIAN"
cat > "$PKGROOT/DEBIAN/control" <<EOF
Package: rakarrack-plus
Version: $PKG_VERSION
Section: sound
Priority: optional
Architecture: $ARCH
Depends: libc6 (>= 2.31)
Recommends: fonts-noto-cjk, jackd
Suggests: qjackctl
Maintainer: rakarrack-plus-cn build
Description: 吉他效果器（RakArrack-Plus 中文版）
 RakArrack-Plus 是一款功能强大的实时吉他音频效果处理器，
 包含失真、混响、延迟、合唱、移相、哇音、声码器等 40 余种效果，
 支持 JACK 音频与 ALSA MIDI，具备调音器、节拍器、循环录音等功能。
 .
 本包为简体中文界面版本，已内置全部运行时依赖库（glibc 除外），
 可直接安装于 Debian 11+/Ubuntu 22.04+/deepin 23+ 等 x86_64/arm64 系统。
EOF

cat > "$PKGROOT/DEBIAN/postinst" <<'EOF'
#!/bin/sh
set -e
if [ -x /usr/bin/update-desktop-database ]; then
    update-desktop-database -q /usr/share/applications || true
fi
if [ -x /usr/bin/gtk-update-icon-cache ]; then
    gtk-update-icon-cache -q -t -f /usr/share/pixmaps || true
fi
exit 0
EOF
chmod 755 "$PKGROOT/DEBIAN/postinst"

# 统一属主为 root，避免容器内属主污染
mkdir -p dist
dpkg-deb --root-owner-group --build "$PKGROOT" "dist" || fakeroot dpkg-deb --build "$PKGROOT" "dist"

OUT="dist/rakarrack-plus_${PKG_VERSION}_${ARCH}.deb"
echo "完成: $OUT"
ls -lh dist/
