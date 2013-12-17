cph=$TestArea/Trigger/TrigCost/
exe="python $cph/TrigCostRate/share/readCostNtuple.py"

lvl=$2
rec=$3
inp=$1/NTUP_TRIGCOST${lvl}/${rec}

$exe $inp --data -k TRIGCOST -o ${inp}/html/ -r ${lvl}_${rec}.root --rosXML=$cph/TrigCostRate/data/rob-ros-robin-2012.xml --read-conf-file --runModXML=${lvl}_${rec}.xml -n 0 &> ${lvl}_${rec}.log & 
