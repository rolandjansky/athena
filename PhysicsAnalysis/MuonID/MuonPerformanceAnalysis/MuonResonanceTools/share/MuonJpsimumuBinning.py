from math import pi as Pi

JpsiMassBin=[150,1500.,4500.]

MuonJpsiBinning={ 
    "mu_1stAuthor": [25, 0, 24],                                  # author of muon leading
    "mu_2ndAuthor": [25, 0, 24],                                  # author of 2nd muon
    "mu_1stPt": [200, 0., 100000.],                               # pt of muon leading
    "mu_2ndPt": [200, 0., 100000.],                               # pt of 2nd muon
    "mu_avPt": [200, 0., 100000.],                                # pt average (pt1+pt2)/2
    "mu_1stPhi": [128, -Pi, Pi],                                  # phi of muon leading
    "mu_2ndPhi": [128, -Pi, Pi],                                  # phi of 2nd muon
    "mu_1stEta": [100, -2.5, 2.5],                                # eta of muon leading
    "mu_2ndEta": [100, -2.5, 2.5],                                # eta of 2nd muon
    "Mmumu": JpsiMassBin,                                         # Mass mumu = Mmumu
    "Z_pt": [200, 0., 100000.],                                   # pt mumu
    "Z_phi": [128, -Pi, Pi],                                      # phi mumu
    
    "h_Zpt_mu_1stPt": [200, 0., 100000., 200, 0., 100000.],       # pt mumu vs pt of muon leading
    "h_mu_1stPt_1stPhi": [128, -Pi, Pi, 200, 0., 100000.],        # pt of muon leading vs phi of muon leaind
    "h_mu_2ndPt_2ndPhi": [128, -Pi, Pi, 200, 0., 100000.],        # pt of 2nd muon vs pt of 2nd muon
    "h_Zpt_mu_avPt": [200, 0., 100000., 200, 0., 100000.],        # Mmumu vs phi of muon leading
    
    "h_Zm_1stPhi": [128, -Pi, Pi, 150, 1500., 4500.],             # Mmumu vs phi of muon leading
    "h_Zm_2ndPhi": [128, -Pi, Pi, 150, 1500., 4500.],             # Mmumu vs phi of 2nd muon
    "h_Zm_1stEta": [20, -2.5, 2.5, 150, 1500., 4500.],            # Mmumu vs eta of muon leading
    "h_Zm_2ndEta": [20, -2.5, 2.5, 150, 1500., 4500.],            # Mmumu vs eta of 2nd muon

    "h_Zm_Eta": [20, -2.5, 2.5, 150, 1500., 4500.],               # Mmumu vs eta, both muons in same etabin, etabin of 0.25

    "h_Zm_1stEta01": [50, -2.5, 2.5, 150, 1500., 4500.],          # Mmumu vs eta of the leading muon, etabin of 0.1

    "h_Zm_1stPt": [200, 0., 100000., 150, 1500., 4500.],          # Mmumu vs pt1 in large bin of eta
    "h_Zm_2ndPt": [200, 0., 100000., 150, 1500., 4500.],          # Mmumu vs pt1 in large bin of eta
    "h_Zm_mu_avPt": [200, 0., 100000., 150, 1500., 4500.],        # Mmumu vs pt1 in large bin of eta
    "h_Zm_Pexp": [200, 0., 100000., 150, 1500., 4500.],           # Mmumu vs p* (not used for Jpsi, used for Z)
    
    #correlation plots (truth-info)
    "h_Zm_1stPhi_truth": [128, -Pi, Pi, 150, 1500., 4500.],       # Mmumu(truth) vs. phi(truth) of leading muon 
    "h_Zm_2ndPhi_truth": [128, -Pi, Pi, 150, 1500., 4500.],       # Mmumu(truth) vs. phi(truth) of subleading muon
    "h_Zm_1stEta_truth": [50, -2.5, 2.5, 150, 1500., 4500.],      # Mmumu(truth) vs. eta(truth) of leading muon
    "h_Zm_2ndEta_truth": [50, -2.5, 2.5, 150, 1500., 4500.],      # Mmumu(truth) vs. eta(truth) of subleading muon
    "h_Zm_1stPt_truth": [200, 0., 100000., 150, 1500., 4500.],    # Mmumu(truth) vs. pt(truth) of leading muon
    "h_Zm_2ndPt_truth": [200, 0., 100000., 150, 1500., 4500.],    # Mmumu(truth) vs. pt(truth) of subleading muon
    "h_Zm_mu_avPt_truth": [200, 0., 100000., 150, 1500., 4500.],  # Mmumu(truth) vs. avPt(truth) of subleading muon
    "h_Zm_Pexp_truth": [200, 0., 100000., 150, 1500., 4500.],     # Mmumu(truth) vs. p*(truth)
    
    #resolution plots
    "Res_mu_1stPt_2D": [200, 0., 100000., 200, -50000., 50000.],  # pt-pt(truth) vs. pt for leading muon
    "Res_mu_2ndPt_2D": [200, 0., 100000., 200, -50000., 50000.],  # pt-pt(truth) vs. pt for subleading muon
    "Res_mu_1stPhi_2D": [128, -Pi, Pi, 128, -Pi, Pi],             # phi-phi(truth) vs. phi for leading muon
    "Res_mu_2ndPhi_2D": [128, -Pi, Pi, 128, -Pi, Pi],             # phi-phi(truth) vs. phi for subleading muon
    "Res_mu_1stEta_2D": [100, -2.5, 2.5, 100, -2.5, 2.5],         # eta-eta(truth) vs. eta for leading muon
    "Res_mu_2ndEta_2D": [100, -2.5, 2.5, 100, -2.5, 2.5],         # eta-eta(truth) vs. eta for subleading muon
    
    "Res_Zm_2D": [150, 1500., 4500., 300, -3000., 3000.],         # Mmumu-Mmumu(truth) vs. Mmumu
    "Res_Zm_Eta_2D": [100, -2.5, 2.5, 300, -3000., 3000.],        # Mmumu-Mmumu(truth) vs. eta of leading muon
    "Res_Zm_Phi_2D": [128, -Pi, Pi, 300, -3000., 3000.],          # Mmumu-Mmumu(truth) vs. phi of leading muon
    "Res_Zm_Pt_2D": [200, 0., 100000., 300, -3000., 3000.],       # Mmumu-Mmumu(truth) vs. pt of leading muon
    "Res_Zm_Pexp_2D": [200, 0., 100000., 300, -3000., 3000.],     # Mmumu-Mmumu(truth) vs. p*
    "Res_Zpt_2D": [200, 0., 100000., 200, -50000., 50000.],       # Zpt-Zpt(truth) vs. Zpt
    "Res_Zphi_2D": [128, -Pi, Pi, 128, -Pi, Pi],                  # Zphi-Zphi(truth) vs. Zphi 

    #Chi^2 of tracks
    "ChiSquared": [200,  0.,  100.],                              # Chi^2/DoF for combined track of leading muon 
    
    #momentum imbalance plots
    "p_pTRUE": [200, -2., 2.],                                    # pt-pt(truth)/pt(truth) of leading muon (momentum imbalance)
    "pID_pME": [200, -2., 2.],                                    # p(Inner Detector)-p(MuonSpectrometer) (momentum imbalance)

    #eta-phi maps
    "Z_m_etaphi": [100, -2.5, 2.5, 128, -Pi, Pi],                 # eta-phi map of Mmumu
    "DeltaZ_m_etaphi": [100, -2.5, 2.5, 128, -Pi, Pi],            # eta-phi map of DeltaM(Z)/M(Z) as in Note 
    "DeltaZ_m_q_etaphi": [100, -2.5, 2.5, 128, -Pi, Pi],          # eta-phi map of q*DeltaM(Z)/M(Z) (combined track)
    
    "p_pTRUE_etapt": [100, -2.5, 2.5, 200, 0., 100000.],          # eta-pt map of momentum imbalance
    "p_pTRUE_etaphi": [100, -2.5, 2.5, 128, -Pi, Pi],             # eta-phi map of momentum imbalance
    
    }
