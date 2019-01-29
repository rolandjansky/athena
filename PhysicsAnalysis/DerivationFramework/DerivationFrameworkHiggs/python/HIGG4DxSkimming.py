# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#################################################
# Common code used for the HIGG4 skimming       #
# Z.Zinonos                                     #
# zenon@cern.ch                                 #
# Nov 2015                                      #
#################################################

from AthenaCommon.GlobalFlags import globalflags

def setup(HIGG4DxName, ToolSvc):

    skimmingTools = []

    #muon quality
    muonQual = "Muons.DFCommonMuonsPreselection"
    
    #electron quality
    eleLoose  = "(Electrons.DFCommonElectronsLHLoose)"
    eleMedium = "(Electrons.DFCommonElectronsLHMedium)"
    eleTight  = "(Electrons.DFCommonElectronsLHTight)"

    # el calo crack region: 1.52 < el->caloCluster()->etaBE(2) < 1.37,  with el->eta: apply a 5% offset 
    # veto events with el->caloCluster()->etaBE(2) ) < 2.47, with el->eta: apply a 5% offset 
    #muonVeto = '(count( Muons.pt > 15.0*GeV && abs(Muons.eta) < 2.5 && '+muonTight+' ) == 0)' #not supported since 18/5/2016
    eleVeto = '(count( Electrons.pt > 15.0*GeV && (abs(Electrons.eta) < 1.30 || abs(Electrons.eta) > 1.60) && abs(Electrons.eta) < 2.35  && '+eleTight+' ) == 0)'
    lepVeto = eleVeto  #+ '&& '+ muonVeto

    #tau prongs and tracks
    tauProngs = "abs(TauJets.charge)==1.0 && (TauJets.nTracks == 1 || TauJets.nTracks == 3)"
    tauTracks = "(TauJets.nTracks == 1 || TauJets.nTracks == 3)"
    
    tauProngs123 = "( ( abs(TauJets.charge)==1.0 && (TauJets.nTracks == 1 || TauJets.nTracks == 3) ) || (TauJets.pt > 100.0*GeV && TauJets.nTracks == 2 ) )"
    tauTracks123 = "(TauJets.nTracks == 1 || TauJets.nTracks == 2 || TauJets.nTracks == 3)"

    if HIGG4DxName == 'HIGG4D1':     
        muonLead = "Muons.pt > 13.0*GeV && abs(Muons.eta) < 2.7 && " + muonQual
        muonSubL = "Muons.pt > 9.0*GeV && abs(Muons.eta) < 2.7 && " + muonQual
        electron = "Electrons.pt > 13.0*GeV && " + eleLoose
        count1 = "( count("+muonLead+") + count("+electron+") ) >=1"
        count2 = "( count("+muonSubL+") + count("+electron+") ) >=2"
        skim_expression = "("+count1+") && ("+count2+")"

    elif HIGG4DxName == 'HIGG4D2':
        #MuTrig: (pT(mu)>18 && pT(tau)>18) || (pT(mu)>12 && pT(tau)>23)  EleTrig: (pT(el)>22 && pT(tau)>18) || (pT(el)>15 && pT(tau)>23)
        e15 = '(count( Electrons.pt > 15.0*GeV && '+eleMedium+') >= 1)'
        e22 = '(count( Electrons.pt > 22.0*GeV && '+eleMedium+') >= 1)'
        mu12 = '(count( Muons.pt > 12.0*GeV && abs(Muons.eta) < 2.5 && '+muonQual+' ) >= 1)'
        mu18 = '(count( Muons.pt > 18.0*GeV && abs(Muons.eta) < 2.5 && '+muonQual+' ) >= 1)'
        tau18 = '(count( TauJets.pt > 18.0*GeV && '+tauProngs123+' ) >= 1)'
        tau23 = '(count( TauJets.pt > 23.0*GeV && '+tauProngs123+' ) >= 1)'
        mutau = '('+mu18+' && '+tau18+') || ('+mu12+' && '+tau23+')'
        etau = '('+e22+' && '+tau18+') || ('+e15+' && '+tau23+')'
        skim_expression = '('+mutau+') || ('+etau+')'

    elif HIGG4DxName == 'HIGG4D3':
        tauTrks = '( (TauJets.nTracks + TauJets.nTracksIsolation >= 1) && (TauJets.nTracks + TauJets.nTracksIsolation <= 8) )'
        tauLead = '(TauJets.pt > 33.0*GeV)'
        tauSubl = '(TauJets.pt > 23.0*GeV)'
        tauId   = '(HIGG4DxJetBDTSigLoose || HIGG4DxJetRNNSigLoose)'
        tauReq0 = '(count( '+tauSubl+' && '+tauTrks+' ) >= 2)'
        tauReq1 = '(count( '+tauSubl+' && '+tauTrks+' && '+tauId+' ) >= 1)'
        tauReq2 = '(count( '+tauLead+' && '+tauTrks+' ) >= 1)'
        skim_expression = tauReq0 + '&&' + tauReq1 + '&&' + tauReq2 + '&&'  + lepVeto

    elif HIGG4DxName == 'HIGG4D4':
        ditau = '(count( '+tauProngs123+' && TauJets.pt > 45.0*GeV) >= 2)'
        tau1 = '(count((TauJets.pt > 100.0*GeV)) >= 1)'
        tau2 = '(count((TauJets.pt > 45.0*GeV)) >= 2)'
        tauTrack = '(count('+tauTracks123+' && TauJets.pt > 45.0*GeV) >= 1)'
        trigger = '( HLT_j15 || HLT_j25 || HLT_j35 || HLT_j55 || HLT_j60 || HLT_j85 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j260 || HLT_j300 || HLT_j320 || HLT_j360 || HLT_j380 || HLT_j400 || HLT_j420 || HLT_j440 || HLT_j460 || HLT_tau80_medium1_tracktwo_L1TAU60 || HLT_tau125_medium1_tracktwo || HLT_tau160_medium1_tracktwo || HLT_tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12 || HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM || HLT_tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40 || HLT_j450 || HLT_tau160_medium1_tracktwo_L1TAU100 || HLT_tau160_medium1_tracktwoEF_L1TAU100 || HLT_tau160_mediumRNN_tracktwoMVA_L1TAU100)'
        DFisMC = (globalflags.DataSource()=='geant4')
        if not DFisMC:
            hadhad = '(((' + ditau + ') || (' + tau1 + ' && ' + tau2 + ' && ' + tauTrack + ')) && ' + trigger + ')'
        else:
            hadhad = ditau 
        skim_expression = hadhad + "&&" + lepVeto

    elif HIGG4DxName == 'HIGG4D5':
        tau = '(count('+tauTracks123+' && TauJets.pt > 30.0*GeV) >= 1)'
        trigger_main = '( HLT_xe70 || HLT_xe70_mht || HLT_xe90_L1XE50 || HLT_xe90_mht_L1XE50 || HLT_xe110_L1XE50 || HLT_xe110_mht_L1XE50 || HLT_j360 || HLT_j380 || HLT_tau80_medium1_tracktwo_L1TAU60 || HLT_tau125_medium1_tracktwo || HLT_tau160_medium1_tracktwo || HLT_noalg_L1J400 || HLT_xe110_pufit_L1XE55 || HLT_j400 || HLT_j420 || HLT_j450 ||  HLT_tau160_medium1_tracktwo_L1TAU100 || HLT_xe90_pufit_L1XE50 || HLT_xe100_pufit_L1XE55 || HLT_xe100_pufit_L1XE50 || HLT_xe110_pufit_L1XE50 || HLT_tau35_medium1_tracktwo_xe70_L1XE45 || HLT_mu50 ||  HLT_mu26_ivarmedium || HLT_xe110_pufit_xe65_L1XE50 || HLT_xe110_pufit_xe70_L1XE50 || HLT_xe120_pufit_L1XE50 || HLT_tau35_medium1_tracktwoEF_xe70_L1XE45 || HLT_tau35_mediumRNN_tracktwoMVA_xe70_L1XE45 || HLT_tau160_medium1_tracktwoEF_L1TAU100 || HLT_tau160_mediumRNN_tracktwoMVA_L1TAU100) '
        trigger_aux = '( HLT_j15 || HLT_j25 || HLT_j35 || HLT_j55 || HLT_j60 || HLT_j85 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j260 || HLT_j300 || HLT_j320 )'
        DFisMC = (globalflags.DataSource()=='geant4')
        if not DFisMC:
            monotau = '('+tau+' && ('+trigger_main+' || '+trigger_aux+'))'
        else:
            monotau = '('+tau+' && '+trigger_main+')'
        skim_expression = monotau + "&&" + lepVeto
    
    elif HIGG4DxName == 'HIGG4D6':
        # here we only apply selection based on trigger and DiTau. After this skim, fat jet building is called. Second DerivationKernel will then apply selection based on jat jets (see below the skimming tool setup)
        ditaujet = '(count((DiTauJets.pt > 300.0*GeV)) >= 1)'
        trigger_main     = '( HLT_j360_a10r_L1J100 || HLT_j360_a10_lcw_L1J100 || HLT_j400_a10r_L1J100 || HLT_j400_a10_lcw_L1J100 || HLT_j420_a10_lcw_L1J100 || HLT_j420_a10r_L1J100 || HLT_j390_a10t_lcw_jes_30smcINF_L1J100 || HLT_j420_a10r_L1J100 || HLT_j420_a10t_lcw_jes_L1J100 || HLT_j420_a10_lcw_subjes_L1J100 || HLT_j420_a10t_lcw_jes_40smcINF_L1J100 || HLT_j440_a10r_L1J100 || HLT_j440_a10t_lcw_jes_L1J100 || HLT_j440_a10_lcw_subjes_L1J100 || HLT_j440_a10t_lcw_jes_40smcINF_L1J100 || HLT_j460_a10r_L1J100 || HLT_j460_a10t_lcw_jes_L1J100 || HLT_j460_a10_lcw_subjes_L1J100 || HLT_j480_a10r_L1J100 || HLT_j480_a10t_lcw_jes_L1J100 || HLT_j480_a10_lcw_subjes_L1J100 )'
        trigger_electron = '(HLT_e26_lhtight_nod0_ivarloose || HLT_e60_lhmedium_nod0 || HLT_e140_lhloose_nod0 || HLT_e300_etcut)'  # for HH->WWtautau analysis
        trigger_muon     = '(HLT_mu26_ivarmedium || HLT_mu50 || HLT_mu60_0eta105_msonly)'  # for HH->WWtautau analysis
        trigger_all      = '({} || {} || {})'.format(trigger_main, trigger_electron, trigger_muon)
        skim_expression = ditaujet + "&&" + trigger_all

    elif HIGG4DxName == 'HDBS1':
	#NADAV- Removed ditau cut, removed jet triggers 
	#NADAV- Added lepton selection (Quality, pt, eta)
        #NADAV- Added jet requirement. The final selection here (exactly on at least two leptons +  at least 2 jets ; this covers all WW decay channels )
        eReq = '( Electrons.pt > 28.0*GeV && abs(Electrons.eta) < 2.5 && '+eleTight+' )'
        muReq = '( Muons.pt > 28.0*GeV && abs(Muons.eta) < 2.5 && '+muonQual+' )'
        jetReq = '( AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 30.0*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.5 )' 
        trigger_electron = '(HLT_e26_lhtight_nod0_ivarloose || HLT_e60_lhmedium_nod0 || HLT_e140_lhloose_nod0 || HLT_e300_etcut)' 
        trigger_muon     = '(HLT_mu26_ivarmedium || HLT_mu50 || HLT_mu60_0eta105_msonly)' 
        onelep = '( ((count('+eReq+') == 1) && ('+trigger_electron+') && (count('+muReq+') == 0)) || ((count('+muReq+') == 1) && ('+trigger_muon+') && (count('+eReq+') == 0)) )' #Single Lepton (with matching trigger, changed now to include no overlap (exactly 1 electron and trigger or 1 muon and trigger)
        dilep = '( (count('+eReq+') == 1) && ('+trigger_electron+') && (count('+muReq+') == 1) && ('+trigger_muon+') )' #Exactly two leptons with matching triggers
        multilep = '( ((count('+eReq+') >= 2) && ('+trigger_electron+')) || ((count('+muReq+') >= 2) && ('+trigger_muon+')) || ('+dilep+') )' #Multilepton events with matching trigger
        fullLep = '( '+multilep+' && (count('+jetReq+') >= 2) )' #At least 2 jets (from t->bW and fully leptonic Ws)
        semiLep = '( '+onelep+' && (count('+jetReq+') >= 3) )' #At least 3 jets (from t->bW and semileptonic Ws) 
        #trigger_all      = '({} || {})'.format(trigger_electron, trigger_muon)
        skim_expression = semiLep  + "||" + fullLep 
    
    else:
        assert False, "HIGG4DxSkimming: Unknown derivation stream '{}'".format(HIGG4DxName)

    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
    HIGG4DxSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name 		= HIGG4DxName+"SkimmingTool",
                                                                       expression 	= skim_expression)
    ToolSvc += HIGG4DxSkimmingTool
    skimmingTools.append(HIGG4DxSkimmingTool)

    return skimmingTools

