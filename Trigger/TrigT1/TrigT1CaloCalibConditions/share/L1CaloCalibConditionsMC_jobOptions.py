include.block ( "TrigT1CaloCalibConditions/L1CaloCalibConditionsMC_jobOptions.py" )
if globalflags.DataSource() != "data":
  _L1CaloFolderList = [
        "/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib",
        "/TRIGGER/L1Calo/V2/Calibration/PpmDeadChannels",
        "/TRIGGER/L1Calo/V2/Conditions/DisabledTowers",
        "/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults",
  ]

  from IOVDbSvc.CondDB import conddb
  for k in _L1CaloFolderList:
    conddb.addFolder("TRIGGER_OFL", folder=k)
  del _L1CaloFolderList

# fix for ATR-XXXX necessary when writing database folders
#_name_type_overrides = [
#  ('ExtBcidThreshold', 'UInt16'),
#  ('SatBcidThreshLow', 'UInt16'),
#  ('SatBcidThreshHigh', 'UInt16'),
#  ('SatBcidLevel', 'UInt16'),
#  ('BcidEnergyRangeLow', 'UInt16'),
#  ('BcidEnergyRangeHigh' , 'UInt16'),
#  ('FirStartBit', 'UInt16'),
#  ('FirCoeff1', 'Int16'),
#  ('FirCoeff2', 'Int16'),
#  ('FirCoeff3', 'Int16'),
#  ('FirCoeff4', 'Int16'),
#  ('FirCoeff5', 'Int16'),
#  ('LutCpStrategy', 'UInt16'),
#  ('LutCpOffset', 'UInt16'),
#  ('LutCpNoiseCut', 'UInt16'),
#  ('LutCpSlope', 'UInt16'),
#  ('LutCpPar1', 'Int16'),
#  ('LutCpPar2', 'Int16'),
#  ('LutCpPar3', 'Int16'),
#  ('LutCpPar4', 'Int16'),
#  ('LutCpScale', 'UInt16'),
#  ('LutJepStrategy', 'UInt16'),
#  ('LutJepOffset', 'UInt16'),
#  ('LutJepNoiseCut', 'UInt16'),
#  ('LutJepSlope', 'UInt16'),
#  ('LutJepPar1', 'Int16'),
#  ('LutJepPar2', 'Int16'),
#  ('LutJepPar3', 'Int16'),
#  ('LutJepPar4', 'Int16'),
#  ('LutJepScale', 'UInt16')
#]
#
#import RegistrationServices.IOVRegistrationSvc
##svcMgr.IOVRegistrationSvc.OverrideNames.extend([n for n,_ in _name_type_overrides])
#svcMgr.IOVRegistrationSvc.OverrideTypes.extend([t for _,t in _name_type_overrides]) 
#del _name_type_overrides
