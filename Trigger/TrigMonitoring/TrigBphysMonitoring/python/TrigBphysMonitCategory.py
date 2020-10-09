# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags

monitoring_bphys = [
                    # For individual chain monitoring folders, to fill DetailedIndividualChains - put them first
                    'HLT_2mu4_bJpsimumu_L12MU4',
                    'HLT_2mu4_bUpsimumu_L12MU4',
                    'HLT_2mu4_bDimu_L12MU4',
                    'HLT_mu6_mu4_bJpsimumu_L1MU6_2MU4',
                    'HLT_mu6_mu4_bUpsimumu_L1MU6_2MU4',
                    'HLT_mu6_mu4_bDimu_L1MU6_2MU4',
                    'HLT_2mu6_bJpsimumu_L12MU6',
                    'HLT_2mu6_bUpsimumu_L12MU6',
                    'HLT_2mu6_bDimu_L12MU6',
                    'HLT_mu11_mu6_bJpsimumu_L1MU11_2MU6',
                    'HLT_mu11_mu6_bUpsimumu_L1MU11_2MU6',
                    'HLT_mu11_mu6_bDimu_L1MU11_2MU6',
                    'HLT_mu11_mu6_bPhi_L1MU11_2MU6',
                    'HLT_mu11_mu6_bTau_L1MU11_2MU6',
                    'HLT_mu11_mu6_bBmumu_L1MU11_2MU6',
                    'HLT_mu11_mu6_bDimu2700_L1MU11_2MU6',
                    'HLT_2mu4_bBmumux_BsmumuPhi_L12MU4',
                    'HLT_mu6_mu4_bBmumux_BsmumuPhi_L1MU6_2MU4',
                    'HLT_2mu4_bBmumux_BpmumuKp_L12MU4',
                    'HLT_mu6_mu4_bBmumux_BpmumuKp_L1MU6_2MU4',
                    # Other chains that used to be here
                    'HLT_mu11_mu6_bDimu', 
                    'HLT_mu11_mu6_bBmumuxv2', 
                    'HLT_mu6_2mu4_bJpsi',
                    'HLT_mu20_bJpsi_Trkloose', 
                    'HLT_mu10_bJpsi_TrkPEBmon', 
                    'HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 
                    'HLT_2mu6_bUpsimumu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6', 
                    'HLT_mu11_mu6noL1_bTau_L1MU11_2MU6', 
                    'HLT_mu11_mu6_bTau', 
                    'HLT_2mu6_bDimu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 
                    'HLT_mu11_mu6_bDimu_novtx_noos', 
                    'HLT_mu11_mu6_bJpsimumu',
                    'HLT_mu11_mu6_bBmumu',
                    'HLT_mu6_mu4_bUpsimumu_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',
                    'HLT_mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4',
                    'HLT_3mu4_bNocut',
                    'HLT_2mu6_bBmumu',
                    'HLT_2mu6_bBmumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                    'HLT_2mu6_bBmumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                    'HLT_2mu6_bBmumux_BsmumuPhi_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                    'HLT_mu6_bJpsi_Trkloose',
                    'HLT_mu4_mu4_idperf_bJpsimumu_noid',
                    
                    ]

primary_bphys = [ 
                 # first should go those for generic folders (BMuMu, MultiMu etc)
                 'HLT_mu11_mu6_bDimu', 
                 'HLT_mu11_mu6_bBmumuxv2', 
                 'HLT_mu6_2mu4_bJpsi',
                 'HLT_mu10_bJpsi_TrkPEBmon', 
                 'HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 
                 'HLT_2mu6_bUpsimumu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6', 
                 'HLT_mu11_mu6_bTau', 
                 'HLT_2mu6_bDimu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 
                 'HLT_mu11_mu6_bDimu_novtx_noos', 
                 # then others, for individual chain folders
                 'HLT_2mu4_bJpsimumu',
                 'HLT_2mu4_bUpsimumu',
                 'HLT_mu6_mu4_bJpsimumu',
                 'HLT_mu6_mu4_bUpsimumu',
                 'HLT_mu6_mu4_bDimu',
                 'HLT_2mu6_bJpsimumu',
                 'HLT_2mu6_bUpsimumu',
                 'HLT_2mu6_bDimu',
                 'HLT_mu11_mu6_bJpsimumu',
                 'HLT_mu11_mu6_bUpsimumu',
                 'HLT_mu11_mu6_bDimu',
                 ]

if TriggerFlags.EDMDecodingVersion == 3 :
  monitoring_bphys = [
                      'HLT_2mu4_bJpsimumu_L12MU4',
                      'HLT_2mu4_bUpsimumu_L12MU4',
                      'HLT_2mu4_bDimu_L12MU4',
                      'HLT_mu6_mu4_bJpsimumu_L1MU6_2MU4',
                      'HLT_mu6_mu4_bUpsimumu_L1MU6_2MU4',
                      'HLT_mu6_mu4_bDimu_L1MU6_2MU4',
                      'HLT_2mu6_bJpsimumu_L12MU6',
                      'HLT_2mu6_bUpsimumu_L12MU6',
                      'HLT_2mu6_bDimu_L12MU6',
                      'HLT_mu11_mu6_bJpsimumu_L1MU11_2MU6',
                      'HLT_mu11_mu6_bUpsimumu_L1MU11_2MU6',
                      'HLT_mu11_mu6_bDimu_L1MU11_2MU6',
                      'HLT_mu11_mu6_bPhi_L1MU11_2MU6',
                      'HLT_mu11_mu6_bTau_L1MU11_2MU6',
                      'HLT_mu11_mu6_bBmumu_L1MU11_2MU6',
                      'HLT_mu11_mu6_bDimu2700_L1MU11_2MU6',
                      'HLT_2mu4_bBmumux_BsmumuPhi_L12MU4',
                      'HLT_mu6_mu4_bBmumux_BsmumuPhi_L1MU6_2MU4',
                      'HLT_2mu4_bBmumux_BpmumuKp_L12MU4',
                      'HLT_mu6_mu4_bBmumux_BpmumuKp_L1MU6_2MU4',
                     ]
  primary_bphys = [
                    'HLT_2mu4_bDimu_L12MU4',
                  ]

primary_bphys_pp = primary_bphys

primary_bphys_hi = [ 'HLT_2mu4_bDimu', 'HLT_2mu4_bDimu_novtx_noos' ]
