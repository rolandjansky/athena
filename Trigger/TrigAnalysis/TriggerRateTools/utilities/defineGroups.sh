#***************************************************************************
#  Author: Miika Klemetti						   *
#  Modified: 12/08/2009							   *	
#  -a script to create group definition jobOptions for TriggerRateTools.   *
#***************************************************************************  

#!/bin/bash
if [ ! -f triggerItems.out ]
	then echo "Run in the same directory with 'triggerItems.out'!"
	exit 1
fi

echo "Creating 'CumulateList.py':"

#extended regular expressions for each item type
L1="^L1_\n"
L2="^L2_\n"
EF="^EF_\n"

singlebjet="^.._b\n^.._[mM][uU][0-9]+_[jJ][0-9]+_matched\n"
bjets="_[0-9]?b\nmatched\n"
Bphys="B\nDsPhiPi\nDiMu\nJpsimumu\nMU[0-9]+_Jpsi\nmu[0-9]+_Jpsi\nUpsimumu\n"

singlejet="^.._[jJ][0-9]+[v0-9]?[_a-zA-Z]*$\n"
jetveto="trackless\ntau[0-9]+hv\n"  
jets="_[0-9]?[jJ][0-9]+\n"

singlefjet="^.._[fF][jJ][0-9]+[v0-9]?[_a-zA-Z]*$\n"
fjets="_[0-9]?[fF][jJ][0-9]+\n"

singlemuon="^.._[mM][uU][0-9]+[_a-zA-Z]*$\n"
muonveto="stau\n" 
muons="_[0-9]?[mM][uU]\nstau\n"

singletau="^.._[tT][aA][uU](NoCut)?[0-9]*[_a-zA-Z]*.?(_[0-9]?b[0-9]+)*$\ntrk[0-9]+\n"
taus="_[0-9]?[tT][aA][uU]\ntrk[0-9]+\n" 

singleelectron="^.._e[0-9]+[_a-zA-Z]*.?$\n"
electrons="_[0-9]?e[0-9]+\nJpsiee\nZee\n"

singlephoton="^.._g[0-9]+[_a-zA-Z]*.?$\n" 
photons="_[0-9]?g[0-9]+\n"

singleem="^.._[eE][mM][0-9+[_a-zA-Z]*.?$\n"
em="_[0-9]?[eE][mM]\n"

xe="_[xX][eE]\nEF[xX][eE]\n"
te="_[tT][eE]\n" 
je="_[jJ][eE]\n"

calib="Calib\ncalib\n"
minbias="MV\nmb\nMb\nMB\nLUCID\nZDC\nLucid\nZdc\nBCM\nBcm\nBM\nZC\nRD0\n"

end="EOF"


# match each item into appropriate groups:
# -'tmp_match' is a set of strings that need to be satisfied 
# -'tmp_veto' is a set of strings that will be vetoed

echo -e 'triggerRateTools.CumulateList += [' > CumulateList.py 

echo -e "\n\n#L1 groups *********************************\n\n" >> CumulateList.py
echo "-L1"

