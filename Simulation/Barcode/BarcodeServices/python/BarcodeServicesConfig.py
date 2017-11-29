# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
BarcodeServices configurations
Elmar Ritsch, 23/10/2014
"""

from AthenaCommon import CfgMgr
from AthenaCommon.Constants import *  # FATAL,ERROR etc.


def getMC15BarcodeSvc(name="Barcode_MC15BarcodeSvc", **kwargs):
    kwargs.setdefault("FirstSecondaryVertexBarcode"   ,  -1000001 )
    kwargs.setdefault("VertexIncrement"               ,        -1 )
    kwargs.setdefault("FirstSecondaryBarcode"         ,   1000001 )
    kwargs.setdefault("SecondaryIncrement"            ,         1 )
    kwargs.setdefault("ParticleRegenerationIncrement" ,  10000000 )
    kwargs.setdefault("DoUnderAndOverflowChecks"      ,      True )
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",     False )
    return CfgMgr.Barcode__GenericBarcodeSvc(name, **kwargs)


def getMC12BarcodeSvc(name="Barcode_MC12BarcodeSvc", **kwargs):
    kwargs.setdefault("FirstSecondaryVertexBarcode" , -200001)
    kwargs.setdefault("VertexIncrement"             , -1)
    kwargs.setdefault("FirstSecondaryBarcode"       , 200001)
    kwargs.setdefault("SecondaryIncrement"          , 1)
    kwargs.setdefault("ParticleGenerationIncrement" , 1000000)
    return CfgMgr.Barcode__LegacyBarcodeSvc(name, **kwargs)


def getMC12LLPBarcodeSvc(name="Barcode_MC12LLPBarcodeSvc", **kwargs):
    return getMC12BarcodeSvc(name, **kwargs)


def getMC12PlusBarcodeSvc(name="Barcode_MC12PlusBarcodeSvc", **kwargs):
    return getMC12BarcodeSvc(name, **kwargs)


def getMC15aPlusBarcodeSvc(name="Barcode_MC15aPlusBarcodeSvc", **kwargs):
    return getMC12BarcodeSvc(name, **kwargs)


def getMC15aPlusLLPBarcodeSvc(name="Barcode_MC15aPlusLLPBarcodeSvc", **kwargs):
    return getMC12BarcodeSvc(name, **kwargs)


def getMC15aBarcodeSvc(name="Barcode_MC15aBarcodeSvc", **kwargs):
    return getMC12BarcodeSvc(name, **kwargs)


def getMC16BarcodeSvc(name="Barcode_MC16BarcodeSvc", **kwargs):
    return getMC12BarcodeSvc(name, **kwargs)


def getMC16LLPBarcodeSvc(name="Barcode_MC16LLPBarcodeSvc", **kwargs):
    return getMC12BarcodeSvc(name, **kwargs)


def getMC18BarcodeSvc(name="Barcode_MC18BarcodeSvc", **kwargs):
    kwargs.setdefault("FirstSecondaryVertexBarcode" ,  -1000001 )
    kwargs.setdefault("FirstSecondaryBarcode"       ,   1000001 )
    kwargs.setdefault("ParticleGenerationIncrement" ,  10000000 )
    return getMC12BarcodeSvc(name, **kwargs)


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


def getGlobalBarcodeSvc(name="Barcode_GlobalBarcodeSvc", **kwargs):
    kwargs.setdefault("FirstSecondaryVertexBarcode"   ,  -200000    );
    kwargs.setdefault("VertexIncrement"               ,  -1000000   );
    kwargs.setdefault("FirstSecondaryBarcode"         ,   200001    );
    kwargs.setdefault("SecondaryIncrement"            ,   1         );
    kwargs.setdefault("DoUnderAndOverflowChecks"      ,   True      );
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",   True      );
    return CfgMgr.Barcode__GlobalBarcodeSvc(name, **kwargs)


def getValidationBarcodeSvc(name="Barcode_ValidationBarcodeSvc", **kwargs):
    kwargs.setdefault("FirstSecondaryVertexBarcode" , -200001)
    kwargs.setdefault("VertexIncrement"             , -1)
    kwargs.setdefault("FirstSecondaryBarcode"       , 200001)
    kwargs.setdefault("SecondaryIncrement"          , 1)
    kwargs.setdefault("ParticleGenerationIncrement" , 1000000)
    kwargs.setdefault("DoUnderAndOverflowChecks"    , True)
    return CfgMgr.Barcode__ValidationBarcodeSvc(name, **kwargs)


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
    currentOffset=offsets.get(strategyName)
    if currentOffset==None:
        raise RuntimeError("No barcode offset listed for truth strategy named "+str(strategyName))
    return currentOffset
