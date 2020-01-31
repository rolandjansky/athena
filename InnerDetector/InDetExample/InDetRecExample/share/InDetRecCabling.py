# block include this file, many package will want to setup cabling
include.block ('InDetRecExample/InDetRecCabling.py')

from InDetRecExample.InDetJobProperties import InDetFlags

if not ('conddb' in dir()):
  IOVDbSvc = Service("IOVDbSvc")
  from IOVDbSvc.CondDB import conddb

from AthenaCommon.DetFlags import DetFlags

#
# --- Pixel cabling
#
if DetFlags.detdescr.pixel_on() and not 'PixelCabling' in dir():
  from PixelCabling.PixelCablingConf import PixelCablingSvc
  PixelCablingSvc = PixelCablingSvc()
  ServiceMgr += PixelCablingSvc
  if (InDetFlags.doPrintConfigurables()):
    printfunc  (PixelCablingSvc)
      
#
# --- SCT cabling
#
if DetFlags.detdescr.SCT_on():
  from AthenaCommon.AlgSequence import AthSequencer
  condSeq = AthSequencer("AthCondSeq")
  if not hasattr(condSeq, "SCT_CablingCondAlgFromCoraCool"):
    from AthenaCommon.CfgGetter import getAlgorithm
    SCT_CablingCondAlgFromCoraCool = getAlgorithm("SCT_CablingCondAlgFromCoraCool")
    condSeq += SCT_CablingCondAlgFromCoraCool
    if (InDetFlags.doPrintConfigurables()):
      printfunc  (SCT_CablingCondAlgFromCoraCool)
  
#
# --- TRT cabling
#
if DetFlags.detdescr.TRT_on() and not 'TRT_Cabling' in dir():
  # if bytestream comes from real data
  from TRT_Cabling.TRT_CablingConf import TRT_FillCablingData_DC3
  TRT_Cabling = TRT_FillCablingData_DC3()
  if not (globalflags.DataSource() == 'geant4'): 
    # TRT needs the hack in "the other direction"
    TRT_Cabling.RealData=True
  ToolSvc += TRT_Cabling 
  if (InDetFlags.doPrintConfigurables()):
    printfunc (TRT_Cabling)

  from TRT_Cabling.TRT_CablingConf import TRT_CablingSvc
  TRT_CablingSvc = TRT_CablingSvc() 

  ServiceMgr += TRT_CablingSvc
  if (InDetFlags.doPrintConfigurables()):
    printfunc  (TRT_CablingSvc)
       
