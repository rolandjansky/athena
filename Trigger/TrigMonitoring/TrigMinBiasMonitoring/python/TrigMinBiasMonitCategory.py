# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----NORMAL USER ACCESS-------

triggers_pp = ["mb_sptrk",
               #"mb_sptrk_noisesup",
               #"mb_sptrk_costr",
               "noalg_L1RD0_FILLED",
               #"noalg_mb_L1RD0_EMPTY",
               "noalg_mb_L1MBTS_2",
               "mb_perf_L1MBTS_2",	
               "mb_sp1800_hmtperf_L1TE5",
               ]


triggers_hi = [ "mb_sp",
                "mb_sptrk_ion_L1ZDC_A_C_VTE50",
                "mb_perf_L1MBTS_1_1",	
                "noalg_mb_L1TE50",
                ]
		
#!----NORMAL USER ACCESS-------
		
#----ADVANCED USER ACCESS-----
#   PLEASE DO NOT MODIFY EXISTING CODE UNLESS YOU ARE PERFECTLY SURE WHAT YOU ARE DOING 

#forces uniqueness of items on the list
triggersFiltered_pp = []
for x in triggers_pp:
    if x not in triggersFiltered_pp:
        triggersFiltered_pp.append(x)

triggersFiltered_hi = []
for x in triggers_hi:
    if x not in triggersFiltered_hi:
        triggersFiltered_hi.append(x)

from RecExConfig.RecFlags import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.Logging        import logging

log = logging.getLogger( 'TrigMinBiasMonitoring' )

if rec.doHeavyIon == True or rec.doHIP == True:
    log.info('HI mode of min-bias trigger monitoring')
    monitoring_minbias = triggersFiltered_hi     
else:
    log.info('pp mode of min-bias trigger monitoring')
    monitoring_minbias = triggersFiltered_pp

