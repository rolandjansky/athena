# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDAnalysis/python/egammaTimeCorrConfig.py
# @author Nikiforos K. Nikiforou <nikiforo@cern.ch> modified from egammaIsIsoConfig.py by scott snyder <snyder@bnl.gov>
# @date Jun, 2012
# @brief Configure egammaTimeCorrAlg to fill UserData.
#


from D3PDMakerConfig.D3PDMakerFlags          import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey         import resolveSGKey
from AthenaCommon.AlgSequence                import AlgSequence
import D3PDMakerCoreComps
import egammaD3PDAnalysis


def egammaTimeCorrConfig \
    (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
    prefix = '',
    sgkey = D3PDMakerFlags.ElectronSGKey(),
     typeName = 'DataVector<xAOD::Electron_v1>',
    allowMissing = False):
    """Configure egammaTimeCorrAlg for D3PD making.

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

    algName = 'egammaTimeCorrAlg' + resolved_sgkey
    if not hasattr (seq, algName):
        
        from IOVDbSvc.CondDB import conddb
        
        conddb.addFolder("LAR_OFL","/LAR/TimeCorrectionOfl/NonRunCon <tag>LARTimeCorrectionOflNonRunCon-00</tag>",force=True)
        conddb.addFolder("LAR_OFL","/LAR/TimeCorrectionOfl/RunCon <tag>LARTimeCorrectionOflRunCon-00</tag>",force=True)
       
        from CaloClusterCorrection.CaloClusterTimeTool import GetCaloClusterTimeTool 
        myCaloClusterTimeTool=GetCaloClusterTimeTool("caloClusterTimeTool", "/LAR/TimeCorrectionOfl/NonRunCon","/LAR/TimeCorrectionOfl/RunCon")
    
        seq += egammaD3PDAnalysis.egammaTimeCorrAlg \
                (algName,
                Getter = DVGetter
                    (prefix + 'egammaTimeCorrGetter',
                    TypeName = typeName,
                    SGKey = sgkey),
                AllowMissing = allowMissing,
                AuxPrefix = auxprefix,
                CaloClusterTimeTool = myCaloClusterTimeTool,
                                    )

    return
