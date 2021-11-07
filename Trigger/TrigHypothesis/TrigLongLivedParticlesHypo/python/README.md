Modules in this directory
-----

* [TrigIsoHPtTrackTriggerHypoToolFromDict](TrigIsoHPtTrackTriggerHypoTool.py)
  * Provides isolated high-pT track trigger hypo tool configuration, given the chain dictionary.
  * `IDinfo` : defines working point on track.
  
     |IDinfo|working point|
     |:---|:---|
     | loose  | d0 significance < 10, eta < 2.5 |
     | medium | d0 significance < 5, eta < 2.5  |
     | tight  | d0 significance < 5, eta < 2.0  |
  * `isoinfo` : defines isolation working point.
     * *iaggrmedium* / *iaggrloose* : the aggregate isolation does a scaler sum of all charged tracks inside the dR cone of the candidate track and vetoes on the sum.
     * *imedium* / *iloose* : the imedium/iloose isolation vetoes any track that has a second charged track in the dR cone with given pT
     
     |IDinfo|working point|
     |:---|:---|
     | iaggrmedium | aggregate isoPt (dR<0.3) < 10 GeV |
     | iaggrloose  | aggregate isoPt (dR<0.3) < 20 GeV |
     | imedium | isoPt (dr<0.3) < 3 GeV |
     | iloose | isoPt (dr<0.3) < 5 GeV |

* [TrigLRTHypoToolFromDict](TrigFullScanLRTHypoTool.py)
  * Provides full-scan large radius tracking (LRT) trigger hypo tool configuration, given the chain dictionary.
  
* [createTrigDisappearingTrackHypoAlg](TrigDisappearingTrackHypoConfig.py) and 
  [TrigDisappearingTrackHypoToolFromDict](TrigDisappearingTrackHypoConfig.py)
  * Provides disappearing track trigger hypo alg configuration. 
  * Provides disappearing track trigger hypo tool configuration, given the chain dictionary.
  * `IDinfo` : defines working point for disappearing track candidate BDT thressholds for (Pix4Sct0, Pix4Sct1p, Pix3Sct0, Pix3Sct1p) categories
  
     |IDinfo|working point|
     |:---|:---|
     | loose  | currently same to medium (looser WP to be defined in future) |
     | medium | (-0.100, -0.085, 0.045, 0.145) for the 4 categories |
     | tight  | (-0.100, -0.040, 0.040, 0.155) for the 4 categories |

* [createTrigHitDVHypoAlg](TrigHitDVHypoConfig.py) and
  [TrigHitDVHypoToolFromDict](TrigHitDVHypoConfig.py)
  * Provides hit-based displaced vertex trigger hypo alg configuration.
  * Provides hit-based displaced vertex trigger hypo tool configuration, given the chain dictionary.
  * `IDinfo` : defines working point on hit-based displaced vertex finder BDT.
  
     |IDinfo|working point|
     |:---|:---|
     | loose  | currently same to medium (looser WP to be defined in future) |
     | medium | seeds=jet+spacepoint, jet eta<2.0 |
     | tight  | seeds=jets only, jet eta<2.0   |

* [createdEdxTrackHypoAlg](dEdxTrackHypoConfig.py) and
  [dEdxTrackHypoToolFromDict](dEdxTrackHypoConfig.py) 
  * Provides large-dE/dx track trigger hypo alg configuration.
  * Provides large-dE/dx track trigger hypo tool configuration, given the chain dictionary.
  * `IDinfo` : defines working point on large-dE/dx candidate track. 
    
     |IDinfo|working point|
     |:---|:---|
     | loose  | dEdx(average)>1.5, nr of hits (with dEdx >1.5) >=1, d0<5.0 mm, eta<2.5 |
     | medium | dEdx(average)>1.7, nr of hits (with dEdx >1.7) >=2, d0<2.5 mm, eta<2.5 |
     | tight  | dEdx(average)>1.8, nr of hits (with dEdx >1.8) >=2, d0<1.5 mm, eta<2.5 |



