# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ElectronToPhotonD3PDObject.py 587551 2014-03-13 19:09:27Z mhance $
#
# @file PhotonD3PDMaker/python/ElectronToPhotonD3PDObject.py
# @author Mike Hance <hance@hep.upenn.edu>
# @date July, 2010
# @brief Configure electron->photon variables
#


from RecExConfig.RecFlags                 import rec
from AthenaCommon.AlgSequence             import AlgSequence
from AthenaCommon.AppMgr                  import ToolSvc
from AthenaCommon.Include                 import include
from D3PDMakerConfig.D3PDMakerFlags       import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject        import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
import CaloD3PDMaker
import EventCommonD3PDMaker
import egammaD3PDMaker
from egammaD3PDMaker.isem_version         import isem_version
from egammaD3PDMaker                      import egammaDetailFillerTool
from ROOT                                 import egammaPID
from ROOT                                 import egammaParameters
from AthenaCommon.DetFlags                import DetFlags 
from AthenaCommon.Constants               import WARNING, INFO, ERROR, VERBOSE

from PhotonAnalysisUtils.PAU_AtlasExtrapolator import PAU_AtlasExtrapolator
theAtlasExtrapolator = PAU_AtlasExtrapolator()

# Set up the overall cluster correction tool
doEgammaDefault=True

if doEgammaDefault:
    from egammaRec.EMClusterTool import egammaClusterTool
    PhotonClusterTool = egammaClusterTool("MyPhotonClusterTool")

else:
    from egammaD3PDAnalysis.egammaSwTool import egammaSwTool
    from CaloClusterCorrection.common import CALOCORR_JO, CALOCORR_COOL
    myegammaSwTool = egammaSwTool('PhotonegammaSwTool',
                                  suffix='EG',
                                  version='v11_calh', #'@GLOBAL',
                                  CaloCells=None,
                                  source=None
                                  )
    ToolSvc += myegammaSwTool
    
    from egammaTools.egammaToolsConf import EMClusterTool
    PhotonClusterTool = EMClusterTool(name                      = "MyPhotonClusterTool",
                                      ClusterCorrectionToolName = myegammaSwTool.getFullName())

ToolSvc += PhotonClusterTool






# Set up the PID builder tool
include("PhotonD3PDMaker/PhotonPIDTool_jobOptions.py")

from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetJetFitterUtils
convUtils = InDet__InDetJetFitterUtils(name                   = "convUtils",
                                       LinearizedTrackFactory = None,
                                       Extrapolator           = None)
ToolSvc += convUtils

from egammaTools.EMClusterErrorsToolFactory import EMClusterErrorsToolFact
PAUemclustererrorstool=EMClusterErrorsToolFact("EMClusterErrorsTool")
ToolSvc += PAUemclustererrorstool

from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
PhotonsDetailsContainerName=PAUflags.PhotonsDetailsContainerName.StoredValue

import EventKernel.ParticleDataType
from egammaTools.egammaToolsConf import EMFourMomBuilder
PAUPhotonFourMomBuilder=EMFourMomBuilder(name      = "PAUfourmombuilder");
PAUPhotonFourMomBuilder.EMErrorDetailContainerName = PhotonsDetailsContainerName
PAUPhotonFourMomBuilder.EMShowerContainerName      = PhotonsDetailsContainerName
PAUPhotonFourMomBuilder.EMTrackFitContainerName    = PhotonsDetailsContainerName
PAUPhotonFourMomBuilder.EMConvertContainerName     = PhotonsDetailsContainerName
PAUPhotonFourMomBuilder.ConversionUtils            = convUtils
PAUPhotonFourMomBuilder.dataType                   = EventKernel.ParticleDataType.Full
PAUPhotonFourMomBuilder.EMClusterErrorsTool        = PAUemclustererrorstool
ToolSvc += PAUPhotonFourMomBuilder

if not rec.readAOD(): 
    CaloCells = 'AllCalo' 
else: 
    CaloCells = 'AODCellContainer' 
 
# second shower shape builder for post processing 
# configure tool egammaShowerShape 
from egammaCaloTools.egammaCaloToolsConf import egammaShowerShape 
PAUegammaShowerShape = egammaShowerShape(name = "PAUegammashowershape", 
                                         TypeAnalysis ="ClusterSeed", 
                                         # calculate quantities base on information in the strips in a region 
                                         # around the cluster. 
                                         # Use 5 strips in phi and cover a region of +-1.1875 
                                         # times 0.025 in eta (corresponds to 19 strips in em barrel) 
                                         # Use 7*7 cluster for middle sampling 
                                         # other samplings arew not used for the moment 
                                         Neta=[0.,2.5,7,0.], 
                                         Nphi=[0.,2.,7,0.], 
                                         # Samplings to execute 
                                         ExecAllVariables = True, 
                                         ExecPreSampler = True, 
                                         ExecEMFirst = True, 
                                         ExecEMSecond = True, 
                                         ExecEMThird = True, 
                                         ExecEMCombined = True, 
                                         ExecOtherVariables = True) 
