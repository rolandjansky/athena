###################################################################
#
# Job options file for accessing LArRawConditions objects from COOL
#   For real data
#
#==================================================================

include.block ( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )

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
    # TEMPORARY --- until everything's been changed to use conditions handles.
    # Run 2 case:
    #This service creates a objects in the DetectorStore that wrap the AttributeListCollections 
    #with the inline representation of the electronic calibration and makes them accessible through the
    #ILArRamp, ILArOFC, etc. abstract interfaces
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
    theLArCondSvc=LArFlatConditionSvc()
    svcMgr+=theLArCondSvc
    svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]   


def addLArFlatFolder (db, obj, calg, qual=''):
    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")

    folder = '/LAR/ElecCalibFlat/' + obj
    conddb.addFolder(db, folder + forceRN + qual,
                     className = 'CondAttrListCollection')
    condSequence += calg (ReadKey=folder, WriteKey='LAr'+obj)
    return

    
def addLArFolder (db, obj, cls, qual=''):
    if db.endswith ('OFL'):
        folder = 'ElecCalibOfl'
    else:
        folder = 'ElecCalibOnl'
    conddb.addFolder (db, '/LAR/' + folder + '/' + obj + forceRN+qual,
                      className = cls)
    return


#Load HVScaleCorr. For run 2,these constants are also used by the CaloNoiseToolDB 
if (haveElecCalibInline):
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArHVScaleCorrFlat_ as LArHVScaleCorrCondAlg
    addLArFlatFolder (ONLDB, 'HVScaleCorr', LArHVScaleCorrCondAlg, sqlDB)
    # TEMPORARY
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
      # Run 2 case:
      #1. uA2MeV
      if larCondFlags.ua2MeVFolder()=="":
          from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LAruA2MeVFlat_ as LAruA2MeVCondAlg 
          addLArFlatFolder ('LAR_ONL', 'uA2MeV', LAruA2MeVCondAlg)
          # TEMPORARY
          theLArCondSvc.uA2MeVInput="/LAR/ElecCalibFlat/uA2MeV"

      else:
          #Load from offline database
          addLArFolder ('LAR_OFL', larCondFlags.ua2MeVFolder(),
                        'LAruA2MeVComplete')
      
      #2. DAC2uA
      from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArDAC2uAFlat_ as LArDAC2uACondAlg 
      addLArFlatFolder ('LAR_ONL', 'DAC2uA', LArDAC2uACondAlg)
      # TEMPORARY
      theLArCondSvc.DAC2uAInput="/LAR/ElecCalibFlat/DAC2uA"

      #3. Pedestal
      from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArPedestalFlat_ as LArPedestalCondAlg 
      addLArFlatFolder (ONLDB, 'Pedestal', LArPedestalCondAlg, sqlDB)
      # TEMPORARY
      theLArCondSvc.PedestalInput="/LAR/ElecCalibFlat/Pedestal"

      #4. Ramp
      from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArRampFlat_ as LArRampCondAlg 
      addLArFlatFolder (ONLDB, 'Ramp', LArRampCondAlg, sqlDB)
      # TEMPORARY
      theLArCondSvc.RampInput="/LAR/ElecCalibFlat/Ramp"
      
      #5. MphysOverMcal
      if larCondFlags.MphysOverMcalFolder()=="":
          from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArMphysOverMcalFlat_ as LArMphysOverMcalCondAlg 
          addLArFlatFolder (ONLDB, 'MphysOverMcal',
                            LArMphysOverMcalCondAlg, sqlDB)
          # TEMPORARY
          theLArCondSvc.MphysOverMcalInput="/LAR/ElecCalibFlat/MphysOverMcal"

      else:
          #Load from offline database:
          addLArFolder ('LAR_OFL', larCondFlags.MphysOverMcalFolder(),
                        'LArMphysOverMcalComplete', sqlDB)

      #6. HVScaleCorr -> moved outside of the if loadElecCalib clause b/c it's now used by the CaloNoiseTool

      #7. OFCs
      if larCondFlags.OFCShapeFolder()=="":
          from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArOFCFlat_ as LArOFCCondAlg 
          addLArFlatFolder (ONLDB, 'OFC', LArOFCCondAlg, sqlDB)
          # TEMPORARY
          theLArCondSvc.OFCInput="/LAR/ElecCalibFlat/OFC"
      else:
          #Load from offline DB
          addLArFolder ('LAR_OFL',
                        'OFC/PhysWave/RTM/'+larCondFlags.OFCShapeFolder(),
                        'LArOFCComplete', selection)

      #8.Shape
      if larCondFlags.useShape():
          if larCondFlags.OFCShapeFolder()=="":
              from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArShapeFlat_ as LArShapeCondAlg 
              addLArFlatFolder (ONLDB, 'Shape', LArShapeCondAlg, sqlDB)
              # TEMPORARY
              theLArCondSvc.ShapeInput="/LAR/ElecCalibFlat/Shape"
          else:
              #Load from offline database
              addLArFolder ('LAR_OFL',
                            'Shape/RTM/'+larCondFlags.OFCShapeFolder(),
                            'LArShapeComplete', selection)


          pass
      pass

  else: #Run 1 case, no COOL-inline electronic calibration

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
          addLArFolder ('LAR_ONL', 'uA2MeV', 'LAruA2MeVMC')
      else:
          #Load from offline database
          addLArFolder ('LAR_OFL', larCondFlags.ua2MeVFolder(), 'LAruA2MeVMC')
      
      #2. DAC2uA
      addLArFolder ('LAR_ONL', 'DAC2uA', 'LArDAC2uAMC')

      #3. Pedestal
      addLArFolder ('LAR_ONL', 'Pedestal', 'LArPedestalComplete')

      #4. Ramp
      addLArFolder ('LAR_ONL', 'Ramp', 'LArRampComplete')
      
      #5. MphysOverMcal
      if larCondFlags.MphysOverMcalFolder()=="":
          addLArFolder ('LAR_ONL', MphysOverMcal,
                        'LArMphysOverMcalComplete')
      else: 
          #Load from offline database:
          addLArFolder ('LAR_OFL', larCondFlags.MphysOverMcalFolder(),
                        'LArMphysOverMcalComplete')

      #6. HVScaleCorr
      addLArFolder ('LAR_ONL', 'HVScaleCorr', 'LArHVScaleCorrComplete')

      #7. OFCs
      if larCondFlags.OFCShapeFolder()=="":
          conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/OFC"+forceRN)
          addLArFolder ('LAR_ONL', 'OFC', 'LArOFCComplete')
      else:
          #Load from offline DB
          addLArFolder ('LAR_OFL', 'OFC/PhysWave/RTM/'+larCondFlags.OFCShapeFolder(),
                        'LArOFCComplete', selection)

      #8.Shape
      if larCondFlags.useShape():
          if larCondFlags.OFCShapeFolder()=="":
              addLArFolder ('LAR_ONL', 'Shape', 'LArShapeComplete')

          else:
              #Load from offline database
              addLArFolder ('LAR_OFL', 'Shape/RTM/'+larCondFlags.OFCShapeFolder(),
                            'LArShapeComplete', selection)
          pass
      pass
  pass
