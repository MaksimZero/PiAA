#!/usr/bin/env bash
g++ -std=c++17 -O2 task2.cpp -o pr_wild
./pr_wild < input_wild.txt > out_wild.txt 2> debug_wild.log
dot -Tpng automaton.dot -o automaton_wild.png
echo "Готово. Ответ: out_wild.txt, лог: debug_wild.log, картинка: automaton_wild.png"