#L1_single_jets
	echo -e '["L1_single_jets","L1",' >> CumulateList.py
	echo -e "$singlejet""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$jetveto""$fjets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_multi_jets
	echo -e '],\n\n["L1_multi_jets","L1",' >> CumulateList.py 
	echo -e "$jets""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$jetveto""$singlejet""$minbias""$fjets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_single_fjets
	echo -e '],\n\n["L1_single_fjets","L1",' >> CumulateList.py 
	echo -e "$singlefjet""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$jets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_multi_fjets
	echo -e '],\n\n["L1_multi_fjets","L1",' >> CumulateList.py
	echo -e "$fjets""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$singlefjet""$jets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_jets
	echo -e '],\n\n["L1_jets","L1",' >> CumulateList.py
	echo -e "$jets""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$fjets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_fjets
	echo -e '],\n\n["L1_fjets","L1",' >> CumulateList.py
	echo -e "$fjets""$end" > tmp_match
	echo -e "$L2""$jets""$EF""$minbias""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_fjets_jets
	echo -e '],\n\n["L1_fjets_jets","L1",' >> CumulateList.py
	echo -e "$jets""$fjets""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_single_EM
	echo -e '],\n\n["L1_single_EM","L1",' >> CumulateList.py
	echo -e "$singleem""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$jets""$fjets""$muons""$electrons""$taus""$photons""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_multi_EM
	echo -e '],\n\n["L1_multi_EM","L1",' >> CumulateList.py
	echo -e "$em""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$jets""$fjets""$muons""$electrons""$taus""$photons""$singleem""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_EM
	echo -e '],\n\n["L1_EM","L1",' >> CumulateList.py
	echo -e "$em""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$jets""$fjets""$muons""$electrons""$taus""$photons""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_single_muons
	echo -e '],\n\n["L1_single_muons","L1",' >> CumulateList.py
	echo -e "$singlemuon""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$jets""$fjets""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_multi_muons
	echo -e '],\n\n["L1_multi_muons","L1",' >> CumulateList.py
	echo -e "$muons""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$muonveto""$jets""$fjets""$singlemuon""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_muons
	echo -e '],\n\n["L1_muons","L1",' >> CumulateList.py
	echo -e "$muons""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$jets""$fjets""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_single_taus
	echo -e '],\n\n["L1_single_taus","L1",' >> CumulateList.py
	echo -e "$singletau""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$bjets""$jets""$fjets""$electrons""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_multi_taus
	echo -e '],\n\n["L1_multi_taus","L1",' >> CumulateList.py
	echo -e "$taus""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$bjets""$jets""$fjets""$electrons""$muons""$singletau""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_taus
	echo -e '],\n\n["L1_taus","L1",' >> CumulateList.py
	echo -e "$taus""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$bjets""$jets""$fjets""$electrons""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_XE
	echo -e '],\n\n["L1_XE","L1",' >> CumulateList.py
	echo -e "$xe""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$jets""$fjets""$electrons""$muons""$taus""$photons""$em""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_TE
	echo -e '],\n\n["L1_TE","L1",' >> CumulateList.py
	echo -e "$te""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$jets""$fjets""$electrons""$muons""$taus""$photons""$em""$xe""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_JE
	echo -e '],\n\n["L1_JE","L1",' >> CumulateList.py
	echo -e "$je""$end" > tmp_match
	echo -e "$L2""$EF""$minbias""$jets""$fjets""$electrons""$muons""$taus""$photons""$em""$xe""$te""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_EM_XEJ
	echo -e '],\n\n["L1_EM_XEJ","L1",' >> CumulateList.py
	echo -e "$em""$end" > tmp_match
	echo -e "$jets""$xe""$end" > tmp_match2
	echo -e "$L2""$EF""$minbias""$fjets""$electrons""$muons""$taus""$photons""$te""$je""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_EM_MU
	echo -e '],\n\n["L1_EM_MU","L1",' >> CumulateList.py
	echo -e "$em""$end" > tmp_match
	echo -e "$muons""$end" > tmp_match2
	echo -e "$L2""$EF""$minbias""$jets""$fjets""$electrons""$taus""$photons""$xe""$je""$te""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_EM_TAU
	echo -e '],\n\n["L1_EM_TAU","L1",' >> CumulateList.py
	echo -e "$em""$end" > tmp_match
	echo -e "$taus""$end" > tmp_match2
	echo -e "$L2""$EF""$minbias""$jets""$fjets""$electrons""$muons""$photons""$xe""$je""$te""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_MU_XEJ
	echo -e '],\n\n["L1_MU_XEJ","L1",' >> CumulateList.py
	echo -e "$muons""$end" > tmp_match
	echo -e "$xe""$jets""$end" > tmp_match2
	echo -e "$L2""$EF""$minbias""$fjets""$electrons""$taus""$photons""$je""$te""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_MU_TAU
	echo -e '],\n\n["L1_MU_TAU","L1",' >> CumulateList.py
	echo -e "$muons""$end" > tmp_match
	echo -e "$taus""$end" > tmp_match2
	echo -e "$L2""$EF""$minbias""$jets""$fjets""$electrons""$photons""$xe""$je""$te""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_JET_XE
	echo -e '],\n\n["L1_JET_XE","L1",' >> CumulateList.py
	echo -e "$jets""$end" > tmp_match
	echo -e "$xe""$end" > tmp_match2
	echo -e "$L2""$EF""$minbias""$fjets""$electrons""$muons""$taus""$photons""$je""$em""$te""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_TAU_XEJ
	echo -e '],\n\n["L1_TAU_XEJ","L1",' >> CumulateList.py
	echo -e "$taus""$end" > tmp_match
	echo -e "$xe""$jets""$end" > tmp_match2
	echo -e "$L2""$EF""$minbias""$fjets""$electrons""$muons""$photons""$je""$em""$te""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L1_Triple_Sigs
        echo -e '],\n\n["L1_Triple_Sigs","L1",' >> CumulateList.py
        echo -e "$jets""$fjets""$end" > tmp_match
        echo -e "$xe""$je""$te""$end" > tmp_match2
        echo -e "$em""$electrons""$muons""$taus""$photons""$end" > tmp_match3
        echo -e "$L2""$EF""$calib""$minbias""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -f tmp_match3 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L1_MinBias
	echo -e '],\n\n["L1_MinBias","L1",' >> CumulateList.py
	echo -e "$minbias""$end" > tmp_match
	echo -e "$L2""$EF""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

