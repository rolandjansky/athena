# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetRec.JetRecStandard import jtm
from JetRec.JetRecFlags import jetFlags
from AthenaCommon.GlobalFlags import globalflags

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkHasTruth

jtm.modifiersMap["calib_notruth"] = jtm.modifiersMap["lctopo_ungroomed"]
if DerivationFrameworkHasTruth:
    jtm.modifiersMap["calib_notruth"].remove('truthassoc')

jtm.modifiersMap["exotJetMods"] = jtm.modifiersMap["lctopo_groomed"]
if DerivationFrameworkHasTruth:
    jtm.modifiersMap["exotJetMods"] += [jtm.truthpartondr]

print "EXOT derivations use the calib_notruth modifier for jets defined as: ", jtm.modifiersMap["calib_notruth"]
print "EXOT derivations use the exotJetMods modifier for jets defined as: ", jtm.modifiersMap["exotJetMods"]


def scheduleMETCustomClustJet(jetcoll='AntiKt4EMTopo', clustColl='CaloCalTopoClusters',
                              outputlist="CustomMET", configlist="CustomMET"):
    from METReconstruction.METAssocConfig import METAssocConfig,AssocConfig
    associators = [AssocConfig('EMJet', jetcoll+'Jets'),
                   AssocConfig('Muon'),
                   AssocConfig('Ele'),
                   AssocConfig('Gamma'),
                   AssocConfig('Tau'),
                   AssocConfig('Soft',clustColl)]
    cfg = METAssocConfig(jetcoll,
                         associators,
                        )
    for assoc in cfg.assoclist:
        assoc.PrimVxColl = 'PrimaryVertices'
    
    from DerivationFrameworkJetEtMiss import METCommon
    METCommon.customMETConfigs.setdefault(configlist,{})[cfg.suffix] = cfg
    METCommon.maplist.append(cfg.suffix)
    METCommon.METLists.setdefault(outputlist,[]).append(cfg.suffix)

    #from METReconstruction.METRecoFlags import metFlags
    #metFlags.METAssocConfigs()[cfg.suffix] = cfg
