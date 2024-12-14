#!/bin/bash

echo "Building the application..."
make config=release

declare -a threads=(
    1 2 4 8 16 32 64 128
)

for i in "${threads[@]}"
do
    echo "$i Threads"
    time ./bin/Release/MultiThreadingAssignment $i > /dev/null 2>&1
    echo ""
done
