import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec

from glob import glob
filelist = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.AOD.e5458_s3126_r9364_r9315/AOD.11182705._000001.pool.root.1"]

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = filelist
#athenaCommonFlags.FilesInput = [Inputfile]
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

############################################################################
# Set up detector description for cell access

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

from RecExConfig.InputFilePeeker import inputFileSummary
#print inputFileSummary
if inputFileSummary['evt_type'][0] == 'IS_DATA':
    globalflags.DataSource = 'data'
else:
    globalflags.DataSource = 'geant4'

globalflags.DetDescrVersion = inputFileSummary['geometry']

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
#DetFlags.detdescr.Calo_setOn()
if hasattr(DetFlags,'BField_on'): DetFlags.BField_setOn()
include('RecExCond/AllDet_detDescr.py')
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from GaudiSequencer.PyComps import PyEvtFilter
#masterSeq = CfgMgr.AthSequencer("AthMasterSeq")
#the following lines are examples, pick one...
# masterSeq += PyEvtFilter("PVSoftTrkTail", evt_list=[
#         # 106239409,
#         # 103677144,
#         # 210091212,
#         # 647377331,
#         # 649083254,
#         # 679943194,
#         # 676957592,
#         # 931088975,
#         # 930867220,
#         # 932105457,
#         # 932761543,
#         # 500249532,
#         # 498552012,
#         # 8093981,
#         # 7747623,
#         # 9713934,
#         ])
#topSequence += masterSeq

############################################################################
# Set up an extra associator for testing
from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METAssocConfig import AssocConfig, METAssocConfig
JetType = 'EMJet'

modConstKey = ""
modClusColls = {}
if metFlags.UseTracks():
	modConstKey="OriginCorr"
	modClusColls={
	'LCOriginCorrClusters':'LCOriginTopoClusters',
	'EMOriginCorrClusters':'EMOriginTopoClusters'
        }



associators = [AssocConfig(JetType),
               AssocConfig('Muon'),
               AssocConfig('Ele'),
               AssocConfig('Gamma'),
               AssocConfig('Tau'),
               AssocConfig('Soft')]
cfg_akt4em = METAssocConfig('NewAntiKt4EMTopo',
                            associators,
                            doPFlow=False,
                            modConstKey=modConstKey,
			    modClusColls=modClusColls
                            )

metFlags.METAssocConfigs()[cfg_akt4em.suffix] = cfg_akt4em
metFlags.METAssocOutputList().append(cfg_akt4em.suffix)

############################################################################
# Set up an extra associator for testing
JetType = 'PFlowJet'

associators = [AssocConfig(JetType),
               AssocConfig('Muon'),
               AssocConfig('Ele'),
               AssocConfig('Gamma'),
               AssocConfig('Tau'),
               AssocConfig('Soft')]
cfg_akt4pf = METAssocConfig('NewAntiKt4EMPFlow',
                            associators,
                            doPFlow=True,
                            )

metFlags.METAssocConfigs()[cfg_akt4pf.suffix] = cfg_akt4pf
metFlags.METAssocOutputList().append(cfg_akt4pf.suffix)

from METReconstruction.METAssocConfig import getMETAssocAlg

# Get the configuration directly from METRecoFlags
# Can also provide a dict of configurations or list of RecoTools or both
metAlg = getMETAssocAlg('METAssociation')
topSequence += metAlg

from METUtilities.METMakerConfig import getMETMakerAlg
makerAlgEM = getMETMakerAlg("NewAntiKt4EMTopo",jetColl="AntiKt4EMTopoJets")
ToolSvc.METMaker_NewAntiKt4EMTopo.OutputLevel=VERBOSE
ToolSvc.METMaker_NewAntiKt4EMTopo.DoRemoveElecTrks=False
topSequence += makerAlgEM
makerAlgPF = getMETMakerAlg("NewAntiKt4EMPFlow",jetColl="AntiKt4EMPFlowJets")
ToolSvc.METMaker_NewAntiKt4EMPFlow.OutputLevel=VERBOSE
ToolSvc.METMaker_NewAntiKt4EMPFlow.DoRemoveElecTrks=False
topSequence += makerAlgPF

# masterSeq += CfgMgr.met__METAssocTestAlg("TestMETAssocEMTopo",
#                                          OutputLevel=VERBOSE,
#                                          FailOnInconsistency=True,
#                                          METMapSuffix="NewAntiKt4EMTopo")
# masterSeq += CfgMgr.met__METAssocTestAlg("TestMETAssocEMPFlow",
#                                          OutputLevel=VERBOSE,
#                                          FailOnInconsistency=True,
#                                          METMapSuffix="NewAntiKt4EMPFlow")

write_xAOD = True
if write_xAOD:

    # The list of output containers/maps is autogenerated and stored in metFlags
    # This jO extracts them with the appropriate formatting
    from AthenaCommon.Resilience import protectedInclude
    protectedInclude("METReconstruction/METReconstructionOutputAODList_jobOptions.py")

    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD3.pool.root" )
    for item in MissingETAODList:
        xaodStream.AddItem(item)

    xaodStream.AddItem('xAOD::MissingETContainer#MET_Reference_Anti*')
    xaodStream.AddItem('xAOD::MissingETAuxContainer#MET_Reference_Anti*Aux.')

    xaodStream.AddItem('xAOD::TrackParticleContainer#InDetTrackParticles*')
    xaodStream.AddItem('xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.')

    # xaodStream.AddAcceptAlgs( "PVSoftTrkTail" )
theApp.EvtMax = 10
ServiceMgr.EventSelector.SkipEvents = 0
ServiceMgr.MessageSvc.defaultLimit = 9999
