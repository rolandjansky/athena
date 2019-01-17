print 'In HIJetRec_AODFix_r207.py'

##
#check if is MC or overlay and configure calibration tool properly
from JetRec.JetRecFlags import jetFlags
from HIJetRec.HIJetRecFlags import HIJetFlags
is_mc_or_overlay=False
is_pp_mc=False
if not jetFlags.useTruth():
	if globalflags.DataSource()=='geant4' : 
		is_mc_or_overlay=True
		#Check if it is pp (not HIJING)
		if rec.doHIP(): is_pp_mc=True
	else :
		from PyUtils import AthFile
		af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
		containers=af.fileinfos['eventdata_items']
		for c in containers: 
			if c[0] is not None and 'Truth' in c[0] : 
				is_mc_or_overlay=True
				break

jetFlags.useTruth.set_Value_and_Lock(is_mc_or_overlay)
	
###

#add new event shape: 100 bins, no layers, demodulated by tower weights
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from HIGlobal.HIGlobalFlags import jobproperties
from HIGlobal.HIGlobalConf import HIEventShapeMaker
from HIGlobal.HIGlobalConf import HIEventShapeFillerTool

ESAlg_W=HIEventShapeMaker("ESAlg_W")
ESAlg_W.OutputContainerKey="EventShapeWeighted"
ESAlg_W.UseCaloCell=False
ESAlg_W.InputTowerKey="HIClusters"

###the following is a workaround
#only necessary because the HIEventShapeMaker algorithm requires a summary tool
#it doesn't use the tool if SummaryContainerKey=="" 
from HIEventUtils.HIEventUtilsConf import HIEventShapeSummaryTool
SummaryTool=HIEventShapeSummaryTool("SummaryTool2")
SummaryTool.SubCalos=jobproperties.HIGlobalFlags.SummarySubCalos()
ESAlg_W.SummaryTool=SummaryTool
ESAlg_W.SummaryContainerKey=""
#####

EventShapeKey="EventShapeWeighted"

from HIGlobal.HIGlobalConf import HIEventShapeFillerTool
ESFiller=HIEventShapeFillerTool("WeightedFiller")
ESFiller.UseClusters=True

from HIEventUtils.HIEventUtilsConf import HITowerWeightTool
TWTool=HITowerWeightTool()
TWTool.ApplyCorrection=HIJetFlags.ApplyTowerEtaPhiCorrection()
TWTool.ConfigDir='HIJetCorrection/'
TWTool.InputFile=HIJetFlags.TWConfigFile()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += HITowerWeightTool()
ESFiller.TowerWeightTool=TWTool
ESAlg_W.HIEventShapeFillerTool=ESFiller
topSequence += ESAlg_W


###configure the package flags for re-running
import AthenaCommon.SystemOfUnits as Units
from HIJetRec.HIJetRecFlags import HIJetFlags
from JetRec.JetRecFlags import jetFlags
from HIJetRec.HIJetRecConfig import *

jetFlags.useTracks.set_Value_and_Lock(True)
HIJetFlags.DoCellBasedSubtraction.set_Value_and_Lock(False)
if rec.doHIP() : SetHIPMode()

##call tools as in standard reco
from HIJetRec.HIJetRecTools import jtm
from HIJetRec.HIJetRecUtils import *

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

#use existing R=0.2 jets from previous reco, only making use of discriminant
seed_prefix='AntiKt%dHIJets' % int(10*HIJetFlags.SeedRValue())
seeds0=jtm.addJetCopier("%s_%s0" % (seed_prefix, HIJetFlags.SeedSuffix()),seed_prefix,[jtm.discrim],shallow=False)
jtm.HIJetRecs+=[seeds0]
iter0=AddIteration(seed_container=seeds0.OutputContainer,shape_name=EventShapeKey,suffix="iter0")
modulator0=iter0.Modulator
subtr1=MakeSubtractionTool(iter0.OutputEventShapeKey,modulator=modulator0)

#now iterate
seeds1=jtm.addJetCopier("%s_%s1" % (seed_prefix,HIJetFlags.SeedSuffix()),seed_prefix,[subtr1,jtm.HICalibMap[seed_prefix],jtm.jetfilHISeeds],shallow=False)
jtm.HIJetRecs+=[seeds1]
iteration_dict=dict(suffix="iter1")
if jetFlags.useTracks() and HIJetFlags.TrackJetSeeds() : iteration_dict['track_jet_seeds']=HIJetFlags.TrackJetContainerName()
iter1=AddIteration(seed_container=seeds1.OutputContainer,shape_name=EventShapeKey,**iteration_dict)

HIJetFlags.IteratedEventShapeKey=iter1.OutputEventShapeKey
modulator1=iter1.Modulator
jtm.modulator=modulator1

from HIJetRec.HIJetRecConf import HIClusterSubtraction
cluster_update=HIClusterSubtraction('HIClusterUpdate')
cluster_update.ClusterKey="HIClusters"
cluster_update.EventShapeKey=HIJetFlags.IteratedEventShapeKey()
cluster_update.Subtractor=jtm.HIJetClusterSubtractor
cluster_update.Modulator=modulator1
cluster_update.UpdateOnly=True
jtm.add(cluster_update)
jtm.jetrecs += [cluster_update]
jtm.HIJetRecs+=[cluster_update]


