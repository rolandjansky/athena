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
  # cabling comes from real data
  from PixelCabling.PixelCablingConf import PixelCablingSvc
  PixelCablingSvc = PixelCablingSvc()

  if not (globalflags.DataSource() == 'geant4'):
    conddb.addFolder("PIXEL","/PIXEL/ReadoutSpeed") 
  else: 
    conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed") 

  ServiceMgr += PixelCablingSvc  
  if (InDetFlags.doPrintConfigurables()):
    print  PixelCablingSvc
      
#
# --- SCT cabling
#
if DetFlags.detdescr.SCT_on() and not 'SCT_CablingSvc' in dir():
  #to read SCT cabling from db 
  from SCT_Cabling.SCT_CablingConf import SCT_CablingSvc
  SCT_CablingSvc = SCT_CablingSvc(DataSource = "CORACOOL") 

  ServiceMgr += SCT_CablingSvc
  if (InDetFlags.doPrintConfigurables()):
    print  SCT_CablingSvc

  if not conddb.folderRequested('/SCT/DAQ/Configuration/ROD'):
    conddb.addFolderSplitMC("SCT","/SCT/DAQ/Configuration/ROD","/SCT/DAQ/Configuration/ROD")
  if not conddb.folderRequested('/SCT/DAQ/Configuration/MUR'):
    conddb.addFolderSplitMC("SCT","/SCT/DAQ/Configuration/MUR","/SCT/DAQ/Configuration/MUR")
  if not conddb.folderRequested('/SCT/DAQ/Configuration/RODMUR'):
    conddb.addFolderSplitMC("SCT","/SCT/DAQ/Configuration/RODMUR","/SCT/DAQ/Configuration/RODMUR")
  if not conddb.folderRequested('/SCT/DAQ/Configuration/Geog'):
    conddb.addFolderSplitMC("SCT","/SCT/DAQ/Configuration/Geog","/SCT/DAQ/Configuration/Geog")
  
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
       
