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
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

if rec.projectName().startswith("data09") :
    larCondFlags.LArCoolChannelSelection="3:238,306,313,319,325,331,338,344,350,1001:1012,1021,1022"
    larCondFlags.OFCShapeFolder="5samples3bins17phases"

if svcMgr.MessageSvc.OutputLevel <= DEBUG :
  printfunc (larCondFlags)


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
#But SG key(=Folder name) is expected to be the same in both cases 
#Solution: Re-key the object when reading from offline DB
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" ) #Needed by BC cond alog
rekeyBC="<key>/LAR/BadChannels/BadChannels</key>"
rekeyMF="<key>/LAR/BadChannels/MissingFEBs</key>"
conddb.addFolderSplitOnline("LAR","/LAR/BadChannels/BadChannels","/LAR/BadChannelsOfl/BadChannels"+forceRN+rekeyBC,className="CondAttrListCollection")
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg
condSeq+=LArBadChannelCondAlg(ReadKey="/LAR/BadChannels/BadChannels")

conddb.addFolderSplitOnline("LAR","/LAR/BadChannels/MissingFEBs","/LAR/BadChannelsOfl/MissingFEBs"+forceRN+rekeyMF,className='AthenaAttributeList')
from LArBadChannelTool.LArBadChannelToolConf import LArBadFebCondAlg
condSeq+=LArBadFebCondAlg(ReadKey="/LAR/BadChannels/MissingFEBs")

if (rec.doESD() or rec.doRDOTrigger()):
   if 'COMP200' not in conddb.GetInstance():
      rekeyBADF="<key>/LAR/BadChannels/KnownBADFEBs</key>"
      rekeyMNBF="<key>/LAR/BadChannels/KnownMNBFEBs</key>"
      conddb.addFolderSplitOnline("LAR","/LAR/BadChannels/KnownBADFEBs","/LAR/BadChannelsOfl/KnownBADFEBs"+forceRN+rekeyBADF,False,False,False,"AthenaAttributeList")
      inkeyBad="/LAR/BadChannels/KnownBADFEBs"
      conddb.addFolderSplitOnline("LAR","/LAR/BadChannels/KnownMNBFEBs","/LAR/BadChannelsOfl/KnownMNBFEBs"+forceRN+rekeyMNBF,False,False,False,"AthenaAttributeList")
      inkeyMNB="/LAR/BadChannels/KnownMNBFEBs"
   else:   
      inkeyBad=""
      inkeyMNB=""
   pass

   condSeq+=LArBadFebCondAlg("LArKnownBadFebAlg",ReadKey=inkeyBad,WriteKey="LArKnownBadFEBs")
   condSeq+=LArBadFebCondAlg("LArKnownMNBFebAlg",ReadKey=inkeyMNB,WriteKey="LArKnownMNBFEBs")

if not larCondFlags.LoadElecCalib.is_locked():
    larCondFlags.LoadElecCalib.set_Value(rec.readRDO()) 


#For run 2 we use cool-inline storage in online database COOLONL_LAR/CONDBR2:
haveElecCalibInline=(conddb.dbdata=="CONDBR2")



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
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArHVScaleCorrFlat_ as LArHVScaleCorrCondFlatAlg
    addLArFlatFolder (ONLDB, 'HVScaleCorr', LArHVScaleCorrCondFlatAlg, sqlDB)
