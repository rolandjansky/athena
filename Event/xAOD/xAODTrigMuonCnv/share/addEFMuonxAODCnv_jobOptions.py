from AthenaCommon.CfgGetter import getAlgorithm, getPublicTool
cnvalg = getAlgorithm('TrigMuonEFInfoToMuonCnvAlgTMEF')
#cnvalg.OutputLevel=DEBUG
topSequence += cnvalg
#getPublicTool('TrigMuonEFInfoToMuonCnvToolNoIDTrkLinks').OutputLevel=DEBUG
topSequence += getAlgorithm('TrigMuonEFInfoToMuonCnvAlgTMG')

topSequence += getAlgorithm('TrigMuonEFInfoCnvTestAlgTMEF')
topSequence.TrigMuonEFInfoCnvTestAlgTMEF.OutputLevel=VERBOSE
