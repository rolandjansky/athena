"""ComponentAccumulator BarcodeServices configurations

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def BarcodeSvcCfg(ConfigFlags, **kwargs):
    """Return the MCxBarcodeSvcCfg config flagged by Sim.TruthStrategy"""
    stratmap = {
        "MC12": MC12BarcodeSvcCfg,
        "MC15": MC15BarcodeSvcCfg,
        "MC12LLP": MC12LLPBarcodeSvcCfg,
        "MC12Plus": MC12PlusBarcodeSvcCfg,
        "MC15aPlus": MC15aPlusBarcodeSvcCfg,
        "MC15aPlusLLP": MC15aPlusLLPBarcodeSvcCfg,
        "MC15a": MC15aBarcodeSvcCfg,
        "MC16": MC16BarcodeSvcCfg,
        "MC16LLP": MC16LLPBarcodeSvcCfg,
        "MC18": MC18BarcodeSvcCfg,
        "MC18LLP": MC18LLPBarcodeSvcCfg,
        "PhysicsProcess": PhysicsProcessBarcodeSvcCfg,
        "Global": GlobalBarcodeSvcCfg,
        "Validation": ValidationBarcodeSvcCfg,
    }
    MCxCfg = stratmap[ConfigFlags.Sim.TruthStrategy]
    return MCxCfg(ConfigFlags, **kwargs)


def MC15BarcodeSvcCfg(ConfigFlags, name="Barcode_MC15BarcodeSvc", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FirstSecondaryVertexBarcode"   ,  -1000001 )
    kwargs.setdefault("VertexIncrement"               ,        -1 )
    kwargs.setdefault("FirstSecondaryBarcode"         ,   1000001 )
    kwargs.setdefault("SecondaryIncrement"            ,         1 )
    kwargs.setdefault("ParticleRegenerationIncrement" ,  10000000 )
    kwargs.setdefault("DoUnderAndOverflowChecks"      ,      True )
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",     False )
    svc = CompFactory.Barcode.GenericBarcodeSvc(name, **kwargs)
    result.addService(svc, primary=True)
    return result


def MC12BarcodeSvcCfg(ConfigFlags, name="Barcode_MC12BarcodeSvc", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FirstSecondaryVertexBarcode" , -200001)
    kwargs.setdefault("VertexIncrement"             , -1)
    kwargs.setdefault("FirstSecondaryBarcode"       , 200001)
    kwargs.setdefault("SecondaryIncrement"          , 1)
    kwargs.setdefault("ParticleGenerationIncrement" , 1000000)
    svc = CompFactory.Barcode.LegacyBarcodeSvc(name, **kwargs)
    result.addService(svc, primary=True)
    return result


def MC12LLPBarcodeSvcCfg(ConfigFlags, name="Barcode_MC12LLPBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(ConfigFlags, name, **kwargs)


def MC12PlusBarcodeSvcCfg(ConfigFlags, name="Barcode_MC12PlusBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(ConfigFlags, name, **kwargs)


def MC15aPlusBarcodeSvcCfg(ConfigFlags, name="Barcode_MC15aPlusBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(ConfigFlags, name, **kwargs)


def MC15aPlusLLPBarcodeSvcCfg(ConfigFlags, name="Barcode_MC15aPlusLLPBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(ConfigFlags, name, **kwargs)


def MC15aBarcodeSvcCfg(ConfigFlags, name="Barcode_MC15aBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(ConfigFlags, name, **kwargs)


def MC16BarcodeSvcCfg(ConfigFlags, name="Barcode_MC16BarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(ConfigFlags, name, **kwargs)


def MC16LLPBarcodeSvcCfg(ConfigFlags, name="Barcode_MC16LLPBarcodeSvc", **kwargs):
    return MC12BarcodeSvcCfg(ConfigFlags, name, **kwargs)


def MC18BarcodeSvcCfg(ConfigFlags, name="Barcode_MC18BarcodeSvc", **kwargs):
    kwargs.setdefault("FirstSecondaryVertexBarcode" ,  -1000001 )
    kwargs.setdefault("FirstSecondaryBarcode"       ,   1000001 )
    kwargs.setdefault("ParticleGenerationIncrement" ,  10000000 )
    return MC12BarcodeSvcCfg(ConfigFlags, name, **kwargs)


def MC18LLPBarcodeSvcCfg(ConfigFlags, name="Barcode_MC18LLPBarcodeSvc", **kwargs):
    return MC18BarcodeSvcCfg(ConfigFlags, name, **kwargs)


def PhysicsProcessBarcodeSvcCfg(ConfigFlags, name="Barcode_PhysicsProcessBarcodeSvc", **kwargs):
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",  False  )
    kwargs.setdefault("FirstSecondaryVertexBarcode"   , -200000 )
    kwargs.setdefault("VertexIncrement"               , -1000000)
    kwargs.setdefault("FirstSecondaryBarcode"         ,  200001 )
    kwargs.setdefault("SecondaryIncrement"            ,  1      )
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",  True   )
    return MC15BarcodeSvcCfg(ConfigFlags, name, **kwargs)


def GlobalBarcodeSvcCfg(ConfigFlags, name="Barcode_GlobalBarcodeSvc", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FirstSecondaryVertexBarcode"   ,  -200000  )
    kwargs.setdefault("VertexIncrement"               ,  -1000000 )
    kwargs.setdefault("FirstSecondaryBarcode"         ,   200001  )
    kwargs.setdefault("SecondaryIncrement"            ,   1       )
    kwargs.setdefault("DoUnderAndOverflowChecks"      ,   True    )
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",   True    )
    svc = CompFactory.Barcode.GlobalBarcodeSvc(name, **kwargs)
    result.addService(svc, primary=True)
    return result


def ValidationBarcodeSvcCfg(ConfigFlags, name="Barcode_ValidationBarcodeSvc", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FirstSecondaryVertexBarcode" , -200001)
    kwargs.setdefault("VertexIncrement"             , -1)
    kwargs.setdefault("FirstSecondaryBarcode"       , 200001)
    kwargs.setdefault("SecondaryIncrement"          , 1)
    kwargs.setdefault("ParticleGenerationIncrement" , 1000000)
    kwargs.setdefault("DoUnderAndOverflowChecks"    , True)
    svc = CompFactory.Barcode.ValidationBarcodeSvc(name, **kwargs)
    result.addService(svc, primary=True)
    return result


def barcodeOffsetForTruthStrategy(strategyName):
    offsets = {'MC12':         200000,
               'MC12LLP':      200000,
               'MC12Plus':     200000,
               'MC15a':        200000,
               'MC15aPlus':    200000,
               'MC15aPlusLLP': 200000,
               'MC15':        1000000,
               'MC16':         200000,
               'MC16LLP':      200000,
               'MC18':        1000000,
               'MC18LLP':     1000000,
               'Validation':   200000
               }
    currentOffset = offsets.get(strategyName)
    if currentOffset is None:
        raise RuntimeError("No barcode offset listed for truth strategy named "+str(strategyName))
    return currentOffset
