cd src 
g++ main.cpp -o main -std=c++11


# array of file names
files=(stage1.txt stage2.txt stage3.txt)
folders=(1 2 3 4 5)


for test_case in "${folders[@]}"; do
    echo "test case $test_case"
    ./main ../IOs/$test_case/scene.txt ../IOs/$test_case/config.txt
    # loop through file names
    for i in "${files[@]}"; do
        if diff --strip-trailing-cr -w -q "$i" "../IOs/$test_case/$i" &>/dev/null; then
            echo "    success with $i"
        else
            echo "    fail in $i"
        fi
    done
    read -p " continue? " yn
done
