#!/bin/sh


calibs="LC EM"
jetRs="4 6"
corrs="energy eta"

#pyCalibFile=EtaMassEnergyFactors-00-01-01.py 
#pyCalibFile=EtaMassEnergyFactors-00-01-01-01.py 
#pyCalibFile=AtlfastIICalibrationConstants_preInclude_a133.py
pyCalibFile=consts.py
#EtaMassEnergyFactors_r2731v3.py

for calib in $calibs; do
    for jetR in $jetRs; do
	for corr in $corrs; do
	    #echo "AntiKt${jetR}_${calib}_${corr}"
	    echo ; echo
	    echo "  #########"
	    echo "  # ${calib}+JES calibration factors for"
	    echo "  # jet ${corr} correction"
	    echo "  #"
	    jet=TopoEM_EM
	    [[ $calib = LC ]] && jet=LCTopo_LC
	    key="${pre}.AntiKt${jetR}${calib}TopoNew_$.EtaDetBin${i}:"
	    pre=JES
	    [[ $corr = eta ]] && pre=EtaCorr
	    grep -A90 "${corr}CorrDict\[ 'jet_AntiKt${jetR}${jet}" $pyCalibFile | tail -90> tmp_calib
	    for i in {0..90} ; do
		c1=`head -$((i+1)) tmp_calib | tail -1 | awk '{print $2}' | cut -f1 -d','`
		c2=`head -$((i+1)) tmp_calib | tail -1 | awk '{print $3}' | cut -f1 -d','`
		c3=`head -$((i+1)) tmp_calib | tail -1 | awk '{print $4}' | cut -f1 -d','`
		c4=`head -$((i+1)) tmp_calib | tail -1 | awk '{print $5}' | cut -f1 -d','`
		c5=`head -$((i+1)) tmp_calib | tail -1 | awk '{print $6}' | cut -f1 -d','`
		c6=`head -$((i+1)) tmp_calib | tail -1 | awk '{print $7}' | cut -f1 -d','`
		c7=`head -$((i+1)) tmp_calib | tail -1 | awk '{print $8}' | cut -f1 -d','`
		key="${pre}.AntiKt${jetR}TopoEM_Bin${i}:"
		[[ $calib = LC ]] && key="${pre}.AntiKt${jetR}LCTopo_Bin${i}:"
	        printf "%-30s %12.4e %12.4e %12.4e %12.4e %12.4e %12.4e %12.4e\n" "$key" $c1 $c2 $c3 $c4 $c5 $c6 $c7
	    done
#	    grep $calib $pyCalibFile | grep AntiKt${jetR} | grep $corr | grep Topo | grep -v Tower
	done
    done
done

