touch epoch.txt
rm epoch.txt
grep Epoch:  trainingCronology.txt | grep -Ev "Minimum"  > epoch.txt
sed -i 's/Epoch:/ /g' epoch.txt
sed -i 's/Error:/ /g' epoch.txt
sed -i 's/Test:/ /g' epoch.txt
sed -i 's/\[/ /g' epoch.txt
sed -i 's/\]/ /g' epoch.txt
counter=$(wc -l < epoch.txt )
let c=$counter-1
touch chron.txt ; rm chron.txt
tail -$c epoch.txt > chron.txt

#maximumTrain=$0
#minimumTrain=$100
#epochesWithRisingError=$0

#fname=chron.txt
#o=0
#while read myline
#  do
#  echo $myline
#  let o++
#  echo $o
#   awko '{ print $1, $2, $3 }'
#   set -- $(awk '{print $1, $2, $3}' )
#   echo $1	
#   awk '{ print $2 }' 	
#  epoch=$(awk '{ print $1 }')
#  trainingError=$(awk '{ print $2 }')
#  testError=$(awk '{ print $3 }')
#  set -- $(awk '{print $1, $2, $3}' )
#  epoch=$1
#  trainingError=$2
#  testError=$3
#  echo $testError
#done < $fname





#awk '{print $2 }'  chron.txt


