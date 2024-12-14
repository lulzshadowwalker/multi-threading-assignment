#!/bin/sh

make config=release
make config=unsafe

#  NOTE: They all write to the same file so only the last one will be saved anyway
declare -a threads=(
    1 4 16 64 256 1024
)

for i in "${threads[@]}"
do
    echo "$i Threads"
    ./bin/Release/MultiThreadingAssignment $i > /dev/null 2>&1
    ./bin/Unsafe/MultiThreadingAssignment $i > /dev/null 2>&1
done

echo
echo "Results are saved in out.txt and unsafe-out.txt"