ToolSvc += PAUegammaShowerShape 
 
# create tool CaloDepthTool 
from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory 
theCaloDepthTool=CaloDepthToolFactory(depth='showerdefault') 
ToolSvc += theCaloDepthTool 
 
# add tool egammaqpoint 
from egammaCaloTools.egammaCaloToolsConf import egammaqpoint 
theegammaqpoint=egammaqpoint("egammaQpoint", 
                             CalorimeterDepth=theCaloDepthTool.getName() ) 
#theegammaqpoint+=theCaloDepthTool # don't do this!  It causes errors. 
ToolSvc+=theegammaqpoint 
 
# add tool egammaqgcld 
from egammaCaloTools.egammaCaloToolsConf import egammaqgcld 
theegammaqgcld=egammaqgcld("egammaQgcld", 
                           CalorimeterDepth=theCaloDepthTool.getName() ) 
#theegammaqgcld+=theCaloDepthTool 
ToolSvc+=theegammaqgcld 
 
# create tool CaloNoiseTool 
from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault 
theCaloNoiseTool=CaloNoiseToolDefault('db') 
ToolSvc += theCaloNoiseTool 
 
# add tool egammaiso 
from egammaCaloTools.egammaCaloToolsConf import egammaIso 
theegammaiso=egammaIso("egammaIso", 
                       CaloNoiseTool= theCaloNoiseTool) 
ToolSvc += theegammaiso 
 
from egammaTools.EMTrackIsolationToolBase import EMAltTrackIsolationToolBase
theemalttrackisolationtool = EMAltTrackIsolationToolBase("emalttrackisolationtool")
ToolSvc+=theemalttrackisolationtool 


from egammaCaloTools.egammaIsoPtCorrectionToolBase import egammaIsoPtCorrectionToolBase 
theegammaisoptcorrection=egammaIsoPtCorrectionToolBase("egammaIsoPtCorrection") 
ToolSvc+=theegammaisoptcorrection 
 
from CaloIdentifier import SUBCALO 
from egammaTools.egammaToolsConf import EMShowerBuilder 
PAUPhotonShowerBuilder = EMShowerBuilder(name                      = "PAUPhotonShowerBuilder", 
                                         CellsName                 = CaloCells, 
                                         CaloNums                  = [SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.TILE], 
                                         EMShowerContainerName     = PhotonsDetailsContainerName, 
                                         UseCaloIsoTool            = False, 
                                         UseTrackIsoTool           = False, 
                                         Print                     = False, 
                                         PointingTool              = theegammaqpoint, 
                                         DepthTool                 = theegammaqgcld, 
                                         ShowerShapeTool           = PAUegammaShowerShape,
					 EMAltTrackIsolationTool   = theemalttrackisolationtool,
                                         EMCaloIsoPtCorrectionTool = theegammaisoptcorrection)
PAUPhotonShowerBuilder.OutputLevel = WARNING
ToolSvc += PAUPhotonShowerBuilder


# Track summary helper tool
from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
PAUInDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "PAUInDetHoleSearchTool",
                                                         Extrapolator = theAtlasExtrapolator)

ToolSvc += PAUInDetHoleSearchTool

#
# Load the InDetTrackSummaryHelperTool
#
from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
PAUInDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "PAUInDetPrdAssociationTool",
                                                                         PixelClusterAmbiguitiesMapName = 'PixelClusterAmbiguitiesMap')
ToolSvc += PAUInDetPrdAssociationTool


from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
if DetFlags.haveRIO.TRT_on():
        PAUInDetTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name         = "PAUInDetSummaryHelper",
                                                                            AssoTool     = PAUInDetPrdAssociationTool,
                                                                            DoSharedHits = False,
                                                                            HoleSearch   = PAUInDetHoleSearchTool)
        ToolSvc += PAUInDetTrackSummaryHelperTool
else:
        PAUInDetTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name         = "PAUInDetSummaryHelper",
                                                                            AssoTool     = PAUInDetPrdAssociationTool,
                                                                            DoSharedHits = False,
                                                                            TRTStrawSummarySvc = "",
                                                                            HoleSearch   = PAUInDetHoleSearchTool)
        ToolSvc += PAUInDetTrackSummaryHelperTool
        
        
ToolSvc += PAUInDetTrackSummaryHelperTool

