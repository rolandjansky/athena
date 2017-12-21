# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from egammaD3PDMaker.isem_version            import isem_version
from egammaD3PDMaker.PAUGammaAssociation     import PAUGammaAssociation
from D3PDMakerCoreComps.release_version      import release_version
#from egammaD3PDAnalysis.PhotonUserDataConfig import PhotonUserDataConfig
from QcdD3PDMaker.QcdPhotonUserDataConfig import PhotonUserDataConfig
from EventCommonD3PDMaker.DRAssociation      import DRAssociation
from D3PDMakerCoreComps.D3PDObject           import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation    import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation     import IndexAssociation
from D3PDMakerConfig.D3PDMakerFlags          import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey         import resolveSGKey
from TriggerD3PDMaker.defineTriggerBits      import defineTriggerBits
from AthenaCommon.AlgSequence                import AlgSequence
from RecExConfig.RecFlags                    import rec
import egammaD3PDMaker
import EventCommonD3PDMaker
import CaloD3PDMaker
import D3PDMakerCoreComps

from ROOT import egammaParameters
from ROOT import egammaPID


PhotonD3PDObject = \
           make_SGDataVector_D3PDObject ('PhotonContainer',
                                         D3PDMakerFlags.PhotonSGKey(),
                                         'ph_', 'QcdPhotonD3PDObject')

def _photonAlgHook (c, prefix, sgkey, typeName,
                    seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
                    *args, **kw):
    PhotonUserDataConfig (seq = seq,
                          prefix = prefix,
                          sgkey = sgkey,
                          typeName = typeName)

    from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import jobproperties
    # set some options here.
    jobproperties.PhotonAnalysisUtilsFlags.DumpNtuple        = False
    jobproperties.PhotonAnalysisUtilsFlags.DumpHistograms    = False
    if hasattr (jobproperties.PhotonAnalysisUtilsFlags, 'DumpTimeDifference'):
        jobproperties.PhotonAnalysisUtilsFlags.DumpTimeDifference = False
    jobproperties.PhotonAnalysisUtilsFlags.AlgSequence       = seq.getName()
    jobproperties.PhotonAnalysisUtilsFlags.DoReRunJetFinding = True
    # probably a few more need to be set

    rlist = release_version()
    if rlist == None:
        # Assume an old version if no version string was stored.
        jobproperties.PhotonAnalysisUtilsFlags.ProductionRelease = 15.0
    elif rlist:
        jobproperties.PhotonAnalysisUtilsFlags.ProductionRelease = \
           rlist[0] + float(rlist[1])/10
    
    #commented out PUU
    #from PhotonAnalysisUtils.PhotonAnalysisUtilsGetter import PhotonAnalysisUtilsGetter
    #pau = PhotonAnalysisUtilsGetter()

    return

PhotonD3PDObject.defineHook (_photonAlgHook)


PhotonD3PDObject.defineBlock (0, 'Kinematics',
                              EventCommonD3PDMaker.FourMomFillerTool,
                              WriteE  = True,
                              WriteEt = True,
                              WriteRect = True)
PhotonD3PDObject.defineBlock (0, 'Author',
                              egammaD3PDMaker.egammaAuthorFillerTool,
                              RecoveredFlag = True)
PhotonD3PDObject.defineBlock (0, 'IsEM',
                              egammaD3PDMaker.egammaIsEMoneFillerTool
                              )
PhotonD3PDObject.defineBlock (0, 'Conversion0',
                              egammaD3PDMaker.egammaConversion0FillerTool,
                              )

#commented out to avoid segmentation error in MCTruthClassifier
#if rec.doTruth():
#    import TruthD3PDMaker.MCTruthClassifierConfig
#    PhotonD3PDObject.defineBlock (1, 'TruthClassification',
#                                  egammaD3PDMaker.egammaTruthClassificationFillerTool)
#    PhotonGenPartAssoc = SimpleAssociation \
#        (PhotonD3PDObject,
#         egammaD3PDMaker.egammaGenParticleAssociationTool,
#         prefix = 'truth_',
#         matched = 'matched',
#         blockname = 'Truth',
#         DRVar = 'deltaRRecPhoton')
#    PhotonGenPartAssoc.defineBlock (0, 'Truth',
#                                    EventCommonD3PDMaker.GenParticleFillerTool,
#                                    WriteE = True,
#                                    WriteM = False)


