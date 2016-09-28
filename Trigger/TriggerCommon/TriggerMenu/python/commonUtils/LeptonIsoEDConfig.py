# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetRec.TrigHLTJetRecConfig import TrigHLTEnergyDensity
class TrigHLTEnergyDensityCentral(TrigHLTEnergyDensity):
    """Supply a specific merge parameter for the anti-kt algorithm"""

    def __init__(self, name="TrigHLTEnergyDensityCentral",
                            toolname_stub="jetTriggerEnergyDensityTool_Central",
                            ed_merge_param=0.5, AbsRapidityMax=1.5,
                            caloClusterContainerSGKey="TopoCaloClusterMaker_topo_FS",
                            eventShapeSGKey="HLTTopoClusterIsoCentralEventShape") :
          super(TrigHLTEnergyDensityCentral, self).__init__(name=name,
                            toolname_stub=toolname_stub,
                            ed_merge_param=ed_merge_param,AbsRapidityMax=AbsRapidityMax,
                            caloClusterContainerSGKey=caloClusterContainerSGKey,
                            eventShapeSGKey=eventShapeSGKey)
    

class TrigHLTEnergyDensityForward(TrigHLTEnergyDensity):
    """Supply a specific merge parameter for the anti-kt algorithm"""

    def __init__(self, name="TrigHLTEnergyDensityForward",
                            toolname_stub="jetTriggerEnergyDensityTool_Forward",
                            ed_merge_param=0.5, AbsRapidityMin=1.5, AbsRapidityMax=3.0,
                            caloClusterContainerSGKey="TopoCaloClusterMaker_topo_FS",
                            eventShapeSGKey="HLTTopoClusterIsoForwardEventShape") :
          super(TrigHLTEnergyDensityForward, self).__init__(name=name,
                            toolname_stub=toolname_stub,
                            ed_merge_param=ed_merge_param, AbsRapidityMin=AbsRapidityMin,
                            AbsRapidityMax=AbsRapidityMax,
                            caloClusterContainerSGKey=caloClusterContainerSGKey,
                            eventShapeSGKey=eventShapeSGKey)

