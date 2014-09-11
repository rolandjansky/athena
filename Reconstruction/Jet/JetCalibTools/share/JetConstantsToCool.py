import sys

# Set up minimal environment, no event loop, no event selector
theApp.EvtSel = "NONE"
theApp.EvtMax=1

if not 'write_tool' in dir():
   write_tool="EtaMassEnergyCorr"

if write_tool=="EtaMassEnergyCorr" and not 'applyMassCorr' in dir():
   applyMassCorr=False

if write_tool=="OffsetCorrection" and not 'dataType' in dir():
   dataType='MC10B'

if not 'JetDB' in dir():
   JetDB="sqlite://;schema=/afs/cern.ch/user/p/pavol/scratch0/JetCalib/cooldummy.db;dbname=OFLP200"

if not 'isMC' in dir():
   isMC=True

if not 'isInline' in dir():
   isInline=True

if not 'pool_file' in dir():
   pool_file="dummy.pool.root"

include ("RegistrationServices/IOVRegistrationSvc_jobOptions.py")
if not 'IOVMin' in dir():
   IOVMin=IOVRunNumberMin

if not 'IOVMax' in dir():
   IOVMax=IOVRunNumberMax
if not 'tag_suffix' in dir():
   tag_suffix="-test"

from JetCalibTools.JetCalibToolsConf import JetCalibCorrDBWriter
from JetCalibTools.DBUtils import *

from JetRec.JetRecFlags import jetFlags
jetFlags.JetCalibDB.set_Value_and_Lock(JetDB)

if 'JetDBFolderBase' in dir():
  jetFlags.JetCalibDBFolderBase.set_Value_and_Lock(JetDBFolderBase)

corr_tool_list = []


if write_tool=="EtaMassEnergyCorr":
   from JetCalibTools.JetCalibToolsConf import EtaMassEnergyCorr
   import JetCalibTools.EtaMassEnergyFactors as EtaMassEnergyFactors
   
   # loop over all constants there
   # there is couple of them in input files
   etaCorr = [ [ ] ] 
   jesCorr = [ [ ] ]
   massCorr   = [ [ ] ]
   for jetname in EtaMassEnergyFactors.energyCorrDict.keys():

       print "Configuring for jetname: ",jetname
       mcitool = EtaMassEnergyCorr("EtaMassEnergyCorr_"+getNameFromJetname(jetname))
       jesCorr = EtaMassEnergyFactors.energyCorrDict[jetname]
       etaCorr = EtaMassEnergyFactors.etaCorrDict[jetname]
       if applyMassCorr:
          massCorr =  EtaMassEnergyFactors.pTCorrDict[jetname]
          mcitool.ApplyMassCorrection = applyMassCorr

       mcitool.JES_Factors = str(jesCorr)
       mcitool.EtaCorr_Factors = str(etaCorr)
       mcitool.MassCorr_Factors = str(massCorr)
       mcitool.order = 0
       mcitool.isDummy = False
       # Eta bins for EtaMassEnergy correction - 90 of them 
       # should be part of the constants in DB !!!! 
       etaBins=[] 
       if len(EtaMassEnergyFactors.energyCorrDict[jetname]) == 90:
          print "90 etaBins...."
          for i in range(-44, 46): 
             etaBins.append(i*0.1) 
       elif len(EtaMassEnergyFactors.energyCorrDict[jetname]) == 50:
          print "50 etaBins...."
          for i in range(0,5):
             etaBins.append(-4.1+i*0.3)
          for i in range(0,7):   
             etaBins.append(-2.7+i*0.2)
          for i in range(0,3):   
             etaBins.append(-1.4+i*0.2)
          for i in range(0,20):   
             etaBins.append(-0.9+i*0.1)
          for i in range(0,2):   
             etaBins.append(1.2+i*0.2)
          for i in range(0,8):   
             etaBins.append(1.5+i*0.2)
          for i in range(0,5):
             etaBins.append(3.2+i*0.3)
             
       #mcitool.EtaBins          = [] # clear first to fix imcompatibility error 
       mystr='['
       for i in range(0,len(EtaMassEnergyFactors.energyCorrDict[jetname])-1):
          mystr += "%.2f" % etaBins[i]+', '      

       mystr += "%.2f" % etaBins[len(EtaMassEnergyFactors.energyCorrDict[jetname])-1]+']'
       #mcitool.EtaBins          = str(etaBins)
       mcitool.EtaBins          = mystr
   
       mcitool.DBstring = getNameFromJetname(jetname)

       print mcitool
   
       corr_tool_list += [mcitool]
       ToolSvc += mcitool

