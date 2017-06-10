# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
BarcodeServices configurations
Elmar Ritsch, 23/10/2014
"""

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags

def getMC15BarcodeSvc(name="Barcode_MC15BarcodeSvc", **kwargs):
    kwargs.setdefault("FirstSecondaryVertexBarcode"   ,  -1000001 )
    kwargs.setdefault("VertexIncrement"               ,        -1 )
    kwargs.setdefault("FirstSecondaryBarcode"         ,   1000001 )
    kwargs.setdefault("SecondaryIncrement"            ,         1 )
    kwargs.setdefault("ParticleRegenerationIncrement" ,  10000000 )
    kwargs.setdefault("DoUnderAndOverflowChecks"      ,      True )
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",     False )
    #
    from BarcodeServices.BarcodeServicesConf import Barcode__GenericBarcodeSvc
    return Barcode__GenericBarcodeSvc(name, **kwargs)

def getMC12BarcodeSvc(name="Barcode_MC12BarcodeSvc", **kwargs):
    kwargs.setdefault("FirstSecondaryVertexBarcode" , -200001)
    kwargs.setdefault("VertexIncrement"             , -1)
    kwargs.setdefault("FirstSecondaryBarcode"       , 200001)
    kwargs.setdefault("SecondaryIncrement"          , 1)
    kwargs.setdefault("ParticleGenerationIncrement" , 1000000)
    #
    from BarcodeServices.BarcodeServicesConf import Barcode__LegacyBarcodeSvc
    return Barcode__LegacyBarcodeSvc(name, **kwargs)

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

def getPhysicsProcessBarcodeSvc(name="Barcode_PhysicsProcessBarcodeSvc", **kwargs):
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",  False  )
    kwargs.setdefault("FirstSecondaryVertexBarcode"   , -200000 )
    kwargs.setdefault("VertexIncrement"               , -1000000)
    kwargs.setdefault("FirstSecondaryBarcode"         ,  200001 )
    kwargs.setdefault("SecondaryIncrement"            ,  1      )
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",  True   )
    #
    getMC15BarcodeSvc(name, **kwargs)

def getGlobalBarcodeSvc(name="Barcode_GlobalBarcodeSvc", **kwargs):
    kwargs.setdefault("FirstSecondaryVertexBarcode"   ,  -200000    );
    kwargs.setdefault("VertexIncrement"               ,  -1000000   );
    kwargs.setdefault("FirstSecondaryBarcode"         ,   200001    );
    kwargs.setdefault("SecondaryIncrement"            ,   1         );
    kwargs.setdefault("DoUnderAndOverflowChecks"      ,   True      );
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",   True      );
    #
    from BarcodeServices.BarcodeServicesConf import Barcode__GlobalBarcodeSvc
    return Barcode__GlobalBarcodeSvc(name, **kwargs)


def getValidationBarcodeSvc(name="Barcode_ValidationBarcodeSvc", **kwargs):
    kwargs.setdefault("FirstSecondaryVertexBarcode" , -200001)
    kwargs.setdefault("VertexIncrement"             , -1)
    kwargs.setdefault("FirstSecondaryBarcode"       , 200001)
    kwargs.setdefault("SecondaryIncrement"          , 1)
    kwargs.setdefault("ParticleGenerationIncrement" , 1000000)
    kwargs.setdefault("DoUnderAndOverflowChecks"    , True)
    #
    from BarcodeServices.BarcodeServicesConf import Barcode__ValidationBarcodeSvc
    return Barcode__ValidationBarcodeSvc(name, **kwargs)

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
               'Validation':   200000
               }
    currentOffset=offsets.get(strategyName)
    if currentOffset==None:
        raise RuntimeError("No barcode offset listed for truth strategy named "+str(strategyName))
    return currentOffset
