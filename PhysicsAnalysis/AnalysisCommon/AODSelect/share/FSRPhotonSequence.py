##-----------------------------------------------------------------------------
## Name: FSRPhotonSequence.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: Define a list of viable FSR photons for muons
##-----------------------------------------------------------------------------


include.block("AODSelect/FSRPhotonSequence.py")


## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

#---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import cppyy
cppyy.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr


# - the photon selector --- for standard photons
AODSelectSeq += CfgMgr.D2PDPhotonSelector( "AODSelectFSRStandardPhotonSelectorAlg",
                                           OutputLevel               = INFO,
                                           inputCollection           = 'PhotonAODCollection',
                                           outputLinkCollection      = 'FSRStandardPhotonCandidateLinkCollection',
                                           #outputCollection          = 'FSRStandardPhotonCandidateCollection',
                                           clusterEtMin              = 3.5*Units.GeV,
                                           photonAcceptAuthorList    = [ egammaParameters.AuthorPhoton,
                                                                         egammaParameters.AuthorRConv ],
                                           egammaParameterList       = [ egammaParameters.f1 ],
                                           egammaParameterCutMinList = [ 0.15 ]
                                           )

# - the photon selector --- for topo-cluster seeded photons
AODSelectSeq += CfgMgr.D2PDPhotonSelector( "AODSelectFSRTopoPhotonSelectorAlg",
                                           OutputLevel               = INFO,
                                           inputCollection           = 'PhotonAODCollection',
                                           outputLinkCollection      = 'FSRTopoPhotonCandidateLinkCollection',
                                           #outputCollection          = 'FSRTopoPhotonCandidateCollection',
                                           clusterEtMin              = 1.0*Units.GeV,
                                           clusterEtMax              = 3.5*Units.GeV,
                                           photonAcceptAuthorList    = [ egammaParameters.AuthorCaloTopo35 ],
                                           egammaParameterList       = [ egammaParameters.f1 ],
                                           egammaParameterCutMinList = [ 0.15 ]
                                           )

# - the electron selector
AODSelectSeq += CfgMgr.D2PDElectronSelector( "AODSelectFSRElectronSelectorAlg",
                                             OutputLevel               = INFO,
                                             inputCollection           = 'ElectronAODCollection',
                                             outputLinkCollection      = 'FSRElectronCandidateLinkCollection',
                                             #outputCollection          = 'FSRElectronCandidateCollection',
                                             clusterEtMin              = 3.5*Units.GeV,
                                             electronAuthor            = egammaParameters.AuthorElectron,
                                             egammaParameterList       = [ egammaParameters.f1 ],
                                             egammaParameterCutMinList = [ 0.15 ]
                                             )


# Combine the selected FSR photons and electrons into one pointer list
from D2PDMaker.AddContainers import AddContainers
AODSelectSeq += AddContainers( "AODSelectFSRPhotonElectronAdderAlg",
                               OutputLevel         = INFO,
                               inputContainerType  = "INav4MomLinkContainer",
                               inputContainerNames = [ "FSRStandardPhotonCandidateLinkCollection",
                                                       "FSRTopoPhotonCandidateLinkCollection",
                                                       "FSRElectronCandidateLinkCollection" ],
                               outputContainerName = "FSRCandidateLinkCollection",
                               removeIdentical     = False,
                               removeNearby        = True,
                               useAttribute        = "cluster",
                               maxDeltaR           = 0.01
                               )


# Set up a DeltaR association/matching tool
ToolSvc += CfgMgr.DeltaRAssociationTool( "AODSelectFSRCandidateDeltaRAssociationTool",
                                         OutputLevel                = INFO,
                                         inputAssociateToCollection = "FSRCandidateLinkCollection",
                                         deltaRMax                  = 0.3,
                                         doSpecialMuElFSRMatch      = True,
                                         specialMuElFSRMatchCut     = 0.001,
                                         writeUserData              = False
                                         )


