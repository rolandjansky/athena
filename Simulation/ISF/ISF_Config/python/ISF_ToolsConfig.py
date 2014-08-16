# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags

from ISF_Config.ISF_jobProperties import ISF_Flags # IMPORTANT: Flags must be set before tools are retrieved


def getGlobalBarcodeSvc(name="ISF_GlobalBarcodeSvc", **kwargs):
    kwargs.setdefault("FirstVertexBarcode"            ,  -200000    );
    kwargs.setdefault("VertexIncrement"               ,  -1000000   );
    kwargs.setdefault("FirstSecondaryBarcode"         ,   200001    );
    kwargs.setdefault("SecondaryIncrement"            ,   1         );
    kwargs.setdefault("DoUnderAndOverflowChecks"      ,   True      );
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",   True      );
    #
    from BarcodeServices.BarcodeServicesConf import Barcode__GlobalBarcodeSvc
    return Barcode__GlobalBarcodeSvc(name, **kwargs)

def getMC12BarcodeSvc(name="ISF_MC12BarcodeSvc", **kwargs):
    kwargs.setdefault("FirstVertexBarcode"          , -200001)
    kwargs.setdefault("VertexIncrement"             , -1)
    kwargs.setdefault("FirstSecondaryBarcode"       , 200001)
    kwargs.setdefault("SecondaryIncrement"          , 1)
    kwargs.setdefault("ParticleGenerationIncrement" , 1000000)

    from BarcodeServices.BarcodeServicesConf import Barcode__LegacyBarcodeSvc
    return Barcode__LegacyBarcodeSvc(name, **kwargs)

def getValidationBarcodeSvc(name="ISF_ValidationBarcodeSvc", **kwargs):
    kwargs.setdefault("FirstVertexBarcode"          , -200001)
    kwargs.setdefault("VertexIncrement"             , -1)
    kwargs.setdefault("FirstSecondaryBarcode"       , 200001)
    kwargs.setdefault("SecondaryIncrement"          , 1)
    kwargs.setdefault("ParticleGenerationIncrement" , 1000000)
    kwargs.setdefault("DoUnderAndOverflowChecks"    , True)

    from BarcodeServices.BarcodeServicesConf import Barcode__ValidationBarcodeSvc
    return Barcode__ValidationBarcodeSvc(name, **kwargs)

def getPhysicsProcessBarcodeSvc(name="ISF_PhysicsProcessBarcodeSvc", **kwargs):
    kwargs.setdefault("FirstVertexBarcode"            ,  -200000    );
    kwargs.setdefault("VertexIncrement"               ,  -1000000   );
    kwargs.setdefault("FirstSecondaryBarcode"         ,   200001    );
    kwargs.setdefault("SecondaryIncrement"            ,   1         );
    kwargs.setdefault("DoUnderAndOverflowChecks"      ,   True      );
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",   True      );

    from BarcodeServices.BarcodeServicesConf import Barcode__GenericBarcodeSvc
    return Barcode__GenericBarcodeSvc(name, **kwargs)

def getLegacyBarcodeService(name="ISF_LegacyBarcodeService", **kwargs):
    from BarcodeServices.BarcodeServicesConf import Barcode__LegacyBarcodeSvc
    return Barcode__LegacyBarcodeSvc(name, **kwargs)

