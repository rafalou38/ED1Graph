set -xe

gcc ./main.c ./tinyexpr/tinyexpr.c -o ed1graph -lraylib -lm

./ed1graph