elif write_tool=="OffsetCorrection":
   #from JetCalibTools.TowerOffsetConstants import retrieveTowerConstants
   #from JetCalibTools.TruthOffsetConstants import retrieveTruthConstants
   from JetCalibTools import TowerOffsetConstants
   from JetCalibTools import TruthOffsetConstants

   from JetCalibTools.JetCalibToolsConf import OffsetCorrection

   #finder="AntiKt"
   #mainParam=0.4
   #input="Tower"

   from AthenaCommon.GlobalFlags import globalflags
   globalflags.DataSource.set_Value_and_Lock("data")

   #from AthenaCommon.AppMgr import ToolSvc
   #from LumiBlockComps.LumiBlockCompsConf import LumiBlockMuTool
   #theLBMTool = LumiBlockMuTool( "LumiBlockMuTool" )
   #ToolSvc += theLBMTool

   from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
   svc = SetupTrigConfigSvc()
   svc.SetStates( 'ds' )
   svc.InitialiseSvc()

   from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
   theBCTool = BunchCrossingTool()
   ToolSvc += theBCTool

   #finder="AntiKt"
   #mainParam=0.4
   #input="Tower"

   listDone=[]
   for jetname in TruthOffsetConstants.offsetDict.keys():
   #for mainParam in [0.4,0.6]:
      #jetname=getConstantsName(0,finder,mainParam,input,isMC)
      print "Configuring for: ",jetname
      if getNameFromJetname(jetname) in listDone:
         continue
      listDone += [getNameFromJetname(jetname)]   
      if not "Tower" in jetname:
         isMC=False
      offsetTool = OffsetCorrection("OffsetCorrection_"+getNameFromJetname(jetname,isMC))
      #offsetTool = OffsetCorrection("OffsetCorrection_"+getConstantsName(0,finder,mainParam,input,isMC))
      finder,mainParam,input,calibType = getNameFromJetname(jetname,isMC,True)
      print "Found finder: "+finder+". mainParam: "+str(mainParam)+", input: "+input

      towerConstantsOOT, towerConstantsIT, towerConstantsOOT50  = TowerOffsetConstants.retrieveTowerConstants(finder, mainParam, input, isMC)
      truthConstantsOOT, truthConstantsIT  = TruthOffsetConstants.retrieveTruthConstants(finder, mainParam, input, calibType, isMC, dataType)

      numTowerMoment = "" if "Tower" in input else "NumTowers"

      offsetTool.CalibrationTag        = "Offset"            # String specifying JetMoment name
      offsetTool.VertexContainerKey    = "VxPrimaryCandidate" # StoreGate key for the PV container
      offsetTool.MinimumPVTracks       = 2                    # Min. number of tracks for counting PV's                                     
 
      offsetTool.TowerBasedConstantsOOT = str(towerConstantsOOT)
      offsetTool.TowerBasedConstantsIT = str(towerConstantsIT)
      offsetTool.TowerBasedConstantsOOT50 = str(towerConstantsOOT50)
 
      offsetTool.TruthBasedConstantsOOT = str(truthConstantsOOT)
      offsetTool.TruthBasedConstantsIT = str(truthConstantsIT)
 
      #offsetTool.LumiBlockMuTool = theLBMTool
      offsetTool.BCTool = theBCTool
      offsetTool.WriteMoments   = True
      # New parameters for the 2011 offsets
      offsetTool.R              = mainParam
      offsetTool.DoTruthOffset    = False  # should probably not use this for now, since it is only good for AntiKt6 topo EM jets. 
      offsetTool.DoTowerOffset    = True
      offsetTool.IsMC           = isMC
      offsetTool.UseNumTowers  = True
      offsetTool.NumTowersMoment = numTowerMoment
 
      offsetTool.DBstring = getConstantsName(offsetTool,finder,mainParam,input,isMC)
      offsetTool.order = 0
      offsetTool.isDummy = False
      
      corr_tool_list += [offsetTool]
      ToolSvc += offsetTool
 
      print offsetTool

