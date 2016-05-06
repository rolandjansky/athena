from AthenaCommon.GlobalFlags import globalflags

from RecExConfig.RecFlags import rec

if globalflags.InputFormat=='bytestream' or (not  TriggerFlags.doLVL1()):
    CBNT_AthenaAware.Members += ["LVL1::CBNTAA_LVL1CaloESD/CBNT_LVL1calo"]
elif (rec.readESD() or rec.readAOD()):
    CBNT_AthenaAware.Members += ["LVL1::CBNTAA_LVL1CaloESD/CBNT_LVL1calo"]
elif (rec.readRDO() and TriggerFlags.doLVL1()):
    CBNT_AthenaAware.Members += [ "LVL1::CBNTAA_LVL1calo/CBNT_LVL1calo" ]
elif ( jobproperties.CaloCellFlags.doFastCaloSim() and TriggerFlags.doLVL1()):
    CBNT_AthenaAware.Members += [ "LVL1::CBNTAA_LVL1calo/CBNT_LVL1calo" ]
       
CBNT_AthenaAware.Members += [ "CBNTAA_ReadRoIBResult/CBNT_ReadRoIBResult" ]

#CBNTAthenaAware=True
include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )
if rec.Commissioning:
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    if DQMonFlags.doCTPMon():
        include ("TrigT1CTMonitoring/NtupleFragment.py")
    if DQMonFlags.doLVL1CaloMon(): 
        include( "TrigT1CaloCalibTools/CBNT_L1Calo_jobOptions.py" ) 
else:
    doL1CaloCBNT_JEM = False
    doL1CaloCBNT_CPM = False
    doL1CaloCBNT_RODHeader = False
    include( "TrigT1CaloCalibTools/CBNT_L1Calo_jobOptions.py" ) 

    include("TrigNtInDet/jobOfragment_TrigNtInDet.py")
    
    #if CBNTAthenaAware:
    #    include("TrigDecisionMaker/jobOfragment_CBNTAA_TriggerDecision.py")