if isem_version() == 1:
    PhotonD3PDObject.defineBlock (0, 'IsEMCuts',
                                  egammaD3PDMaker.egammaIsEMFillerTool,
                                  IsEM  = [egammaPID.PhotonTightOLDRel,
                                           'tight',
                                           ])
else:
    PhotonD3PDObject.defineBlock (0, 'IsEMCuts',
                                  egammaD3PDMaker.egammaIsEMFillerTool,
                                  IsEM  = [egammaPID.PhotonLoose,
                                           'loose',
                                           egammaPID.PhotonTight,
                                           'tight',
                                           egammaPID.PhotonTightIso,
                                           'tightIso',
                                           ])


PhotonD3PDObject.defineBlock (1, 'HadLeakage',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.ethad,
                                           'Ethad',
                                           egammaParameters.ethad1,
                                           'Ethad1',
                                           ])
PhotonD3PDObject.defineBlock (1, 'Layer0Shape',
                              egammaD3PDMaker.egammaDetailFillerTool,
                              Details = [egammaParameters.e033,
                                         'E033'
                                         ])
PhotonD3PDObject.defineBlock (1, 'Layer1Shape',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.f1,
                                           'f1',
                                           egammaParameters.f1core,
                                           'f1core',
                                           egammaParameters.emins1,
                                           'Emins1',
                                           egammaParameters.fracs1,
                                           'fside',
                                           egammaParameters.e2tsts1,
                                           'Emax2',
                                           egammaParameters.weta1,
                                           'ws3',
                                           egammaParameters.wtots1,
                                           'wstot',
                                           egammaParameters.e132,
                                           'E132',
                                           egammaParameters.e1152,
                                           'E1152',
                                           egammaParameters.emaxs1,
                                           'emaxs1',
                                           ])
PhotonD3PDObject.defineBlock (1, 'Layer1ShapeExtra',
                              egammaD3PDMaker.egammaLayer1ExtraFillerTool)
PhotonD3PDObject.defineBlock (1, 'Layer2Shape',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.e233,
                                           'E233',
                                           egammaParameters.e237,
                                           'E237',
                                           egammaParameters.e277,
                                           'E277',
                                            egammaParameters.weta2,
                                           'weta2',
                                           ])
PhotonD3PDObject.defineBlock (1, 'Layer3Shape',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.f3,
                                           'f3',
                                           egammaParameters.f3core,
                                           'f3core',
                                           ])
PhotonD3PDObject.defineBlock (1, 'Iso',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.r33over37allcalo,
                                           'rphiallcalo',
                                           egammaParameters.etcone,
                                           'Etcone45',
                                           egammaParameters.etcone20,
                                           'Etcone20',
                                           egammaParameters.etcone30,
                                           'Etcone30',
                                           egammaParameters.etcone40,
                                           'Etcone40',
                                           egammaParameters.ptcone30,
                                           'ptcone30',
                                           ])
PhotonD3PDObject.defineBlock (1, 'ConvFlags',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.convAngleMatch,
                                           'convanglematch',
                                           egammaParameters.convTrackMatch,
                                           'convtrackmatch',
                                           ])
PhotonD3PDObject.defineBlock (1, 'Conversion',
                              egammaD3PDMaker.egammaConversionFillerTool)
PhotonD3PDObject.defineBlock (1, 'Retaphi',
                              egammaD3PDMaker.egammaRetaphiFillerTool)
                                           


PhotonD3PDObject.defineBlock (2, 'Rings',
                                egammaD3PDMaker.egammaDetailFillerTool,
    Details = [egammaParameters.etringnoisedR03Sig2,
               'EtringnoisedR03sig2',
               egammaParameters.etringnoisedR03Sig3,
               'EtringnoisedR03sig3',
               egammaParameters.etringnoisedR03Sig4,
               'EtringnoisedR03sig4',
               ])
