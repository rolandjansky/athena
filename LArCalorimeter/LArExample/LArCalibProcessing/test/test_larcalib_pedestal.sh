#!/bin/sh
#
# art-description: Testing LAR pedestal
# art-type: build
# art-include: 21.0/Athena
# art-include: master/Athena
 
athena.py -c 'doMonitoring=False; runAccumulator=False; doLArCalibDataQuality=True; GainList=["HIGH"]; PartitionType="EMB"; Partition="EB-EMBA"; PartitionTypeGeneric="EMBPS"; RunNumberList=[174585]; RunNumberFlag=int(RunNumberList[0]); InputDir=" "; FullFileName=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/LArCalibProcessing/data11_calib.00174585.calibration_LArElec-Pedestal-7s-High-BarrelPS.daq.RAW._lb0000._EB-EMBA._0001.data"]; GroupingType="ExtendedSubDetector"; from AthenaCommon.GlobalFlags import globalflags; globalflags.DatabaseInstance.set_Value_and_Lock ("COMP200"); from AthenaCommon.JobProperties import jobproperties; jobproperties.Global.DetDescrVersion.set_Value_and_Lock ("ATLAS-R1-2011-02-00-00"); DBConnectionCOOL="COOLOFL_LAR/COMP200" ' LArCalibProcessing/UseLArExtendedSubDetGrouping.py  LArCalibProcessing/LArCalib_PedestalAutoCorr_jobOptions.py LArCalibProcessing/LArCalib_PedestalAutoCorr_testing.py
 
echo  "art-result: $? pedestal"