echo -e "],\n\n#L2 groups *********************************\n\n" >> CumulateList.py
echo "-L2"

#L2_calib
	echo -e '["L2_calib","L2",' >> CumulateList.py
	echo -e "$calib""$end" > tmp_match
	echo -e "$L1""$EF""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L2_bjets
	echo -e '],\n\n["L2_bjets","L2",' >> CumulateList.py
	echo -e "$bjets""$end" > tmp_match
	echo -e "$L1""$EF""$minbias""$fjets""$electrons""$taus""$photons""$je""$em""$te""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L2_single_bjets
	echo -e '],\n\n["L2_single_bjets","L2",' >> CumulateList.py
	echo -e "$singlebjet""$end" > tmp_match
	echo -e "$L1""$EF""$minbias""$fjets""$electrons""$taus""$photons""$je""$em""$te""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L2_multi_bjets
	echo -e '],\n\n["L2_multi_bjets","L2",' >> CumulateList.py
	echo -e "$bjets""$end" > tmp_match
	echo -e "$L1""$EF""$minbias""$singlebjet""$fjets""$electrons""$muons""$taus""$photons""$je""$em""$te""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py 

#L2_single_jets
        echo -e '],\n\n["L2_single_jets","L2",' >> CumulateList.py
        echo -e "$singlejet""$end" > tmp_match
        echo -e "$L1""$EF""$jetveto""$minbias""$fjets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_multi_jets
        echo -e '],\n\n["L2_multi_jets","L2",' >> CumulateList.py 
        echo -e "$jets""$end" > tmp_match
        echo -e "$L1""$EF""$jetveto""$minbias""$singlejet""$fjets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_single_fjets
        echo -e '],\n\n["L2_single_fjets","L2",' >> CumulateList.py 
        echo -e "$singlefjet""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$jets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_multi_fjets
        echo -e '],\n\n["L2_multi_fjets","L2",' >> CumulateList.py
        echo -e "$fjets""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$singlefjet""$jets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_jets
        echo -e '],\n\n["L2_jets","L2",' >> CumulateList.py
        echo -e "$jets""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$fjets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_fjets
        echo -e '],\n\n["L2_fjets","L2",' >> CumulateList.py
        echo -e "$fjets""$end" > tmp_match
        echo -e "$L1""$jets""$minbias""$EF""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_fjets_jets
        echo -e '],\n\n["L2_fjets_jets","L2",' >> CumulateList.py
        echo -e "$jets""$fjets""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_EM_objects
        echo -e '],\n\n["L2_EM_objects","L2",' >> CumulateList.py
        echo -e "$em""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$muons""$electrons""$taus""$photons""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_single_muons
        echo -e '],\n\n["L2_single_muons","L2",' >> CumulateList.py
        echo -e "$singlemuon""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$Bphys""$bjets""$jets""$fjets""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_multi_muons
        echo -e '],\n\n["L2_multi_muons","L2",' >> CumulateList.py
        echo -e "$muons""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$muonveto""$Bphys""$bjets""$jets""$fjets""$singlemuon""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_muons
        echo -e '],\n\n["L2_muons","L2",' >> CumulateList.py
        echo -e "$muons""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$Bphys""$bjets""$jets""$fjets""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_single_photons
	echo -e '],\n\n["L2_single_photons","L2",' >> CumulateList.py
        echo -e "$singlephoton""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$taus""$muons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_multi_photons
	echo -e '],\n\n["L2_multi_photons","L2",' >> CumulateList.py
        echo -e "$photons""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$taus""$muons""$singlephoton""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_photons
	echo -e '],\n\n["L2_photons","L2",' >> CumulateList.py
        echo -e "$photons""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$taus""$muons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_single_taus
        echo -e '],\n\n["L2_single_taus","L2",' >> CumulateList.py
        echo -e "$singletau""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$bjets""$jets""$fjets""$electrons""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_multi_taus
        echo -e '],\n\n["L2_multi_taus","L2",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$bjets""$jets""$fjets""$electrons""$muons""$singletau""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_taus
        echo -e '],\n\n["L2_taus","L2",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$bjets""$jets""$fjets""$electrons""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_single_electron
	echo -e '],\n\n["L2_single_electrons","L2",' >> CumulateList.py
        echo -e "$singleelectron""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$taus""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_multi_electron
	echo -e '],\n\n["L2_multi_electrons","L2",' >> CumulateList.py
        echo -e "$electrons""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$singleelectron""$taus""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_electrons
	echo -e '],\n\n["L2_electrons","L2",' >> CumulateList.py
        echo -e "$electrons""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$taus""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_xe
        echo -e '],\n\n["L2_xe","L2",' >> CumulateList.py
        echo -e "$xe""$end" > tmp_match
	echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$muons""$taus""$photons""$em""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_te
        echo -e '],\n\n["L2_te","L2",' >> CumulateList.py
        echo -e "$te""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$muons""$taus""$photons""$em""$xe""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_je
        echo -e '],\n\n["L2_je","L2",' >> CumulateList.py
        echo -e "$je""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$muons""$taus""$photons""$em""$xe""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Bphys
	echo -e '],\n\n["L2_Bphys","L2",' >> CumulateList.py
        echo -e "$Bphys""$end" > tmp_match
        echo -e "$L1""$EF""$minbias""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Photon_MET
	echo -e '],\n\n["L2_Photon_MET","L2",' >> CumulateList.py
        echo -e "$photons""$end" > tmp_match
        echo -e "$xe""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$taus""$muons""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Electron_Muon
	echo -e '],\n\n["L2_Electron_Muon","L2",' >> CumulateList.py
        echo -e "$electrons""$end" > tmp_match
        echo -e "$muons""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$taus""$xe""$photons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Tau_Jet
	echo -e '],\n\n["L2_Tau_Jet","L2",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$jets""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$photons""$fjets""$electrons""$muons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Tau_MET
	echo -e '],\n\n["L2_Tau_MET","L2",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$xe""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$photons""$muons""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Muon_MET
	echo -e '],\n\n["L2_Muon_MET","L2",' >> CumulateList.py
        echo -e "$muons""$end" > tmp_match
        echo -e "$xe""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$taus""$photons""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Muon_Jet
	echo -e '],\n\n["L2_Muon_Jet","L2",' >> CumulateList.py
        echo -e "$muons""$end" > tmp_match
        echo -e "$jets""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$bjets""$fjets""$electrons""$taus""$photons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Tau_Electron
	echo -e '],\n\n["L2_Tau_Electron","L2",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$electrons""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$photons""$muons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Tau_Muon
	echo -e '],\n\n["L2_Tau_Muon","L2",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$muons""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$photons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Photon_Muon
	echo -e '],\n\n["L2_Photon_Muon","L2",' >> CumulateList.py	
        echo -e "$photons""$end" > tmp_match
        echo -e "$muons""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$taus""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Electron_Jet
	echo -e '],\n\n["L2_Electron_Jet","L2",' >> CumulateList.py
        echo -e "$electrons""$end" > tmp_match
        echo -e "$jets""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$fjets""$photons""$taus""$muons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Electron_MET
	echo -e '],\n\n["L2_Electron_MET","L2",' >> CumulateList.py
        echo -e "$electrons""$end" > tmp_match
        echo -e "$xe""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$photons""$taus""$muons""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Tau_Bjet
	echo -e '],\n\n["L2_Tau_Bjet","L2",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$bjets""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$electrons""$muons""$photons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Jet_MET
	echo -e '],\n\n["L2_Jet_MET","L2",' >> CumulateList.py
        echo -e "$jets""$end" > tmp_match
        echo -e "$xe""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$fjets""$electrons""$taus""$muons""$photons""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Photon_Electron
	echo -e '],\n\n["L2_Photon_Electron","L2",' >> CumulateList.py
        echo -e "$photons""$end" > tmp_match
        echo -e "$electrons""$end" > tmp_match2
        echo -e "$L1""$EF""$minbias""$jets""$fjets""$muons""$taus""$je""$te""$xe""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_Triple_Sigs
	echo -e '],\n\n["L2_Triple_Sigs","L2",' >> CumulateList.py
        echo -e "$jets""$fjets""$end" > tmp_match
        echo -e "$xe""$je""$te""$end" > tmp_match2
        echo -e "$em""$electrons""$muons""$taus""$photons""$end" > tmp_match3
        echo -e "$L1""$EF""$minbias""$calib""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -f tmp_match3 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#L2_MinBias
	echo -e '],\n\n["L2_MinBias","L2",' >> CumulateList.py
	echo -e "$minbias""$end" > tmp_match
	echo -e "$L1""$EF""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

