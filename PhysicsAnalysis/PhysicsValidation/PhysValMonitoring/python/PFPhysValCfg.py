# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def PFlowPhysValCfg(inputFlags):

    result=ComponentAccumulator()

    from AthenaConfiguration.ComponentFactory import CompFactory
    monMan = CompFactory.AthenaMonManager("PhysValMonManager")
    monMan.ManualDataTypeSetup = True
    monMan.DataType            = "monteCarlo"
    monMan.Environment         = "altprod"
    monMan.ManualRunLBSetup    = True
    monMan.Run                 = 1
    monMan.LumiBlock           = 1
    monMan.FileKey = "PhysVal"
        
    THistSvc=CompFactory.THistSvc
   
    histsvc = THistSvc()
    histsvc.Output += ["PhysVal DATAFILE='PhysVal.root' OPT='RECREATE'"]
    result.addService(histsvc)

    from AthenaCommon.Constants import WARNING

    PhysValFE_charged=CompFactory.PhysValFE("PhysValFE_charged")
    PhysValFE_charged.OutputLevel= WARNING
    PhysValFE_charged.DetailLevel= 10
    PhysValFE_charged.EnableLumi=False
    PhysValFE_charged.FlowElementContainerName="JetETMissChargedParticleFlowObjects"
    PhysValFE_charged.useNeutralFE= False
    monMan.AthenaMonTools += [ PhysValFE_charged ]

    PhysValFE_neutral=CompFactory.PhysValFE("PhysValFE_neutral")
    PhysValFE_neutral.OutputLevel= WARNING
    PhysValFE_neutral.DetailLevel= 10
    PhysValFE_neutral.EnableLumi=False
    PhysValFE_neutral.FlowElementContainerName="JetETMissNeutralParticleFlowObjects"
    PhysValFE_neutral.useNeutralFE=True
    monMan.AthenaMonTools += [ PhysValFE_neutral ]

    result.addEventAlgo(monMan)

    return result
