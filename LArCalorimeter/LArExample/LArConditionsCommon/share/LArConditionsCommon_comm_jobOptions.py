###################################################################
#
# Job options file for accessing LArRawConditions objects from COOL
#   For real data
#
#==================================================================

include.block ( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )

if not "SuperCells" in dir():
   SuperCells=False

from RecExConfig.RecFlags import rec
from LArConditionsCommon.LArCondFlags import larCondFlags 

from IOVDbSvc.CondDB import conddb

if rec.projectName().startswith("data09") :
    larCondFlags.LArCoolChannelSelection="3:238,306,313,319,325,331,338,344,350,1001:1012,1021,1022"
    larCondFlags.OFCShapeFolder="5samples3bins17phases"

if svcMgr.MessageSvc.OutputLevel <= DEBUG :
  print larCondFlags


if larCondFlags.LArElecCalibSqlite.statusOn and larCondFlags.LArElecCalibSqlite()!="":
  # Assume sqlite file contains the online database. Adjust other flags as needed
   sqlDB="<db>sqlite://;schema="+larCondFlags.LArElecCalibSqlite()+";dbname=CONDB2</db>"
   ONLDB=""
   larCondFlags.ua2MeVFolder=""
   larCondFlags.MphysOverMcalFolder=""
   larCondFlags.OFCShapeFolder=""
else:
  ONLDB="LAR_ONL"
  sqlDB=""

forceRN=""
if larCondFlags.LArForceIOVRunNumber.statusOn and larCondFlags.LArForceIOVRunNumber()!="": 
    forceRN="<forceRunNumber>" +larCondFlags.LArForceIOVRunNumber() + "</forceRunNumber>"

#Load Bad-Channel folder: (Should be move this elsewhere?)

#Hack following the Online/Offline split of the BadChannel database:
#Online applications read from COOLONL_LAR/COMP200, folder /LAR/BadChannels/BadChannels
#Offline applications read from COOLOFL_LAR/COMP200, folder /LAR/BadChannelsOfl/BadChannels
#But SG key(=Folder name) is expected to be the same in both cases (default set in LArBadChanTool.cxx)
#Solution: Re-key the object when reading from offline DB

rekeyBC="<key>/LAR/BadChannels/BadChannels</key>"
rekeyMF="<key>/LAR/BadChannels/MissingFEBs</key>"
conddb.addFolderSplitOnline("LAR","/LAR/BadChannels/BadChannels","/LAR/BadChannelsOfl/BadChannels"+forceRN+rekeyBC)
conddb.addFolderSplitOnline("LAR","/LAR/BadChannels/MissingFEBs","/LAR/BadChannelsOfl/MissingFEBs"+forceRN+rekeyMF)


if not larCondFlags.LoadElecCalib.is_locked():
    larCondFlags.LoadElecCalib.set_Value(rec.readRDO()) 


#For run 2 we use cool-inline storage in online database COOLONL_LAR/CONDBR2:
haveElecCalibInline=(conddb.dbdata=="CONDBR2")


if (haveElecCalibInline):
    # Run 2 case:
    #This service creates a objects in the DetectorStore that wrap the AttributeListCollections 
    #with the inline representation of the electronic calibration and makes them accessible through the
    #ILArRamp, ILArOFC, etc. abstract interfaces
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
    theLArCondSvc=LArFlatConditionSvc()
    svcMgr+=theLArCondSvc
    svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]   


#Load HVScaleCorr. For run 2,these constants are also used by the CaloNoiseToolDB 
if (haveElecCalibInline):
    conddb.addFolder(ONLDB,"/LAR/ElecCalibFlat/HVScaleCorr"+forceRN+sqlDB)
    theLArCondSvc.HVScaleCorrInput="/LAR/ElecCalibFlat/HVScaleCorr"


