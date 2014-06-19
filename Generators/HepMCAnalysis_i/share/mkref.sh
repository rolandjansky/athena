#!/bin/zsh

#---------------------------------------------------------------------- 
# Setup part 
#---------------------------------------------------------------------- 
User=spiec
EventsPerJob=50000

#---------------------------------------------------------------------- 
JobCoreName=Pythia8_Wenu
#JobCoreName=Pythia8_Wmunu
#JobCoreName=Pythia8_Wtaunu
#JobCoreName=Pythia8_Zee
#JobCoreName=Pythia8_Zmumu
#JobCoreName=Pythia8_Ztautau
#JobCoreName=Pythia6_Wenu
#JobCoreName=Pythia6_Wmunu
#JobCoreName=Pythia6_Wtaunu
#JobCoreName=Pythia8_Jz0
#JobCoreName=Pythia8_Jz7
#JobCoreName=Herwigpp_Zee
#JobCoreName=Herwigpp_Zmumu

#---------------------------------------------------------------------- 
RunNr=147810
#RunNr=147811
#RunNr=147812
#RunNr=147816
#RunNr=147817
#RunNr=147818
#RunNr=147830
#RunNr=147831
#RunNr=147832
#RunNr=147900
#RunNr=147907
#RunNr=108294
#RunNr=108295

#---------------------------------------------------------------------- 
InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID147xxx/MC12.147810.Pythia8_AU2CTEQ6L1_Wenu.py
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID147xxx/MC12.147811.Pythia8_AU2CTEQ6L1_Wmunu.py 
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID147xxx/MC12.147812.Pythia8_AU2CTEQ6L1_Wtaunu.py
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID147xxx/MC12.147816.Pythia8_AU2CTEQ6L1_Zee.py
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID147xxx/MC12.147817.Pythia8_AU2CTEQ6L1_Zmumu.py
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID147xxx/MC12.147818.Pythia8_AU2CTEQ6L1_Ztautau.py
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID147xxx/MC12.147830.Pythia_AUET2BCTEQ6L1_Wenu.py
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID147xxx/MC12.147831.Pythia_AUET2BCTEQ6L1_Wmunu.py
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID147xxx/MC12.147832.Pythia_AUET2BCTEQ6L1_Wtaunu.py
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID147xxx/MC12.147900.Pythia8_AU2CT10_jetjet_JZ0.py
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID147xxx/MC12.147907.Pythia8_AU2CT10_jetjet_JZ7.py
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID108xxx/MC12.108294.Herwigpp_UEEE3_CTEQ6L1_Zee.py
#InputJoFile=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID108xxx/MC12.108295.Herwigpp_UEEE3_CTEQ6L1_Zmumu.py

#---------------------------------------------------------------------- 
OutputName=RTT.MC12.147810.Pythia8_AU2CTEQ6L1_Wenu.root
#OutputName=RTT.MC12.147811.Pythia8_AU2CTEQ6L1_Wmunu.root
#OutputName=RTT.MC12.147812.Pythia8_AU2CTEQ6L1_Wtaunu.root
#OutputName=RTT.MC12.147816.Pythia8_AU2CTEQ6L1_Zee.root
#OutputName=RTT.MC12.147817.Pythia8_AU2CTEQ6L1_Zmumu.root
#OutputName=RTT.MC12.147818.Pythia8_AU2CTEQ6L1_Ztautau.root
#OutputName=RTT.MC12.147830.Pythia_AUET2BCTEQ6L1_Wenu.root
#OutputName=RTT.MC12.147831.Pythia_AUET2BCTEQ6L1_Wmunu.root
#OutputName=RTT.MC12.147832.Pythia_AUET2BCTEQ6L1_Wtaunu.root
#OutputName=RTT.MC12.147900.Pythia8_AU2CT10_jetjet_JZ0.root
#OutputName=RTT.MC12.147907.Pythia8_AU2CT10_jetjet_JZ7.root
#OutputName=RTT.MC12.108294.Herwigpp_UEEE3_CTEQ6L1_Zee.root
#OutputName=RTT.MC12.108295.Herwigpp_UEEE3_CTEQ6L1_Zmumu.root

#---------------------------------------------------------------------- 
PostJoFile=jobOptions.RTT.MC12.147810.Pythia8_AU2CTEQ6L1_Wenu.py
#PostJoFile=jobOptions.RTT.MC12.147811.Pythia8_AU2CTEQ6L1_Wmunu.py
#PostJoFile=jobOptions.RTT.MC12.147812.Pythia8_AU2CTEQ6L1_Wtaunu.py
#PostJoFile=jobOptions.RTT.MC12.147816.Pythia8_AU2CTEQ6L1_Zee.py
#PostJoFile=jobOptions.RTT.MC12.147817.Pythia8_AU2CTEQ6L1_Zmumu.py
#PostJoFile=jobOptions.RTT.MC12.147818.Pythia8_AU2CTEQ6L1_Ztautau.py
#PostJoFile=jobOptions.RTT.MC12.147830.Pythia_AUET2BCTEQ6L1_Wenu.py
#PostJoFile=jobOptions.RTT.MC12.147831.Pythia_AUET2BCTEQ6L1_Wmunu.py
#PostJoFile=jobOptions.RTT.MC12.147832.Pythia_AUET2BCTEQ6L1_Wtaunu.py
#PostJoFile=jobOptions.RTT.MC12.147900.Pythia8_AU2CT10_jetjet_JZ0.py
#PostJoFile=jobOptions.RTT.MC12.147907.Pythia8_AU2CT10_jetjet_JZ7.py
#PostJoFile=jobOptions.RTT.MC12.108294.Herwigpp_UEEE3_CTEQ6L1_Zee.py
#PostJoFile=jobOptions.RTT.MC12.108295.Herwigpp_UEEE3_CTEQ6L1_Zmumu.py

#---------------------------------------------------------------------- 
# Start execution
#---------------------------------------------------------------------- 
echo "Submitting reference files' production..."
echo "Output directory:" /tmp/$User/$JobCoreName

i=0

for i in {0..10};
do 
  echo "Job #" $i;
  
  [ -d /tmp/$User/$JobCoreName ] || mkdir /tmp/$User/$JobCoreName;
  [ -d /tmp/$User/$JobCoreName/job_$i ] || mkdir /tmp/$User/$JobCoreName/job_$i;
  cp $PostJoFile /tmp/$User/$JobCoreName/job_$i;
  cp make_avg.C /tmp/$User/$JobCoreName;
  cd /tmp/$User/$JobCoreName/job_$i;
  
  tmpRnd=$RANDOM
   
  JOBOPTSEARCHPATH=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/common:$JOBOPTSEARCHPATH; 
  Generate_trf.py ecmEnergy=8000. runNumber=$RunNr firstEvent=1 maxEvents=$EventsPerJob randomSeed=$tmpRnd \
    jobConfig=$InputJoFile outputEVNTFile=EVNT.$OutputName \
    postInclude=$PostJoFile >| $JobCoreName_$i.log 2>&1 &;
  
  cd -

done

