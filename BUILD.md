# Fedora
Tested on fedora 30

Command to install dependencies
` sudo dnf install SDL2-devel SDL2_gfx-devel SDL2_image-devel SDL2_mixer-devel SDL2_net-devel SDL2_ttf-devel ncurses-devel flac-devel libvorbis-devel libpng12-devel libmikmod-devel libwebp-devel libpng-devel curl-devel bzip2-libs freetype-devel -y`

Create a build folder somewhere `mkdir build && cd $_`, and run cmake in the
folder `cmake -G "Unix Makefiles" -S ../projects/cmake -B ./`. Then to build,
run `make`.
