# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
# add tools and algorithms to the ConfiguredFactory, see:
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConfiguredFactory
addTool('InDetRecExample.InDetRecExampleConfig.KalmanCompetingPixelClustersTool','KalmanCompetingPixelClustersTool')
addTool('InDetRecExample.InDetRecExampleConfig.KalmanCompetingSCT_ClustersTool','KalmanCompetingSCT_ClustersTool')
# addTool('InDetRecExample.InDetRecExampleConfig.KalmanCompetingRIOsTool','KalmanCompetingRIOsTool')
addTool('InDetRecExample.InDetRecExampleConfig.KalmanCompetingRIOsTool','KalmanCompetingRIOsTool')
addTool('InDetRecExample.InDetRecExampleConfig.InDetDNAdjustor','InDetDNAdjustor')
addTool('InDetRecExample.InDetRecExampleConfig.InDetDNASeparator','InDetDNASeparator')
addTool('InDetRecExample.InDetRecExampleConfig.InDetAnnealFKFRef','InDetAnnealFKFRef')
addTool('InDetRecExample.InDetRecExampleConfig.InDetFKFRef','InDetFKFRef')
addTool('InDetRecExample.InDetRecExampleConfig.InDetAnnealFKF','InDetAnnealFKF')
addTool('InDetRecExample.InDetRecExampleConfig.InDetFKF','InDetFKF')
addTool('InDetRecExample.InDetRecExampleConfig.InDetBKS','InDetBKS')
addTool('InDetRecExample.InDetRecExampleConfig.InDetAnnealBKS','InDetAnnealBKS')

addTool('InDetRecExample.InDetRecExampleConfig.KalmanInternalDAF','KalmanInternalDAF')
addTool('InDetRecExample.InDetRecExampleConfig.KalmanInternalDAFRef','KalmanInternalDAFRef')
addTool('InDetRecExample.InDetRecExampleConfig.KOL_RecoveryID','KOL_RecoveryID')
addTool('InDetRecExample.InDetRecExampleConfig.InDetKOL','InDetKOL')
addTool('InDetRecExample.InDetRecExampleConfig.InDetMeasRecalibST','InDetMeasRecalibST')
# addTool('InDetRecExample.InDetRecExampleConfig.InDetKalmanTrackFitterBase','InDetKalmanTrackFitterBase')
addTool('InDetRecExample.InDetRecExampleConfig.KalmanFitter','KalmanFitter')
addTool('InDetRecExample.InDetRecExampleConfig.ReferenceKalmanFitter','ReferenceKalmanFitter')
addTool('InDetRecExample.InDetRecExampleConfig.KalmanDNAFitter','KalmanDNAFitter')
