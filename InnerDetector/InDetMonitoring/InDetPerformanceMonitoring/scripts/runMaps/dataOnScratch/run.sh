#!/bin/bash
inputStr="PostD0fix_304198_308084"
#inputStr="PostTSI_preD0fix_301912_304178"
#inputStr="PreTSI_297730_300908"
echo $inputStr

for input in `find runNumberFiles/ -name "*_PostD0fix_304198_308084_*.txt"`
do
    echo "This is the Inputfile: $input"
    python
done
