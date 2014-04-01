# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: PanTauAnalysis/python/RunTimeHistos.py
# @purpose: Add a list of histograms to monitor variables in the seed builder

__doc__    = "Add a list of histograms to monitor variables in the seed builder"
__author__ = "C.Limbach"
#__version__= "$Revision:  $"


def getRTHList():
    
    #Note the hists have to be inside this variable:
    RunTimeHisto_List = []
    
    ###########################################
    # CREATION OF HISTOGRAMS
    #
    # NOTE: The NameOfHisto must match the string assigned to a variable in the TauSeedBuilderAlg!
    # NOTE: i.e. if a histo is created with
    # RunTimeHisto_List += ['(X)ImportantVariable', '(X) itsXaxistitle', 100, 0.0, 1.0]
    # where (for convenience) add the (X) bit like this:
    #   (G) for 'G'lobal variables that exist once per event
    #   (T) for 'T'au variables that exist once per tau
    #   (E) for 'E'FO variables that exist once per EFO
    #   (SY) for 'S'ingle variables that are filled with values directly (see below)
    #       In addition, specify whether this single variable is 
    #       (G) per Event   -> (SG)
    #       (T) per Tau     -> (ST)
    #       (E) per EFO     -> (SE)
    #       NOTE:   for the x axis title, G, T or E should suffice, since the S is only for
    #               internal convenience.
    
    ###########################################
    # FILLING OF HISTOGRAMS
    #
    # Filling is done in PanTauAlgs/src/TauSeedBuilderAlg.cxx
    # Histograms are filled at the end of each event.
    # The value that is to be filled has to be added to the RunTimeMonitoring by using
    #    m_RunTimeMonitoring->AddEventVariable("eventVar", eventVar);
    #    m_RunTimeMonitoring->AddTauVariable("tauVar", eventVar);
    #    m_RunTimeMonitoring->AddEFOVariable("efoVar", eventVar);
    #
    # Then, if FillHists() is invoked, it is looped through all the histograms given here in this
    # file, and for each histogram it is searched for a variable (Event, Tau or EFO) with the same name.
    # If such a variable is found (and this is the case if it has been added in the way described above),
    # then its value will be filled into the histogram.
    # Note that more than one value can be added to the variable (they are vectors)
    # 
    # Since there can be more than one tau per event, you need to make sure, that
    #    m_RunTimeMonitoring->AddTau();
    # is called exactly once at the beginning of the loop over the tau seeds.
    # Analogously, you need to call
    #    m_RunTimeMonitoring->AddEFO();
    # since a tau may consist of more than one EFO.
    #
    # There is also the possibility to fill histograms directly. This is done by (for instance)
    #    FillSingleHisto->("(SG)singleVar", 4711);
    #    FillSingleHisto->("(ST)singleVar", 4711);
    #    FillSingleHisto->("(SE)singleVar", 4711);
    # NOTE: It is sufficient to fill the single variables like this, because the value is dumped
    #       into the hist directly.
    #
    # WARNING:  If you use FillSingleHisto on a histo that is already filled with a variable
    #           that has been added by Add...Variable(...), you mess up your histograms!
    # HOWEVER:  If you stick with using the (X) prefix you should be save :)
    # STILL:    There is no protection in the framework to catch any errors you make in this context
    
    
    ###########################################
    # GLOBAL VARIABLES (i.e. once per event)
    #
    # RunTimeHisto_List += [['(G)', '(G)', , ,]]
    RunTimeHisto_List += [['(G)Number_TausInTauContainer',              '(G) N(#tau) in tauRec container', 101, -0.5, 100.5]]
    RunTimeHisto_List += [['(G)Number_EFOsInEFOContainer',              '(G) N(EFO) in eflow tauMode container', 201, -0.5, 200.5]]
    RunTimeHisto_List += [['(G)Number_InputSeeds',                      '(G) Number of input seeds for PanTau', 101, -0.5, 100.5]]
    RunTimeHisto_List += [['(G)Number_OutputSeeds',                     '(G) Number of output seeds', 101, -0.5, 100.5]]
    RunTimeHisto_List += [['(G)Number_GoodSeeds',                       '(G) Number of good seeds', 101, -0.5, 100.5]]
    RunTimeHisto_List += [['(G)Number_BadSeeds',                        '(G) Number of bad seeds', 101, -0.5, 100.5]]
    RunTimeHisto_List += [['(G)Number_BadSeeds_Invalid',                '(G) Number of bad seeds due to invalid seeds', 101, -0.5, 100.5]]
    RunTimeHisto_List += [['(G)Number_BadSeeds_FailPresel',             '(G) Number of bad seeds due to failed preselection', 101, -0.5, 100.5]]
    RunTimeHisto_List += [['(G)Number_InputSeeds-OutputSeeds',          '(G) N(input seeds) - N(output seeds)', 41, -20.5, 20.5]]
    RunTimeHisto_List += [['(G)Number_Used_EFOs_Sum',                          '(G) N(EFOs used) sum-region', 151, -0.5, 150.5]]
    RunTimeHisto_List += [['(G)Number_Used_EFOs_Core',                         '(G) N(EFOs used) core-region', 151, -0.5, 150.5]]
    RunTimeHisto_List += [['(G)Number_EFOsAvailable-EFOsUsed_Sum',      '(G) N(EFOs) - N(EFOs used)', 171, -20.5, 150.5]]
    RunTimeHisto_List += [['(G)Number_EFOsAvailable-EFOsUsed_Core',     '(G) N(EFOs) - N(EFOs used core)', 171, -20.5, 150.5]]
    RunTimeHisto_List += [['(G)EventsWithInvalidTauSeedsOnly',          '(G) All (x) tauRecs invalid', 21, -0.5, 20.5]]
    
    RunTimeHisto_List += [['(G)Vicinity_DeltaRBetweenTauRecCandidates',          '(G) #Delta R(#tau^{tauRec}_{i}, #tau^{tauRec}_{j})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(G)Vicinity_DeltaRMinTauRecsForSingleTauRecCand',    '(G) #Delta R_{min}(#tau^{tauRec}_{i}, #tau^{tauRec}_{j}) for each #tau^{tauRec}', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(G)Vicinity_DeltaRMinTauRecsInEvent',                '(G) #Delta R_{min}(#tau^{tauRec}_{i}, #tau^{tauRec}_{j}) in event', 400, 0.0, 8.0]]
    
    RunTimeHisto_List += [['(G)Usage_UnusedEFO_n',                      '(G) Number of unused EFO in event', 40, -0.5, 39.5]]
    RunTimeHisto_List += [['(G)Usage_UnusedEFO_nCharged',               '(G) Number of unused EFO^{#pm} in event', 40, -0.5, 39.5]]
    RunTimeHisto_List += [['(G)Usage_UnusedEFO_nNeutral',               '(G) Number of unused EFO^{0} in event', 40, -0.5, 39.5]]
    RunTimeHisto_List += [['(G)Usage_UnusedEFO_nInCone',                '(G) Number of unused EFOs inside 0.4 wrt tauSeed', 40, -0.5, 39.5]]
    RunTimeHisto_List += [['(G)Usage_UnusedEFO_nOutOfCone',             '(G) Number of unused EFOs outside 0.4 wrt tauSeed', 40, -0.5, 39.5]]
    RunTimeHisto_List += [['(G)Usage_UnusedEFO_nOutClusters-nUnused',   '(G) N(clusters outside 0.4 wrt tauSeed) - N(unused EFOs)', 41, -20.5, 20.5]]
    
    RunTimeHisto_List += [['(G)Usage_Unused_MinDrTau',                          '(G) #Delta R_{min}(EFO_{unused}, #tau)',                                      200, 0.3, 0.7]]
    RunTimeHisto_List += [['(G)Usage_Unused_MoreUnusedThanCluster_MinDrTau',    '(G) #Delta R_{min}(EFO_{unused}, #tau) for N(EFO_{unused}) > N(outcluster)',  200, 0.3, 0.7]]
    RunTimeHisto_List += [['(G)Usage_Unused_EqualUnusedThanCluster_MinDrTau',   '(G) #Delta R_{min}(EFO_{unused}, #tau) for N(EFO_{unused}) = N(outcluster)',  200, 0.3, 0.7]]
    RunTimeHisto_List += [['(G)Usage_Unused_LessUnusedThanCluster_MinDrTau',    '(G) #Delta R_{min}(EFO_{unused}, #tau) for N(EFO_{unused}) < N(outcluster)',  200, 0.3, 0.7]]
    
    
    ###########################################
    # TAU VARIABLES (i.e. once per tau)
    #
    # RunTimeHisto_List += [['(T)', '(T)', , ,]]
    RunTimeHisto_List += [['(T)InSeed_IsValid',                         '(T) Input Seed is valid', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)InSeed_NullTau',                         '(T) Input seed has null pointer to #tau_{tauRec}', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)InSeed_BadNumTrack',                     '(T) Input Seed has bad numTrack', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)InSeed_FlawedPt',                        '(T) Input Seed has bad p_{T}', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)InSeed_FlawedPtNullJet',                 '(T) Input Seed has bad p_{T} and null jet pointer', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)InSeed_FlawedPtBadJetPt',                '(T) Input Seed has bad p_{T} and its jet too', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)InSeed_numTrackAll',                     '(T) numTrack of all input tauRec seeds', 21, -0.5, 20.5]]
    RunTimeHisto_List += [['(T)InSeed_numTrackValid',                   '(T) numTrack of valid input tauRec seeds', 21, -0.5, 20.5]]
    
    RunTimeHisto_List += [['(T)InSeedClus_DeltaRTauJetClusterToTauJet',         '(T) #Delta R(#tau^{jet}_{cluster i}, #tau^{jet})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)InSeedClus_DeltaRTauJetClusterToTauSeed',        '(T) #Delta R(#tau^{jet}_{cluster i}, #tau^{seed})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)InSeedClus_DeltaRMaxTauJetClusterToTauJet',      '(T) #Delta R_{max}(#tau^{jet}_{cluster i}, #tau^{jet})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)InSeedClus_DeltaRMaxTauJetClusterToTauSeed',     '(T) #Delta R_{max}(#tau^{jet}_{cluster i}, #tau^{jet})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)InSeedClus_NClusterOutsideCollectionConeWrtSeed','(T) N(clusters) in tau outside of EFO collection cone wrt. Tau', 52, -1.5, 50.5]]
    RunTimeHisto_List += [['(T)InSeedClus_NClusterOutsideCollectionConeWrtJet', '(T) N(clusters) in tau outside of EFO collection cone wrt. Jet', 52, -1.5, 50.5]]
    
    RunTimeHisto_List += [['(T)InSeedClus_DeltaRMaxTauJetValidClusterToTauJet',      '(T) #Delta R_{max}(#tau^{jet}_{cluster i}, #tau^{jet}) for E_{T}(cluster)>0.1 GeV', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)InSeedClus_DeltaRMaxTauJetValidClusterToTauSeed',     '(T) #Delta R_{max}(#tau^{jet}_{cluster i}, #tau^{jet}) for E_{T}(cluster)>0.1 GeV', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)InSeedClus_NValidClusterOutsideCollectionConeWrtSeed','(T) N(clusters, E_{T}>0.1GeV) in tau outside of EFO collection cone wrt. Tau', 52, -1.5, 50.5]]
    RunTimeHisto_List += [['(T)InSeedClus_NValidClusterOutsideCollectionConeWrtJet', '(T) N(clusters, E_{T}>0.1GeV) in tau outside of EFO collection cone wrt. Jet', 52, -1.5, 50.5]]
    
    
    RunTimeHisto_List += [['(T)InSeedTrak_DeltaRTauSeedTrackToTauSeed',         '(T) #Delta R(#tau^{seed}_{track i}, #tau^{seed})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)InSeedTrak_DeltaRMaxTauSeedTrackToTauSeed',      '(T) #Delta R_{max}(#tau^{seed}_{track i}, #tau^{seed})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)InSeedTrak_NTracksOutsideCollectionConeWrtSeed', '(T) N(tracks) in tau outside of EFO collection cone wrt. Seed', 52, -1.5, 50.5]]
    
    RunTimeHisto_List += [['(T)InSeedTrak_DeltaRTauSeedTrackToTauJet',          '(T) #Delta R(#tau^{seed}_{track i}, #tau^{jet})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)InSeedTrak_DeltaRMaxTauSeedTrackToTauJet',       '(T) #Delta R_{max}(#tau^{seed}_{track i}, #tau^{jet})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)InSeedTrak_NTracksOutsideCollectionConeWrtJet',  '(T) N(tracks) in tau outside of EFO collection cone wrt. Jet', 52, -1.5, 50.5]]
    
    
    RunTimeHisto_List += [['(T)AllTauRec_AssociatedEFOs',                   '(T) Associated EFOs for tauRec with any numTrack', 21, -0.5, 20.5]]
    RunTimeHisto_List += [['(T)AllTauRec_AssociatedChargedEFOs',            '(T) Associated charged EFOs for tauRec with any numTrack', 21, -0.5, 20.5]]
    RunTimeHisto_List += [['(T)AllTauRec_AssociatedNeutralEFOs',            '(T) Associated neutral EFOs for tauRec with any numTrack', 21, -0.5, 20.5]]
    RunTimeHisto_List += [['(T)AllTauRec_DeltaRSeedToClosestAssocEFO',      '(T) #Delta R of associated EFO closest to tauRec with any numTrack', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)AllTauRec_DeltaRSeedToFarestAssocEFO',       '(T) #Delta R of associated EFO farest from tauRec with any numTrack', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)AllTauRec_DeltaRSeedToFarestAssocChrgEFO',   '(T) #Delta R of associated EFO^{#pm} farest from tauRec with any numTrack', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)AllTauRec_DeltaRSeedToFarestAssocNeutEFO',   '(T) #Delta R of associated EFO^{0} farest from tauRec with any numTrack', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)AllTauRec_DeltaRSeedToClosestEFO_NoEFOs',    '(T) #Delta R of closest EFO to tauRec with any numTrack without EFOs', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)AllTauRec_DeltaRSeedToClosestChrgEFO_NoEFOs','(T) #Delta R of closest EFO^{#pm} to tauRec with any numTrack without EFOs', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)AllTauRec_DeltaRSeedToClosestNeutEFO_NoEFOs','(T) #Delta R of closest EFO^{0} to tauRec with any numTrack without EFOs', 400, 0.0, 8.0]]
    
    RunTimeHisto_List += [['(T)MultTrkTauRec_AssociatedEFOs',                   '(T) Associated EFOs for tauRec with numTrack >5 or 0', 21, -0.5, 20.5]]
    RunTimeHisto_List += [['(T)MultTrkTauRec_AssociatedChargedEFOs',            '(T) Associated charged EFOs for tauRec_{=0}^{>5}', 21, -0.5, 20.5]]
    RunTimeHisto_List += [['(T)MultTrkTauRec_AssociatedNeutralEFOs',            '(T) Associated neutral EFOs for tauRec_{=0}^{>5}', 21, -0.5, 20.5]]
    RunTimeHisto_List += [['(T)MultTrkTauRec_DeltaRSeedToClosestAssocEFO',      '(T) #Delta R of associated EFO closest to tauRec_{=0}^{>5} seed', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)MultTrkTauRec_DeltaRSeedToFarestAssocEFO',       '(T) #Delta R of associated EFO farest from tauRec_{=0}^{>5} seed', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)MultTrkTauRec_DeltaRSeedToFarestAssocChrgEFO',   '(T) #Delta R of associated EFO^{#pm} farest from tauRec_{=0}^{>5} seed', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)MultTrkTauRec_DeltaRSeedToFarestAssocNeutEFO',   '(T) #Delta R of associated EFO^{0} farest from tauRec_{=0}^{>5} seed', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)MultTrkTauRec_DeltaRSeedToClosestEFO_NoEFOs',    '(T) #Delta R of closest EFO to tauRec_{=0}^{>5} seed without EFOs', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)MultTrkTauRec_DeltaRSeedToClosestChrgEFO_NoEFOs','(T) #Delta R of closest EFO^{#pm} to tauRec_{=0}^{>5} seed without EFOs', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)MultTrkTauRec_DeltaRSeedToClosestNeutEFO_NoEFOs','(T) #Delta R of closest EFO^{0} to tauRec_{=0}^{>5} seed without EFOs', 400, 0.0, 8.0]]
    
    
    RunTimeHisto_List += [['(T)Seed_AssociatedEFOs',                        '(T) Associated EFOs in seed', 31, -0.5, 30.5]]
    RunTimeHisto_List += [['(T)Seed_AssociatedChargedEFOs',                 '(T) Associated charged EFOs in seed', 31, -0.5, 30.5]]
    RunTimeHisto_List += [['(T)Seed_AssociatedNeutralEFOs',                 '(T) Associated neutral EFOs in seed', 31, -0.5, 30.5]]
    RunTimeHisto_List += [['(T)Seed_DeltaRSeedToClosestAssocEFO',           '(T) #Delta R of associated EFO closest to tauRec seed', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)Seed_DeltaRSeedToFarestAssocEFO',            '(T) #Delta R of associated EFO farest from tauRec seed', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)Seed_DeltaRSeedToFarestAssocChrgEFO',        '(T) #Delta R of associated EFO^{#pm} farest from tauRec seed', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)Seed_DeltaRSeedToFarestAssocNeutEFO',        '(T) #Delta R of associated EFO^{0} farest from tauRec seed', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)Seed_DeltaRSeedToClosestEFO_NoEFOs',         '(T) #Delta R of closest EFO to tauRec seed without EFOs', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)Seed_DeltaRSeedToClosestChrgEFO_NoEFOs',     '(T) #Delta R of closest EFO^{#pm} to tauRec seed without EFOs', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(T)Seed_DeltaRSeedToClosestNeutEFO_NoEFOs',     '(T) #Delta R of closest EFO^{0} to tauRec seed without EFOs', 400, 0.0, 8.0]]
    
    RunTimeHisto_List += [['(T)Presel_SeedPassesSelection',             '(T) Seed passes preselection', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)Presel_FailPt',                          '(T) Preselection: Fail p_{T} cut', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)Presel_FailEta',                         '(T) Preselection: Fail #eta cut', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)Presel_FailEtaLower',                    '(T) Preselection: Fail #eta_{min} cut (too small)', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)Presel_FailEtaUpper',                    '(T) Preselection: Fail #eta_{max} cut (too large)', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)Presel_FailAbsSumCharge',                '(T) Preselection: Fail cut on |#sum q(EFO)|', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)Presel_FailAbsSumChargeLower',           '(T) Preselection: Fail cut on |#sum q(EFO)|_{min} (too small)', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)Presel_FailAbsSumChargeUpper',           '(T) Preselection: Fail cut on |#sum q(EFO)|_{max} (too large)', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)Presel_FailNChargedEFO',                 '(T) Preselection: Fail cut on N(EFO^{#pm})', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)Presel_FailNChargedEFOLower',            '(T) Preselection: Fail cut on N(EFO^{#pm})_{min} (too small)', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(T)Presel_FailNChargedEFOUpper',            '(T) Preselection: Fail cut on N(EFO^{#pm})_{max} (too large)', 4, -1.5, 2.5]]
    
    
    
    RunTimeHisto_List += [['(T)Seed_NumTrack_ChargedEFOs',            '(T) numTrack(tauRec) - NChargedEFO', 41, -20.5, 20.5]]
    
    
    ###########################################
    # EFO VARIABLES (i.e. once per EFO in tauSeed)
    #
    # RunTimeHisto_List += [['(E)', '(E)', , ,]]
    RunTimeHisto_List += [['(E)ChrgEFOpt_ChrgEFOTrkpt',       '(E) Charged EFO pt - Charged EFO track pt [MeV]', 200, -10000.0, 10000.0]]
    
    RunTimeHisto_List += [['(E)EFOTruthMatch_EFOIsMatched',   '(E) Neutral EFO is matched to true pi0', 4, -1.5, 2.5]]
    RunTimeHisto_List += [['(E)EFOTruthMatch_Reso_Et',        '(E) True #pi^{0}: #frac{E^{reco}_{T} - E^{true}_{T}}{E^{true}_{T}}', 80, -1.0, 1.0]]
    RunTimeHisto_List += [['(E)EFOTruthMatch_Reso_Phi',       '(E) True #pi^{0}: #frac{#phi^{reco} - #phi^{true}}{#phi^{true}}', 80, -1.0, 1.0]]
    RunTimeHisto_List += [['(E)EFOTruthMatch_Reso_Eta',       '(E) True #pi^{0}: #frac{#eta^{reco} - #eta^{true}}{#eta^{true}}', 80, -1.0, 1.0]]
    RunTimeHisto_List += [['(E)EFOTruthMatch_Match_DeltaR',   '(E) #Delta R(#pi^{0}_{true}, EFO^{0})', 40, 0.0, 0.4]]
    
    RunTimeHisto_List += [['(E)AllEFOs_Charge',                     '(E) All EFOs: Charge',     7, -3.5, 3.5]]
    RunTimeHisto_List += [['(E)AllEFOs_E',                          '(E) All EFOs: E',          300, -5000, 25000]]
    RunTimeHisto_List += [['(E)AllEFOs_Et',                         '(E) All EFOs: Et',         300, -5000, 25000]]
    RunTimeHisto_List += [['(E)AllEFOs_Eta',                        '(E) All EFOs: Eta',        100, -5.0, 5.0]]
    RunTimeHisto_List += [['(E)AllEFOs_Phi',                        '(E) All EFOs: Phi',        64, -3.2, 3.2]]
    RunTimeHisto_List += [['(E)AllEFOs_MinDrTau',                   '(E) All EFOs: MinDrToTau', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)AllEFOs_MinDrTau_ChrgEFO',           '(E) All EFOs: MinDrToTau (EFO^{#pm})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)AllEFOs_MinDrTau_NeutEFO',           '(E) All EFOs: MinDrToTau (EFO^{0})', 400, 0.0, 8.0]]
    
    RunTimeHisto_List += [['(E)Usage_EFOIsUsedXTimes',                '(E) EFO is used x times',      12, -2.5, 9.5]]
    RunTimeHisto_List += [['(E)Usage_MultiUsedEFO_Charge',            '(E) MultiUsedEFO: Charge',     7, -3.5, 3.5]]
    RunTimeHisto_List += [['(E)Usage_MultiUsedEFO_E',                 '(E) MultiUsedEFO: E',          300, -5000, 25000]]
    RunTimeHisto_List += [['(E)Usage_MultiUsedEFO_Et',                '(E) MultiUsedEFO: Et',         300, -5000, 25000]]
    RunTimeHisto_List += [['(E)Usage_MultiUsedEFO_Eta',               '(E) MultiUsedEFO: Eta',        100, -5.0, 5.0]]
    RunTimeHisto_List += [['(E)Usage_MultiUsedEFO_Phi',               '(E) MultiUsedEFO: Phi',        64, -3.2, 3.2]]
    RunTimeHisto_List += [['(E)Usage_MultiUsedEFO_MinDrTau',          '(E) MultiUsedEFO: MinDrToTau', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)Usage_MultiUsedEFO_MinDrTau_ChrgEFO',  '(E) MultiUsedEFO: MinDrToTau (EFO^{#pm})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)Usage_MultiUsedEFO_MinDrTau_NeutEFO',  '(E) MultiUsedEFO: MinDrToTau (EFO^{0})', 400, 0.0, 8.0]]
    
    RunTimeHisto_List += [['(E)Usage_UsedEFO_Charge',                 '(E) UsedEFO: Charge',     7, -3.5, 3.5]]
    RunTimeHisto_List += [['(E)Usage_UsedEFO_E',                      '(E) UsedEFO: E',          300, -5000, 25000]]
    RunTimeHisto_List += [['(E)Usage_UsedEFO_Et',                     '(E) UsedEFO: Et',         300, -5000, 25000]]
    RunTimeHisto_List += [['(E)Usage_UsedEFO_Eta',                    '(E) UsedEFO: Eta',        100, -5.0, 5.0]]
    RunTimeHisto_List += [['(E)Usage_UsedEFO_Phi',                    '(E) UsedEFO: Phi',        64, -3.2, 3.2]]
    RunTimeHisto_List += [['(E)Usage_UsedEFO_MinDrTau',               '(E) UsedEFO: MinDrToTau', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)Usage_UsedEFO_MinDrTau_ChrgEFO',       '(E) UsedEFO: MinDrToTau (EFO^{#pm})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)Usage_UsedEFO_MinDrTau_NeutEFO',       '(E) UsedEFO: MinDrToTau (EFO^{0})', 400, 0.0, 8.0]]
    
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_Charge',               '(E) UnusedEFO: Charge',     7, -3.5, 3.5]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_E',                    '(E) UnusedEFO: E',          300, -5000, 25000]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_Et',                   '(E) UnusedEFO: Et',         300, -5000, 25000]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_Eta',                  '(E) UnusedEFO: Eta',        100, -5.0, 5.0]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_Phi',                  '(E) UnusedEFO: Phi',        64, -3.2, 3.2]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_MinDrTau',             '(E) UnusedEFO: MinDrToTau', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_MinDrTau_ChrgEFO',     '(E) UnusedEFO: MinDrToTau (EFO^{#pm})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_MinDrTau_NeutEFO',     '(E) UnusedEFO: MinDrToTau (EFO^{0})', 400, 0.0, 8.0]]
    
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_OutOfInDet_Charge',              '(E) UnusedEFO with |#eta| > 2.5: Charge',     7, -3.5, 3.5]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_OutOfInDet_E',                   '(E) UnusedEFO with |#eta| > 2.5: E',          300, -5000, 25000]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_OutOfInDet_Et',                  '(E) UnusedEFO with |#eta| > 2.5: Et',         300, -5000, 25000]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_OutOfInDet_Eta',                 '(E) UnusedEFO with |#eta| > 2.5: Eta',        100, -5.0, 5.0]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_OutOfInDet_Phi',                 '(E) UnusedEFO with |#eta| > 2.5: Phi',        64, -3.2, 3.2]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_OutOfInDet_MinDrTau',            '(E) UnusedEFO with |#eta| > 2.5: MinDrToTau', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_OutOfInDet_MinDrTau_ChrgEFO',    '(E) UnusedEFO with |#eta| > 2.5: MinDrToTau (EFO^{#pm})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)Usage_UnusedEFO_OutOfInDet_MinDrTau_NeutEFO',    '(E) UnusedEFO with |#eta| > 2.5: MinDrToTau (EFO^{0})', 400, 0.0, 8.0]]
    
    RunTimeHisto_List += [['(E)Usage_VetoedEFO_Charge',               '(E) VetoedEFO: Charge',     7, -3.5, 3.5]]
    RunTimeHisto_List += [['(E)Usage_VetoedEFO_E',                    '(E) VetoedEFO: E',          300, -5000, 25000]]
    RunTimeHisto_List += [['(E)Usage_VetoedEFO_Et',                   '(E) VetoedEFO: Et',         300, -5000, 25000]]
    RunTimeHisto_List += [['(E)Usage_VetoedEFO_Eta',                  '(E) VetoedEFO: Eta',        100, -5.0, 5.0]]
    RunTimeHisto_List += [['(E)Usage_VetoedEFO_Phi',                  '(E) VetoedEFO: Phi',        64, -3.2, 3.2]]
    RunTimeHisto_List += [['(E)Usage_VetoedEFO_MinDrTau',             '(E) VetoedEFO: MinDrToTau', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)Usage_VetoedEFO_MinDrTau_ChrgEFO',     '(E) VetoedEFO: MinDrToTau (EFO^{#pm})', 400, 0.0, 8.0]]
    RunTimeHisto_List += [['(E)Usage_VetoedEFO_MinDrTau_NeutEFO',     '(E) VetoedEFO: MinDrToTau (EFO^{0})', 400, 0.0, 8.0]]
    
    ###########################################
    # SPECIAL RUNTIME VARIABLES (i.e. once for all efos in event, or once for every EFO<->Track combination
    #
    #
    
    
    return RunTimeHisto_List
# end getRTHList

def getRTH2DList():
    
    #NOTE: This variable must be filled with the TH2F information:
    RunTimeHisto2D_List = []
    
    ###########################################
    # CREATION OF HISTOGRAMS
    #
    # To create a new TH2F for the RunTime Monitoring, add this to the RunTimeHisto2D_List
    #    RunTimeHisto2D_List += [['(A)variableX', '(A)variableY', 'xAxisTitle', 'yAxisTitle', nBinsX, MinX, MaxX, nBinsY, minY, MaxY]]
    # where (A) is (G), (T) or (E).
    #
    # These histograms are filled automatically, if variableX and variableY have been added via Add...Variables like described above.
    RunTimeHisto2D_List += [['(G)nEflowObjects_Total', '(G)nEflowObjects_Used', 'N(EFO) in event', 'N(EFO) used in seeds', 200, -0.5, 199.5, 200, -0.5, 199.5]]
    
    return RunTimeHisto2D_List
#end getRTH2DList

def configRunTimeHistos(   seedBuilder = None):
    
    if seedBuilder == None:
        print("PanTau: RunTimeHistConfig.py: No seedBuilder given! Not adding any RunTimeHists!")
        return
    # end if
    
    ###########################################
    # 1 DIMENSIONAL HISTOGRAMS
    # create the lists/vectors to be passed to the seedbuilder
    #print("RunTimeHistConfig.py: Gathering information about one dimenstional RunTimeHistograms...")
    HistNames       = []
    HistAxisTitle   = []
    HistNBins       = []
    HistMinBins     = []
    HistMaxBins     = []
    
    #include the file which contains the histogram information
    RunTimeHisto_List = getRTHList()
    
    #process contents of file
    for iRTH in RunTimeHisto_List:
        #print("RunTimeHistConfig.py: Adding information: %s / %s / %d / %d / %d" % (iRTH[0], iRTH[1], iRTH[2], iRTH[3], iRTH[4]))
        HistNames.append(iRTH[0])
        HistAxisTitle.append(iRTH[1])
        HistNBins.append(iRTH[2])
        HistMinBins.append(iRTH[3])
        HistMaxBins.append(iRTH[4])
    #end for
    
    #let seedbuilder know about the RTHs
    seedBuilder.RunTimeHists_Names      = HistNames
    seedBuilder.RunTimeHists_AxisTitles = HistAxisTitle
    seedBuilder.RunTimeHists_NBins      = HistNBins
    seedBuilder.RunTimeHists_MinBin     = HistMinBins
    seedBuilder.RunTimeHists_MaxBin     = HistMaxBins
    
    
    ###########################################
    # 2 DIMENSIONAL HISTOGRAMS
    # create the lists/vectors to be passed to the seedbuilder
    #print("RunTimeHistConfig.py: Gathering information about two dimenstional RunTimeHistograms...")
    Hist2DVarX      = []
    Hist2DVarY      = []
    Hist2DTitleX    = []
    Hist2DTitleY    = []
    HistnBinsX      = []
    HistMinBinsX    = []
    HistMaxBinsX    = []
    HistnBinsY      = []
    HistMinBinsY    = []
    HistMaxBinsY    = []
    
    #include the file which contains the histogram information
    RunTimeHisto_List2D = getRTH2DList()
    
    #process contents of file
    for iRTH2D in RunTimeHisto_List2D:
        #print("RunTimeHistConfig.py ====> New 2D RunTimeHisto:")
        #print("RunTimeHistConfig.py     Adding information for x (var, title, binning):  %s, %s  (%d: %d - %d)" % (iRTH2D[0], iRTH2D[2], iRTH2D[4], iRTH2D[5], iRTH2D[6]))
        #print("RunTimeHistConfig.py     Adding information for x (var, title, binning):  %s, %s  (%d: %d - %d)" % (iRTH2D[1], iRTH2D[3], iRTH2D[7], iRTH2D[8], iRTH2D[9]))
        
        Hist2DVarX.append(iRTH2D[0])
        Hist2DVarY.append(iRTH2D[1])
        Hist2DTitleX.append(iRTH2D[2])
        Hist2DTitleY.append(iRTH2D[3])
        
        HistnBinsX.append(iRTH2D[4])
        HistMinBinsX.append(iRTH2D[5])
        HistMaxBinsX.append(iRTH2D[6])
        
        HistnBinsY.append(iRTH2D[7])
        HistMinBinsY.append(iRTH2D[8])
        HistMaxBinsY.append(iRTH2D[9])
    #end for
    
    #let seedbuilder know about the RTHs
    #seedBuilder.RunTimeHists_Names      = HistNames
    #seedBuilder.RunTimeHists_AxisTitles = HistAxisTitle
    #seedBuilder.RunTimeHists_NBins      = HistNBins
    #seedBuilder.RunTimeHists_MinBin     = HistMinBins
    #seedBuilder.RunTimeHists_MaxBin     = HistMaxBins
    
    return
#end configRunTimeHistos

