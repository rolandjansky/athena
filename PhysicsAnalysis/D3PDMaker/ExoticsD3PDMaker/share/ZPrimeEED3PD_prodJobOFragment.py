# $Id: ZPrimeEED3PD_prodJobOFragment.py 505560 2012-06-14 15:38:08Z mkaneda $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "ZPrimeEE D3PD cart".

# This jobO should not be included more than once:
include.block( "ExoticsD3PDMaker/ZPrimeEED3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
ZPrimeEED3PDStream_msg = logging.getLogger( 'ZPrimeEED3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteZPrimeEED3PD.isVirtual:
    ZPrimeEED3PDStream_msg.error( "The ZPrimeEE D3PD stream can't be virtual! " +
                              "It's a configuration error!" )
    raise NameError( "ZPrimeEE D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the ZPrimeEE D3PD:
streamName = prodFlags.WriteZPrimeEED3PD.StreamName
fileName   = buildFileName( prodFlags.WriteZPrimeEED3PD )
ZPrimeEED3PDStream_msg.info( "Configuring ZPrimeEED3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# Add specific containers
include ("PhysicsD3PDMaker/MyJetMake.py")

# Trigger information definition
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
#D3PDMakerFlags.egammaL1TrigPattern = 'L1_2EM3.*|L1_2EM5.*|L1_2EM7.*|L1_EM3.*|L1_EM5.*|L1_EM7|L1_EM12|L1_EM16VH|L1_EM18VH'
#D3PDMakerFlags.ElectronL2TrigPattern = 'L2_2e.*|L2_e10.*|L2_e5.*|L2_e60.*|L2_eb.*|L2_em3_empty.*'
#D3PDMakerFlags.ElectronEFTrigPattern = 'EF_2e.*|EF_e10.*|EF_e5.*|EF_e60.*|EF_eb.*|EF_e22vh_medium|EF_e22vh_medium1'
D3PDMakerFlags.egammaL1TrigPattern = 'L1_2EM10VH|L1_2EM12|L1_EM10VH|L1_EM12|L1_EM14VH|L1_EM16V|L1_EM16VH|L1_EM18VH|L1_EM30'
D3PDMakerFlags.ElectronL2TrigPattern = 'L2_22vh_loose*|L2_e24vh_loose*|L2_2e12Tvh_loose*|L2_e22vh_medium*|L2_e22vhi_medium1|L2_e24vh_medium1|L2_e24vhi_medium1|L2_e45_medium1|L2_e60_medium1|L2_e18_medium1_g25_loose|L2_g*_etcut|L2_2g40_loose|L2_g*_loose_g*_loose|L2_2g20_loose|L2_e18_loose1_g25_medium|L2_e18_loose1_g35_loose|L2_e18_loose1_g35_medium|L2_g*_loose|L2_g30_medium_g20_medium|L2_2g20vh_medium|L2_g*_medium|L2_e18_loose1|L2_e18_medium1|L2_e18vh_medium1'
D3PDMakerFlags.ElectronEFTrigPattern = 'EF_22vh_loose*|EF_e24vh_loose*|EF_2e12Tvh_loose*|EF_e22vh_medium*|EF_e22vhi_medium1|EF_e24vh_medium1|EF_e24vhi_medium1|EF_e45_medium1|EF_e60_medium1|EF_e18_medium1_g25_loose|EF_g*_etcut|EF_2g40_loose|EF_g*_loose_g*_loose|EF_2g20_loose|EF_e18_loose1_g25_medium|EF_e18_loose1_g35_loose|EF_e18_loose1_g35_medium|EF_g*_loose|EF_g30_medium_g20_medium|EF_2g20vh_medium|EF_g*_medium|EF_e18_loose1|EF_e18_medium1|EF_e18vh_medium1'


# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
ZPrimeEED3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )

# Skimming/thinning definition

# Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

# Get preseq
from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

# electron selection
from ExoticsD3PDMaker import ZPrimeElectronSelector
NewElectronCollection='ZPrimeHighPtElectrons'
preseq += ZPrimeElectronSelector( "ZPrimeElectronSelector",
            inputCollection     = "ElectronAODCollection",
            outputCollection    = NewElectronCollection,
            minNumberPassed     = 2,
            ptMin               = 20.*Units.GeV,
            etMin               = 20.*Units.GeV,
            clPtMin             = 20.*Units.GeV,
            reclPtMin           = 20.*Units.GeV
            )

# photon selection
from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector
NewPhotonCollection='ZPrimeHighPtPhotons'
preseq += D2PDPhotonSelector( "ZPrimePhotonSelector",
            inputCollection  = "PhotonAODCollection",
            outputCollection = NewPhotonCollection,
            ptMin            = 20.*Units.GeV,
            )

# Set require algorithm
ZPrimeEED3PDStream.RequireAlgs+=['ZPrimeElectronSelector']

# Now add all the content to this stream:
from ExoticsD3PDMaker.ZPrimeEED3PD import ZPrimeEED3PD
ZPrimeEED3PD( ZPrimeEED3PDStream ,electronContainer = NewElectronCollection, photonContainer = NewPhotonCollection)

# Add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
ZPrimeEED3PDStream.MetadataTools += [ CutFlowMetadataConfig( ZPrimeEED3PDStream ) ]