elif write_tool=="JetPropertyInvertResponseTool":
   # try if we could rename the tool after configuring
   from JetCalibTools.SetupCaloFractionCalibrators_fromjO import *
   from JetCalibTools.SetupWidthCalibrators_fromjO import *
   from JetCalibTools.JetCalibToolsConf import JetPropertyInvertResponseTool

   l_finder=["AntiKt"]
   l_mparam=[4,6]
   l_input=["Topo", "TopoTower"]
   l_tools=[doTile1Fraction, doHec0Fraction, doFcal1Fraction, doPresamplerFraction, doEm2FractionTransition, doEm2FractionSmallTransition, doEm2FractionEndcap, doEm3FractionBarrel, doEm3FractionEndcap, doEm3FractionForward, doWidth]
   for calibrator in l_tools:
      for finder in l_finder:
         for mparam in l_mparam:
            for input in l_input:
            
               mytool=calibrator(finder,mparam,input)
               mytool1=mytool.clone(mytool.name()+"_"+getConstantsName(0,finder,mparam,input))

               mytool1.DBstring = mytool.name()+"_"+getConstantsName(0,finder,mparam,input)
               mytool1.order = 0
               mytool1.isDummy = False
    
               corr_tool_list += [mytool1]
               ToolSvc += mytool1
               #print mytool1

elif write_tool=="TrackCorr":
    from JetCalibTools.JetCalibToolsConf import TrackBasedResponseCorrection
    from JetCalibTools.TrackBasedConstants import TrackFitDict     # Calibration Constants
    from JetRec.JetGetterBase import editParm

#    jetname =  finder + editParm(mainParam) + input + "Jets"
    jetname = "AntiKt4TopoJets" # only available constants so far

    nTrackFit=[]

    if jetname in TrackFitDict:
        nTrackFit = TrackFitDict[jetname]
    else:
        print "WARNING: Jet algorithm %s is not supported in the current track-based response correction" % jetname

    # Eta Bins
    etaBins = [0.8,1.4]

    # Setup the tool
    TrackCorrTool = TrackBasedResponseCorrection("TrackBasedResponseCorrection_"+getNameFromJetname(jetname))
    TrackCorrTool.nTrackFit = str(nTrackFit)
    TrackCorrTool.EtaBins = str(etaBins)
    TrackCorrTool.CalibrationTag = "TrackCorr"
    TrackCorrTool.order = 0
    TrackCorrTool.isDummy = False
    TrackCorrTool.DBstring = getNameFromJetname(jetname)

    print TrackCorrTool
   
    corr_tool_list += [TrackCorrTool]
    ToolSvc += TrackCorrTool

elif write_tool=="TrackAngularCorr":
      
    from JetCalibTools.JetCalibToolsConf import TrackAngularCorrection
    from JetRec.JetGetterBase import editParm

    jetname = "AntiKt4TrackJets"

    trackangulartool = TrackAngularCorrection("TrackAngularCorrection_"+getNameFromJetname(jetname))
    trackangulartool.CalibrationTag = "TrackAngularCorrection"
    
    # Track jet collection and delta R
    trackangulartool.DeltaR = 0.4
    trackangulartool.TrackJetCollectionKey = jetname

    # this guy had constants inside the SetupJetCalibrators.py, putted them in separate temp file and read out from there
    import JetCalibTools.TrackAngularConstants as TAC
    trackangulartool.CaloJetEtaBins = str(TAC.CaloEtaArray)
    trackangulartool.TrackJetEtaBins = str(TAC.TrackEtaArray)
    trackangulartool.CaloJetPtBins = str(TAC.CaloPtArray)
    trackangulartool.TrackJetPtBins = str(TAC.TrackPtArray)
    
    trackangulartool.CaloJetEtaConstants = TAC.calo_eta_constants
    trackangulartool.CaloJetPhiConstants = TAC.calo_phi_constants
    trackangulartool.TrackJetEtaConstants = TAC.track_eta_constants
    trackangulartool.TrackJetPhiConstants = TAC.track_phi_constants

    trackangulartool.order = 0
    trackangulartool.isDummy = False
    trackangulartool.DBstring = getNameFromJetname(jetname)

    print trackangulartool
   
    corr_tool_list += [trackangulartool]
    ToolSvc += trackangulartool

