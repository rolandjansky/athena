# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkNeuralNetworkUtils package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def NeuralNetworkToHistoToolCfg(flags, name="NeuralNetworkToHistoTool", **kwargs):
    acc = ComponentAccumulator()
    NeuralNetworkToHistoTool=CompFactory.Trk.NeuralNetworkToHistoTool(name, **kwargs)
    acc.setPrivateTools(NeuralNetworkToHistoTool)
    return acc
