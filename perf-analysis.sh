#!/bin/bash
for i in ./traces/perf_traces/RAND-*-q.cmd
do
    perf stat --repeat 5 -o "${i%.cmd}"-report -e cache-misses,branches,instructions,context-switches ./qtest -v 0 -f "$i"
done

for i in ./traces/perf_traces/RAND-*-list.cmd
do
    perf stat --repeat 5 -o "${i%.cmd}"-report -e cache-misses,branches,instructions,context-switches ./qtest -v 0 -f "$i"
done