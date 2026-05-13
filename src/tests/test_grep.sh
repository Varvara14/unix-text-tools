MY_GREP="./apps/grep/grep"
REAL_GREP="grep"

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

rm -f test_*.txt output_real output_my

echo -e "qwert\nasdf\nCZXCV\naaa 123\n12345" > test_1.txt
echo -e "hello world\nHELLO WORLD\nhELLo wOrld" > test_2.txt
echo -e "match\nnope\nmatch again\nnothing here" > test_3.txt
touch test_empty.txt

TESTS=(
    "Флаг -e: поиск 'qwert'|-e|qwert|test_1.txt"
    "Флаг -i: игнор регистра 'hello'|-i|hello|test_2.txt"
    "Флаг -v: инвертировать вывод 'match'|-v|match|test_3.txt"
    "Флаг -c: подсчёт совпадений 'hello'|-c|hello|test_2.txt"
    "Флаг -l: вывод имени файла при совпадении 'match'|-l|match|test_3.txt"
    "Флаг -n: вывод номеров строк 'qwert'|-n|qwert|test_1.txt"
    "Флаг -e: поиск в нескольких файлах|-e|hello|test_2.txt test_1.txt"
    "Флаг -c: подсчёт в нескольких файлах|-c|hello|test_2.txt test_1.txt"
    "Флаг -l: имя файла при совпадении в нескольких|-l|match|test_3.txt test_1.txt"
)

PASSED=0
TOTAL=${#TESTS[@]}

run_test() {
    IFS='|' read -r description flags pattern files <<< "$1"
    IFS=' ' read -ra file_array <<< "$files"
    
    echo -n "$description: "
    
    args=()
    if [ -n "$flags" ]; then
        args+=("$flags")
    fi
    if [ -n "$pattern" ]; then
        args+=("$pattern")
    fi
    args+=("${file_array[@]}")

    echo -e "${NC}Выполняется: '$MY_GREP ${args[@]}'${NC}"

    "$REAL_GREP" "${args[@]}" > output_real 2>&1
    "$MY_GREP" "${args[@]}" > output_my 2>&1

    if diff -q output_real output_my > /dev/null; then
        echo -e "${GREEN}PASS${NC}"
        ((PASSED++))
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

rm -f test_*.txt output_real output_my

echo "Результат: $PASSED из $TOTAL тестов пройдено."
if [ "$PASSED" -eq "$TOTAL" ]; then
    echo -e "${GREEN}Все тесты успешны!${NC}"
else
    echo -e "${RED}Есть проваленные тесты.${NC}"
fi