else:
    # In Run-1 it's used also, an ESD2AOD is not covered so far, so adding it here
    if rec.readESD():
      addLArFolder ('LAR_ONL', 'HVScaleCorr', 'LArHVScaleCorrComplete')


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
          from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LAruA2MeVFlat_ as LAruA2MeVCondAlg 
          addLArFlatFolder ('LAR_ONL', 'uA2MeV', LAruA2MeVCondAlg)
      else:
          #Load from offline database
          addLArFolder ('LAR_OFL', larCondFlags.ua2MeVFolder(),
                        'LAruA2MeVComplete')
      
      #2. DAC2uA
      from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArDAC2uAFlat_ as LArDAC2uACondAlg 
      addLArFlatFolder ('LAR_ONL', 'DAC2uA', LArDAC2uACondAlg)

      #3. Pedestal
      from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArPedestalFlat_ as LArPedestalCondAlg 
      addLArFlatFolder (ONLDB, 'Pedestal', LArPedestalCondAlg, sqlDB)

      #4. Ramp
      from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArRampFlat_ as LArRampCondAlg 
      addLArFlatFolder (ONLDB, 'Ramp', LArRampCondAlg, sqlDB)
      
      #5. MphysOverMcal
      if larCondFlags.MphysOverMcalFolder()=="":
          from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArMphysOverMcalFlat_ as LArMphysOverMcalCondAlg 
          addLArFlatFolder (ONLDB, 'MphysOverMcal',
                            LArMphysOverMcalCondAlg, sqlDB)

      else:
          #Load from offline database:
          addLArFolder ('LAR_OFL', larCondFlags.MphysOverMcalFolder(),
                        'LArMphysOverMcalComplete', sqlDB)

      #6. HVScaleCorr -> moved outside of the if loadElecCalib clause b/c it's now used by the CaloNoiseTool

      #7. OFCs
      if larCondFlags.OFCShapeFolder()=="":
          from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArOFCFlat_ as LArOFCCondAlg 
          addLArFlatFolder (ONLDB, 'OFC', LArOFCCondAlg, sqlDB)
      else:
          #Load from offline DB
          if 'RekeyOFC' in dir():    
            addLArFolder ('LAR_OFL',
                        'OFC/PhysWave/RTM/'+larCondFlags.OFCShapeFolder(),
                        'LArOFCComplete', selection+"<key>"+RekeyOFC+"</key>")
          else:  
            addLArFolder ('LAR_OFL',
                        'OFC/PhysWave/RTM/'+larCondFlags.OFCShapeFolder(),
                        'LArOFCComplete', selection)

      #8.Shape
      if larCondFlags.useShape():
          if larCondFlags.OFCShapeFolder()=="":
              from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArShapeFlat_ as LArShapeCondAlg 
              addLArFlatFolder (ONLDB, 'Shape', LArShapeCondAlg, sqlDB)
          else:
              #Load from offline database
              if 'RekeyShape' in dir():
                addLArFolder ('LAR_OFL',
                            'Shape/RTM/'+larCondFlags.OFCShapeFolder(),
                            'LArShapeComplete', selection+"<key>"+RekeyShape+"</key>")
              else:  
                addLArFolder ('LAR_OFL',
                            'Shape/RTM/'+larCondFlags.OFCShapeFolder(),
                            'LArShapeComplete', selection)


          pass
      pass
   else:
      printfunc ("In SuperCell case... so far will not initialise folders."   )

  else: #Run 1 case, no COOL-inline electronic calibration
   if not SuperCells: 
      from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
      LArMCSymCondAlgDefault()
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

      #Schedule Sym-Cond algo for uA2MeV
      from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LAruA2MeVMC_LAruA2MeVSym_ as LAruA2MeVSymAlg
      condSeq+=LAruA2MeVSymAlg(ReadKey="LAruA2MeV",WriteKey="LAruA2MeVSym")
      
      #2. DAC2uA
      addLArFolder ('LAR_ONL', 'DAC2uA', 'LArDAC2uAMC')
      #Schedule Sym-Cond algo for DACuA
      from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArDAC2uAMC_LArDAC2uASym_ as LArDAC2uASymAlg
      condSeq+=LArDAC2uASymAlg(ReadKey="LArDAC2uA",WriteKey="LArDAC2uASym")

      #3. Pedestal
      addLArFolder ('LAR_ONL', 'Pedestal', 'LArPedestalComplete',
                    # POOL files set the key for this to `Pedestal',
                    # but we want to use `LArPedestal'.
                    '<key>LArPedestal</key>')

      #4. Ramp
      addLArFolder ('LAR_ONL', 'Ramp', 'LArRampComplete')
      
      #5. MphysOverMcal
      if larCondFlags.MphysOverMcalFolder()=="":
          addLArFolder ('LAR_ONL', 'MphysOverMcal',
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
   else:
      printfunc ("In SuperCell case... so far will not initialise folders.")
  pass


#special case for overlay jobs: We need LArfSampl 
from AthenaCommon.DetFlags import DetFlags
if DetFlags.overlay.LAr_on() and larCondFlags.LArfSamplTag()!="":
   printfunc ("Tag=%s" % larCondFlags.LArfSamplTag())
   conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/fSampl",larCondFlags.LArfSamplTag(),force=True,forceMC=True,className="LArfSamplMC")
   from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArfSamplMC_LArfSamplSym_ as LArfSamplSymAlg
   from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
   LArMCSymCondAlgDefault()
   condSeq+= LArfSamplSymAlg(ReadKey="LArfSampl",WriteKey="LArfSamplSym")