PhotonD3PDObject.defineBlock (2, 'PhotDiscrim',
                              egammaD3PDMaker.egammaPIDFillerTool,
    PID  = [egammaPID.IsolationLikelihood_jets,
            'isolationlikelihoodjets',
            egammaPID.IsolationLikelihood_HQDelectrons,
            'isolationlikelihoodhqelectrons',
            egammaPID.PhotonWeight,
            'loglikelihood',
            egammaPID.PhotonWeight,
            'photonweight',
            egammaPID.BgPhotonWeight,
            'photonbgweight',
            egammaPID.NeuralNet,
            'neuralnet',
            egammaPID.Hmatrix,
            'Hmatrix',
            egammaPID.Hmatrix5,
            'Hmatrix5',
            egammaPID.AdaBoost,
            'adaboost',
            ])
PhotonD3PDObject.defineBlock (2, 'Pointing',
                                egammaD3PDMaker.egammaDetailFillerTool,
                                Details = [egammaParameters.zvertex,
                                           'zvertex',
                                           egammaParameters.errz,
                                           'errz',
                                           egammaParameters.etap,
                                           'etap',
                                           egammaParameters.depth,
                                           'depth'
                                           ])

PhotonClusterAssoc = SimpleAssociation \
    (PhotonD3PDObject,
     egammaD3PDMaker.egammaClusterAssociationTool)
PhotonClusterAssoc.defineBlock \
    (1, 'ClusterKin', EventCommonD3PDMaker.FourMomFillerTool,
     prefix = 'cl_',
     WriteE = True,
     WriteM = False)
PhotonClusterAssoc.defineBlock \
    (1, 'Samplings', CaloD3PDMaker.ClusterEMSamplingFillerTool)


PhotonRawClusterAssoc = SimpleAssociation \
    (PhotonD3PDObject,
     egammaD3PDMaker.egammaRawClusterAssociationTool,
     prefix = 'rawcl_',
     AssocGetter = D3PDMakerCoreComps.SGObjGetterTool
        ('PhotonRawClusterAssocGetter',
         #SGKey filled in by hook fn below.
         TypeName = 'INav4MomAssocs'))
PhotonRawClusterAssoc.defineBlock \
    (2, 'RawSamplings', CaloD3PDMaker.ClusterEMSamplingFillerTool)
PhotonRawClusterAssoc.defineBlock \
    (2, 'RawClusterKin', EventCommonD3PDMaker.FourMomFillerTool,
     WriteE = True,
     WriteM = False)
def _photonRawClusterAssocHook (c, prefix, sgkey, typeName,
                                *args, **kw):
    assoc = getattr(c, c.name() + '_D3PD__egammaRawClusterAssociationTool',
                    None)
    if assoc:
        assoc.Associator.AssocGetter.SGKey = \
                  (resolveSGKey(typeName, sgkey) +
                   D3PDMakerFlags.RawClustersAssocSGKeySuffix())
    return
PhotonD3PDObject.defineHook (_photonRawClusterAssocHook)


############################################################################
# From UserData
#

if D3PDMakerFlags.HaveEgammaUserData() or D3PDMakerFlags.MakeEgammaUserData():
    defineBlockAndAlg (PhotonD3PDObject, 
                       1, 'UDLayer1Shape',
                       D3PDMakerCoreComps.AuxDataFillerTool,
                       'egammaDeltaEmax2Config',
                       AuxPrefix = auxprefix,
                       Vars = ['deltaEmax2',
                               ])

    PhotonD3PDObject.defineBlock (
        1, 'TopoClusterIsolationCones',
        D3PDMakerCoreComps.AuxDataFillerTool,
        Vars = ['topoEtcone20 = topoetcone20_core57cells,topoetcone20',
                'topoEtcone30 = topoetcone30_core57cells,topoetcone30',
                'topoEtcone40 = topoetcone40_core57cells,topoetcone40',
                ])



############################################################################
# Jet associations
#

PhotonJetD3PDAssoc = DRAssociation (PhotonD3PDObject,
                                    'JetCollection',
                                    D3PDMakerFlags.JetSGKey(),
                                    0.2,
                                    'jet_',
                                    level = 2,
                                    blockname = 'JetMatch')
