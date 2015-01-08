#!/bin/zsh

#foreach i in `seq 0 11`
foreach i in `seq 0 22`
    athena.py -c "tagNum=$i" LArOnlDbPrep/LArDSPThresholdTopOptions.py
end

#foreach i in `seq 0 16`
#    tagNum=$i; python LArDSPThresholdTopOptions.py
#end

##mv *.db *.txt *.xml *.root /afs/cern.ch/user/s/smajewsk/public/LAr/.

##rm *.xml.BAK