#subtraction BEFORE iteration for moment
subtr2=MakeSubtractionTool(HIJetFlags.IteratedEventShapeKey(),modulator=modulator1)

###
#subtracted algorithms
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtDeltaRTool
unsubtr_suffix=HIJetFlags.UnsubtractedSuffix()
for R in HIJetFlags.AntiKtRValues() : 
        in_name="AntiKt%sHIJets" % int(10*R) 
        #make main jets from unsubtr collections w/ same R, add modifiers for subtraction
        if R in [0.2,0.3,0.4] :
            copier=jtm.addJetCopier("DF"+in_name,in_name,GetHIModifierList(in_name,[subtr2],[jtm.jetfilHI,jtm.jetsorter]),shallow=False)
            jtm.HIJetRecs+=[copier]
        #make new large R jets, add modifiers for subtraction
        else :
            ktdr_tool = KtDeltaRTool("ktdr%s" % (int(10*R)),JetRadius = R)
            jtm += ktdr_tool
            #Using 0.4 jet calibration 
            largeRmodifier=GetHIModifierList('AntiKt4HIJets',[subtr2],[ktdr_tool, jtm.ktsplitter, jtm.pull, jtm.angularity, jtm.planarflow,jtm.ktmassdrop])
            largeRfinder=jtm.addJetFinder("DF"+in_name, 'AntiKt', R, 'HI',largeRmodifier) 
            jtm.HIJetRecs+=[largeRfinder]

#make missing truth large jet collections
if is_mc_or_overlay:
    for R in HIJetFlags.AntiKtRValues() :
        if not HasCollection("AntiKt%sTruthJets" % int(10*R)):
            truthjetfinder=jtm.addJetFinder("AntiKt%sTruthJets" % int(10*R),'AntiKt',R,'truth')
            jtm.HIJetRecs+=[truthjetfinder]

###
JetAlgFromTools(jtm.HIJetRecs,suffix="HI",persistify=True)
for t in jtm.HIJetRecs : jtm.jetrecs.remove(t)

#==================================
#       BTAGGING
#==================================
if HIJetFlags.DoHIBTagging():
    JetCollectionList = [ 'DFAntiKt4HIJets',]

    from AthenaCommon.AppMgr import ToolSvc
    from ParticleJetTools.ParticleJetToolsConf import JetAssocConstAlg
    from BTagging.BTaggingConfiguration import defaultTrackAssoc, defaultMuonAssoc

    assocalg = \
        JetAssocConstAlg(
            "HIBTaggingParticleAssocAlg",
            JetCollections=JetCollectionList,
            Associators=[defaultTrackAssoc, defaultMuonAssoc]
        )

    topSequence += assocalg

    SA = 'HIAODFix_'
    from BTagging.BTaggingConfiguration import getConfiguration
    BTagConf = getConfiguration("AODFix")
    BTagConf.PrefixxAODBaseName(False)
    BTagConf.PrefixVertexFinderxAODBaseName(False)

    BTagConf.doNotCheckForTaggerObstacles()
    from BTagging.BTaggingConf import Analysis__StandAloneJetBTaggerAlg as StandAloneJetBTaggerAlg

    btag = "BTagging_"
    BTaggingFlags.CalibrationChannelAliases += [ "DFAntiKt4HI->AntiKt4EMTopo" ]
    AuthorSubString = [ btag+name[:-4] for name in JetCollectionList]
    for i, jet in enumerate(JetCollectionList):
        try:
            btagger = BTagConf.setupJetBTaggerTool(ToolSvc, JetCollection=jet[:-4],
                                                   AddToToolSvc=True, #SetupScheme="Retag",
                                                           TaggerList = ['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2rmu', 'MV2r', 'JetVertexCharge', 'MV2c100', 'MV2cl100' , 'DL1', 'DL1rmu','DL1mu', 'RNNIP'])
            SAbtagger = StandAloneJetBTaggerAlg(name=SA + AuthorSubString[i].lower(),
                                      JetBTaggerTool=btagger,
                                      JetCollectionName = jet,
                                      )

            topSequence += SAbtagger
        except AttributeError as error:
            print '#BTAG# --> ' + str(error)
            print '#BTAG# --> ' + jet
            print '#BTAG# --> ' + AuthorSubString[i]

    # Both standard and aux container must be listed explicitly. For release 19, the container version must be explicit.
    BaseName    = "xAOD::BTaggingContainer_v1#"
    BaseAuxName = "xAOD::BTaggingAuxContainer_v1#"
    #AOD list
    BTaggingFlags.btaggingAODList += [ BaseName + author for author in AuthorSubString]
    BTaggingFlags.btaggingAODList += [ BaseAuxName + author + 'Aux.' for author in AuthorSubString]
    BTaggingFlags.btaggingAODList += [ BaseName + author + 'AOD' for author in AuthorSubString]
    BTaggingFlags.btaggingAODList += [ BaseAuxName + author + 'AODAux.' for author in AuthorSubString]
    #ESD list
    BTaggingFlags.btaggingESDList += [ BaseName + author for author in AuthorSubString]
    BTaggingFlags.btaggingESDList += [ BaseAuxName + author + 'Aux.' for author in AuthorSubString]
