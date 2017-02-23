from AthenaCommon.CfgGetter import getAlgorithm, getPublicTool
cnvalg = getAlgorithm('MuonFeatureCnvAlgL2SA')
topSequence += cnvalg

topSequence += getAlgorithm('MuonFeatureCnvTestAlgL2SA')
topSequence.MuonFeatureCnvTestAlgL2SA.OutputLevel=VERBOSE
