

JetTagD3PD_flavor='Slim'

if not 'JetTagD3PD_redoTagging' in dir():
    JetTagD3PD_redoTagging=True


if not 'JetTagD3PD_JetFitterCharm' in dir():
  JetTagD3PD_JetFitterCharm=True

if not 'JetTagD3PD_MV3' in dir():
  JetTagD3PD_MV3=True



include("JetTagD3PDMaker/JetTagD3PD_prodFragmentCore.py")

include("JetTagD3PDMaker/JetTagD3PD_configGhostBtagging.py")


from AthenaCommon.JobProperties import jobproperties
btagprodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName


if btagprodFlags.WriteBTAGGHOSTD3PD.isVirtual:
  raise NameError( "BTAGD3PD set to be a virtual stream" )
  pass 

streamName = btagprodFlags.WriteBTAGGHOSTD3PD.StreamName
fileName   = buildFileName( btagprodFlags.WriteBTAGGHOSTD3PD )

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.NewRootStream(btagprodFlags.WriteBTAGGHOSTD3PD.StreamName, fileName, JetTagD3PDKeys.D3PDTupleName())

from JetTagD3PDMaker.BTaggingD3PD import BTaggingD3PD
BTaggingD3PD(alg, **(JetTagD3PDFlags.D3PDPropDict()))


include("JetTagD3PDMaker/JetTagD3PD_prodFragmentPostStream.py")
