# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from eflowRec.eflowRecFlags import jobproperties

if jobproperties.eflowRecFlags.usePFTauFlowElementAssoc and jobproperties.eflowRecFlags.useFlowElements :
    from eflowRec.eflowRecConf import PFTauFlowElementAssoc
    PFTauFlowElementAssoc=PFTauFlowElementAssoc("PFTauFlowElementAssoc")
    topSequence += PFTauFlowElementAssoc

if jobproperties.eflowRecFlags.doFlowElementValidation and jobproperties.eflowRecFlags.useFlowElements:
    # since FE are not added at Tier0 yet, need to add it to the eflowRec routine
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    monMan = AthenaMonManager( "PhysValMonManager" )
    monMan.ManualDataTypeSetup = True
    monMan.DataType            = "monteCarlo"
    monMan.Environment         = "altprod"
    monMan.ManualRunLBSetup    = True
    monMan.Run                 = 1
    monMan.LumiBlock           = 1
    monMan.FileKey = "PhysVal"
    
          
    include("PhysValMonitoring/PhysValPFlow_FlowElements_jobOptions.py")
    
    for tool in monMan.AthenaMonTools:
        tool.EnableLumi = False
        tool.ManagerName = 'PhysValMonManager'
        
    from AthenaCommon.AppMgr import ServiceMgr
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
    svcMgr.THistSvc.Output += ["PhysVal DATAFILE='PhysVal.root' OPT='RECREATE'"]
    topSequence += monMan  
      
