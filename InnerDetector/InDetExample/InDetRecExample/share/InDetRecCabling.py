# block include this file, many package will want to setup cabling
include.block ('InDetRecExample/InDetRecCabling.py')

from InDetRecExample.InDetJobProperties import InDetFlags

if not ('conddb' in dir()):
  IOVDbSvc = Service("IOVDbSvc")
  from IOVDbSvc.CondDB import conddb

#
# --- Pixel cabling
#
if DetFlags.detdescr.pixel_on() and not 'PixelCabling' in dir():
  from AthenaCommon.CfgGetter import getService
  PixelCablingSvc = getService("PixelCablingSvc")
  ServiceMgr += PixelCablingSvc
  if (InDetFlags.doPrintConfigurables()):
    print  PixelCablingSvc
      
#
# --- SCT cabling
#
if DetFlags.detdescr.SCT_on() and not 'SCT_CablingSvc' in dir():
  from AthenaCommon.CfgGetter import getService
  SCT_CablingSvc = getService("SCT_CablingSvc")
  ServiceMgr += SCT_CablingSvc
  if (InDetFlags.doPrintConfigurables()):
    print  SCT_CablingSvc
  
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
    print  TRT_Cabling

  from TRT_Cabling.TRT_CablingConf import TRT_CablingSvc
  TRT_CablingSvc = TRT_CablingSvc() 

  ServiceMgr += TRT_CablingSvc
  if (InDetFlags.doPrintConfigurables()):
    print  TRT_CablingSvc  
       
