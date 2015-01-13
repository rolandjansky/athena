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
  SCTCablingDataSource='CORACOOL'
  SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'
  #if its CONDBR2, use new folders...
  if (conddb.dbdata == "CONDBR2"):
      SCTConfigurationFolderPath='/SCT/DAQ/Config/'
  #...but now check if we want to override that decision with explicit flag (if there is one)
  try:
      if InDetFlags.ForceCoraCool():
          SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'
  except:
      pass
        
  #to read SCT cabling from db 
  from SCT_Cabling.SCT_CablingConf import SCT_CablingSvc
  SCT_CablingSvc = SCT_CablingSvc(DataSource = "CORACOOL") 

  ServiceMgr += SCT_CablingSvc
  if (InDetFlags.doPrintConfigurables()):
    print  SCT_CablingSvc
  SCTRodConfigPath=SCTConfigurationFolderPath+'ROD'
  SCTMurConfigPath=SCTConfigurationFolderPath+'MUR'
  SCTRodMurConfigPath=SCTConfigurationFolderPath+'RODMUR'
  SCTGeogConfigPath=SCTConfigurationFolderPath+'Geog'
  #
  if not conddb.folderRequested(SCTRodConfigPath):
    conddb.addFolderSplitMC("SCT",SCTRodConfigPath,SCTRodConfigPath)
  if not conddb.folderRequested(SCTMurConfigPath):
    conddb.addFolderSplitMC("SCT",SCTMurConfigPath,SCTMurConfigPath)
  if not conddb.folderRequested(SCTRodMurConfigPath):
    conddb.addFolderSplitMC("SCT",SCTRodMurConfigPath,SCTRodMurConfigPath)
  if not conddb.folderRequested(SCTGeogConfigPath):
    conddb.addFolderSplitMC("SCT",SCTGeogConfigPath,SCTGeogConfigPath)
  
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
       