#Load Electronic Calibration folders:
if larCondFlags.LoadElecCalib():
    
  #Some examples based on ExtendedFTGrouping, not duely tested.
  #Always use correction channels
  #LArCoolChannelSelection="0,1,3:473"                           #Ignore low gain
  #LArCoolChannelSelection="0:3,3:66,117:180,238:304,355:418"    #Barrel High, Med gain
  #LArCoolChannelSelection="0:238,306,313,319,325,331,338,334,350" #EM High, HEC medium gain

  selection=""
  if larCondFlags.LArCoolChannelSelection.statusOn and larCondFlags.LArCoolChannelSelection()!="": 
    selection="<channelSelection>"+larCondFlags.LArCoolChannelSelection()+"</channelSelection>"
    pass


  if (haveElecCalibInline):
   if not SuperCells: 
      # Run 2 case:
      #1. uA2MeV
      if larCondFlags.ua2MeVFolder()=="":
          conddb.addFolder("LAR_ONL","/LAR/ElecCalibFlat/uA2MeV"+forceRN)
          theLArCondSvc.uA2MeVInput="/LAR/ElecCalibFlat/uA2MeV"

      else:
          #Load from offline database
          conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/"+larCondFlags.ua2MeVFolder()+forceRN)
          pass
      
      #2. DAC2uA
      conddb.addFolder("LAR_ONL","/LAR/ElecCalibFlat/DAC2uA"+forceRN)
      theLArCondSvc.DAC2uAInput="/LAR/ElecCalibFlat/DAC2uA"

      #3. Pedestal
      conddb.addFolder(ONLDB,"/LAR/ElecCalibFlat/Pedestal"+forceRN+sqlDB)
      theLArCondSvc.PedestalInput="/LAR/ElecCalibFlat/Pedestal"

      #4. Ramp
      conddb.addFolder(ONLDB,"/LAR/ElecCalibFlat/Ramp"+forceRN+sqlDB)
      theLArCondSvc.RampInput="/LAR/ElecCalibFlat/Ramp"
      
      #5. MphysOverMcal
      if larCondFlags.MphysOverMcalFolder()=="":
          conddb.addFolder(ONLDB,"/LAR/ElecCalibFlat/MphysOverMcal"+forceRN+sqlDB)
          theLArCondSvc.MphysOverMcalInput="/LAR/ElecCalibFlat/MphysOverMcal"
      else: 
          #Load from offline database:
          conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/"+larCondFlags.MphysOverMcalFolder()+forceRN+sqlDB)
          pass

      #6. HVScaleCorr -> moved outside of the if loadElecCalib clause b/c it's now used by the CaloNoiseTool
      #conddb.addFolder(ONLDB,"/LAR/ElecCalibFlat/HVScaleCorr"+forceRN+sqlDB)
      #theLArCondSvc.HVScaleCorrInput="/LAR/ElecCalibFlat/HVScaleCorr"

      #7. OFCs
      if larCondFlags.OFCShapeFolder()=="":
          conddb.addFolder(ONLDB,"/LAR/ElecCalibFlat/OFC"+forceRN+sqlDB)
          theLArCondSvc.OFCInput="/LAR/ElecCalibFlat/OFC"
      else:
          #Load from offline DB
          if 'RekeyOFC' in dir():    
             conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/OFC/PhysWave/RTM/"+larCondFlags.OFCShapeFolder()+selection+forceRN+"<key>"+RekeyOFC+"</key>")
          else:   
             conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/OFC/PhysWave/RTM/"+larCondFlags.OFCShapeFolder()+selection+forceRN)
          pass
      pass       
      #8.Shape
      if larCondFlags.useShape():
          if larCondFlags.OFCShapeFolder()=="":
              conddb.addFolder(ONLDB,"/LAR/ElecCalibFlat/Shape"+forceRN+sqlDB)
              theLArCondSvc.ShapeInput="/LAR/ElecCalibFlat/Shape"
          else:
              #Load from offline database
              if 'RekeyShape' in dir():
                 conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/Shape/RTM/"+larCondFlags.OFCShapeFolder()+selection+forceRN+"<key>"+RekeyShape+"</key>")
              else:   
                 conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/Shape/RTM/"+larCondFlags.OFCShapeFolder()+selection+forceRN)
              pass
          pass
      pass
   else:
      print "In SuperCell case... so far will not initialise folders."   

  else: #Run 1 case, no COOL-inline electronic calibration

   if not SuperCells: 
      #For run 1 we read some electronic calibration constants from the offline DB:
      if not larCondFlags.ua2MeVFolder.is_locked():
          larCondFlags.ua2MeVFolder="uA2MeV/Symmetry"
          pass
      if not larCondFlags.MphysOverMcalFolder.is_locked():
          larCondFlags.MphysOverMcalFolder="MphysOverMcal/RTM"
          pass
      if not larCondFlags.OFCShapeFolder.is_locked():
          larCondFlags.OFCShapeFolder="5samples1phase"
          pass



      #1. uA2MeV
      if larCondFlags.ua2MeVFolder()=="":
          conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/uA2MeV"+forceRN)
      else:
          #Load from offline database
          conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/"+larCondFlags.ua2MeVFolder()+forceRN)
          pass
      
      #2. DAC2uA
      conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/DAC2uA"+forceRN)

      #3. Pedestal
      conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/Pedestal"+forceRN)

      #4. Ramp
      conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/Ramp"+forceRN)
      
      #5. MphysOverMcal
      if larCondFlags.MphysOverMcalFolder()=="":
          conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/MphysOverMcal"+forceRN)
      else: 
          #Load from offline database:
          conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/"+larCondFlags.MphysOverMcalFolder()+forceRN)
          pass

      #6. HVScaleCorr 
      conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/HVScaleCorr"+forceRN)

      #7. OFCs
      if larCondFlags.OFCShapeFolder()=="":
          conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/OFC"+forceRN)
      else:
          #Load from offline DB
          conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/OFC/PhysWave/RTM/"+larCondFlags.OFCShapeFolder()+selection+forceRN)
          pass
      #8.Shape
      if larCondFlags.useShape():
          if larCondFlags.OFCShapeFolder()=="":
              conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/Shape"+forceRN)
          else:
              #Load from offline database
              conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/Shape/RTM/"+larCondFlags.OFCShapeFolder()+selection+forceRN)
              pass
          pass
      pass
   else:
      print "In SuperCell case... so far will not initialise folders."   
  pass
