#!/bin/bash
rm outputclasses.py
for (( i=5; i<50; ++i )); do echo $i ; 
cat outputclass_template.txt | sed "s%XYZ%${i}%g" >> outputclasses.py
done

