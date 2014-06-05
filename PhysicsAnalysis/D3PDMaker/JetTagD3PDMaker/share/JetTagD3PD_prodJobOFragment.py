#general d3pd maker fragment

include("JetTagD3PDMaker/JetTagD3PD_prodFragmentCore.py")


from AthenaCommon.JobProperties import jobproperties
btagprodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName


if btagprodFlags.WriteBTAGD3PD.isVirtual:
  raise NameError( "BTAGD3PD set to be a virtual stream" )
  pass 

streamName = btagprodFlags.WriteBTAGD3PD.StreamName
fileName   = buildFileName( btagprodFlags.WriteBTAGD3PD )

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.NewRootStream(btagprodFlags.WriteBTAGD3PD.StreamName, fileName, JetTagD3PDKeys.D3PDTupleName())

from JetTagD3PDMaker.BTaggingD3PD import BTaggingD3PD
BTaggingD3PD(alg, **(JetTagD3PDFlags.D3PDPropDict()))


include("JetTagD3PDMaker/JetTagD3PD_prodFragmentPostStream.py")
