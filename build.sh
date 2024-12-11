if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    CC=g++
elif [[ "$OSTYPE" == "darwin"* ]]; then
    CC=clang
else
    echo "Unknown operating system \"$OSTYPE\"."
    exit 1
fi

mkdir -p build
"$CC" -g -std=c++20 -fsanitize=undefined -o build/querysnipe code/main.cpp
"$CC" -g -std=c++20 -fsanitize=undefined -o build/test       code/test.cpp
