#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#


#
# Configure legacy L1
#
def createL1Calo( name , info, OutputLevel=0 ):
    
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaCommon.SystemOfUnits import GeV
    import re

    #chainName = info['chainName']
    L1Item = info['chainParts'][0]['L1threshold']
    L1thr = float( re.findall(r'\d+', L1Item)[0] )
    wp = 0 # default

    #                        [Default, Tight , Medium, Loose ]
    HadCoreCutMin          = [ 1.0   ,  1.0  ,  1.0  ,  1.0  ]
    HadCoreCutOff          = [-0.2   , -0,2  , -0.2  , -0.2  ]
    HadCoreSlope           = [ 1/23. ,  1/23.,  1/23.,  1/23.]
    EmIsolCutMin           = [ 2.0   ,  1.0  ,  1.0  ,  1.5  ]
    EmIsolCutOff           = [-1.8   , -2.6  , -2.0  , -1.8  ]
    EmIsolSlope            = [ 1/8.  ,  1/8. ,  1/8. ,  1/8. ]

    # Configure L1
    L1CaloTool = CompFactory.Trig.TrigEgammaEmulationL1CaloHypoTool(
                                name                   = name,
                                L1Item                 = L1Item,
                                L1Thr                  = L1thr * GeV,
                                HadCoreCutMin          = HadCoreCutMin[wp],
                                HadCoreCutOff          = HadCoreCutOff[wp],
                                HadCoreSlope           = HadCoreSlope[wp],
                                EmIsolCutMin           = EmIsolCutMin[wp],
                                EmIsolCutOff           = EmIsolCutOff[wp],
                                EmIsolSlope            = EmIsolSlope[wp],
                                IsolCutMax             = 50,
                                OutputLevel            = OutputLevel)

    return L1CaloTool