MY_CAT="./apps/cat/cat"

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

rm -f test*.txt output_my output_real

echo -e "Hello, world!\nThis is a test.\n\nAnother line." > test1.txt
echo -e "Line 1\nLine 2\nLine 3" > test2.txt
touch test3_empty.txt
echo $'Binary data \x00\t\x01\xFF\nAnother line' > test4_binary.txt
echo -e "\n\n\n" > test5_multiblank.txt

TESTS=(
    "Тест 1: -b (нумерация непустых строк)|-b|test1.txt"
    "Тест 2: -n (нумерация всех строк)|-n|test2.txt"
    "Тест 3: -e (показывает \$ и невидимки)|-e|test4_binary.txt"
    "Тест 4: -t (показывает табы как ^I)|-t|test4_binary.txt"
    "Тест 5: -s (сжатие нескольких пустых строк)|-s|test5_multiblank.txt"
    "Тест 6: -n с несколькими файлами|-n|test1.txt test2.txt"
)

PASSED=0
TOTAL=0

run_test() {
    IFS='|' read -r description flags files <<< "$1"

    IFS=' ' read -ra file_array <<< "$files"

    echo -n "$description: "
    TOTAL=$((TOTAL + 1))

    args=()
    if [[ -n "$flags" ]]; then
        IFS=' ' read -ra flag_array <<< "$flags"
        args+=("${flag_array[@]}")
    fi
    args+=("${file_array[@]}")

    echo -e "${NC}Выполняется: '$MY_CAT ${args[@]}'${NC}"

    cat "${args[@]}" > output_real 2>&1
    "$MY_CAT" "${args[@]}" > output_my 2>&1

    if [ $? -eq 124 ]; then
        echo -e "${RED}FAIL (my_cat завис)${NC}"
        return
    fi

    if diff -q output_real output_my > /dev/null; then
        echo -e "${GREEN}PASS${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}FAIL${NC}"
        echo "Ожидаемый вывод:"
        cat output_real
        echo "Полученный вывод:"
        cat output_my
    fi
}

for test_case in "${TESTS[@]}"; do
    run_test "$test_case"
done

rm -f output_my output_real test*.txt

echo "Результат: $PASSED из $TOTAL тестов пройдено."
if [ "$PASSED" -eq "$TOTAL" ]; then
    echo -e "${GREEN}Все тесты успешны!${NC}"
else
    echo -e "${RED}Есть проваленные тесты.${NC}"
fi