PhotonJetD3PDAssoc.defineBlock (2, 'JetKinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE = True)


#if rec.doTruth():
#    JetTruthJetD3PDAssoc = DRAssociation (PhotonJetD3PDAssoc,
#                                          'JetCollection',
#                                          D3PDMakerFlags.TruthJetSGKey(),
#                                          0.2,
#                                          'truth_',
#                                          level = 2,
#                                          blockname = 'TrueJetMatch')
#    JetTruthJetD3PDAssoc.defineBlock (2, 'TrueJetKinematics',
#                                      EventCommonD3PDMaker.FourMomFillerTool,
#                                      WriteE = True)




#commented out PUU
############################################################################
# PhotonAnalysisUtils associations
#
#
#PhotonPAUAssoc = PAUGammaAssociation (PhotonD3PDObject,
#                                      'PAU::gammaContainer',
#                                      'PAUgammaCollection',
#                                      level = 2,
#                                      blockname = 'PAU')
#PhotonPAUAssoc.defineBlock (2, 'PAUVariables',
#                            egammaD3PDMaker.PAUFillerTool,
#                            FillHPV = False,
#                            FillAllCorrectedIsolation = False)

############################################################################
# Topo cluster associations.
#


PhotonTopoD3PDAssoc = DRAssociation (PhotonD3PDObject,
                                     'CaloClusterContainer',
                                     D3PDMakerFlags.ClusterSGKey(),
                                     0.1,
                                     'topo',
                                     level = 2,
                                     blockname = 'TopoMatch')
PhotonTopoD3PDAssoc.defineBlock (2, 'TopoKinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteM = False)

PhotonTopoEMD3PDAssoc = DRAssociation (PhotonD3PDObject,
                                       'CaloClusterContainer',
                                       D3PDMakerFlags.EMTopoClusterSGKey(),
                                       0.1,
                                       'topoEM',
                                       level = 2,
                                       blockname = 'TopoEMMatch')
PhotonTopoEMD3PDAssoc.defineBlock (2, 'TopoEMKinematics',
                                   EventCommonD3PDMaker.FourMomFillerTool,
                                   WriteM = False)


############################################################################
# Trigger matching
#

if D3PDMakerFlags.DoTrigger():
#comment out trigger bit
#    defineTriggerBits (PhotonD3PDObject, 0,
#                       ['EF_g20_loose',
#                        ])
#    defineTriggerBits (PhotonD3PDObject, 1,
#                       D3PDMakerFlags.egammaL1TrigPattern())
#    defineTriggerBits (PhotonD3PDObject, 1,
#                       D3PDMakerFlags.PhotonL2TrigPattern())
#    defineTriggerBits (PhotonD3PDObject, 1,
#                       D3PDMakerFlags.PhotonEFTrigPattern())


    ### Matching indices.
    
    PhotonEFIndexAssoc = IndexAssociation(
        PhotonD3PDObject,
        egammaD3PDMaker.egammaEFTriggerObjectAssociationTool,
        target = "trig_EF_ph_", prefix = "EF_", level = 1,
        blockname = "EFIndex",
        ChainPattern = D3PDMakerFlags.PhotonEFTrigPattern(),
        allowMissing = True )
    
    PhotonL2IndexAssoc = IndexAssociation(
        PhotonD3PDObject,
        egammaD3PDMaker.PhotonL2TriggerObjectAssociationTool,
        target = "trig_L2_ph_", # For IndexAssociation.
        prefix = "L2_", level = 1,
        blockname = "L2Index",
        ChainPattern = D3PDMakerFlags.PhotonL2TrigPattern(),
        allowMissing = True )
    
    PhotonL1IndexAssoc = IndexAssociation(
        PhotonD3PDObject,
        egammaD3PDMaker.egammaL1TriggerObjectAssociationTool,
        target = "trig_L1_emtau_", prefix = "L1_", level = 1,
        ChainPattern = D3PDMakerFlags.egammaL1TrigPattern(),
        blockname = "L1Index")

    ### Matched objects by containment.

    PhotonEFAssoc = SimpleAssociation \
        (PhotonD3PDObject,
         egammaD3PDMaker.egammaEFTriggerObjectAssociationTool,
         matched = 'matched',
         blockname = 'EFInfo',
         prefix = 'EF_',
         ChainPattern = D3PDMakerFlags.PhotonEFTrigPattern(),
         WriteDR = False)
    PhotonEFAssoc.defineBlock (1, 'EFKinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteE = True,
                                 WriteEt = True,
                                 WriteM = False)
    PhotonEFAssoc.defineBlock (1, 'EFHadLeakage',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.ethad,
                                            'Ethad',
                                            egammaParameters.ethad1,
                                            'Ethad1',
                                            ])
    PhotonEFAssoc.defineBlock (1, 'EFLayer1Shape',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.f1,
                                            'f1',
                                            egammaParameters.emins1,
                                            'Emins1',
                                            egammaParameters.fracs1,
                                            'fside',
                                            egammaParameters.e2tsts1,
                                            'Emax2',
                                            egammaParameters.weta1,
                                            'ws3',
                                            egammaParameters.wtots1,
                                            'wstot',
                                            ])
    PhotonEFAssoc.defineBlock (1, 'EFLayer2Shape',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.e233,
                                            'E233',
                                            egammaParameters.e237,
                                            'E237',
                                            egammaParameters.e277,
                                            'E277',
                                            egammaParameters.weta2,
                                            'weta2',
                                            ])
    PhotonEFAssoc.defineBlock (1, 'EFIso',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.r33over37allcalo,
                                            'rphiallcalo',
                                            ])

    PhotonEFAssoc.defineBlock (2, 'EFIso2',
                                 egammaD3PDMaker.egammaDetailFillerTool,
                                 Details = [egammaParameters.etcone,
                                            'Etcone45',
                                            egammaParameters.etcone20,
                                            'Etcone20',
                                            egammaParameters.etcone30,
                                            'Etcone30',
                                            egammaParameters.etcone40,
                                            'Etcone40',
                                            ])
    PhotonEFAssoc.defineBlock (2, 'EFPointing',
                               egammaD3PDMaker.egammaDetailFillerTool,
                               Details = [egammaParameters.zvertex,
                                          'zvertex',
                                          egammaParameters.errz,
                                          'errz',
                                          ])



    PhotonL2Assoc = SimpleAssociation \
        (PhotonD3PDObject,
         egammaD3PDMaker.PhotonL2TriggerObjectAssociationTool,
         matched = 'matched',
         blockname = 'L2Info',
         prefix = 'L2_',
         ChainPattern = D3PDMakerFlags.PhotonL2TrigPattern(),
         WriteDR = False)
    PhotonL2Assoc.defineBlock (1, 'L2Kinematics',
                               EventCommonD3PDMaker.FourMomFillerTool,
                               WriteE = True,
                               WriteEt = True,
                               WriteM = False)
    PhotonL2Assoc.defineBlock (1, 'L2Calo1',
                               egammaD3PDMaker.TrigPhotonCalo1FillerTool)
    PhotonL2ClusAssoc = SimpleAssociation \
       (PhotonL2Assoc,
        egammaD3PDMaker.TrigPhotonClusterAssociationTool)
    PhotonL2ClusAssoc.defineBlock (2, 'L2Clus2',
                                   egammaD3PDMaker.TrigEMClusterVarsFillerTool)

    PhotonL1Assoc = SimpleAssociation \
        (PhotonD3PDObject,
         egammaD3PDMaker.egammaL1TriggerObjectAssociationTool,
         matched = 'matched',
         blockname = 'L1Info',
         prefix = 'L1_',
         ChainPattern = D3PDMakerFlags.egammaL1TrigPattern(),
         WriteDR = False)
    PhotonL1Assoc.defineBlock (1, 'L1Kinematics',
                               EventCommonD3PDMaker.FourMomFillerTool,
                               WriteEt = False,
                               WritePt = False,
                               WriteM = False)
    PhotonL1Assoc.defineBlock (1, 'L1Isol',
                               egammaD3PDMaker.EmTau_ROIIsoFillerTool)
    PhotonL1Assoc.defineBlock (1, 'L1EmET',
                               egammaD3PDMaker.EmTau_ROIEmETFillerTool)