## the following skiming tool is used by the HIGG4D6 skim. The provided skimming tool ought to be attached to the main DerivationKernel in HIGG4D6 JO.
## This kernel will already have access ro the fat jets collection, which is 
## The only skim criterium used if on fat jets. All the other cuts of the HIGG4D6 are applied before.
def setupFatJetSkim(HIGG4DxName, ToolSvc):
    
    skimmingTools = []
    tauProngs13 = "( abs(TauJets.charge)==1.0 && (TauJets.nTracks == 1 || TauJets.nTracks == 3) )"
    if HIGG4DxName == 'HDBS1':
        ditau = '(count( (DiTauJetsLowPt.pt > 50.0*GeV) && (DiTauJetsLowPt.nSubjets >=2 ) ) >= 1)'
        twotau = '(count( TauJets.pt > 20.0*GeV && '+tauProngs13+' ) >= 2)'
        tauReq = '( '+ditau+' || '+twotau+' )'
        Bjet = '(count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 30.0*GeV) && AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_MV2c10) >= 1)'
        skim_expression = tauReq + "&&" + Bjet 
    elif HIGG4DxName == 'HIGG4D6':
        fatjet   = '(count((AntiKt10LCTopoJets.pt > 300.0*GeV)) >= 2)'
        skim_expression = fatjet
    else:
        assert False, "HIGG4DxSkimming.setupFatJetSkim: FatJet skimming is not expected to be used with format '{}'".format(HIGG4DxName)
    
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
    HIGG4DxFatJetSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name 		= HIGG4DxName+"FatJetSkimmingTool",
                                                                             expression 	= skim_expression)
    ToolSvc += HIGG4DxFatJetSkimmingTool
    skimmingTools.append(HIGG4DxFatJetSkimmingTool)
    
    return skimmingTools

