# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
BarcodeServices configurations
Elmar Ritsch, 23/10/2014
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from BarcodeServices.BarcodeServicesConf import Barcode__LegacyBarcodeSvc

#Functions yet to be migrated:
#getMC15BarcodeSvc, getMC18BarcodeSvc, getMC18LLPBarcodeSvc, getPhysicsProcessBarcodeSvc, getMC15BarcodeSvc, getGlobalBarcodeSvc, getValidationBarcodeSvc, barcodeOffsetForTruthStrategy

def MC12BarcodeSvcCfg(ConfigFlags, name="Barcode_MC12BarcodeSvc", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FirstSecondaryVertexBarcode" , -200001)
    kwargs.setdefault("VertexIncrement"             , -1)
    kwargs.setdefault("FirstSecondaryBarcode"       , 200001)
    kwargs.setdefault("SecondaryIncrement"          , 1)
    kwargs.setdefault("ParticleGenerationIncrement" , 1000000)
    result.addService(Barcode__LegacyBarcodeSvc(name, **kwargs))
    return result


def getMC12LLPBarcodeSvc(name="Barcode_MC12LLPBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(name, **kwargs)


def getMC12PlusBarcodeSvc(name="Barcode_MC12PlusBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(name, **kwargs)


def MC15aPlusBarcodeSvcCfg(ConfigFlags, name="Barcode_MC15aPlusBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(ConfigFlags, name, **kwargs)


def getMC15aPlusLLPBarcodeSvc(name="Barcode_MC15aPlusLLPBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(name, **kwargs)


def getMC15aBarcodeSvc(name="Barcode_MC15aBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(name, **kwargs)


def getMC16BarcodeSvc(name="Barcode_MC16BarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(name, **kwargs)


def getMC16LLPBarcodeSvc(name="Barcode_MC16LLPBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(name, **kwargs)


def getMC18BarcodeSvc(name="Barcode_MC18BarcodeSvc", **kwargs):
    kwargs.setdefault("FirstSecondaryVertexBarcode" ,  -1000001 )
    kwargs.setdefault("FirstSecondaryBarcode"       ,   1000001 )
    kwargs.setdefault("ParticleGenerationIncrement" ,  10000000 )
    return MC12BarcodeSvcCfg(name, **kwargs)


def getMC18LLPBarcodeSvc(name="Barcode_MC18LLPBarcodeSvc", **kwargs):
    return getMC18BarcodeSvc(name, **kwargs)


def getPhysicsProcessBarcodeSvc(name="Barcode_PhysicsProcessBarcodeSvc", **kwargs):
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",  False  )
    kwargs.setdefault("FirstSecondaryVertexBarcode"   , -200000 )
    kwargs.setdefault("VertexIncrement"               , -1000000)
    kwargs.setdefault("FirstSecondaryBarcode"         ,  200001 )
    kwargs.setdefault("SecondaryIncrement"            ,  1      )
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",  True   )
    return getMC15BarcodeSvc(name, **kwargs)