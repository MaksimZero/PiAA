#!/usr/bin/env bash
g++ -std=c++17 -O2 task1.cpp -o pr_ac
./pr_ac < input_ac.txt > out_ac.txt 2> debug_ac.log
dot -Tpng automaton.dot -o automaton_ac.png
echo "Готово. Ответ: out_ac.txt, лог: debug_ac.log, картинка: automaton_ac.png"