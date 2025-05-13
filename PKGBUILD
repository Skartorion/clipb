# Maintainer: Skartorion skartorion@icloud.com
pkgname=clipb
pkgver=1.0.0
pkgrel=1
pkgdesc="CLI clipboard"
arch=('x86_64')
url="https://github.com/Skartorion/clipb"
license=('GPL-3.0')
depends=()
makedepends=('gcc')
source=('clipb.cpp')
md5sums=('SKIP')

build() {
  cd "$srcdir"
  g++ clipb.cpp -o clipb -std=c++17
}

package() {
  install -Dm755 "$srcdir/clipb" "$pkgdir/usr/bin/clipb"
  ln -sf clipb "$pkgdir/usr/bin/cl"
}
