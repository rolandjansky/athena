# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, METConfig
from METReconstruction.METAssocConfig import METAssocConfig, AssocConfig

## Simple truth terms

cfg_truth = METConfig('Truth',
                      [BuildConfig('NonInt'),
                       BuildConfig('Int'),
                       BuildConfig('IntOut'),
                       BuildConfig('IntMuons')],
                      doRegions=True
                      )

metFlags.METConfigs()[cfg_truth.suffix] = cfg_truth
metFlags.METOutputList().append(cfg_truth.suffix)
metFlags.METOutputList().append(cfg_truth.suffix+"Regions")

### Truth association maps
#
#############################################################################
## AntiKt4LCTopo
#cfg_truthassoc_akt4lc = METAssocConfig('Truth_AntiKt4LCTopo',
#                                       [AssocConfig('Truth','AntiKt4LCTopoJets')],
#                                       doTruth=True
#                                       )
#
#metFlags.METAssocConfigs()[cfg_truthassoc_akt4lc.suffix] = cfg_truthassoc_akt4lc
#metFlags.METAssocOutputList().append(cfg_truthassoc_akt4lc.suffix)
#
#############################################################################
## AntiKt4EMTopo
#cfg_truthassoc_akt4em = METAssocConfig('Truth_AntiKt4EMTopo',
#                                       [AssocConfig('Truth','AntiKt4EMTopoJets')],
#                                       doTruth=True
#                                       )
#
#metFlags.METAssocConfigs()[cfg_truthassoc_akt4em.suffix] = cfg_truthassoc_akt4em
#metFlags.METAssocOutputList().append(cfg_truthassoc_akt4em.suffix)
#
#############################################################################
## AntiKt4EMPFlow
#
#from RecExConfig.RecFlags import rec
#if rec.doInDet() and metFlags.DoPFlow():
#    cfg_truthassoc_akt4pf = METAssocConfig('Truth_AntiKt4EMPFlow',
#                                           [AssocConfig('Truth','AntiKt4EMPFlowJets')],
#                                           doTruth=True
#                                           )
#
#    metFlags.METAssocConfigs()[cfg_truthassoc_akt4pf.suffix] = cfg_truthassoc_akt4pf
#    metFlags.METAssocOutputList().append(cfg_truthassoc_akt4pf.suffix)
