#!/bin/bash
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Submit pathena jobs to make Z counting HIST files for data
# Adjust DATASETS and LOWMU variables to configure the job
# Also see below for commented out code that enables using a GRL

#DATASETS=$(rucio ls 'data16_13TeV.00*physics_Main*.AOD*' --filter account=tzero --short)
#DATASETS=$(rucio list-content data16_13TeV:data16_13TeV.periodG.physics_Main.PhysCont.AOD.repro21_v01 --short)
#DATASETS="data18_13TeV.00356124.physics_Main.merge.AOD.f1035_m1999 data18_13TeV.00359823.physics_Main.merge.AOD.f1034_m2020 data18_13TeV.00363738.physics_Main.merge.AOD.f1006_m2037 data18_13TeV.00364292.physics_Main.merge.AOD.f1002_m2037 data18_13TeV.00350361.physics_Main.merge.AOD.f934_m1960"

#DATASETS="data17_13TeV:data17_13TeV.00338834.physics_Main.merge.AOD.r10258_p3399_tid13253867_00"
#LOWMU=0

# 2017 5 TeV low mu
#DATASETS=$(rucio list-content data17_5TeV:data17_5TeV.periodM.physics_Main.PhysCont.AOD.pro22_v02 --short)
#LOWMU=1

# 2017 13 TeV low mu
#DATASETS=$(rucio list-content data17_13TeV:data17_13TeV.periodN.physics_Main.PhysCont.AOD.pro22_v03 --short)
#LOWMU=1

# 2018 13 TeV low mu period J
#DATASETS=$(rucio list-content data18_13TeV:data18_13TeV.periodJ.physics_Main.PhysCont.AOD.t0pro22_v01 --short)
#LOWMU=1

# 2018 13 TeV low mu period G2
#DATASETS=$(rucio list-content data18_13TeV:data18_13TeV.periodG2.physics_Main.PhysCont.AOD.t0pro22_v01 --short)
#LOWMU=1

# 2018 13 TeV low mu period G2
#DATASETS=$(rucio list-content data18_13TeV:data18_13TeV.periodG4.physics_Main.PhysCont.AOD.t0pro22_v01 --short)
#LOWMU=1

# 2015 13 TeV high mu
#DATASETS=$(rucio list-content data15_13TeV:data15_13TeV.periodAllYear.physics_Main.PhysCont.AOD.repro21_v03 --short)
#LOWMU=0

# 2016 13 TeV high mu
#DATASETS=$(rucio list-content data16_13TeV:data16_13TeV.periodAllYear.physics_Main.PhysCont.AOD.repro21_v03 --short)
#LOWMU=0

# 2017 13 TeV high mu
#DATASETS=$(rucio list-content data17_13TeV:data17_13TeV.periodAllYear.physics_Main.PhysCont.AOD.pro22_v03 --short)
#LOWMU=0

# 2018 13 TeV high mu
DATASETS=$(rucio list-content data18_13TeV:data18_13TeV.periodB.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodC.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodD.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodE.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodF.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodH.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodI.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodJ.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodK.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodL.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodM.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodN.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodO.physics_Main.PhysCont.AOD.t0pro22_v01 data18_13TeV:data18_13TeV.periodQ.physics_Main.PhysCont.AOD.t0pro22_v01 --short)
LOWMU=0

PREFIX=user.ponyisi.2020320a2.merge
./generateDSFile.py $PREFIX ${DATASETS}
#POSTEXEC="ToolSvc.DQTMuonSelectionTool.MaxEta=0.9;ToolSvc.DQTGlobalWZFinderTool.MuonMaxEta=0.9"
#POSTEXEC="pass"
POSTEXEC="ToolSvc.DQTGlobalWZFinderTool.FillBCIDTrees=True"
if [[ $LOWMU == 1 ]] ; then {
    POSTEXEC=${POSTEXEC}";\
ToolSvc.monTrigTransTool.triggerMapping[\"primary_single_ele_iso\"]=\"\";\
ToolSvc.monTrigTransTool.triggerMapping[\"primary_single_ele\"]=\"HLT_e15_lhloose_nod0_L1EM12\";\
ToolSvc.monTrigTransTool.triggerMapping[\"monitoring_muonIso\"]=\"\";\
ToolSvc.monTrigTransTool.triggerMapping[\"monitoring_muonNonIso\"]=\"HLT_mu14\""
}; elif [[ $DATASETS == data15* ]] ; then {
    POSTEXEC=${POSTEXEC}";\
ToolSvc.monTrigTransTool.triggerMapping[\"primary_single_ele_iso\"]=\"\";\
ToolSvc.monTrigTransTool.triggerMapping[\"primary_single_ele\"]=\"HLT_e24_lhmedium_L1EM20VH,HLT_e60_lhmedium,HLT_e120_lhloose\";\
ToolSvc.monTrigTransTool.triggerMapping[\"monitoring_muonIso\"]=\"HLT_mu20_iloose_L1MU15\""
}; else {
    POSTEXEC=${POSTEXEC}";\
ToolSvc.monTrigTransTool.triggerMapping[\"primary_single_ele_iso\"]=\"HLT_e26_lhtight_nod0_ivarloose\";\
ToolSvc.monTrigTransTool.triggerMapping[\"primary_single_ele\"]=\"HLT_e60_lhmedium_nod0,HLT_e140_lhloose_nod0\""
    }; fi

if [[ $DATASETS == data18* ]] ; then {
    export GRL=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190318/physics_25ns_Triggerno17e33prim.xml
    export GRLNAME=PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim
} ; elif [[ $DATASETS == data17* ]] ; then {
    export GRL=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.xml
    export GRLNAME=PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim
} ; elif [[ $DATASETS == data16* ]] ; then {
    export GRL=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20180129/physics_25ns_21.0.19.xml
    export GRLNAME=PHYS_StandardGRL_All_Good_25ns
} ; elif [[ $DATASETS == data15* ]] ; then {
    export GRL=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20170619/physics_25ns_21.0.19.xml
    export GRLNAME=PHYS_StandardGRL_All_Good_25ns
} ; else {
    export GRL=None
    export GRLNAME=None
} ; fi

EXCLUDE=""
# following gives a GRL filter
#CMD="export GRL=$GRL;export GRLNAME=$GRLNAME;Reco_tf.py --preExec='rec.doApplyAODFix.set_Value_and_Lock(False);DQMonFlags.set_All_Off();DQMonFlags.doGlobalMon.set_Value_and_Lock(True)' --postExec='$POSTEXEC' --inputAODFile=%IN --outputHIST_AODFile=%OUT.HIST_AOD.root --postInclude='grl.py'"
CMD="Reco_tf.py --preExec='rec.doApplyAODFix.set_Value_and_Lock(False);DQMonFlags.set_All_Off();DQMonFlags.doGlobalMon.set_Value_and_Lock(True)' --postExec='$POSTEXEC' --inputAODFile=%IN --outputHIST_AODFile=%OUT.HIST_AOD.root"

ADDITIONAL="--addNthFieldOfInDSToLFN=2 --mergeOutput --mergeScript='merge.py' --excludedSite=ANALY_GOEGRID,ANALY_GLASGOW_SL7,UKI-SCOTGRID-GLASGOW,ANALY_LANCS_SL7,ANALY_FZU"

echo pathena $EXCLUDE --trf "$CMD" $ADDITIONAL --inOutDsJson inOutDs.json
pathena $EXCLUDE --trf "$CMD" $ADDITIONAL --inOutDsJson inOutDs.json

rm inOutDs.json
