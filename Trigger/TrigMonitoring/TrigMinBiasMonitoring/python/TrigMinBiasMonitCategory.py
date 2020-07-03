# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

# PbPb triggers 2015
triggers_hi = [ "mb_sp",
                "mb_sptrk_ion_L1ZDC_A_C_VTE50",
                "mb_perf_L1MBTS_1_1",	
                "noalg_mb_L1TE50",
                ]

# pPb triggers 2016
triggers_pPb = ["mb_sptrk",
               #"mb_sptrk_noisesup",
               #"mb_sptrk_costr", 
               "noalg_L1RD0_FILLED",
               #"noalg_mb_L1RD0_EMPTY",
               "noalg_mb_L1MBTS_2",
               "mb_perf_L1MBTS_2",
               "mb_mbts_L1MBTS_2",
               "noalg_mb_L1ZDC_A",
               "noalg_mb_L1ZDC_C",
               "mb_sp1800_hmtperf_L1TE5",
               "noalg_mb_L1TE5",
               "hi_upc_FgapAC_mb_sptrk_L1MBTS_2",
               "hi_upc_FgapAC_mb_sptrk_exclusiveloose",
               "mb_sptrk_exclusiveloose_L1ZDC",
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

triggersFiltered_pPb = []
for x in triggers_pPb:
    if x not in triggersFiltered_pPb:
        triggersFiltered_pPb.append(x)

from RecExConfig.RecFlags import rec
from AthenaCommon.Logging        import logging

log = logging.getLogger( 'TrigMinBiasMonitoring' )

if rec.doHeavyIon is True:
    log.info('HI mode of min-bias trigger monitoring')
    monitoring_minbias = triggersFiltered_hi     
elif rec.doHIP is True:
    log.info('doHI mode of min-bias trigger monitoring')
    monitoring_minbias = triggersFiltered_pPb
else:
    log.info('pp mode of min-bias trigger monitoring')
    monitoring_minbias = triggersFiltered_pp

