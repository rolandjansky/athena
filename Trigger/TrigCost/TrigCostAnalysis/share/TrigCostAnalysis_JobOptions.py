# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

jps.AthenaCommonFlags.FilesInput = ["myESD.pool.root"]
#import AthenaRootComps.ReadAthenaxAODHybrid # To be used once we have proper AOD/ESD files
import AthenaPoolCnvSvc.ReadAthenaPool

algseq = CfgMgr.AthSequencer("AthAlgSeq") #gets the main AthSequencer

from EnhancedBiasWeighter.EnhancedBiasWeighterConf import EnhancedBiasWeighter
enhancedBiasWeighter = EnhancedBiasWeighter()
enhancedBiasWeighter.RunNumber = 327265
enhancedBiasWeighter.UseBunchCrossingTool = False

trigCostAnalysis = CfgMgr.TrigCostAnalysis()
trigCostAnalysis.OutputLevel = DEBUG
trigCostAnalysis.RootStreamName = "COSTSTREAM"
trigCostAnalysis.BaseEventWeight = 1.0
trigCostAnalysis.EnhancedBiasTool = enhancedBiasWeighter
trigCostAnalysis.UseEBWeights = False

algseq += trigCostAnalysis

if not hasattr(svcMgr, 'THistSvc'): 
  svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["COSTSTREAM DATAFILE='TrigCostRoot_Results.root' OPT='RECREATE'"]
