from HIJetRec.HIJetRecFlags import HIJetFlags
from HIGlobal.HIGlobalFlags import jobproperties
from JetRec.JetRecFlags import jetFlags
from HIJetRec.HIJetRecConfig import *
jetFlags.useCells.set_Value(True);


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from CaloRec.CaloTowerCmbGetter import CaloTowerCmbGetter
CaloTowerCmbGetter()


#check for event shape
#if hasattr(topSequence,"HIEventShapeAlgorithm")
EventShapeKey=jobproperties.HIGlobalFlags.EventShapeKey()
ClusterKey=HIJetFlags.HIClusterKey()

#import utility functions
from  HIJetRec.HIJetRecUtils import *
HIClusterGetter()


#equip basic tools
from  HIJetRec.HIJetRecTools import jtm

#------------------------------ 
#set flags, keys and configure new event shape algorithm
#>check if is MC or overlay and configure calibration tool properly
from JetRec.JetRecFlags import jetFlags
is_mc_or_overlay=False
is_pp_mc=False
if not jetFlags.useTruth():
    if globalflags.DataSource()=='geant4' : 
        is_mc_or_overlay=True
        #Check if it is pp (not HIJING)
        if rec.doHIP(): is_pp_mc=True
jetFlags.useTruth.set_Value_and_Lock(is_mc_or_overlay)


#Tower level subtraction
HIJetFlags.DoCellBasedSubtraction.set_Value_and_Lock(False)
#Use tracks
jetFlags.useTracks.set_Value_and_Lock(True)
#HIP mode
if rec.doHIP() : SetHIPMode()

theSubtrTool=jtm.HIJetClusterSubtractor
theSubtrTool.unlock()
theSubtrTool.UseSamplings=False
theSubtrTool.lock()

if is_mc_or_overlay :
    for theCalibTool in jtm.HICalibMap.values() :
        theCalibTool.unlock()
        theCalibTool.IsData=False
        theCalibTool.CalibSequence='EtaJES'
        theCalibTool.lock()

if not HIJetFlags.DoCellBasedSubtraction():
    #Make new event shape at tower level
    from HIGlobal.HIGlobalConf import HIEventShapeMaker
    from HIGlobal.HIGlobalConf import HIEventShapeFillerTool
    #EventShapeKey set to point to weighted container, can remove code on L16
    EventShapeKey=jobproperties.HIGlobalFlags.EventShapeKey()+'Weighted'
    ESAlg_W=HIEventShapeMaker("ESAlg_W")
    ESAlg_W.OutputContainerKey=EventShapeKey
    ESAlg_W.UseCaloCell=False
    ESAlg_W.InputTowerKey="HIClusters"

    #Hack needed because ES algorithm requires a summary tool, this disables it
    from HIEventUtils.HIEventUtilsConf import HIEventShapeSummaryTool
    SummaryTool=HIEventShapeSummaryTool("SummaryTool2")
    SummaryTool.SubCalos=jobproperties.HIGlobalFlags.SummarySubCalos()
    ESAlg_W.SummaryTool=SummaryTool
    ESAlg_W.SummaryContainerKey=""

    #Add filler tool
    from HIGlobal.HIGlobalConf import HIEventShapeFillerTool
    ESFiller=HIEventShapeFillerTool("WeightedFiller")
    ESFiller.UseClusters=True

    #Add weight tool to filler tool
    from HIEventUtils.HIEventUtilsConf import HITowerWeightTool
    TWTool=HITowerWeightTool()
    TWTool.ApplyCorrection=HIJetFlags.ApplyTowerEtaPhiCorrection()
    TWTool.ConfigDir='HIJetCorrection/'
    TWTool.InputFile=HIJetFlags.TWConfigFile()
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += HITowerWeightTool()
    ESFiller.TowerWeightTool=TWTool

    #Add to top sequence
    ESAlg_W.HIEventShapeFillerTool=ESFiller
    topSequence += ESAlg_W

#------------------------------------------------------------

