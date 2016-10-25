#!/bin/bash

for jo in $(ls CaloTests*Dig*py)
do
echo "doing $jo"
sed "s/#DetFlags.LVL1_setOff()/DetFlags.LVL1_setOff()/g" $jo > tmp
mv tmp $jo
done
