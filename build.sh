set -xe


emcc ./main.c ./tinyexpr/tinyexpr.c -o index.html \
  -I ./raylib/include \
  -L ./raylib/lib \
  -lraylib.web -lm \
  --preload-file roboto/Roboto-Bold.ttf \
  -O3 -Wall \
  -s USE_GLFW=3 \
  -s ASYNCIFY