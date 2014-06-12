# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# MuonEfficiencyCorrections 
# MuonMomentumCorrections
# needed by both
MuonsBranches = [
'mu_pt',
'mu_eta',
'mu_phi',
'mu_charge',
'mu_allauthor',
'mu_author',
# MuonEfficiencyCorrections is Period dependent so we need a simulated run number
'RunNumber',
# needed by MuonMomentumCorrections
'mu_id_phi_exPV',
'mu_id_theta_exPV',
'mu_id_qoverp_exPV',
'mu_me_phi_exPV',
'mu_me_theta_exPV',
'mu_me_qoverp_exPV',
# needed for standard muon selection:
'mu_caloMuonIdTag',
'mu_caloLRLikelihood',
'mu_loose',
'mu_medium',
'mu_tight',
'mu_momentumBalanceSignificance',
'mu_nprecisionLayers',
'mu_nBLHits',
'mu_nPixHits',
'mu_nSCTHits',
'mu_nTRTHits',
'mu_nPixHoles',
'mu_nSCTHoles',
'mu_nTRTOutliers',
'mu_nPixelDeadSensors',
'mu_nSCTDeadSensors',
'mu_expectBLayerHit' ]



