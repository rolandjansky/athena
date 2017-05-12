#!/bin/bash                                                                                                                                                                        
#***************************************************************************************************************************************************  
FILE=/afs/cern.ch/work/d/deroy/ServiceTaskHandsOn/CentralMCStatusPageMC15New/WjetsSherpa_systematics.csv
cat $FILE | while read line
do
    [ -z "$line" ] && continue
    echo $line
    str=`echo $line |sed -e 's/\,/ /g' |awk '{print $1}'`
    PMGXSEC=`echo $line |sed -e 's/\,/ /g' |awk '{print $2}'`
    if [[ $PMGXSEC == *[e]* ]]
    then
	PMGXSEC=`echo $(printf "%.10f" "$PMGXSEC")`
    else
	PMGXSEC=`echo $(printf "%.6f" "$PMGXSEC")`
    fi
    PMGkfact=`echo $line |sed -e 's/\,/ /g' |awk '{print $3}'`
    if [[ $PMGkfact == *[e]* ]]
    then
	PMGkfact=`echo $(printf "%.10f" "$PMGkfact")`
    else
	PMGkfact=`echo $(printf "%.6f" "$PMGkfact")`
    fi
    PMGFeff=`echo $line |sed -e 's/\,/ /g' |awk '{print $4}'`
    if [[ $PMGFeff == *[e]* ]]
    then
        PMGFeff=`echo $(printf "%.10f" "$PMGFeff")`
    else
	PMGFeff=`echo $(printf "%.6f" "$PMGFeff")`
    fi
    PMGTotXSEC=`echo $line |sed -e 's/\,/ /g' |awk '{print $5}'`
    if [[ $PMGTotXSEC == *[e]* ]]
    then
        PMGTotXSEC=`echo $(printf "%.10f" "$PMGTotXSEC")`
    else
	PMGTotXSEC=`echo $(printf "%.6f" "$PMGTotXSEC")`
    fi
    AMIXSEC=`echo $line |sed -e 's/\,/ /g' |awk '{print $6}'`
    if [[ $AMIXSEC == *[e]* ]]
    then
        AMIXSEC=`echo $(printf "%.10f" "$AMIXSEC")`
    else
	AMIXSEC=`echo $(printf "%.6f" "$AMIXSEC")`
    fi
    AMIkfact=`echo $line |sed -e 's/\,/ /g' |awk '{print $7}'`
    if [[ $AMIkfact == *[e]* ]]
    then
        AMIkfact=`echo $(printf "%.10f" "$AMIkfact")`
    else
	AMIkfact=`echo $(printf "%.6f" "$AMIkfact")`
    fi
    AMIFeff=`echo $line |sed -e 's/\,/ /g' |awk '{print $8}'`
    if [[ $AMIFeff == *[e]* ]]
    then
        AMIFeff=`echo $(printf "%.10f" "$AMIFeff")`
    else
	AMIFeff=`echo $(printf "%.6f" "$AMIFeff")`
    fi
    r0=$(echo "$AMIFeff < 0" | bc)
    if [ "$r0" -eq 1 ]
    then
        neg0=-1
        AMIFeff=`echo "$AMIFeff*$neg0" |bc`
    fi
    AMITotXSEC=`echo $line |sed -e 's/\,/ /g' |awk '{print $9}'`
    if [[ $AMITotXSEC == *[e]* ]]
    then
        AMITotXSEC=`echo $(printf "%.10f" "$AMITotXSEC")`
    else
	AMITotXSEC=`echo $(printf "%.6f" "$AMITotXSEC")`
    fi
    r1=$(echo "$AMITotXSEC < 0" | bc)
    if [ "$r1" -eq 1 ]
    then
        neg1=-1
        AMITotXSEC=`echo "$AMITotXSEC*$neg1" |bc`
    fi
    diff=`echo "$PMGTotXSEC-$AMITotXSEC" |bc`
    r=$(echo "$diff < 0" | bc)
    if [ "$r" -eq 1 ]
    then
	neg=-1
        diff=`echo "$diff*$neg" |bc`
    fi
    product=`echo "$PMGTotXSEC*0.0001" |bc`
    if ((  $(bc <<< "$diff >= $product") )); then
	if [ "$PMGXSEC" != "$AMIXSEC" ]; then
	    echo "DSID:" ${str} "check sample as total cross section difference is more/equal to 0.01% between AMI and PMG due to" "AMIXSEC:" ${AMIXSEC} "PMGXSEC:" ${PMGXSEC}
	fi
	if [ "$PMGkfact" != "$AMIkfact" ]; then
	    echo "DSID:" ${str} "check sample as total cross section difference is more/equal to 0.01% between AMI and PMG due to" "AMIkfact:" ${AMIkfact} "PMGkfact:" ${PMGkfact}
	fi
	if [ "$PMGFeff" != "$AMIFeff" ]; then
	    echo "DSID:" ${str} "check sample as total cross section difference is more/equal to 0.01% between AMI and PMG due to" "AMIFeff:" ${AMIFeff} "PMGFeff:" ${PMGFeff}
	fi
    else
	continue
    fi
done
