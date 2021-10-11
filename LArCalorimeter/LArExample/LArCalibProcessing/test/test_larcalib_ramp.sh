#!/bin/sh
#
# art-description: Testing LAR ramp
# art-type: build
# art-include: 21.0/Athena
# art-include: master/Athena
 
athena.py -c 'doMonitoring=False; doLArCalibDataQuality=True; CorrectBadChannels=True; ApplyAdHocCorrection=False; GainList=["HIGH"]; PartitionType="EMB"; Partition="EB-EMBA"; PartitionTypeGeneric="EMBPS"; RunNumberList=[174586]; RunNumberFlag=int(RunNumberList[0]); InputDir=" "; FullFileName=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/LArCalibProcessing/data11_calib.00174586.calibration_LArElec-Ramp-7s-High-BarrelPS.daq.RAW._lb0000._EB-EMBA._0001.data"]; GroupingType="ExtendedSubDetector"; from AthenaCommon.GlobalFlags import globalflags; globalflags.DatabaseInstance.set_Value_and_Lock ("COMP200"); from AthenaCommon.JobProperties import jobproperties; jobproperties.Global.DetDescrVersion.set_Value_and_Lock ("ATLAS-R1-2011-02-00-00"); DBConnectionCOOL="COOLOFL_LAR/COMP200"; InputDBConnectionOFC="COOLONL_LAR/COMP200";InputDBConnectionBadChannel="COOLOFL_LAR/COMP200";CaliOFCFolder="/LAR/ElecCalibOnl/OFC"; CaliOFCTagSpec="HEAD" ' LArCalibProcessing/UseLArExtendedSubDetGrouping.py  LArCalibProcessing/LArCalib_Ramp_jobOptions.py LArCalibProcessing/LArCalib_Ramp_testing.py
 
echo  "art-result: $? ramp"