elif write_tool=="MCNumInvCorr":

   from JetCalibTools.JetCalibToolsConf import MCNumInversionResponse
   import JetCalibTools.MCNumInvConstants as MCNumInvConstants

   # constants from jO
   for jetname in MCNumInvConstants.etaFitDict.keys():

       print "Configuring for jetname: ",jetname
       mcitool = MCNumInversionResponse("MCNumInversionResponse_"+getNameFromJetname(jetname))
       etaFit = MCNumInvConstants.etaFitDict[jetname]
       mcitool.etaFit = str(etaFit)
       mcitool.order = 0
       mcitool.isDummy = False
       # Eta bins for EtaMassEnergy correction - 90 of them 
       # should be part of the constants in DB !!!! 
       etaBins=[] 
       for i in range(1, 45): 
          etaBins.append(i*0.1) 
             
       #mcitool.EtaBins          = [] # clear first to fix imcompatibility error 
       mcitool.EtaBins          = str(etaBins)
   
       mcitool.DBstring = getNameFromJetname(jetname)

       print mcitool
   
       corr_tool_list += [mcitool]
       ToolSvc += mcitool

   # constants from old db
   import JetCalibTools.DBConfig as DBConfig    
   for jetname in MCNumInvConstants.existingKeysInDB:

       print "Configuring for jetname: ",jetname
       mcitool = MCNumInversionResponse("MCNumInversionResponse_"+getNameFromJetname(jetname))
       dbkey = DBConfig.loadJetFolder( jetname  )
       mcitool.detStoreKey = dbkey
       mcitool.order = 0
       mcitool.isDummy = False
       # Eta bins for EtaMassEnergy correction - 90 of them 
       # should be part of the constants in DB !!!! 
       etaBins=[] 
       for i in range(1, 45): 
          etaBins.append(i*0.1) 
             
       #mcitool.EtaBins          = [] # clear first to fix imcompatibility error 
       mcitool.EtaBins          = str(etaBins)
   
       mcitool.DBstring = getNameFromJetname(jetname)

       print mcitool
   
       corr_tool_list += [mcitool]
       ToolSvc += mcitool
else:
   print 'Wrong tool, do not writing anything....'
   sys.exit(1)

print "Size of corr_tool_list: ",len(corr_tool_list)    
if isInline:
   corr_output_list=["CondAttrListCollection#"+getFolder(corr_tool_list[0])]
   tag_list=[getFolderTag(corr_tool_list[0])+tag_suffix]
else:   
   corr_output_list=[]
   tag_list=[]
   for tool in corr_tool_list:
       corr_output_list += ["CaloRec::ToolConstants#"+tool.name()+"#"+getFolder(tool)]
       tag_list += [getFolderTag(tool)+tag_suffix]


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

jetCorrDBWrite = JetCalibCorrDBWriter()
jetCorrDBWrite.JetCalibToolNames = [t.getFullName() for t in corr_tool_list]
jetCorrDBWrite.JetCalibTools = corr_tool_list
if isInline:
   jetCorrDBWrite.COOLInlineFolder = getFolder(corr_tool_list[0])
topSequence += jetCorrDBWrite

print topSequence.JetCalibCorrDBWriter
# include the basic setup for the conditions output stream
#pool_file = CALOCORR_POOLFILE
#if pool_file.startswith ('LFN:'):
#    pool_file = pool_file[4:] + '.pool.root'
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg = OutputConditionsAlg (outputFile = pool_file,
                                           ObjectList = corr_output_list,
                                           IOVTagList = tag_list,
                                           WriteIOV = True)

#OutputConditionsAlg.OutputLevel=DEBUG
#ToolSvc.ConditionsAlgStream.OutputLevel=DEBUG


from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.DetectorStore.Dump = True

try:
    ll = theApp.OutStream
    ll.remove ('AthenaOutputStream/Stream1')
    theApp.OutStream = ll
except ValueError:
    pass

# Now to handle the cool registration.
# Set flag to register and run interval Run1/LB1 to Run2/LB2
#   Usually, only need to set Run1, others go to default
OutputConditionsAlg.WriteIOV   = True
#OutputConditionsAlg.Run1       = IOVMin 
OutputConditionsAlg.Run1       = 1 
OutputConditionsAlg.LB1        = IOVLBNumberMin
OutputConditionsAlg.Run2       = IOVMax
OutputConditionsAlg.LB2        = IOVLBNumberMax


from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.writeKeyInfo = False
#svcMgr.IOVRegistrationSvc.OutputLevel = INFO

if write_tool=="JetPropertyInvertResponseTool":
   # overwrite the cool type
   svcMgr.IOVRegistrationSvc.OverrideNames += ["Constants"]
   svcMgr.IOVRegistrationSvc.OverrideTypes += [ "Blob16M" ]

   svcMgr.IOVRegistrationSvc.PayloadTable = True
   
if isMC:
   from IOVDbSvc.CondDB import conddb
   conddb.blockFolder('/Digitization/Parameters')


# Set the connection string
import IOVDbSvc.IOVDb

#svcMgr.IOVRegSvc.RecreateFolders = True
#svcMgr.IOVDbSvc.OutputLevel=DEBUG
svcMgr.IOVDbSvc.dbConnection = JetDB

svcMgr.MessageSvc.OutputLevel = DEBUG