#truth, track and unsubtracted jet setup:
if jetFlags.useTruth(): 
    for R in HIJetFlags.AntiKtRValues() : 
        tname="AntiKt%dTruthJets" % int(10*R)
        collExists=False
        if tname in jtm.tools : continue
        if rec.readESD() :
            from RecExConfig.ObjKeyStore import objKeyStore
            inputcontent = objKeyStore['inputFile'].list()
            for t in inputcontent :
                if tname in t:
                    print 'Truth collection %s already exists, no need to rebuild it' % tname
                    collExists=True
                    break
        if collExists: continue
        f=jtm.addJetFinder(tname,"AntiKt", R,"truth", ptmin= HIJetFlags.TruthJetPtMin())
        print 'Adding %s' %tname
        AddToOutputList(tname)
        jtm.HIJetRecs+=[f]

#track jets
if jetFlags.useTracks():
    tname="AntiKt4HITrackJets"
    hi_trk_modifiers=jtm.modifiersMap["HITrack"]
    hi_trk_modifiers += AddPtAssociationTools(0.4,False)
    f=jtm.addJetFinder(tname, "AntiKt", 0.4, "HITrack", hi_trk_modifiers, ptmin= HIJetFlags.TrackJetPtMin())
    HIJetFlags.TrackJetContainerName=tname
    AddToOutputList(tname)
    jtm.HIJetRecs+=[f]


#initial unsubtracted jets
for R in HIJetFlags.AntiKtRValues() :  AddHIJetFinder(R)

#initial seeds
if HIJetFlags.SeedRValue() not in HIJetFlags.AntiKtRValues() : AddHIJetFinder(HIJetFlags.SeedRValue())
seed_prefix='AntiKt%dHIJets' % int(10*HIJetFlags.SeedRValue())
seeds0=jtm.addJetCopier("%s_%s0" % (seed_prefix, HIJetFlags.SeedSuffix()),"%s_Unsubtracted" % seed_prefix,[jtm.discrim],shallow=False)
jtm.HIJetRecs+=[seeds0]

#code nearly identical, but new behavior since upstream ES container and package flags are different
iter0=AddIteration(seed_container=seeds0.OutputContainer,shape_name=EventShapeKey,suffix="iter0")
modulator0=iter0.Modulator
subtr1=MakeSubtractionTool(iter0.OutputEventShapeKey,modulator=modulator0)

#now iterate
seeds1=jtm.addJetCopier("%s_%s1" % (seed_prefix,HIJetFlags.SeedSuffix()),"%s_Unsubtracted" % seed_prefix,[subtr1,jtm.HICalibMap[seed_prefix],jtm.jetfilHISeeds],shallow=False)
jtm.HIJetRecs+=[seeds1]
iteration_dict=dict(suffix="iter1")
if jetFlags.useTracks() and HIJetFlags.TrackJetSeeds() : iteration_dict['track_jet_seeds']=HIJetFlags.TrackJetContainerName()
iter1=AddIteration(seed_container=seeds1.OutputContainer,shape_name=EventShapeKey,**iteration_dict)

HIJetFlags.IteratedEventShapeKey=iter1.OutputEventShapeKey
modulator1=iter1.Modulator
jtm.modulator=modulator1

#subtraction BEFORE iteration for moment
subtr1=MakeSubtractionTool(iter0.OutputEventShapeKey,moment_name="NoIteration",momentOnly=True,modulator=modulator0)
#main subtractor
subtr2=MakeSubtractionTool(HIJetFlags.IteratedEventShapeKey(),modulator=modulator1)

#put subtraction tool at the FRONT of the jet modifiers list
hi_tools=[subtr1,subtr2]
hi_tools+=GetFlowMomentTools(iter1.OutputEventShapeKey,iter1.ModulationEventShapeKey)


#==========#==========#==========#==========#==========#==========
#special addition for egamma
#Downstream egamma jo will call SubtractedCellGetter, it assumes that the container pointed to by 
#HIJetFlags.IteratedEventShapeKey() is defined per layer/per eta bin, which is no longer true w/ above changes
#The fix is to define one more container from the seeds above just as in original reconstruction
if not HIJetFlags.DoCellBasedSubtraction():
    iteration_dict=dict(suffix="iter_egamma")
    iter_egamma=AddIteration(seed_container=seeds1.OutputContainer,shape_name=jobproperties.HIGlobalFlags.EventShapeKey(),useClusters=False,**iteration_dict)
    cell_level_shape_key=iter_egamma.OutputEventShapeKey
    #HIJetFlags.IteratedEventShapeKey=iter_egamma.OutputEventShapeKey

