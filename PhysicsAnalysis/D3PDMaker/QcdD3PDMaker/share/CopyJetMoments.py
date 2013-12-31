from JetMomentTools.JetCopyMomentsAlg import JetCopyMomentsAlg
topSequence+= JetCopyMomentsAlg('AntiKt4TopoEMJetsCoypMomAlg', JetCollectionSrc = 'AntiKt4TopoEMJets',JetCollectionDest = 'AntiKt4TopoEMNewJets',
                                Moments= ['EtaOrigin','PhiOrigin','MOrigin', 'BCH_CORR_CELL', 'BCH_CORR_JET', 'BCH_CORR_DOTX',
                                          'LArQuality','HECQuality',
                                          'isBadLooseMinus','isBadLoose','isBadMedium','isBadTight',
                                          'nTrk',       'sumPtTrk',
                                          'GSCFactorF','WidthFraction',
                                          'N_BAD_CELLS', 'ENG_BAD_CELLS', 'n90', 'NumTowers', 'ootFracCells5', 'ootFracCells10'])
topSequence+= JetCopyMomentsAlg('AntiKt6TopoEMJetsCoypMomAlg', JetCollectionSrc = 'AntiKt6TopoEMJets',JetCollectionDest = 'AntiKt6TopoEMNewJets',
                                Moments= ['EtaOrigin','PhiOrigin','MOrigin', 'BCH_CORR_CELL', 'BCH_CORR_JET', 'BCH_CORR_DOTX',
                                          'LArQuality','HECQuality',
                                          'isBadLooseMinus','isBadLoose','isBadMedium','isBadTight',
                                          'nTrk',       'sumPtTrk',
                                          'GSCFactorF','WidthFraction',
                                          'N_BAD_CELLS', 'ENG_BAD_CELLS', 'n90', 'NumTowers', 'ootFracCells5', 'ootFracCells10'])
topSequence+= JetCopyMomentsAlg('AntiKt4LCTopoJetsCoypMomAlg', JetCollectionSrc = 'AntiKt4LCTopoJets',JetCollectionDest = 'AntiKt4LCTopoNewJets',
                                Moments= ['EtaOrigin','PhiOrigin','MOrigin', 'BCH_CORR_CELL', 'BCH_CORR_JET', 'BCH_CORR_DOTX',
                                          'LArQuality','HECQuality',
                                          'isBadLooseMinus','isBadLoose','isBadMedium','isBadTight',
                                          'nTrk',       'sumPtTrk',
                                          'N_BAD_CELLS', 'ENG_BAD_CELLS', 'n90', 'NumTowers', 'ootFracCells5', 'ootFracCells10'])
topSequence+= JetCopyMomentsAlg('AntiKt6LCTopoJetsCoypMomAlg',JetCollectionSrc = 'AntiKt6LCTopoJets',JetCollectionDest = 'AntiKt6LCTopoNewJets',
                                Moments= ['EtaOrigin','PhiOrigin','MOrigin', 'BCH_CORR_CELL', 'BCH_CORR_JET', 'BCH_CORR_DOTX',
                                          'LArQuality','HECQuality',
                                          'isBadLooseMinus','isBadLoose','isBadMedium','isBadTight',
                                          'nTrk',       'sumPtTrk',
                                          'N_BAD_CELLS', 'ENG_BAD_CELLS', 'n90', 'NumTowers', 'ootFracCells5', 'ootFracCells10'])
topSequence+= JetCopyMomentsAlg('AntiKt5LCTopoJetsCoypMomAlg',JetCollectionSrc = 'AntiKt4LCTopoJets',JetCollectionDest = 'AntiKt5LCTopoNewJets',
                                Moments= ['EtaOrigin','PhiOrigin','MOrigin', 'BCH_CORR_CELL', 'BCH_CORR_JET', 'BCH_CORR_DOTX',
                                          'LArQuality','HECQuality',
                                          'isBadLooseMinus','isBadLoose','isBadMedium','isBadTight',
                                          'nTrk',       'sumPtTrk',
                                          'N_BAD_CELLS', 'ENG_BAD_CELLS', 'n90', 'NumTowers', 'ootFracCells5', 'ootFracCells10'])
