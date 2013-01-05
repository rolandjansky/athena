# $Id: ZPrimeMMD3PD_prodJobOFragment.py 522215 2012-10-18 18:34:05Z jcogges $

#

# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to

# create the "ZPrimeMM D3PD cart".



# This jobO should not be included more than once:

include.block( "PhysicsD3PDMaker/ZPrimeMMD3PD_prodJobOFragment.py" )



# Common import(s):

from AthenaCommon.JobProperties import jobproperties

prodFlags = jobproperties.D3PDProdFlags

from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName



# Set up a logger:

from AthenaCommon.Logging import logging

ZPrimeMMD3PDStream_msg = logging.getLogger( 'ZPrimeMMD3PD_prodJobOFragment' )



# Check if the configuration makes sense:

if prodFlags.WriteZPrimeMMD3PD.isVirtual:

    ZPrimeMMD3PDStream_msg.error( "The ZPrimeEE D3PD stream can't be virtual! " +

                              "It's a configuration error!" )

    raise NameError( "ZPrimeMM D3PD set to be a virtual stream" )

    pass



# Construct the stream and file names for the SMWZ D3PD:

streamName = prodFlags.WriteZPrimeMMD3PD.StreamName

fileName   = buildFileName( prodFlags.WriteZPrimeMMD3PD )

ZPrimeMMD3PDStream_msg.info( "Configuring ZPrimeMMD3PD with streamName '%s' and fileName '%s'" % \

                            ( streamName, fileName ) )



# add specific truth jets

include ("PhysicsD3PDMaker/MyJetMake.py")



#configure MuonScatteringAngleSignificanceTool

include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")





## Get pre sequence

from AthenaCommon.AlgSequence import AlgSequence

preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())



# Skimming definition // no thinning currently

from D2PDMaker.D2PDMakerConf                           import D2PDMuonSelector

from D2PDMaker.D2PDMakerConf                           import D2PDParticleCombiner

import AthenaCommon.SystemOfUnits as Units



from D2PDMaker.D2PDMakerConf import D2PDMuonSelector



ZPrimeStacoMuonSelector = D2PDMuonSelector( "ZPrimeStacoMuonSelector",

                                            OutputLevel          = INFO,

                                            inputCollection      ='StacoMuonCollection',

                                            minNumberPassed      = 2,

                                            ptMin                = 20.0*Units.GeV,

                                            muonRequireIsCombined= True

                                            )



ZPrimeMuidSelector     = D2PDMuonSelector( "ZPrimeMuidSelector",

                                            OutputLevel          = INFO,

                                            inputCollection      = 'MuidMuonCollection',

                                            minNumberPassed      = 2,

                                            ptMin                = 20.0*Units.GeV,

                                            muonRequireIsCombined= True

                                            )



ZPrimeMuonsSelector     = D2PDMuonSelector( "ZPrimeMuonsSelector",

                                            OutputLevel          = INFO,

                                            inputCollection      = 'Muons',

                                            minNumberPassed      = 2,

                                            ptMin                = 20.0*Units.GeV,

                                            muonRequireIsCombined= True

                                            )



preseq+= ZPrimeStacoMuonSelector

preseq+= ZPrimeMuidSelector

preseq+= ZPrimeMuonsSelector



# Create the D3PD streams:

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

ZPrimeMMD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )



# Set require algorithm

ZPrimeMMD3PDStream.AcceptAlgs+=['ZPrimeStacoMuonSelector']

ZPrimeMMD3PDStream.AcceptAlgs+=['ZPrimeMuidSelector']

ZPrimeMMD3PDStream.AcceptAlgs+=['ZPrimeMuonsSelector']



# Now add all the content to this stream:

from ExoticsD3PDMaker.ZPrimeMMD3PD import ZPrimeMMD3PD

ZPrimeMMD3PD( ZPrimeMMD3PDStream )



# Add cut flow information

from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig

ZPrimeMMD3PDStream.MetadataTools += [ CutFlowMetadataConfig( ZPrimeMMD3PDStream ) ]
