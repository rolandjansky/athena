#!/bin/bash

for F in {cosmics,collisions,heavyions}_{run,minutes10}.config
do 
    MergeConfigs.py $F . $F $@
done
