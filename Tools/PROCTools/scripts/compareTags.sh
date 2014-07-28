#! /bin/sh 

if [ -z "$CMTCONFIG" ]; then
    echo "No athena configuration found. Please use asetup."
    exit 1
fi

if [ $# -ne 2 ]; then 
    echo "# compareTags.sh needs two input parameters."
    echo "# ./compareTags.sh x84 x85"
else
    tag=($1 $2)
    
    for i in 0 1 
      do
      text[i]=`uuidgen`"."${tag[i]}".txt"
      GetCommand.py AMI=${tag[i]} keepFullCommandUntouched=True 1> ${text[i]} 2> /dev/null
      list[i]=`uuidgen`"."${tag[i]}".list"
      for parameter in `grep Reco_trf.py ${text[i]}`
	do
	echo ${parameter} >> ${list[i]}
      done
      sort ${list[i]} > ${text[i]}
      rm ${list[i]}
    done
    
    diff ${text[0]} ${text[1]}
    
    rm ${text[0]} ${text[1]}
fi