from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
PAUInDetTrackSummaryTool = Trk__TrackSummaryTool(name = "PAUInDetTrackSummaryTool",
                                                 InDetSummaryHelperTool = PAUInDetTrackSummaryHelperTool,
                                                 doSharedHits           = False,
                                                 InDetHoleSearchTool    = PAUInDetHoleSearchTool)
ToolSvc += PAUInDetTrackSummaryTool

# Helper Tool
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionFinderUtils
InDetConversionHelper = InDet__ConversionFinderUtils(name = "ConversionFinderUtils")
ToolSvc += InDetConversionHelper

from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__SingleTrackConversionTool
PAUInDetSingleTrackConversion = InDet__SingleTrackConversionTool(name                       = "PAUSingleTrackConversionTool",
                                                                 ConversionFinderHelperTool = InDetConversionHelper,
                                                                 TrackSummaryTool           = PAUInDetTrackSummaryTool,
                                                                 MinInitialHitRadius        = 0.,
                                                                 MinRatioOfHLhits           = 0.)  #0.)  #0.95)
ToolSvc += PAUInDetSingleTrackConversion

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import ElectronToPhotonTool
ElectronToPhotonTool = ElectronToPhotonTool("ElectronToPhotonTool",
                                            ClusterTool = PhotonClusterTool,
                                            PIDBuilderTool = ToolSvc.PhotonPIDBuilder,
                                            FourMomentumTool = PAUPhotonFourMomBuilder,
                                            EMShowerBuilderTool = PAUPhotonShowerBuilder,
                                            SingleTrackConversionTool = PAUInDetSingleTrackConversion,
                                            TrackParticleContainerName = D3PDMakerFlags.TrackSGKey())
ToolSvc += ElectronToPhotonTool

preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import ElectronToPhotonAlg
preseq += ElectronToPhotonAlg("ElectronToPhotonAlg",
                              ElectronContainer = D3PDMakerFlags.ElectronSGKey(),
                              ElectronToPhotonTool = ElectronToPhotonTool);


ElectronToUnconvertedPhotonD3PDObject = make_SGDataVector_D3PDObject('PhotonContainer',
                                                                     'PAUElectronToUnconvertedPhotonCollection',
                                                                     'el_as_unconv_ph_');

ElectronToConvertedPhotonD3PDObject = make_SGDataVector_D3PDObject('PhotonContainer',
                                                                   'PAUElectronToConvertedPhotonCollection',
                                                                   'el_as_conv_ph_');

for newPhotonD3PDObject in [ElectronToUnconvertedPhotonD3PDObject,ElectronToConvertedPhotonD3PDObject]:
    PhotonClusterAssoc = SimpleAssociation \
                         (newPhotonD3PDObject,
                          egammaD3PDMaker.egammaClusterAssociationTool)
    PhotonClusterAssoc.defineBlock (0, 'ClusterKin', EventCommonD3PDMaker.FourMomFillerTool,
                                    prefix = 'cl_',
                                    WriteE = True,
                                    WriteM = False)
    PhotonClusterAssoc.defineBlock (0, 'Samplings', CaloD3PDMaker.ClusterEMSamplingFillerTool)

    newPhotonD3PDObject.defineBlock(0, 'IsEM',  egammaD3PDMaker.egammaIsEMoneFillerTool)
    if isem_version() == 1:
        newPhotonD3PDObject.defineBlock (0, 'IsEMCuts',
                                         egammaD3PDMaker.egammaIsEMFillerTool,
                                         IsEM  = [egammaPID.PhotonTightOLDRel,
                                                  'tight',
                                                  ])
    else:
        newPhotonD3PDObject.defineBlock (0, 'IsEMCuts',
                                         egammaD3PDMaker.egammaIsEMFillerTool,
                                         IsEM  = [egammaPID.PhotonLoose,
                                                  'loose',
                                                  egammaPID.PhotonTight,
                                                  'tight',
                                                  egammaPID.PhotonTightIso,
                                                  'tightIso',
                                                  ])

    newPhotonD3PDObject.defineBlock (0, 'HadLeakage',
                                     egammaD3PDMaker.egammaDetailFillerTool,
                                     Details = [egammaParameters.ethad,
                                                'Ethad',
                                                egammaParameters.ethad1,
                                                'Ethad1',
                                                ])
    newPhotonD3PDObject.defineBlock (0, 'Layer1Shape',
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
    newPhotonD3PDObject.defineBlock (0, 'Layer1ShapeExtra',
                                     egammaD3PDMaker.egammaLayer1ExtraFillerTool)
    newPhotonD3PDObject.defineBlock (0, 'Layer2Shape',
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