#Subtraction for egamma and to get layers
ApplySubtractionToClusters(name="HIClusterSubtraction_egamma", event_shape_key=cell_level_shape_key, cluster_key=ClusterKey, modulator=modulator1, CalculateMoments=True, useClusters=False)
#Cluster subtraction for jets
ApplySubtractionToClusters(event_shape_key=HIJetFlags.IteratedEventShapeKey(), update_only=True, cluster_key=ClusterKey, modulator=modulator1, CalculateMoments=False, useClusters=True)
###
#subtracted algorithms
#make main jets from unsubtr collections w/ same R, add modifiers for subtraction
unsubtr_suffix=HIJetFlags.UnsubtractedSuffix()
for k in jtm.jetrecs :
    if unsubtr_suffix in k.name() :
        in_name=k.OutputContainer
        out_name=in_name.replace("_%s" % unsubtr_suffix,"")
        #>slight tweak in case R=1.0 jets are requestd, add some substructure tools
        modifiers=GetHIModifierList(out_name,hi_tools)
        if '10HIJets' in k.name() : 
            from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtDeltaRTool
            jtm += KtDeltaRTool('ktdr10',JetRadius =1.0)
            modifiers+=[jtm.ktdr10, jtm.ktsplitter, jtm.pull, jtm.angularity, jtm.planarflow,jtm.ktmassdrop]
        copier=jtm.addJetCopier(out_name,in_name,modifiers,shallow=False)
        AddToOutputList(out_name)
        jtm.HIJetRecs+=[copier]

AppendOutputList(jetFlags.jetAODList)
JetAlgFromTools(jtm.HIJetRecs,suffix="HI",persistify=True)

HIJetFlags.IteratedEventShapeKey=cell_level_shape_key

# code cloned from BTagging_jobOptions.py
# to allow b-tagging over HI jets

if HIJetFlags.DoHIBTagging():

  if not BTaggingFlags.DoNotSetupBTagging: # Temporary measure so the JetRec people can test setting this all up from their side.
    #
    # ========== Load and configure everything
    #

    from BTagging.BTaggingConfiguration import getConfiguration
    ConfInstance = getConfiguration()

    if ConfInstance.checkFlagsUsingBTaggingFlags():

      #Jet collections
      JetCollectionList = ['AntiKt4HIJets']
      from JetRec.JetRecFlags import jetFlags

      BTaggingFlags.Jets = [ name[:-4] for name in JetCollectionList]

      #BTagging list
      btag = ConfInstance.getOutputFilesPrefix() #BTaggingFlags.OutputFilesBTag #"BTagging_"

      #TODO define name author (now BTagging_AntiKt4LCTopo)
      AuthorSubString = [ btag+name[:-4] for name in JetCollectionList]

      NotInJetToolManager = [] # For jet collections
      from JetRec.JetRecStandard import jtm
      for i, jet in enumerate(JetCollectionList):
          try:
            btagger = ConfInstance.setupJetBTaggerTool(ToolSvc, jet) #The [:-4] is not needed here; this function automatically removes trailing 'jets' or 'Jets'.
            if btagger is None:
              continue
            jet = jet.replace("Track", "PV0Track")
            jetname = getattr(jtm, jet)
            jetname.unlock()
            jetname.JetModifiers += [ btagger ]
            jetname.lock()
            if BTaggingFlags.OutputLevel < 3:
              print ConfInstance.getJetCollectionTool(jet[:-4])
          except AttributeError as error:
            print '#BTAG# --> ' + str(error)
            NotInJetToolManager.append(AuthorSubString[i])

      if len(NotInJetToolManager) > 0:
          AuthorSubString = list(set(AuthorSubString) - set(NotInJetToolManager))
