# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class TrigBJetD3PDFlags(JobPropertyContainer):
    """ The HLT bjet D3PD flag/job property  container."""
    BJetSliceTriggerNames = {
                             # mu4_jX
                             "EF_mu4_j10_a4tc_EFFS":1,
                             # 2b/3j
                             "EF_2b10_medium_3L1J10":1, "EF_2b15_medium_3L1J15":1, "EF_2b20_medium_3L1J20":1,
                             # 2b/4j
                             "EF_2b10_medium_4L1J10":1, "EF_2b10_medium_4j30_a4tc_EFFS":1,
                             # 2b/JE
                             "EF_2b10_medium_L1JE100":1, "EF_2b10_medium_L1JE140":1, "EF_2b10_medium_L1_2J10J50":1,
                             # 3b/4j
                             "EF_3b10_loose_4L1J10":1, "EF_3b15_loose_4L1J15":1,
                             # 2b/2j
                             "EF_2b10_medium_j75_j30_a4tc_EFFS":1,
                             # b+MET
                             "EF_b10_medium_EFxe25_noMu_L1JE140":1,
                             # 4j
                             "EF_4j30_a4tc_EFFS":1,

                             # PPv2, PPv3 or HIv1 menus
                             "EF_b10_L2Star_IDTrkNoCut":1,
                             'EF_b10':1,
                             'EF_b10_loose':1,
                             'EF_b15_loose':1,
                             'EF_b10_medium':1,
                             'EF_b10_medium_4L1J10':1,
                             'EF_b15_medium':1,
                             'EF_b15_medium_4L1J15':1,
                             'EF_b15_medium_2L1J30':1,
                             'EF_b15_medium_L1J75':1,
                             'EF_b15_medium_L12J15J50':1,
                             'EF_b20_medium':1,
                             'EF_b75_medium':1,
                             'EF_b10_tight':1,
                             'EF_b10_tight_4L1J10':1,
                             'EF_b10_tight_L1JE100':1,
                             'EF_b10_tight_L1JE140':1,
                             'EF_b15':1,
                             'EF_b10_IDTrkNoCut':1,
                             'EF_b15_IDTrkNoCut':1,
                             'EF_b20_IDTrkNoCut':1,
                             'EF_b10_L2Star_IDTrkNoCut_multiroi':1,
                             'EF_b10_j30_a4tc_EFFS_IDTrkNoCut':1,
                             'EF_b10_EFj10_a4tc_EFFS_IDTrkNoCut':1,
                             'EF_b10_tight_4j30_a4tc_EFFS':1,
                             'EF_b10_medium_4j30_a4tc_EFFS':1,
                             'EF_b10_medium_j75_j55_2j30_a4tc_EFFS':1,
                             'EF_b15_medium_4j45_a4tc_EFFS':1,
                             'EF_b15_medium_2j55_a4tc_EFFS':1,
                             'EF_b10_tight_j75_j55_2j30_a4tc_EFFS':1,
                             'EF_b15_medium_j100_j40_a4tc_EFFS':1,
                             'EF_b15_medium_j100_j40_a4tc_EFFS_ht300':1,
                             'EF_b15_medium_j75_j40_a4tc_EFFS':1,
                             'EF_b75_medium_j100_a4tc_EFFS':1,
                             'EF_b75_medium_j100_a4tc_EFFS_ht300':1,
                             'EF_b75_medium_j100_a4tc_EFFS_ht200':1,
                             'EF_mu4_j10_a4tc_EFFS_matched':1,
                             'EF_mu4_j20_a4tc_EFFS_matched':1,
                             'EF_mu4_MSonly_j15_a2hi_EFFS_matched':1,
                             'EF_mu4_L1J5_matched':1,
                             'EF_mu4_L1J10_matched':1,
                             'EF_mu4_L1J15_matched':1,
                             'EF_mu4_L1J20_matched':1,
                             'EF_mu4_L1J30_matched':1,
                             'EF_mu4_L1J50_matched':1,
                             'EF_mu4_L1J75_matched':1,
                             'EF_mu4_j100_a4tc_EFFS_L1matched':1,
                             'EF_mu4_j135_a4tc_EFFS_L1matched':1,
                             'EF_mu4_j180_a4tc_EFFS_L1matched':1,

                             # v4 menu for bjets (primary triggers)
                             "EF_b145_loose_j145_a4tchad":1,
                             "EF_b180_loose_j180_a4tchad_L2j140":1,
                             "EF_b220_loose_j220_a4tchad_L2j140":1,
                             "EF_b280_loose_j280_a4tchad_L2j140":1,
                             "EF_b360_loose_j360_a4tchad_L2j140":1,
                             "EF_b55_looseEF_j55_a4tchad":1,
                             "EF_b80_looseEF_j80_a4tchad":1,
                             "EF_b110_looseEF_j110_a4tchad":1,
                             "EF_b15_looseEF_j15_a4tchad":1,
                             "EF_b25_looseEF_j25_a4tchad":1,
                             "EF_b35_looseEF_j35_a4tchad":1,
                             "EF_b45_looseEF_j45_a4tchad":1,

                             "EF_b55_medium_4j55_a4tchad":1,
                             "EF_2b55_medium_4j55_a4tchad":1,
                             "EF_2b55_loose_4j55_a4tchad":1,
                             "EF_b55_medium_3j55_a4tchad_L2FS_4L1J15":1,
                             "EF_b45_medium_3j45_a4tchad_L2FS_4L1J15":1,
                             "EF_b35_medium_3j35_a4tchad_L2FS_4L1J15":1,
                             "EF_2b55_medium_3j55_a4tchad_L2FS_4L1J15":1,
                             "EF_2b45_medium_3j45_a4tchad_L2FS_4L1J15":1,
                             "EF_2b35_medium_3j35_a4tchad_L2FS_4L1J15":1,

                             # not combined b-jet triggers (#91828)
                             #(i) 1b/2j and 2b/2j triggers
                             #(ii)1b/4j and 2b/4j 
                             "EF_2b55_loose_j110_j55_a4tchad":1,
                             "EF_b55_medium_j110_j55_a4tchad":1,
                             "EF_2b55_loose_j110_j55_a4tchad_1bL2":1,
                             "EF_b55_medium_4j55_a4tchad_L2FS":1,
                             "EF_2b55_medium_4j55_a4tchad_L2FS":1,
                             "EF_2b55_loose_4j55_a4tchad_L2FS":1,
                             "EF_b55_loose_4j55_a4tchad":1,

                             #(iii) non-standard 1b/4j and 2b/4j triggers
	                     'EF_b55_medium_3j55_a4tchad_4L1J15':1,
	                     'EF_b45_medium_3j45_a4tchad_4L1J15':1,
	                     'EF_b35_medium_3j35_a4tchad_4L1J15':1,
	                     'EF_2b55_medium_3j55_a4tchad_4L1J15':1,
	                     'EF_2b45_medium_3j45_a4tchad_4L1J15':1,
	                     'EF_2b35_medium_3j35_a4tchad_4L1J15':1,
	             
	                     #(iv)NoCut trigger
	                     'EF_b15_NoCut_j15_a4tchad':1,
	                     'EF_b55_NoCut_j55_a4tchad':1,
	                     'EF_b55_NoCut_j55_a4tchad_L2FS':1,
	                     'EF_b35_NoCut_4j35_a4tchad':1,
	                     'EF_b35_NoCut_4j35_a4tchad_L2FS':1,
	             
	                     # b-tagged fat jets (#91900)
	                     'EF_b110_loose_j110_a10tcem_L2FS_L1J75':1,
	                     'EF_b145_loose_j145_a10tcem_L2FS':1,
	                     'EF_b180_loose_j180_a10tcem_L2FS':1,
	                     'EF_b180_loose_j180_a10tcem_L2j140':1,
	                     'EF_b180_medium_j180_a10tcem_L2j140':1,
	                     'EF_b220_loose_j220_a10tcem':1,
	                     'EF_b280_loose_j280_a10tcem':1,
	                     'EF_b240_loose_j240_a10tcem_L2FS':1,
	                     'EF_b240_loose_j240_a10tcem':1,
	
	                     #HT triggers (#91860)
	                     'EF_b145_medium_j145_a4tchad_ht400':1,
	                     'EF_b165_medium_j165_a4tchad_ht500':1,
	                     'EF_2b55_medium_j110_j55_a4tchad_ht500':1,
	                     'EF_2b55_loose_j110_j55_a4tchad_ht500':1,
	                     'EF_2b55_medium_j165_j55_a4tchad_ht500':1,
	                     'EF_2b80_medium_j165_j80_a4tchad_ht500':1,
	                     'EF_b45_medium_j145_j45_a4tchad_ht400':1,
	                     'EF_b45_medium_j145_j45_a4tchad_ht500':1,
	             
	                     #ZH triggers (#92105)
	                     'EF_b80_loose_j80_a4tchad':1,
	                     'EF_b110_loose_j110_a4tchad':1,
	             
	                     #chain for bjet+xe
	                     'EF_b35_mediumEF_j35_a4tchad':1,
	                     'EF_b55_mediumEF_j110_j55_a4tchad':1,
	                     'EF_b45_mediumEF_j110_j45_a4tchad':1,
	                     'EF_b145_mediumEF_j145_a4tchad':1,
	                     'EF_2b35_loose_j110_2j35_a4tchad':1,
	                     'EF_2b55_loose_j145_j55_a4tchad':1,
	                     'EF_2b45_loose_j145_j45_a4tchad':1,
	                     'EF_b35_loose_j110_2j35_a4tchad':1,
	                     'EF_b45_looseEF_j145_a4tchad':1,
	             
	                     # bjet chains for mu+jet combined matched chains  (#91661)
	                     'EF_b25_NoCut_j25_a4tchad':1,
	                     'EF_b35_NoCut_j35_a4tchad':1,
	                     'EF_b45_NoCut_j45_a4tchad':1,
	                     'EF_b65_NoCut_j65_a4tchad':1,
	                     'EF_b80_NoCut_j80_a4tchad':1,
	                     'EF_b110_NoCut_j110_a4tchad':1,
	                     'EF_b145_NoCut_j145_a4tchad':1,
	                     'EF_b180_NoCut_j180_a4tchad':1,
	                     'EF_b220_NoCut_j220_a4tchad':1,
	                     'EF_b280_NoCut_j280_a4tchad':1,
	                     'EF_b360_NoCut_j360_a4tchad':1,
	
	
	                     'EF_b45_NoCut_j45_a4tchad_L2FS':1,
	                     'EF_b55_NoCut_j55_a4tchad_L2FS':1,
	                     'EF_b65_NoCut_j65_a4tchad_L2FS':1,
	                     'EF_b80_NoCut_j80_a4tchad_L2FS':1,
	                     'EF_b110_NoCut_j110_a4tchad_L2FS':1,
	                     'EF_b145_NoCut_j145_a4tchad_L2FS':1,
	                     'EF_b180_NoCut_j180_a4tchad_L2FS':1,
	                     'EF_b220_NoCut_j220_a4tchad_L2FS':1,
	                     'EF_b280_NoCut_j280_a4tchad_L2FS':1,
	                     'EF_b360_NoCut_j360_a4tchad_L2FS':1,
	
	                     #Chain for Z->bb trigger (#92592)
	                     'EF_2b35_loose_j145_j100_j35_a4tchad':1,
	             
	                     # alternative 4J15-seeded bjet triggers (#92019)
	                     'EF_b45_medium_4j45_a4tchad':1,
	                     'EF_2b35_loose_4j35_a4tchad':1,
	                     'EF_2b35_loose_3j35_a4tchad_4L1J15':1,
	                     'EF_2b35_loose_j145_2j35_a4tchad':1,
	                     'EF_2b35_loose_j145_j35_a4tchad':1,
	             
	                     'EF_2b35_loose_3j35_a4tchad_L2FS_4L1J15':1,
	                     'EF_2b35_loose_4j35_a4tchad_L2FS':1,
	                     'EF_b55_loose_4j55_a4tchad_L2FS':1,
	             
	                     # Additional Multijet Triggers (#92451)
 	                     'EF_b55_medium_4j55_a4tchad_4L1J10':1,
 	                     'EF_b55_medium_4j55_a4tchad_L2FS_4L1J10':1,
 	                     'EF_b45_medium_4j45_a4tchad_4L1J10':1,
 	                     'EF_b45_medium_4j45_a4tchad_L2FS_4L1J10':1,
 	                     'EF_2b35_loose_3j35_a4tchad_4L1J10':1,
 	                     'EF_2b35_loose_3j35_a4tchad_L2FS_4L1J10':1,
 	                     'EF_b35_loose_4j35_a4tchad_L2FS_5L1J10':1,
 	                     'EF_b35_NoCut_4j35_a4tchad_L2FS_5L1J10':1,
 	                     'EF_b35_loose_4j35_a4tchad_5L1J10':1,
 	                     'EF_b35_NoCut_4j35_a4tchad_5L1J10':1,
             
                             # combined chains
                             'EF_b180_loose_j180_a10tcem_L2j140_EFxe50_tclcw':1,
                             'EF_b180_medium_j180_a10tcem_L2j140_EFxe50_tclcw':1,
                             'EF_b55_mediumEF_j110_j55_xe60_tclcw':1,
                             'EF_b45_mediumEF_j110_j45_xe60_tclcw':1,
                             'EF_b80_loose_j80_a4tchad_xe70_tclcw':1,
                             'EF_b145_mediumEF_j145_a4tchad_L2EFxe60_tclcw':1,
                             'EF_2b45_loose_j145_j45_a4tchad_EFxe40_tclcw':1,
                             'EF_b35_loose_j110_2j35_a4tchad_EFxe80_tclcw':1,
                             'EF_b45_looseEF_j145_a4tchad_L2EFxe60_tclcw':1,
                             'EF_b80_loose_j80_a4tchad_xe55_tclcw':1,
                             'EF_b80_loose_j80_a4tchad_xe60_tclcw':1,
                             'EF_b80_loose_j80_a4tchad_xe75_tclcw':1,
                             'EF_b110_loose_j110_a4tchad_xe55_tclcw':1,
                             'EF_b110_loose_j110_a4tchad_xe60_tclcw':1,
                             
                             # lepton + bjet
                             'EF_mu24_tight_b35_mediumEF_j35_a4tchad':1,
                             'EF_e24vh_medium1_b35_mediumEF_j35_a4tchad':1,
                             'EF_mu22_j30_a4tc_EFFS':1,
             
                             # mymujet
                             'EF_mu4T_j15_a4tchad_matched':1,
                             'EF_mu4T_j25_a4tchad_matched':1,
                             'EF_mu4T_j35_a4tchad_matched':1,
                             'EF_mu4T_j45_a4tchad_matched':1,
                             'EF_mu4T_j55_a4tchad_matched':1,
                             'EF_mu4T_j65_a4tchad_matched':1,
                             'EF_mu4T_j80_a4tchad_matched':1,
                             'EF_mu4T_j110_a4tchad_matched':1,
                             'EF_mu4T_j145_a4tchad_matched':1,
                             'EF_mu4T_j180_a4tchad_matched':1,
                             'EF_mu4T_j220_a4tchad_matched':1,
                             'EF_mu4T_j280_a4tchad_matched':1,
                             'EF_mu4T_j360_a4tchad_matched':1,
             
                             # mu jet triggers with deltaR and deltaZ matching
                             'EF_mu4T_j15_a4tchad_matchedZ':1,
                             'EF_mu4T_j25_a4tchad_matchedZ':1,
                             'EF_mu4T_j35_a4tchad_matchedZ':1,
                             'EF_mu4T_j45_a4tchad_matchedZ':1,
                             'EF_mu4T_j55_a4tchad_matchedZ':1,
             
                             # L1.5 for mu+jets
                             'EF_mu4T_j45_a4tchad_L2FS_matched':1,
                             'EF_mu4T_j55_a4tchad_L2FS_matched':1,
                             'EF_mu4T_j65_a4tchad_L2FS_matched':1,
                             'EF_mu4T_j80_a4tchad_L2FS_matched':1,
                             'EF_mu4T_j110_a4tchad_L2FS_matched':1,
                             'EF_mu4T_j145_a4tchad_L2FS_matched':1,
                             'EF_mu4T_j180_a4tchad_L2FS_matched':1,
                             'EF_mu4T_j220_a4tchad_L2FS_matched':1,
                             'EF_mu4T_j280_a4tchad_L2FS_matched':1,
                             'EF_mu4T_j360_a4tchad_L2FS_matched':1,
                             'EF_mu4T_j45_a4tchad_L2FS_matchedZ':1,
                             'EF_mu4T_j55_a4tchad_L2FS_matchedZ':1,
             
                             # muon jets no TRT
                             'EF_mu4_MSonly_j15_a2hi_EFFS_L1TE10':1,
                            }
             
    GlobalTriggerPrescale = 1
jobproperties.add_Container(TrigBJetD3PDFlags)

#short-cut to get the TrigBJetD3PDFlags container with this one line:
#'from TrigBJetD3PDMaker.TrigBJetD3PDFlags import trigbjetD3PDFlags'
#Note that name has to be different to avoid problems with pickle
trigbjetD3PDFlags=jobproperties.TrigBJetD3PDFlags

class ApplyTriggerSkimming(JobProperty):
    """ Apply the skimming based on Trigger Decision Tool for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.TrigBJetD3PDFlags.add_JobProperty(ApplyTriggerSkimming)

class ApplyTriggerPrescales(JobProperty):
    """ Apply the prescales based on Trigger Decision Tool for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.TrigBJetD3PDFlags.add_JobProperty(ApplyTriggerPrescales)

