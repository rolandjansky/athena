from math import pi as Pi

YMassBin=[100,5000,15000]

MuonYBinning={ 
    "mu_1stAuthor": [25, 0, 24], 
    "mu_2ndAuthor": [25, 0, 24],
    "mu_1stPt": [500, 1., 1000000.],
    "mu_2ndPt": [500, 1., 1000000.],
    "mu_avPt": [500, 1., 1000000.],
    "mu_1stPhi": [100, -Pi, Pi],
    "mu_2ndPhi": [100, -Pi, Pi],
    "mu_1stEta": [100, -3.5, 3.5],
    "mu_2ndEta": [100, -3.5, 3.5],
    "Mmumu": YMassBin,
    "Z_pt": [ 500, 1., 1000000. ],
    "Z_phi": [ 128, -Pi, Pi ],
    
    "h_Zpt_mu_1stPt": YMassBin+[ 500, 1., 1000000.],
    "h_mu_1stPt_1stPhi": [128, -Pi, Pi, 500, 1., 1000000.],
    "h_mu_2ndPt_2ndPhi": [ 128, -Pi, Pi, 500, 1., 1000000.],
    "h_Zpt_mu_avPt": [ 500, 1., 1000000., 500, 1., 1000000.],

    "h_Zm_1stPhi": [ 128, -Pi, Pi, 100, 1., 5000.],
    "h_Zm_2ndPhi": [ 128, -Pi, Pi, 100, 1., 5000.],
    "h_Zm_1stEta": [ 100, -3.5, 3.5, 100, 1., 5000.],
    "h_Zm_2ndEta": [ 100, -3.5, 3.5, 100, 1., 5000.],
    "h_Zm_1stPt":  [ 500, 1., 1000000., 500, 1., 1000000. ],
    "h_Zm_2ndPt":  [ 500, 1., 1000000., 500, 1., 1000000. ],
    "h_Zm_mu_avPt": YMassBin+[ 500, 1., 1000000.],
    "h_Zm_Pexp": [ 200, 1., 2000000.]+ YMassBin,                          #Mmumu vs. p* (as defined in ATL-COM-MUON 2014-001)
    
    #correlation plots (truth-info)
    "h_Zm_1stPhi_truth" :[ 128, -Pi, Pi, 100, 1., 5000.],                 #Mmumu(truth) vs. phi(truth) of leading muon 
    "h_Zm_2ndPhi_truth" :[ 128, -Pi, Pi, 100, 1., 5000.],                 #Mmumu(truth) vs. phi(truth) of subleading muon
    "h_Zm_1stEta_truth" :[ 100, -3.5, 3.5, 100, 1., 5000.],               #Mmumu(truth) vs. eta(truth) of leading muon
    "h_Zm_2ndEta_truth" :[ 100, -3.5, 3.5, 100, 1., 5000.],               #Mmumu(truth) vs. eta(truth) of subleading muon
    "h_Zm_1stPt_truth":  [ 500, 1., 1000000., 500, 1., 1000000. ],        #Mmumu(truth) vs. eta(truth) of leading muon
    "h_Zm_2ndPt_truth":  [ 500, 1., 1000000., 500, 1., 1000000. ],        #Mmumu(truth) vs. eta(truth) of subleading muon
    "h_Zm_mu_avPt_truth": YMassBin+[ 500, 1., 1000000.],                  #Mmumu(truth) vs. avPt(truth) of muons
    "h_Zm_Pexp_truth"   :[ 200, 1., 2000000.]+ YMassBin,                  #Mmumu(truth) vs. p*(truth)
    
    #resolution plots
    "Res_mu_1stPt_2D" :[ 500, 1., 1000000., 50, -50000., 50000.],         #pt-pt(truth) vs. pt for leading muon
    "Res_mu_2ndPt_2D" :[ 500, 1., 1000000., 50, -50000., 50000.],         #pt-pt(truth) vs. pt for subleading muon
    "Res_mu_1stPhi_2D":[ 128, -Pi, Pi, 128, -Pi, Pi],                     #phi-phi(truth) vs. phi for leading muon
    "Res_mu_2ndPhi_2D":[ 128, -Pi, Pi, 128, -Pi, Pi],                     #phi-phi(truth) vs. phi for subleading muon
    "Res_mu_1stEta_2D":[ 100, -3.5, 3.5, 100, -3.5, 3.5],                 #eta-eta(truth) vs. eta for leading muon
    "Res_mu_2ndEta_2D":[ 100, -3.5, 3.5, 100, -3.5, 3.5],                 #eta-eta(truth) vs. eta for subleading muon

    "Res_Zm_2D"     : [ 50, -50000., 50000.]+YMassBin,                    #Mmumu-Mmumu(truth) vs. Mmumu
    "Res_Zm_Eta_2D" :[ 100,  -3.5,  3.5, 50, -50000., 50000.],            #Mmumu-Mmumu(truth) vs. eta of leading muon
    "Res_Zm_Phi_2D" :[ 128,  -Pi,  Pi, 50, -50000., 50000.],              #Mmumu-Mmumu(truth) vs. phi of leading muon
    "Res_Zm_Pt_2D"  : [50, -50000., 50000.]+YMassBin,                     #Mmumu-Mmumu(truth) vs. pt of leading muon
    "Res_Zm_Pexp_2D":[ 500, 1., 2000000., 50, -50000., 50000.],           #Mmumu-Mmumu(truth) vs. p*
    "Res_Zpt_2D"    :[ 500, 1., 1000000., 50, -50000., 50000.],           #Zpt-Zpt(truth) vs. Zpt
    "Res_Zphi_2D"   :[ 128, -Pi, Pi, 50, -50000., 50000.],                #Zphi-Zphi(truth) vs. Zphi 
    
    
    #Chi^2 of tracks
    "ChiSquared":[  200,  1.,  100.],                                     #Chi^2/DoF for combined track of leading muon 
   
    #momentum imbalance plots
    "p_pTRUE"  :[    200, -2., 2.],                                       #pt-pt(truth)/pt(truth) of leading muon (momentum imbalance)
    "pID_pME"  :[    200, -2., 2.],                                       #p(Inner Detector)-p(MuonSpectrometer) (momentum imbalance)

    #eta-phi maps
    "Z_m_etaphi"          :[ 100, -3.5, 3.5, 128, -Pi, Pi],               #eta-phi map of Mmumu
    "DeltaZ_m_etaphi"     :[ 100, -3.5, 3.5, 128, -Pi, Pi],               #eta-phi map of DeltaM(Z)/M(Z) as in ATL-COM-MUON 2014-001 
    "DeltaZ_m_q_etaphi"   :[ 100, -3.5, 3.5, 128, -Pi, Pi],               #eta-phi map of q*DeltaM(Z)/M(Z) (combined track)
    
    
    "p_pTRUE_etapt"   :[  100, -3.5, 3.5, 500, 1., 1000000.],             #eta-pt map of momentum imbalance
    "p_pTRUE_etaphi"  :[  100, -3.5, 3.5, 128, -Pi, Pi],                  #eta-phi map of momentum imbalance
    
    }
