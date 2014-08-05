# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: egammaNbCellsGainAlgConfig.py 
#
# @file egammaD3PDAnalysis/python/egamaNbCellsGainAlgConfig.py
# @date Aug. 2013
# @brief Configure egamaNbCellsGainAlg to fill UserData.
#


from D3PDMakerConfig.D3PDMakerFlags          import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey         import resolveSGKey
from AthenaCommon.AlgSequence                import AlgSequence
import D3PDMakerCoreComps
import egammaD3PDAnalysis



def egammaNbCellsGainAlgConfig \
    (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
    prefix = '',
    sgkey = D3PDMakerFlags.ElectronSGKey(),
    typeName = 'ElectronContainer',
    allowMissing = False):
    """Configure egammaNbCellsGainAlg for D3PD making.

    SEQ is the Gaudi sequence to which the algorithm should be added.
    Default is that given by PreD3PDAlgSeqName.

    PREFIX is a prefix to add to the name of the algorithm scheduled.

    SGKEY/TYPENAME is the StoreGate key of the input electron container
    and the name of its type.

    If ALLOWMISSING is true, don't fail if the SG key doesn't exist.
    """

    if (not D3PDMakerFlags.MakeEgammaUserData() or
        D3PDMakerFlags.HaveEgammaUserData()):
        return

    DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool
    resolved_sgkey = resolveSGKey (typeName, sgkey)
    auxprefix = (D3PDMakerFlags.EgammaUserDataPrefix() + '_' +
                 resolved_sgkey + '_')

    algName = 'egammaNbCellsGainAlg' + resolved_sgkey
    if not hasattr (seq, algName):
        
        myAlg = egammaD3PDAnalysis.egammaNbCellsGainAlg \
                (algName,
                Getter = DVGetter
                    (prefix + 'egammaNbCellsGainAlgGetter',
                    TypeName = typeName,
                    SGKey = sgkey),
                AllowMissing = allowMissing,
                AuxPrefix = auxprefix,)

        seq += myAlg

    return