echo -e "],\n\n#EF groups *********************************\n\n" >> CumulateList.py
echo "-EF"

#EF_calib
        echo -e '["EF_calib","EF",' >> CumulateList.py
        echo -e "$calib""$end" > tmp_match
        echo -e "$L1""$L2""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_bjets
        echo -e '],\n\n["EF_bjets","EF",' >> CumulateList.py
        echo -e "$bjets""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$fjets""$electrons""$taus""$photons""$je""$em""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_single_bjets
        echo -e '],\n\n["EF_single_bjets","EF",' >> CumulateList.py
        echo -e "$singlebjet""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$fjets""$electrons""$taus""$photons""$je""$em""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_multi_bjets
        echo -e '],\n\n["EF_multi_bjets","EF",' >> CumulateList.py
        echo -e "$bjets""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$singlebjet""$fjets""$electrons""$muons""$taus""$photons""$je""$em""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_single_jets
        echo -e '],\n\n["EF_single_jets","EF",' >> CumulateList.py
        echo -e "$singlejet""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jetveto""$minbias""$fjets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_multi_jets
        echo -e '],\n\n["EF_multi_jets","EF",' >> CumulateList.py 
        echo -e "$jets""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jetveto""$minbias""$singlejet""$fjets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_single_fjets
        echo -e '],\n\n["EF_single_fjets","EF",' >> CumulateList.py 
        echo -e "$singlefjet""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_multi_fjets
        echo -e '],\n\n["EF_multi_fjets","EF",' >> CumulateList.py
        echo -e "$fjets""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$singlefjet""$jets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_jets
        echo -e '],\n\n["EF_jets","EF",' >> CumulateList.py
        echo -e "$jets""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$fjets""$minbias""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_fjets
        echo -e '],\n\n["EF_fjets","EF",' >> CumulateList.py
        echo -e "$fjets""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_fjets_jets
        echo -e '],\n\n["EF_fjets_jets","EF",' >> CumulateList.py
        echo -e "$jets""$fjets""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$minbias""$muons""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_EM_objects
        echo -e '],\n\n["EF_EM_objects","EF",' >> CumulateList.py
        echo -e "$em""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$muons""$electrons""$taus""$photons""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_single_muons
        echo -e '],\n\n["EF_single_muons","EF",' >> CumulateList.py
        echo -e "$singlemuon""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$bjets""$Bphys""$jets""$fjets""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_multi_muons
        echo -e '],\n\n["EF_multi_muons","EF",' >> CumulateList.py
        echo -e "$muons""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$muonveto""$bjets""$Bphys""$jets""$fjets""$singlemuon""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_muons
        echo -e '],\n\n["EF_muons","EF",' >> CumulateList.py
        echo -e "$muons""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$bjets""$Bphys""$jets""$fjets""$electrons""$taus""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_single_photons
        echo -e '],\n\n["EF_single_photons","EF",' >> CumulateList.py
        echo -e "$singlephoton""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$taus""$muons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_multi_photons
        echo -e '],\n\n["EF_multi_photons","EF",' >> CumulateList.py
        echo -e "$photons""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$taus""$muons""$singlephoton""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_photons
        echo -e '],\n\n["EF_photons","EF",' >> CumulateList.py
        echo -e "$photons""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$taus""$muons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_single_taus
        echo -e '],\n\n["EF_single_taus","EF",' >> CumulateList.py
        echo -e "$singletau""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$bjets""$jets""$fjets""$electrons""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_multi_taus
        echo -e '],\n\n["EF_multi_taus","EF",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$muons""$singletau""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_taus
        echo -e '],\n\n["EF_taus","EF",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
	echo -e "$L1""$L2""$minbias""$bjets""$jets""$fjets""$electrons""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
       egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_single_electron
        echo -e '],\n\n["EF_single_electrons","EF",' >> CumulateList.py
        echo -e "$singleelectron""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$taus""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_multi_electron
        echo -e '],\n\n["EF_multi_electrons","EF",' >> CumulateList.py
        echo -e "$electrons""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$singleelectron""$taus""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_electron
        echo -e '],\n\n["EF_electrons","EF",' >> CumulateList.py
        echo -e "$electrons""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$taus""$muons""$photons""$em""$xe""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_xe
        echo -e '],\n\n["EF_xe","EF",' >> CumulateList.py
        echo -e "$xe""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$muons""$taus""$photons""$em""$te""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_te
        echo -e '],\n\n["EF_te","EF",' >> CumulateList.py
        echo -e "$te""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$muons""$taus""$photons""$em""$xe""$je""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_je
        echo -e '],\n\n["EF_je","EF",' >> CumulateList.py
        echo -e "$je""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$muons""$taus""$photons""$em""$xe""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Bphys
        echo -e '],\n\n["EF_Bphys","EF",' >> CumulateList.py
        echo -e "$Bphys""$end" > tmp_match
        echo -e "$L1""$L2""$minbias""$minbias""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Photon_MET
        echo -e '],\n\n["EF_Photon_MET","EF",' >> CumulateList.py
        echo -e "$photons""$end" > tmp_match
        echo -e "$xe""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$taus""$muons""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Electron_Muon
        echo -e '],\n\n["EF_Electron_Muon","EF",' >> CumulateList.py
        echo -e "$electrons""$end" > tmp_match
        echo -e "$muons""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$taus""$xe""$photons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Tau_Jet
        echo -e '],\n\n["EF_Tau_Jet","EF",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$jets""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$photons""$fjets""$electrons""$muons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Tau_MET
        echo -e '],\n\n["EF_Tau_MET","EF",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$xe""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$photons""$muons""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Muon_MET
        echo -e '],\n\n["EF_Muon_MET","EF",' >> CumulateList.py
        echo -e "$muons""$end" > tmp_match
        echo -e "$xe""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$taus""$photons""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Muon_Jet
        echo -e '],\n\n["EF_Muon_Jet","EF",' >> CumulateList.py
        echo -e "$muons""$end" > tmp_match
        echo -e "$jets""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$bjets""$fjets""$electrons""$taus""$photons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Tau_Electron
        echo -e '],\n\n["EF_Tau_Electron","EF",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$electrons""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$photons""$muons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Tau_Muon
        echo -e '],\n\n["EF_Tau_Muon","EF",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$muons""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$photons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Photon_Muon
        echo -e '],\n\n["EF_Photon_Muon","EF",' >> CumulateList.py        
        echo -e "$photons""$end" > tmp_match
        echo -e "$muons""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$taus""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Electron_Jet
        echo -e '],\n\n["EF_Electron_Jet","EF",' >> CumulateList.py
        echo -e "$electrons""$end" > tmp_match
        echo -e "$jets""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$fjets""$photons""$taus""$muons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Electron_MET
        echo -e '],\n\n["EF_Electron_MET","EF",' >> CumulateList.py
        echo -e "$electrons""$end" > tmp_match
        echo -e "$xe""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$photons""$taus""$muons""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Tau_Bjet
        echo -e '],\n\n["EF_Tau_Bjet","EF",' >> CumulateList.py
        echo -e "$taus""$end" > tmp_match
        echo -e "$bjets""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$electrons""$muons""$photons""$xe""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Jet_MET
        echo -e '],\n\n["EF_Jet_MET","EF",' >> CumulateList.py
        echo -e "$jets""$end" > tmp_match
        echo -e "$xe""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$fjets""$electrons""$taus""$muons""$photons""$je""$te""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Photon_Electron
        echo -e '],\n\n["EF_Photon_Electron","EF",' >> CumulateList.py
        echo -e "$photons""$end" > tmp_match
        echo -e "$electrons""$end" > tmp_match2
        echo -e "$L1""$L2""$minbias""$jets""$fjets""$muons""$taus""$je""$te""$xe""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_Triple_Sigs
        echo -e '],\n\n["EF_Triple_Sigs","EF",' >> CumulateList.py
        echo -e "$jets""$fjets""$end" > tmp_match
        echo -e "$xe""$je""$te""$end" > tmp_match2
        echo -e "$em""$electrons""$muons""$taus""$photons""$end" > tmp_match3
        echo -e "$L1""$L2""$minbias""$calib""$end" > tmp_veto
        egrep -f tmp_match triggerItems.out | egrep -f tmp_match2 | egrep -f tmp_match3 | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#EF_MinBias
	echo -e '],\n\n["EF_MinBias","EF",' >> CumulateList.py
	echo -e "$minbias""$end" > tmp_match
	echo -e "$L1""$L2""$end" > tmp_veto
	egrep -f tmp_match triggerItems.out | egrep -vf tmp_veto | sed 's:\(.*\):"\1",:' >> CumulateList.py

#*****************
echo "-Ungrouped"
egrep -f triggerItems.out CumulateList.py | sed "s:^\"\(.*\)\",$:\1:" > tmp_comparison

#L1_ungrouped
	echo -e '],\n\n["L1_Ungrouped","L1",' > tmp_list
	egrep -vxf tmp_comparison triggerItems.out | egrep "^L1" | sed 's:\(.*\):"\1",:' >> tmp_list

#L2_ungrouped
	echo -e '],\n\n["L2_Ungrouped","L2",' >> tmp_list
	egrep -vxf tmp_comparison triggerItems.out | egrep "^L2" | sed 's:\(.*\):"\1",:' >> tmp_list

#L1_ungrouped
	echo -e '],\n\n["EF_Ungrouped","EF",' >> tmp_list
	egrep -vxf tmp_comparison triggerItems.out | egrep "^EF" | sed 's:\(.*\):"\1",:' >> tmp_list

echo -e ']]' >> tmp_list

cat tmp_list >> CumulateList.py

#clean up
rm tmp_match
rm tmp_match2
rm tmp_match3
rm tmp_veto
rm tmp_comparison
rm tmp_list

echo "Done!" 
