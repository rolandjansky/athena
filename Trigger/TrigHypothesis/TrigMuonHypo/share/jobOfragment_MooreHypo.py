# define effective thresholds as nominal thresholds - 2 sigma 
include( "TrigMuonHypo/jobOfragment_TrigMooreHypo.py" )
include.block( "TrigMuonHypo/jobOfragment_TrigMooreHypo.py" )
TrigMooreHypo_6GeV_EF = Algorithm( "TrigMooreHypo_6GeV_EF" )
TrigMooreHypo_6GeV_EF.ptThreshold = 5.76*GeV # 6 GeV 

TrigMooreHypo_20GeV_EF = Algorithm( "TrigMooreHypo_20GeV_EF" )
TrigMooreHypo_20GeV_EF.ptThreshold = 19.08*GeV  # 20 GeV 

TrigMooreHypo_4GeV_EF = Algorithm( "TrigMooreHypo_4GeV_EF" )
TrigMooreHypo_4GeV_EF.ptThreshold = 3.84*GeV # 4 GeV 

TrigMooreHypo_5GeV_EF = Algorithm( "TrigMooreHypo_5GeV_EF" )
TrigMooreHypo_5GeV_EF.ptThreshold = 4.8*GeV  # 5 GeV
 
TrigMooreHypo_2GeV_EF = Algorithm( "TrigMooreHypo_2GeV_EF" )
TrigMooreHypo_2GeV_EF.ptThreshold = 2*GeV

#print TrigMooreHypo_6GeV_EF.ptThreshold
#TrigMooreHypo_6GeV_EF.OutputLevel = DEBUG
