set -xe


emcc ./main.c ./tinyexpr/tinyexpr.c \
  -o index.html \
  --shell-file static/shell.html \
  -I ./raylib/include \
  -L ./raylib/lib \
  -lraylib.web -lm \
  --preload-file roboto/Roboto-Bold.ttf \
  -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" \
  -s NO_EXIT_RUNTIME=1 \
  -s EXPORTED_FUNCTIONS="['_main', '_set_eq']" \
  -DPLATFORM_WEB \
  -O3 \
  -s USE_GLFW=3 \
  -s ASYNCIFY