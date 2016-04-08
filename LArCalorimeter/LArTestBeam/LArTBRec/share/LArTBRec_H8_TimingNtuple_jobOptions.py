from TBRec.TBRecConf import CBNT_Timing
CBNT_Timing = CBNT_Timing()
CBNT_Timing.EnergyCut = 2.*GeV
CBNT_Timing.IncludedSamplings += ["EMB2", "EMB1", "EMB0", "EMB3"]
CBNT_AthenaAware += CBNT_Timing
