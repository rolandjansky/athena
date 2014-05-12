#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
#   This python script compares trigger counts from text files.  It assumes that the files are in 
#   the following directory structure
#
#    <rel>/<tag>/EF.txt,L2.txt,L1.txt,totalEvents.txt  (EF, L2, L1 counts and total events, respectively)
#
#  These summaries can be extracted from the Log files using the script extractTD.zsh
#
#   For example the command 
#
#   countsTable.py -lL1 -t'[triglist( "test","16.0.2.6", "e603_s932_r1787",2.2,"16-00-00","ATLAS-GEO",7,0), \
#                            triglist( "ref" ,"16.0.2.6", "e603_s932_r1812",2.2,"16-00-00","ATLAS-GEO",7,0)  ]'
#
#   compares the files in the directory  16.0.2.6/e603_s932_r1787  with those in 16.0.2.6/e603_s932_r1718 
#   there can be only one reference.
#
#   Note that only first three arguments to triglist are required.
#  
#   In the above example 2.2 is the pileup.  This is used for minbias samples if rates are requested.
#
#   The strings "ATLAS-GEO" and  "16-00-00"  are only used in printout 
#
#    the following linkes are useful for getting details on triggers and tags:
#
# https://twiki.cern.ch/twiki/bin/view/Atlas/MCTriggerDB
# https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasGeomDBTags
# http://www-f9.ijs.si/atlpy/atlprod/prodtag/?q=r520
#
#

import getopt
import sys
import string
import re
import glob

from math import sqrt

glevel="L1"
grates=False
gdir=""

#
# categorize triggers
#
#    P = primary, B = backup,  S = supporting, C = calibration
#
#    assigment to slices is below
#

TriggerGroups = {
    'Primary'             : [
                          #egamma
                          'e20_medium', '2e10_medium', 'e60_loose', 'g40_tight',
                          #tau
                          'tau100_medium', 'tau29_tau38_medium1',
                          #tau+X
                          'tau16_loose_e15_tight', 'tau16_medium_mu15i', 'tau16i_loose_mu10', 'tau29_medium_xe35_noMu',
                          #muon
                          'mu20', 'mu40_MSonlyBarrel', 'mu20_slow','2mu10','mu15mu10', 
                          #Bphysics
                          '2mu4DiMu','mu4_Upsilon_FS',
                          #jet
                          'j115', 
                          #forward jet
                          '2fj18', 
                          #jet+X
                          'j70_xe30', '2j42_xe30',
                          #b-jet
                          '2b23_3L1J23', '3b18_4L1J18',
                          #MET
                          'xe50', 'xe50_noMu', 
                          ],
    'Supporting'          : [ ],
    'Calibration'         : [ ],
    'Backup'              : [ ],
    'Commissioning'       : [ ],
    'Test'                : [ ],
    }


#
#   triggers assigment to slices -- triggers can be assigned to more than one slice
#

#egamma#
electron=[
    'e20_medium',
    'e18_medium',
    'e30_loose',
    'e5_medium1_trk2',
    'e5_medium1_trk3',
    'e5_tight_SiTrk',
    'e5_tight_TRT',
    'e5_tight_e5_NoCut',
    'em3_empty_larcalib',
    'g17_etcut_EFxe30_noMu',
    'e5_medium_trk3',
    'e5_tight',
    'EM10',
    'EM10I',
    'EM14',
    'EM14I',
    'EM3',
    'EM5',
    'EM85',
    '2EM3',
    '2EM3_EM5',
    '2EM5',
    '2EM10',
    '2EM10I',
    '2EM14',
    '3EM5',
    '3EM10',
    'e10_NoCut',
    'e5_medium',  'e5_medium1','e5_medium_Ringer', 
    'e6_medium', 'e6_medium1','e7_medium',
    'e10_medium',  'e10_medium_SiTrk_robust', 
    'e10_loose','e10_loose_IdScan','e10_loose_SiTrk','e10_medium_SiTrk',
    'e10_loose_TRT',
    'e12_medium',
    'em15_passHLT','em15i_passHLT',
    'e15_medium', 'e15i_medium',
    'e20_loose_SiTrk_robust', 'e5_medium_e10_loose',  
    'e20_loose_IdScan','e20_loose_NoTrkCut','e20_loose_SiTrk','e20i_loose', 
    'em20_passHLT',
    'e20_loose', 
    'e20_loose_passL2', 'e20_loose_passEF', 'e20_loose_passHLT', 
    'em20i_passHLT',
    'e20_loose_NoIDTrkCut','e20_loose_TRT',
    'e25_medium','e25i_medium',
    'e22i_tight', 'e25_loose', 'e25i_loose', 'e25i_medium1',
    'e55_loose1', 'e105_loose1',
    'em105_passHLT', 
    'g3_nocut',
    'g3_nocut_larcalib',
    'g10', 
    'g10_calib','g10_loose','g10_nocut',
    'g10_larcalib','g3_larcalib',
    'g10_loose_larcalib''g20_loose_larcalib',
    'g15','g15i', 'g15_loose',
    'g20_loose','g20_loose_cnv',
    'g20_larcalib','g20i_larcalib',
    'g20i','g20i_loose','g20i_calib','g25i',
    'g20','g25','g25_loose',
    'g25i_loose', 'g55_tight', 'g105', 'g150',
    'g20_loose_passL2', 'g20_loose_passEF', 'g20_loose_passHLT',
    '2e5_medium1', '2e6_medium', '2e6_medium1', 'e12_medium',
    '2e5_medium', '2e5_medium1', '2e6_medium', '2e6_medium1',
    '2g20',
    '2e12_tight', '2e15_medium', '3e15_medium', '2e20_loose',
    '2g17i_tight', 'Zee', 'Jpsiee',
    '2g10_loose', '2g20_loose','3g10_loose',
    'g10_loose_larcalib','g20_loose_larcalib','g20_loose_xe20',
    'g20i_loose_larcalib','g25_loose_xe30',
    'e5_medium_L2SW',
    'Zee_monitor',
    'e10_loose_passEF',
    'e10_loose_passL2',
    'e3_NoCut',
    'e3_NoCut_IdScan',
    'e3_NoCut_TRT',
    'e5_EFfullcalo',
    'e5_NoCut',
    'e5_NoCut_FwdBackTrk',
    'e5_NoCut_IdScan',
    'e5_NoCut_L2SW',
    'e5_NoCut_Ringer',
    'e5_NoCut_SiTrk',
    'e5_NoCut_TRT',
    'e5_NoCut_cells',
    'e5_loose',
    'e5_loose_IdScan',
    'e5_loose_NoIDTrkCut',
    'e5_loose_TRT',
    'e5_medium_IdScan',
    'e5_medium_TRT',
    'g5_EFfullcalo',
    'g5_loose',
    'g5_loose_larcalib',
    'g5_nocut',
    'e10i_loose',
    'e15_loose',
    'e15i_loose',
    'e3_loose',
    'e3_loose_IdScan',
    'e3_loose_TRT',
    'e3_medium',
    'e3_medium_IdScan',
    'e3_medium_TRT',
    'e5_NoCut_cosmic',
    'e5_NoCut_firstempty',
    'e5_NoCut_unpaired',
    'e6_loose',
    'g10i_loose',
    'g15i_loose',
    'g3_loose',
    'g3_nocut_unpaired',
    'g5_nocut_cosmic',
    'g5_nocut_firstempty',
    'g5_nocut_unpaired',
    'g6_loose',
    '2e3_NoCut',
    '2e3_loose_EM2',
    '2e3_medium_EM2',
    '2e6_nocut',
    '2g10_nocut',
    '2g3_loose',
    '2g3_nocut',
    '2g5_loose',
    'L1ItemStreamer_L1_2EM2',
    'L1ItemStreamer_L1_2EM3',
    'L1ItemStreamer_L1_2EM4',
    'L1ItemStreamer_L1_2EM5',
    'L1ItemStreamer_L1_2EM5I',
    'L1ItemStreamer_L1_EM10',
    'L1ItemStreamer_L1_EM10I',
    'L1ItemStreamer_L1_EM14',
    'L1ItemStreamer_L1_EM2',
    'L1ItemStreamer_L1_EM3',
    'L1ItemStreamer_L1_EM3_EMPTY',
    'L1ItemStreamer_L1_EM3_FIRSTEMPTY',
    'L1ItemStreamer_L1_EM3_MV',
    'L1ItemStreamer_L1_EM3_UNPAIRED',
    'L1ItemStreamer_L1_EM4',
    'L1ItemStreamer_L1_EM5',
    'L1ItemStreamer_L1_EM5I',
    'SeededStreamerL1Calo',
    'SeededStreamerL1CaloEM',
    'SeededStreamerCosmicCalo',
    'SeededStreamerCosmicCaloEM',
    '2EM2',
    '2EM4',
    '2EM5I',
    'EM2',
    'EM3_EMPTY',
    'EM3_FIRSTEMPTY',
    'EM3_UNPAIRED',
    'EM4',
    'EM5I',
    '2e3_loose',
    '2e3_medium',
    '2e5_loose',
    'e10_medium_IdScan',
    'e10_medium_TRT',
    'L1ItemStreamer_L1_EM2_UNPAIRED',
    # new in pp
    '2e10_loose',
    '2e3_loose_IdScan',
    '2e3_loose_cells',
    '2e3_tight',
    '2g15_loose',
    '2g7_loose',
    'e10_NoCut_IdScan',
    'e10_NoCut_TRT',
    'e10_loose_mu10',
    'e10_medium_NoIDTrkCut',
    'e10_tight',
    'e15_NoCut',
    'e15_NoCut_IdScan',
    'e15_NoCut_TRT',
    'e15_loose_NoIDTrkCut',
    'e15_tight',
    'e20_loose_xe20_noMu',
    'e5_loose_mu4',
    'e5_medium_cells',
    'em3_empty_noAlg',
    'g11_etcut',
    'g20_loose_xe20_noMu',
    'g20_loose_xe30_noMu',
    'g20_tight',
    'g30_loose',
    'g30_tight',
    'g3_loose_larcalib',
    'g40_loose',
    'g40_loose_larcalib',
    'g40_tight',
    'g50_loose',
    'g50_loose_larcalib',
    'g7_loose',
    '2e10_medium',
    '2e15_loose',
    '2e3_loose_SiTrk',
    '2e3_loose_TRT',
    '2e3_medium_SiTrk',
    '2e3_medium_TRT',
    '2e5_medium_SiTrk',
    '2e5_medium_TRT',
    'L1Calo_NoAlg',
    'e10_medium_IDTrkNoCut',
    'e10_tight_TRT',
    'e15_loose_IDTrkNoCut',
    'e15_loose_TRT',
    'e15_medium_SiTrk',
    'e15_medium_TRT',
    'e15_tight_TRT',
    'e20_loose_IDTrkNoCut',
    'e20_loose_xe30_noMu',
    'e3_loose_SiTrk',
    'e3_medium_SiTrk',
    'e5_medium_SiTrk',
    'g17_etcut',
    'g17_etcut_EFxe20_noMu',
    'g25_loose_xe30_noMu',
    'g3_NoCut_unpaired_iso',
    'g3_NoCut_unpaired_noniso',
    'g5_NoCut_cosmic',
    'g5_loose_cnv',

    'EM2_UNPAIRED',
    'EM2_UNPAIRED_ISO',
    'EM2_UNPAIRED_NONISO',
    '2e5_tight',


    ]

#muon#
muon   =[
    'mu20',
    'mu13_MG_tight',
    '2mu6_MG',
    'mu10_tight',
    'mu13_MG',
    'mu13_tight',
    'mu15_MG',
    'mu40_MSonly',
    'mu4_Jpsimumu_SiTrk_FS',
    'mu4_Upsimumu_SiTrk_FS',
    'mu0_NoAlg',
    'mu0_comm_NoAlg',
    'mu0_comm_empty_NoAlg',
    'mu0_comm_firstempty_NoAlg',
    'mu0_comm_unpaired_iso_NoAlg',
    'mu0_comm_unpaired_noniso_NoAlg',
           'mu0_empty_NoAlg',
           'mu0_firstempty_NoAlg',
           'mu0_outOfTime1',
           'mu0_outOfTime2',
           'mu0_unpaired_iso_NoAlg',
           'mu0_unpaired_noniso_NoAlg',
           'mu10_Ecut12',
           'mu10_Ecut123',
           'mu10_Ecut13',
           'mu10_IDTrkNoCut',
           'mu10_MSonly_Ecut12',
           'mu10_MSonly_Ecut123',
           'mu10_MSonly_Ecut13',
           'mu10_MSonly_tight',
           'mu10_NoAlg',
           'mu15_NoAlg',
           'mu20_NoAlg',
           'mu4_IDTrkNoCut',
           'mu4_MSonly_outOfTime',
           'mu4_comm_unpaired_iso',
           'mu4_tile_cosmic',
           'mu4_unpaired_iso',
           'mu6_Ecut12',
           'mu6_Ecut123',
           'mu6_Ecut13',
           'mu6_Ecut2',
           'mu6_Ecut3',
           'mu6_IDTrkNoCut',
           'mu6_MSonly_Ecut12',
           'mu6_MSonly_Ecut123',
           'mu6_MSonly_Ecut13',
           'mu6_MSonly_Ecut2',
           'mu6_MSonly_Ecut3',
           'mu6_MSonly_outOfTime',
           'mu6_NoAlg',

         'MU0',
         'MU0_COMM',
         'MU6',
         'MU10',
         'MU11',
         'MU15',
         'EM19I',
         'MU20',
         'MU40',
         '2MU0',
         '2MU6',
         '2MU10',
         '2MU20',
         '3MU6',
         'mu4',  'mu4_SiTrk','mu4_trod','mu4_tile',
         'mu6',
         'mu10','mu10i_loose', 'mu10_SiTrk','mu10_MG', 'mu10_MuTagIMO',
         'mu15', 'mu15i_loose', 'mu20i_loose',
         'mu20_MSonly',    'mu20i_loose_MSonly',   'mu20_passHLT',
         'mu40_passHLT',
         'mu40',
         '2mu20', '3mu6', 'stau',
         '2mu4_cl',
         '2mu4','2mu6','2mu10' 
         'mu4_MG',
         'mu6_MG',
         'mu10_NoIDTrkCut',
         'mu15_xe20',
         'mu20_slow',
         'mu4_MG',
         'mu4_j10_matched',
         'mu4_j20',
         'mu4_j20_matched',
         'mu4_j40_matched',
         'mu4_j70_matched',
         'mu4_mu6',
         'mu10_NoIDTrkCut',
         '2mu10',
         '2MU0_MU6',
         'mu10_L2MSonly_EFFS_passL2',
         'mu10_MSonly',
         'mu10_MSonly_EFFS_passL2',
         'mu10_passHLT',
         'mu13',
         'mu4_L2MSonly_EFFS_passL2',
         'mu4_MB2_noL2_EFFS',
         'mu4_MSonly',
         'mu4_MSonly_EFFS_passL2',
         'mu4_NoIDTrkCut',
         'mu4_j5_matched',
         'mu4_passHLT',
         'mu4_tile_SiTrk',
         'mu4_trod_SiTrk',
         'mu6_L2MSonly_EFFS_passL2',
         'mu6_MSonly',
         'mu6_MSonly_EFFS_passL2',
         'mu6_SiTrk',
         'mu6_passHLT',
         '2mu4_MSonly',
         'mu10_MSonly_cosmic',
         'm`u10_cosmic',
         'mu10_muCombTag',
         'mu10_muCombTag_SiTrk',
         'mu4_DiMu_FS_noOS',
         'mu4_Jpsie5e3',
         'mu4_Jpsie5e3_FS',
         'mu4_Jpsie5e3_SiTrk',
         'mu4_Jpsie5e3_SiTrk_FS',
         'mu4_MSonly_MB2_noL2_EFFS',
         'mu4_MSonly_cosmic',
         'mu4_MSonly_firstempty',
         'mu4_comm',
         'mu4_comm_MSonly',
         'mu4_comm_MSonly_cosmic',
         'mu4_comm_MSonly_firstempty',
         'mu4_comm_cosmic',
         'mu4_comm_firstempty',
         'mu4_cosmic',
         'mu4_firstempty',
         'mu4_muCombTag',
         'mu4_muCombTag_SiTrk',
         'mu6_MSonly_cosmic',
         'mu6_MSonly_firstempty',
         'mu6_MSonly_unpaired',
         'mu6_cosmic',
         'mu6_firstempty',
         'mu6_muCombTag',
         'mu6_muCombTag_SiTrk',
         'mu6_unpaired',
         'mu10_cosmic',
         'L1ItemStreamer_L1_2MU0',
         'L1ItemStreamer_L1_2MU0_MU6',
         'L1ItemStreamer_L1_2MU10',
         'L1ItemStreamer_L1_2MU6',
         'L1ItemStreamer_L1_MU0',
         'L1ItemStreamer_L1_MU0_COMM',
         'L1ItemStreamer_L1_MU0_COMM_EMPTY',
         'L1ItemStreamer_L1_MU0_COMM_FIRSTEMPTY',
         'L1ItemStreamer_L1_MU0_COMM_UNPAIRED',
         'L1ItemStreamer_L1_MU0_EM3',
         'L1ItemStreamer_L1_MU0_EMPTY',
         'L1ItemStreamer_L1_MU0_FIRSTEMPTY',
         'L1ItemStreamer_L1_MU0_UNPAIRED',
         'L1ItemStreamer_L1_MU10',
         'L1ItemStreamer_L1_MU10_EMPTY',
         'L1ItemStreamer_L1_MU15',
         'L1ItemStreamer_L1_MU20',
         'L1ItemStreamer_L1_MU6',
         'L1ItemStreamer_L1_MU6_EM3',
         'L1ItemStreamer_L1_MU6_EMPTY',
         'L1ItemStreamer_L1_MU6_FIRSTEMPTY',
         'L1ItemStreamer_L1_MU6_J5',
         'L1ItemStreamer_L1_MU6_UNPAIRED',
         'SeededStreamerMuonswBeam',
         'SeededStreamerCosmicMuons',
         'MU0_COMM_EMPTY',
         'MU0_COMM_FIRSTEMPTY',
         'MU0_COMM_UNPAIRED',
         'MU0_EMPTY',
         'MU0_FIRSTEMPTY',
         'MU0_UNPAIRED',
         'MU10_EMPTY',
         'MU6_EMPTY',
         'MU6_FIRSTEMPTY',
         'MU6_UNPAIRED',
         'mu0_missingRoi',
         'mu0_rpcOnly',
         'mu4_unpaired',
         'mu0_comm_noAlg',
         'mu10_MSonly_MB2_noL2_EFFS',
         'mu10_j30',
         'mu30_MSonly',
         'mu4_j30',
         'mu4_l1j10_matched',
         'mu4_l1j15_matched',
         'mu4_l1j30_matched',
         'mu4_l1j55_matched',
         'mu4_l1j5_matched',
         'mu6_MSonly_MB2_noL2_EFFS',
         'mu6_NoIDTrkCut',
         'rd1_empty_noAlg',
         '2mu0_NoAlg',
         '2mu10_NoAlg',
         '2mu4_Jpsimumu_IDTrkNoCut',
         '2mu6_NoAlg',
         'L1Muon_NoAlg',
         'MU0_COMM_UNPAIRED_ISO',
         'MU0_COMM_UNPAIRED_NONISO',
         'MU0_UNPAIRED_ISO',
         'MU0_UNPAIRED_NONISO',
         'MU10_FIRSTEMPTY',
         '2MU0_FIRSTEMPTY',
        ]

#jet#
jet  = [
    '2j25_j45_anymct175',
    '2j25_j45_leadingmct150',
    '2j25_j45_leadingmct175',
    '2j25_j70_anymct150',
    '2j25_j70_anymct175',
    '2j25_j70_leadingmct150',
    '2j25_j70_leadingmct175',
    '2j30_anymct175',
    '2j30_leadingmct150',
    '2j30_leadingmct175',
    '2j20_deta3_5_jetNoEF',
    '2j20_deta5_jetNoEF',
    '2j25_j45_anymct150_NoEF',
    '2j25_j45_anymct175_NoEF',
    '2j25_j45_leadingmct150_NoEF',
    '2j25_j45_leadingmct175_NoEF',
    '2j25_j70_anymct150_NoEF',
    '2j25_j70_anymct175_NoEF',
    '2j25_j70_leadingmct150_NoEF',
    '2j25_j70_leadingmct175_NoEF',
    '2j30_anymct150_NoEF',
    '2j30_anymct175_NoEF',
    '2j30_deta3_5_jetNoEF',
    '2j30_leadingmct150_NoEF',
    '2j30_leadingmct175_NoEF',
    '2j35_deta3_5_jetNoEF',
    'j35_L1TAU_HV_jetNoEF',
    'j5_unpaired_iso_NoAlg',
    'j5_unpaired_noniso_NoAlg',
    'je300_NoEF',
    '2j25_j45_anymct150',
    '2j30_anymct150',
    '2j30_xe12_noMu',
    '2j30_xe20_noMu',
    '2j30_xe30_noMu',
    'j30_L1TAU_HV_jetNoEF',
    'j5_empty_larcalib',
    'j70_EFxe20_noMu',
    'j70_EFxe25_noMu',
    'j70_EFxe30_noMu',
    'j70_EFxe35_noMu',
    'j70_EFxe40_noMu',
    '2fj30_jetNoEF',
    '2fj50_jetNoEF',
    '2fj75_jetNoEF',
    '2j20_jetNoEF',
    '2j25_j45_anymct100_NoEF',
    '2j25_j45_dphi03_NoEF',
    '2j25_j45_leadingmct100_NoEF',
    '2j25_j70_dphi03_NoEF',
    '2j30_anymct100_NoEF',
    '2j30_j90_dphi03_NoEF',
    '2j30_jetNoEF',
    '2j30_leadingmct100_NoEF',
    '2j35_jetNoEF',
    '2j50_jetNoEF',
    '2j75_jetNoEF',
    '3j20_j50_jetNoEF',
    '3j30_2j50_jetNoEF',
    '3j30_jetNoEF',
    '3j35_jetNoEF',
    '3j50_jetNoEF',
    '4j20_j35_jetNoEF',
    '4j20_j50_jetNoEF',
    '4j30_3j35_jetNoEF',
    '4j30_jetNoEF',
    '4j35_jetNoEF',
    '4j50_jetNoEF',
    '5j30_jetNoEF',
    '6j30_jetNoEF',
    'fj30_jetNoEF',
    'fj30_jetNoEF_larcalib',
    'fj50_jetNoEF',
    'fj50_jetNoEF_larcalib',
    'fj75_jetNoEF',
    'j20_jetNoEF',
    'j20_jetNoEF_larcalib',
    'j30_fj30_jetNoEF',
    'j30_jetNoEF',
    'j30_jetNoEF_cosmic',
    'j30_jetNoEF_firstempty',
    'j30_jetNoEF_unpaired',
    'j35_jetNoEF',
    'j50_jetNoEF',
    'j50_jetNoEF_cosmic',
    'j50_jetNoEF_firstempty',
    'j50_jetNoEF_larcalib',
    'j50_jetNoEF_unpaired',
    'j50_jetNoEF_xe20_noMu',
    'j50_jetNoEF_xe30_noMu',
    'j50_jetNoEF_xe40_noMu',
    'j75_jetNoEF',
    'j95_jetNoEF',
    'j95_jetNoEF_larcalib',
    'je135_NoEF',
    'je195_NoEF',
    'je255_NoEF',
    'je350_NoEF',


         'J05','J5','J10','J15','J23', 'J30', 'J55','J75','J95',
          'j40', 'J50'  , 'J60', 'J80','j80v2','2J10','2J15',
         'J110',  'J115', 'J140','j140','J150','J180','j200','J205', 'j260','J265', 'J350', 
         '3J10','3J15','3J15_J30','3J30',
         '4J10','4J15','4J30','5J10','6J10',
         '4J10_3J15',
         'j7','j10','j15','j20','j30','j60','j80','j90','j130', 'j170','j200v2',
         'JE120','JE220', 'JE280',  'JE340', 'JE400',
         'jE120','jE220',
         'jE280','jE340',
         '3J25' , '3J60' ,  '3J180',
         '4J45' , '4J80' ,  '4J95' , '4J125',
         'FJ18' , 'FJ35' ,  'FJ70' , 'FJ120', 
         '2FJ18', '2FJ35',  '2FJ70' ,
         '3j20','3j40','3j20_j80','3j40','3j40_j80','3j40c4','3j60v2','4j20_3j40_2j60',
         '3j80','4j20_3j40_2j60','4j20_3j40_j60',
         '4j30','4j40','4j80','5j20','6j20',
         '3j15_j60',
         '3j15_test',
         '3j30','3j30_j60',
         '3j30c4','3j60',
         '4j15',
         '4j35',
         '4j50',
         '4j125',
         'j15_3j30_j40',
         '4j15_3j30_2j40',
         '4j15_3j30_j40',
         '4j15_test','4j20','4j60'
         '4j35_3j45_2j50_j60', '5j10_4j35_3j45_j60', '4j45_j60', '3j18_j42', '3j35c4',
         'fj120', '2fj35', '2fj70',
         'je120', 'je220', 'je280', 'je340',
         'j35_L1tau40hv', 'j35trackless',
         '2j20_deta5','2j30_deta4','2j30_deta5','2j30_deta4','2j40_deta4',
         'j10_larcalib','j90_larcalib','j10v3','j20','j20v2','j300','j300v2',
         'j10v3_larcalib',
         'j80_larcalib',
         'j80_xe30',
         'j80v2_larcalib',
         '3J10_J30',
         '3J30_JE220',
         '4J15_JE220',
         '6J10_JE220',
         '2j15_deta5',
         '2j23_deta4',
         '2j23_deta5',
         '3j15',
         '4j60',
         '5j15',
         '6j15',
         'j10_c4',
         'j10_cosmic',
         'j10_noise',
         'j20_c4',
         'j20_noise',
         '2j10',
         '2j10_deta3_5',
         '2j10_deta5',
         '2j20',
         '2j20_deta3_5',
         '2j40',
         '2j40_deta3_5',
         'FJ18_larcalib',
         'FJ3',
         'FJ3_larcalib',
         '2FJ15',
         '2FJ5',
         'FJ10',
         'FJ15',
         'FJ15_larcalib',
         'FJ30',
         'FJ5',
         'FJ5_larcalib',
         '20_MV_MbSpTrk',
         'j20_win6',
         'j5_cosmic',
         'j5_empty_exotic_L1J10',
         'j5_empty_exotic_L1J30',
         'j5_firstempty',
         'j5_firstempty_exotic_L1J10',
         'j5_firstempty_exotic_L1J30',
         'j5_unpaired',
         'j5_unpaired_exotic_L1J10',
         'j5_unpaired_exotic_L1J30',
         'je100',
         'je60',
         'fj5_cosmic',
         'fj5_firstempty',
         'fj5_unpaired',
         'j20_MV_MbSpTrk',
         '4j10',
         'L1ItemStreamer_L1_2FJ15',
         'L1ItemStreamer_L1_2FJ18',
         'L1ItemStreamer_L1_2FJ3',
         'L1ItemStreamer_L1_2FJ5',
         'L1ItemStreamer_L1_2J10',
         'L1ItemStreamer_L1_2J10_win6',
         'L1ItemStreamer_L1_2J15',
         'L1ItemStreamer_L1_2J5',
         'L1ItemStreamer_L1_3J10',
         'L1ItemStreamer_L1_4J10',
         'L1ItemStreamer_L1_4J5',
         'L1ItemStreamer_L1_FJ10',
         'L1ItemStreamer_L1_FJ15',
         'L1ItemStreamer_L1_FJ18',
         'L1ItemStreamer_L1_FJ3',
         'L1ItemStreamer_L1_FJ30',
         'L1ItemStreamer_L1_FJ3_EMPTY',
         'L1ItemStreamer_L1_FJ3_UNPAIRED',
         'L1ItemStreamer_L1_FJ5',
         'L1ItemStreamer_L1_FJ5_EMPTY',
         'L1ItemStreamer_L1_FJ5_FIRSTEMPTY',
         'L1ItemStreamer_L1_FJ5_UNPAIRED',
         'L1ItemStreamer_L1_J10',
         'L1ItemStreamer_L1_J10_EMPTY',
         'L1ItemStreamer_L1_J10_FIRSTEMPTY',
         'L1ItemStreamer_L1_J10_UNPAIRED',
         'L1ItemStreamer_L1_J10_win6',
         'L1ItemStreamer_L1_J15',
         'L1ItemStreamer_L1_J15_LV',
         'L1ItemStreamer_L1_J15_MV',
         'L1ItemStreamer_L1_J15_NL',
         'L1ItemStreamer_L1_J30',
         'L1ItemStreamer_L1_J30_EMPTY',
         'L1ItemStreamer_L1_J30_FIRSTEMPTY',
         'L1ItemStreamer_L1_J30_UNPAIRED',
         'L1ItemStreamer_L1_J5',
         'L1ItemStreamer_L1_J55',
         'L1ItemStreamer_L1_J5_EMPTY',
         'L1ItemStreamer_L1_J5_FIRSTEMPTY',
         'L1ItemStreamer_L1_J5_UNPAIRED',
         'L1ItemStreamer_L1_J75',
         'L1ItemStreamer_L1_J95',
         'L1ItemStreamer_L1_JE100',
         'L1ItemStreamer_L1_JE120',
         'L1ItemStreamer_L1_JE220',
         'L1ItemStreamer_L1_JE280',
         'L1ItemStreamer_L1_JE60',
         'SeededStreamerL1CaloJet',
         '2j20_win6',
         '2j15',
         '2j15_deta3_5',
         '2j15_win6',
         '2j30',
         '2j30_deta3_5',
         '2j7',
         '2j7_deta3_5',
         '2j7_deta5',
         '4j7',
         'j15_noise',
         'j15_win6',
         'j20_Trackless_HV',
         'j7_cosmic',
         'j7_noise',
         'FJ3_EMPTY',
         'FJ3_UNPAIRED',
         'FJ5_EMPTY',
         'FJ5_FIRSTEMPTY',
         'FJ5_UNPAIRED',
         'J10_EMPTY',
         'J10_FIRSTEMPTY',
         'J10_UNPAIRED',
         'J10_win6',
         'J15_LV',
         'J15_MV',
         'J15_NL',
         'J30_EMPTY',
         'J30_FIRSTEMPTY',
         'J30_UNPAIRED',
         'J5_EMPTY',
         'J5_FIRSTEMPTY',
         'J5_UNPAIRED',
         'JE100',
         'JE60',
         '2FJ3',
         '2J10_win6',
         '2J5',
         '4J5',
         '2J10_J30',
         '2J10_J55',
         '2J15_J75',
         '2J15_J95',
         '2j15_j60_dphi04',
         '2j15_j90_dphi035',
         '2j30_j130_dphi03',
         '2j30_j170_dphi025',
         '2j20_j140_dphi035',
         '2j20_j80_dphi04',
         '2j60_j200_dphi03',
         '2j60_j260_dphi025',
# add in pp menu
         '2FJ10',
         '2FJ30',
         '2FJ55',
         '2j35',
         '2j35_deta3_5',
         '2j50',
         '2j75',
         '2l1j95_noAlg',
         '2lj10_j30_noAlg',
         '2lj10_j55_noAlg',
         '2lj15_j75_noAlg',
         '3j20_j50',
         '3j30_2j50',
         '3j30_j50',
         '3j35',
         '3j50',
         '4j20_j35',
         '4j20_j50',
         '4j30_3j35',
         '5j30',
         '6j30',
         'FJ10_larcalib',
         'FJ30_larcalib',
         'FJ55',
         'FJ95',
         'FJ95_larcalib',
         'fj10_empty_noAlg',
         'j50',
         'j50_cosmic',
         'j50_firstempty',
         'j50_larcalib',
         'j50_unpaired',
         'j50_xe20_noMu',
         'j50_xe30_noMu',
         'j50_xe40_noMu',
         'j5_empty_noAlg',
         'j75',
         'j95',
         'j95_larcalib',
         'l1j115_noAlg',
         'l1j95_noAlg',
         'l2j25_MV_MbSpTrk',
         'l2j30_MV_MbSpTrk',
         'j20_larcalib',
         'j30_cosmic',
         'j30_firstempty',
         'j30_fj10',
         'j30_unpaired',
         'j35',
         '2fj30',
         '2fj30_jetNoCut',
         '2fj50',
         '2fj50_jetNoCut',
         '2fj75',
         '2fj75_jetNoCut',
         '2j20_jetNoCut',
         '2j25_j45_anymct100_noEF',
         '2j25_j45_dphi03_noEF',
         '2j25_j45_leadingmct100_noEF',
         '2j25_j70_dphi03_noEF',
         '2j30_anymct100_noEF',
         '2j30_j90_dphi03_noEF',
         '2j30_jetNoCut',
         '2j30_leadingmct100_noEF',
         '2j35_jetNoCut',
         '2j50_jetNoCut',
         '2j75_jetNoCut',
         'L12J10_J30_NoAlg',
         'L12J10_J55_NoAlg',
         'L12J15_J75_NoAlg',
         'L12J95_NoAlg',
         'L1FJ95_NoAlg',
         'L1J115_NoAlg',
         'L1J95_NoAlg',
         'fj10_empty_NoAlg',
         'fj30',
         'fj30_jetNoCut',
         'fj30_jetNoCut_larcalib',
         'fj50',
         'fj50_jetNoCut',
         'fj50_jetNoCut_larcalib',
         'fj75',
         'fj75_jetNoCut',
         'fj95_jetNoCut_larcalib',

         'j115_jetNoCut_larcalib',
         'j20_jetNoCut',
         'j20_jetNoCut_larcalib',
         'j20a4',
         'j20a4fs',
         'j20a6',
         'j20a6fs',
         'j30_fj30_jetNoCut',
         'j30_jetNoCut',
         'j30_jetNoCut_cosmic',
         'j30_jetNoCut_firstempty',
         'j30_jetNoCut_unpaired',
         'j35_jetNoCut',
         'j50_jetNoCut',
         'j50_jetNoCut_cosmic',
         'j50_jetNoCut_firstempty',
         'j50_jetNoCut_larcalib',
         'j50_jetNoCut_unpaired',
         'j5_empty_NoAlg',
         'j75_jetNoCut',
         'j95_jetNoCut',
         'j95_jetNoCut_larcalib',
         'je135_noEF',
         'je195_noEF',
         'je255_noEF',
         'je350_noEF',

         '2J30',
         '2J55',
         '2J95',
         '3J10_2J30',
         '3J5_J30',

         '4J5_J15',
         '4J5_J30',

         'FJ10_EMPTY',
         'FJ10_FIRSTEMPTY',
         'FJ10_UNPAIRED_ISO',
         'J10_FJ10',

         'J5_UNPAIRED_ISO',
         'J5_UNPAIRED_NONISO',
         'JE140',
         'JE200',

         '2fj25',
         '2fj45',
         '2j25',
         '2j25_deta3_5',
         '2j25_j45_anymct100',
         '2j25_j45_dphi03',
         '2j25_j45_leadingmct100',
         '2j25_j70_dphi03',
         '2j30_anymct100',
         '2j30_j90_dphi03',
         '2j30_leadingmct100',
         '2j35_jetNoEF_xe20_noMu',
         '2j35_jetNoEF_xe30_noMu',
         '2j35_jetNoEF_xe40_noMu',
         '2j45',
         '2j70',
         '3j15_j45',
         '3j25',
         '3j25_2j45',
         '3j45',
         '4j15_j30',
         '4j15_j45',
         '4j25',
         '4j25_3j30',
         '4j45',
         '5j25',
         '6j25',
         'fj25',
         'fj25_larcalib',
         'fj45',
         'fj45_larcalib',
         'fj70',
         'j15_larcalib',
         'j25',
         'j25_MV_MbSpTrk',
         'j25_cosmic',
         'j25_firstempty',
         'j25_fj25',
         'j25_jetNoCut_cosmic',
         'j25_jetNoCut_firstempty',
         'j25_jetNoCut_unpaired',
         'j25_unpaired',
         'j30_MV_MbSpTrk',
         'j45',
         'j45_cosmic',
         'j45_firstempty',
         'j45_jetNoCut_cosmic',
         'j45_jetNoCut_firstempty',
         'j45_jetNoCut_unpaired',
         'j45_larcalib',
         'j45_unpaired',
         'j70',
         'je135',
         'je195',
         'je255',
         'je350',



          ]

#bjet#
bjet  =['2b23_3L1J23','3b23_3L1J23','3b18_4L1J18','3b20_4L1J10','3b20_4L1J20',
        '4b20_4L1J20',
          #backup
          '2b23_4L1J23','4b18_4L1J18',
          #supporting
          'b18','b18_IdScan','mu4_J18', 'mu4_J23', 'mu4_J35, Lmu4_J42',
        #back-up
        '2b23_4L1J23', '4b18_4L1J18',
        #test
        'b18_noCut', 'b20_noCut',
        #commissioning
        'b18_IDScan','b20_IDScan',
        # new
        'b20','b23','b40','b35','b42','b70',
        '1b40_2b20_3L1J10','1b40_2b20_3L1J20','11b40_2b20_3L1J20','1b40_2b20_4L1J20','1b40_3b20_4L1J10',
        '2b20_3L1J20','2b20_4L1J20','2b40_3L1J20','2b40_4L1J20','1b40_2b20_4L1J10','1b40_3b20_4L1J10',
        'b10',
        'b10_IDScan',
        'b10_noCut',
        '2b10_3L1J10',
        '2b10_4L1J5',
        '2b15_3L1J15', 
        '3b10_4L1J10',
        '3b10_4L1J5',
        '3b15_4L1J15',
        'b10_4L1J10',
        'b10_IDScan_noCut',
        'b10_NoIDTrkCut',
        'b15',
        'b15_4L1J5',
        'b20_IDScan_noCut',
        'b30',
        '1b30_2b15_3L1J10',
        '1b30_2b15_3L1J15',
        '1b30_2b15_4L1J15',
        '2b15_3b10_4L1J10',
        '2b30_3L1J15',
        '3b15_4L1J10',
        '3j20_j50_jetNoCut',
        '3j30_2j50_jetNoCut',
        '3j30_jetNoCut',
        '3j35_jetNoCut',
        '3j50_jetNoCut',
        '4j20_j35_jetNoCut',
        '4j20_j50_jetNoCut',
        '4j30_3j35_jetNoCut',
        '4j35_jetNoCut',
        '4j50_jetNoCut',
        '5j30_jetNoCut',
        '6j30_jetNoCut',
        'L1J95_NoAlg',
        'b10_IDTrkNoCut'
        ]

#tau#
tau     =[
    '2tau29_loose1',
    'tau16_medium',
    'tau16_medium_xe22_noMu',
    'tau16_medium_xe25_noMu',
    'tau16_medium_xe25_tight_noMu',
    'tau29_loose1',
    'tau12_loose_e10_tight',
    'tau5_empty_larcalib',
    'tauNoCut_hasTrk_e10_tight',
    'tau12_loose_3j30',
    'tau16_loose_xe25_tight_noMu',
    'tau12_loose_3j35_jetNoEF',
    'tau16_loose_3j35_jetNoEF',
          'TAU5',
          'TAU6I',
          'TAU8',
          'TAU20',
          'TAU11',
          'TAU30',
          'TAU6',
          '2TAU11',
          'TAU11I',
          '2TAU11I',
          '2TAU6',
          'trk9_loose','trk9i_id','trk16i_id','trk29_id',
          'trk9i_calib','trk16i_calib',
          'trk16_loose',
          'trk16i_IDCalib',
          'trk29_IDCalib',
          'trk9_IDCalib',
          'tau16i_loose', 
          'tau50_loose', 
          #backup
          'tau50_medium','tau84_loose', 'tau50_tight', 'tau125_loose',
          #tau combined
          '2tau20i_loose',
          #backup
          '2tau29i_loose',
          #supporting
          '2tau12', 
          '2tau20i_PT', 
          'tau16_loose_xe20', 
          'tau16_loose_xe25',
          'tau16i_xe30',
          #primary xe mixed
          'tau16i_EFxe30', 
          'tau16i_loose_EFxe30',
          #backup
          'tau16i_loose_xe30', 
          'tau16i_loose_EFxe40',
          'tau29i_medium_EFxe40', 
          'tau16i_medium_EFxe70',
          'tau38_xe40',
          'tau38_loose_xe40',
          # primary e
          'tau16i_e10','tau16i_e10_loose',
          #backup 
          'tau12_e10',
          'tau12_loose_e10_medium',
          'tau12_e10_loose',
          'tau12_loose_e10_loose',
          'tau16i_e15i',
          'tau16i_loose_e15i',
          'tau16i_e15i_medium',
          'tau16i_loose_e15i_medium',
          'tau20i_loose',
          #primary mu
          'tau16i_mu10',
          'tau16i_loose_mu10',
          'tau16i_mu6',
          'tau16i_loose_mu6',
          'tau16_mu10i',
          'tau16_loose_mu10i',
          #primary jets
          'tau16i_3j23', 
          'tau16i_loose_3j23', 
          'tau16i_j120',
          'tau16i_loose_j120',
          'tau20i_2b23', 
          'tau20i_loose_2b23', 
          #supporting
          'tau16i_j70_WO',
          'tau16i_loose_j70_WO',
          'tau16i_4j23',
          'tau16i_loose_4j23',
          'tau16i_2j23',
          'tau16i_loose_2j23',
          'tau16i_j70',
          'tau16i_loose_j70',
    
        #back-up
          'tau50_medium', 'tau50_tight', 'tau84_loose', 'tau125_loose', '2tau29i_loose', '2tau29i_medium',
        #test
          'tau12_loose', 'tau12_loose_PT', 'tau16_loose', 'tau16_loose_PT', 'tau20_loose', 'tau29_loose',
          'tau29i_loose','tau29i_medium', 'tau38_loose', 'tau50_loose_PT',
          'tauNoCut_TRT',
          #commissioning
          'tauNoCut', 'tauNoCut_SiTrk',
          #new
          'tau12_PT','tau16i_tight','tau16i_medium',
          '2tau20i_loose_PT',
          '2tau29_loose','2tau29i',
          '2tau20_loose',
          '2tau20_loose_PT',
          'tau16_loose_SiTrk',
          'tau16i_NoIDTrkCut',
          'tau50_loose_SiTrk',
          'tauNoCut_NoIDTrkCut',
          'tau20_loose_SiTrk',
          'tauNoCut_cells',
          'tauNoCut_cosmic',
          'tauNoCut_hasTrk',
          'tauNoCut_hasTrk_SiTrk',
          'tau12_loose_mu10',
          'tau16_loose_TAU8',
          'tau20_medium',
          'tau29_medium',
          'tau50_loose_TAU30',
          'tauNoCut_firstempty',
          'tauNoCut_hasTrk_xe20',
          'tauNoCut_unpaired',
          '2tau12_loose',
          '2tau16_loose',
          'L1ItemStreamer_L1_2TAU5',
          'L1ItemStreamer_L1_2TAU5_EM5',
          'L1ItemStreamer_L1_2TAU6',
          'L1ItemStreamer_L1_2TAU6I',
          'L1ItemStreamer_L1_TAU11',
          'L1ItemStreamer_L1_TAU11I',
          'L1ItemStreamer_L1_TAU20',
          'L1ItemStreamer_L1_TAU30',
          'L1ItemStreamer_L1_TAU5',
          'L1ItemStreamer_L1_TAU5_EMPTY',
          'L1ItemStreamer_L1_TAU5_FIRSTEMPTY',
          'L1ItemStreamer_L1_TAU5_MU6',
          'L1ItemStreamer_L1_TAU5_MV',
          'L1ItemStreamer_L1_TAU5_UNPAIRED',
          'L1ItemStreamer_L1_TAU5_XE10',
          'L1ItemStreamer_L1_TAU6',
          'L1ItemStreamer_L1_TAU6I',
          'L1ItemStreamer_L1_TAU6_XE10',
          'L1ItemStreamer_L1_TAU8',
          'SeededStreamerL1CaloTau',
          'TAU5_EMPTY',
          'TAU5_FIRSTEMPTY',
          'TAU5_MU6',
          'TAU5_UNPAIRED',
          '2TAU5',
          '2TAU6I',

          'hadCalib_trk9',
          'hadCalib_trk9phi',

          'tauNoCut_hasTrk9_xe25_tight',
          'tauNoCut_hasTrk9_xe30_medium',

          # new for pp menu
          'tau125_medium',
          'tau12_NoIDTrkCut',
          'tau38_medium',
          'tau50_NoIDTrkCut',
          'tau84_medium',
          '2tau38_loose',
         # keep xe triggers with tau

          'tauNoCut_hasTrk6_EFxe15',
          'tauNoCut_hasTrk6_SiTrk_xe20_noMu',
          'tauNoCut_hasTrk6_xe20_noMu',
          'tauNoCut_hasTrk9_xe20_noMu',
 
          'tau12_loose_EFxe12',
          'tau12_loose_SiTrk_xe15_noMu',
          'tau12_loose_xe15_noMu',
          'tau12_loose_xe20_noMu',
          'tau16_loose_xe20_noMu',
          'tau16_loose_xe25_noMu',
          'tau16_loose_xe30_noMu',
          'tau20_loose_xe25_noMu',
          'L1TAU30_NoAlg',
          'tau12_IDTrkNoCut',
          'tau12_loose_3j35',
          'tau12_loose_3j35_jetNoCut',
          'tau12_loose_EFxe12_noMu',
          'tau12_loose_IdScan_EFxe12_noMu',
          'tau12_loose_IdScan_xe15_noMu',
          'tau16_loose_3j35_jetNoCut',
          'tau50_IDTrkNoCut',
          'tau50_loose_IdScan',
          'tauNoCut_IdScan',
          'tauNoCut_hasTrk6_EFxe15_noMu',
          'tauNoCut_hasTrk6_IdScan_EFxe15_noMu',
          'tauNoCut_hasTrk6_IdScan_xe20_noMu',
          'tauNoCut_hasTrk_xe20_noMu',
          'tauNoCut_unpaired_iso',
          'tauNoCut_unpaired_noniso',
          'TAU50',
          'TAU5_3J5_2J15',
          'TAU5_4J5_3J15',
          'TAU5_UNPAIRED_ISO',
          'TAU5_UNPAIRED_NONISO',
          'TAU6_MU15',
          '2TAU6_EM10',
          '2TAU6_EM5',
        ]
#MET#
#xe#
xe      =[
    'te450_medium',
    'te500_loose',
    'xe40_tight_noMu',
    'te250_loose',
    'xe27_tight_noMu',
    'xe32_tight_noMu',
          'XE10',
          'XE15',
          'XE20',
          'XE25',
          'XE30',
          'TE150',
          'TE250',
          'TE350',
          'TE650',
          'TE360', 
          'xe15', 
          'xe15_noMu',
          'xe15_loose',  'xe15_loose_noMu',
          'xe20',
          'xe20_noMu',
          'xe20_loose',  'xe20_loose_noMu',
          'xe23_loose',  'xe132_noMu',
          'xe25',
          'xe25_noMu',
          'xe25_loose',  'xe25_loose_noMu',
          'xe25_tight',  'xe25_tight_noMu',
          'xe30', 
          'xe30_noMu',
          'xe30_allL1',  'xe30_allL1_noMu',
          'xe30_loose',  'xe30_loose_noMu',
          'xe30_tight',  'xe30_tight_noMu',
          'xe30_unbiased',  'xe30_unbiased_noMu',
          'xe32',
          'xe32_noMu',
          'xe35_noMu',
          'xe35','xe35_loose','xe35_loose_noMu',
          'xe38','xe38_noMu',
          'xe40', 
          'xe40_noMu',
          'xe40_loose'
          'xe40_loose_noMu'
          'xe40_debug',
          'xe45_loose',
          'xe45_loose_noMu',
          'xe50', 
          'xe50_noMu',
          'xe55',
          'xe55_noMu',
          'xe65',
          'xe65_noMu',
          'xe70',
          'xe70_noMu',
          'xe80',
          'xe80_noMu',
          'te150','te250','te360','te650'
        
          #back-up
          'xe80', 'xe80_noMu', 'te650',
          #test
          'xe30_unseeded', #crashes
          #
          'xe35_tight','xe35_tight_noMu','xe40_debug',
          'xe40_loose','xe40_loose_noMu','xe45_loose_noMu'
          'te100',
          'xe20_FEB',
          'xe20_allCells',
          'xe30_allL1_FEB',
          'xe30_allL1_allCells',
          'xe30_unbiased_FEB',
          'xe30_unbiased_allCells',
          'te100',
          'te150_medium',
          'te250_medium',
          'te350_medium',
          'te90',
          'xe45_noMu',
          'xe60_medium_noMu',
          'xe70_medium_noMu',
          'xe80_medium_noMu',
          'L1ItemStreamer_L1_TE10',
          'L1ItemStreamer_L1_TE100',
          'L1ItemStreamer_L1_TE180',
          'L1ItemStreamer_L1_TE30',
          'L1ItemStreamer_L1_XE10',
          'L1ItemStreamer_L1_XE15',
          'L1ItemStreamer_L1_XE20',
          'L1ItemStreamer_L1_XE25',
          'L1ItemStreamer_L1_XE30',
          'L1ItemStreamer_L1_XE35',
          'L1ItemStreamer_L1_XE40',
          'L1ItemStreamer_L1_XE50',
          'SeededStreamerL1CaloMET',
          'te10',
          'te100_medium',
          'te180_medium',
          'te30_medium',
          'xe12',
          'xe12_FEB',
          'xe12_allCells',
          'xe12_noMu',
          'xe15_FEB',
          'xe15_allCells',
          'xe40_medium_noMu',
          'xe50_medium_noMu',
          'TE10',
          'TE100',
          'TE180',
          'TE30',
          'XE35',
          'XE40',
          'XE50',
          'te100_loose',
          'te180',
          'xe12_loose',
          'xe12_loose_noMu',
          'xe12_medium',
          'xe12_medium_noMu',
          'xe15_medium',
          'xe15_medium_noMu',
          'xe17_tight',
          'xe17_tight_noMu',
          'xe20_medium_noMu',
          'xe25_medium',
          'xe25_medium_noMu',
          'xe45',
          'xe55_medium_noMu',
          'te300_loose',
          'xe30_medium',
          'xe30_medium_noMu',
          'xe35_medium_noMu',
          'xe40_medium',
          'xe65_loose',
          'xe65_medium_noMu',

          # new for pp
          'te150_tight',
          'te90_medium',

          'xe15_unbiased_noMu',
          'xe20_tight_noMu',
          'xe20_tight_vfj_noMu',
          'xe25_tight_vfj_noMu',
          'xe25_vfj_noMu',
          'xe30_medium_vfj_noMu',
          'xe30_tight_vfj_noMu',
          'xe30_vfj_noMu',
          'xe60_medium',
          'xe80_medium',
           # put mixed jet and xe algos here
          '2J15_XE10',
          '2J15_XE25',
          'J15_XE15_EM10',
          'J30_XE10',
          'J30_XE25',
          'TE20',
          'TE50',


          'L1J55_EFxe20_noMu',
          'L1J55_EFxe25_noMu',
          'L1J55_EFxe30_noMu',
          'L1J55_EFxe35_noMu',
          'L1J55_EFxe40_noMu',

          'j45_xe12_noMu',
          'j45_xe20_noMu',
          'j45_xe30_noMu',

          'te20_medium',
          'te50_tight',
          'xe15_medium_vfj_noMu',
          'xe15_tight_noMu',
          'xe15_tight_vfj_noMu',
          'xe15_vfj_noMu',
          'xe17_tight_vfj_noMu',
          'xe20_vfj_noMu',
          'xe22_loose_noMu',
          'xe22_tight_noMu',
          'xe22_tight_vfj_noMu',

          'j75_jetNoEF_EFxe20_noMu',
          'j75_jetNoEF_EFxe25_noMu',
          'j75_jetNoEF_EFxe30_noMu',
          'j75_jetNoEF_EFxe35_noMu',
          'j75_jetNoEF_EFxe40_noMu',

          'te350',

          ]

#mixed#
mixed   =[
    'mu4_j30_jetNoEF',
    'mu4_j20_jetNoEF',
    'mu4_j25',
    'j30_jetNoCut_xe20_e15_medium',
    'j30_xe20_e15_medium',
    'e10_xe30', 'e10_loose_xe30', 
          'e15_xe20', 'e15_loose_xe20',
          'e20_xe15', 'e20_loose_xe15',
          'e20_xe30', 'e20_loose_xe30',
          'g20_xe15', 'g20_loose_xe15',
          'e20_g20',  'e20_loose_g20',
          'e10_mu6',  'e10_loose_mu6', 
          '2e10_mu6', '2e10_loose_mu6',
          '2mu6_e10', '2mu6_e10_loose',
          'j70_xe30', 
          '2j42_xe30',
          'mu4_j10', 
          'm10_j18','4j23_e15i_loose', 
          '4j23_e15i', 
          '4j23_mu15',
          'j42_xe30_e15i', 'j42_xe30_e15i_loose'
          'j42_xe30_mu15',
          'g20_loose_xe15', '2g10_loose_mu6',
          #backup
          'tau12_loose_e10_loose', 'tau16i_loose_e15i_medium', 'tau16i_loose_EFxe40', 'tau16i_medium_EFxe70',
          'tau29i_medium_EFxe40', 'tau38_loose_xe40', 'tau16i_loose_mu6',
          'e20_loose_xe15', 'e20_loose_xe30',
          #test
          'tau16i_loose_xe30', 'tau16i_loose_4j23_WO', 'tau16i_loose_2j23', 'tau16i_loose_j70',
          'tau16i_loose_2j70', 'tau20i_loose_j120', 'tau16i_loose_mu15i_loose', 'tau16i_loose_EFxe30_j50',
          'mu15_xe15', 'mu20_xe30','2mu6_e10_loose', '2mu6_g10_loose', 
          '4j23_e15i_medium', '4j23_mu15', 'j42_xe30_e15i_medium', 'j42_xe30_mu15',
          'tau16i_mu10i_loose',
          'e10_loose_mu6', 
          '2j40_xe30',
          '4j20_e15i_medium',
          '4j20_mu15',
          'e20_loose_xe20',
          'j40_xe30_e15i_medium',
          'j40_xe30_mu15',
          'tau12_loose_xe20',
          'tau16_loose_3j40',
          'tau16_loose_4j40',
          'tau16_loose_4j40_WO',
          'tau16_loose_EFxe30',
          'tau16_loose_e10_loose',
          'tau16_loose_j140',
          'tau16_loose_j80',
          'tau16_loose_j80_WO',
          'tau16_loose_mu10',
          'tau16_loose_mu6',
          'tau16i_loose_mu10i_loose',
          'tau16i_loose_xe25',
          'tau20_loose_2b20',
          'tauNoCut11_hasTrk6_xe25',
          'tauNoCut_hasTrk6_xe25',
          'tauNoCut_hasTrk9_xe25',
          'TAU11_XE10',
          'TAU11_XE20',
          'TAU5_XE10',
          'TAU6I_2J5_J15',
          'TAU6I_MU10',
          'TAU6I_XE10',
          'TAU6_2J5_J30',
          'TAU6_2J5_J55',
          'TAU6_3J5_2J15',
          'TAU6_4J5_3J15',
          'TAU6_5J5_4J15',
          'TAU6_MU10',
          'TAU6_MU6',
          'TAU6_XE10',
          'MU0_EM3',
          'MU0_J10',
          'MU0_J15',
          'MU0_J30',
          'MU0_J5',
          'MU0_J55',
          'MU10_J5',
          'MU11_XE10',
          'MU15_XE10',
          'MU20_XE15',
          'MU6_EM3',
          'MU6_J5',
          'EM5_MU6',
          'J15_XE15_EM10I',
          'J15_XE20_MU11',
          'J15_XE20_MU15',
          'J30_XE15',
          'EM14_XE10',
          'EM14_XE15',
          '4J10_EM10I',
          '4J10_MU11',
          '4J10_MU15',
          '2EM5_MU6',
          '2J15_XE15',
          '2MU6_EM5',
          '2TAU5_EM5',
          '2TAU5_TAU6_EM5',
          '2TAU6I_EM10I',
          'tau16_loose_3j30',
          'tau16_loose_4j30',
          'tau16_loose_4j30_WO',
          'tau16_loose_j60',
          'tau16_loose_j60_WO',
          'tau16_loose_j90',
          '4j15_e15i_medium',
          'cost_monitor',
          'j30_xe20_e15i_medium',
          'j60_xe20',
          'mu4_j15',
          'L1ItemStreamer_L1_MU0_J10',
          'L1ItemStreamer_L1_MU0_J15',
          'L1ItemStreamer_L1_MU0_J5',
          'L1ItemStreamer_L1_MU0_MV',
          'L1ItemStreamer_L1_TAU6_4J5_3J15',
          'J15_XE15_MU15',

          'tau12_loose_2b15',
          'tau12_loose_3j40',



          'tau16_loose_2b15',
          'tau16_loose_3j35',
          'tau16_loose_e15_loose',
          'tau16_loose_mu15',
          '2j35_xe20_noMu',
          '2j35_xe30_noMu',
          '2j35_xe40_noMu',
          '2j35_jetNoCut_xe20_noMu',
          '2j35_jetNoCut_xe30_noMu',
          '2j35_jetNoCut_xe40_noMu',
          '4j30_e15_medium',
          '4j30_jetNoCut_e15_medium',
          '4j30_jetNoCut_mu15',
          '4j30_jetNoCut',
          '4j30_mu15',  

          'e5_medium_mu4',
          'j35_jetNoCut_xe30_e15_medium',
          'j35_jetNoCut_xe30_mu15',
          'j35_xe30_e15_medium',
          'j35_xe30_mu15',
          'j50_jetNoCut_xe20_noMu',
          'j50_jetNoCut_xe30_noMu',
          'j50_jetNoCut_xe40_noMu',
          'mu4_L1J10_matched',
          'mu4_L1J15_matched',
          'mu4_L1J30_matched',
          'mu4_L1J55_matched',
          'mu4_L1J5_matched',
          'mu4_j20_jetNoCut',
          'mu4_j30_jetNoCut',

          'MU10_J10',
          '4J10_EM10',

          'EM5_MU10',

          '4j25_e15_medium',
          '4j25_jetNoCut_e15_medium',

        ]

exotic = [ 
    '2MUL1_l2j30_HV',
    'j40_L1TAU_HV',
    'j40_Trackless_HV',
    '2MUL1_j40_HV',
    '2MUL1_j40_HV',
    'j30_L1TAU_HV',
    'j30_Trackless_HV',
    'l2j30_Trackless_HV',
    'j35_L1TAU_HV',

           ]
#twophoton#
twophoton = [ 'MU0_MV',    
              'EM3_MV',
              'e5_medium_MV',       
              'MU0_MV',
              'EM3_MV',
              'MU0_MV',
              'mu4_MV',
              'J10_MV',
              'j15_MV_MbSpTrk',
              'TAU5_MV',
              'tauNoCut_hasTrk_MV',
            ]
#calibration#
calibration = [ 
    'TRT',
    'Background',
    'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_AllID',
    'trk16_Central_Tau_IDCalib',
    'trk16_Fwd_Tau_IDCalib',
    'trk29_Central_Tau_IDCalib',
    'trk29_Fwd_Tau_IDCalib',
    'trk9_Central_Tau_IDCalib',
    'trk9_Central_Tau_SiTrack_IDCalib',
    'trk9_Fwd_Tau_IDCalib',
    'trk9_Fwd_Tau_SiTrack_IDCalib',
    'TileCalib_cis',
    'TileCalib_laser',
    'TileMon',
# also include montoring here 
    'PixelNoise',
    'SCTNoise',
    'L2IdScan_FS',
    'L2IdScan_FS_MB2',
    'L2SiTrack_FS',
    'L2SiTrack_FS_MB2',
    'L1ItemStreamer_L1_BCM_AtoC',
    'L1ItemStreamer_L1_BCM_AtoC_EMPTY',
    'L1ItemStreamer_L1_BCM_CtoA',
    'L1ItemStreamer_L1_BCM_CtoA_EMPTY',
    'L1ItemStreamer_L1_BCM_HT',
    'L1ItemStreamer_L1_BCM_HT_EMPTY',
    'L1ItemStreamer_L1_BCM_MUL_1',
    'L1ItemStreamer_L1_BCM_MUL_1_EMPTY',
    'L1ItemStreamer_L1_BCM_MUL_2',
    'L1ItemStreamer_L1_BCM_MUL_2_EMPTY',
    'L1ItemStreamer_L1_BCM_MUL_3',
    'L1ItemStreamer_L1_BCM_MUL_3_EMPTY',
    'L1ItemStreamer_L1_BCM_MUL_4',
    'L1ItemStreamer_L1_BCM_MUL_4_EMPTY',
    'L1ItemStreamer_L1_BCM_MUL_5',
    'L1ItemStreamer_L1_BCM_MUL_5_EMPTY',
    'L1ItemStreamer_L1_BCM_MUL_6',
    'L1ItemStreamer_L1_BCM_MUL_6_EMPTY',
    'L1ItemStreamer_L1_BCM_Wide',
    'L1ItemStreamer_L1_BCM_Wide_EMPTY',
    'L1ItemStreamer_L1_BGRP0',
    'L1ItemStreamer_L1_BGRP1',
    'L1ItemStreamer_L1_BPTX0_BGRP0',
    'L1ItemStreamer_L1_BPTX0_BPTX1_BGRP0',
    'L1ItemStreamer_L1_BPTX1_BGRP0',
    'L1ItemStreamer_L1_NIM_S8C2B21',
    'L1ItemStreamer_L1_NIM_S8C2B22',
    'L1ItemStreamer_L1_NIM_S8C2B23',
    'L1ItemStreamer_L1_NIM_S8C2B24',
    'L1ItemStreamer_L1_NIM_S8C2B25',
    'L1ItemStreamer_L1_NIM_S8C2B26',
    'L1ItemStreamer_L1_NIM_S8C2B27',
    'L1ItemStreamer_L1_NIM_S9C0A20',
    'L1ItemStreamer_L1_NIM_S9C0B21',
    'L1ItemStreamer_L1_NIM_S9C0B22',
    'L1ItemStreamer_L1_NIM_S9C0B23',
    'L1ItemStreamer_L1_NIM_S9C0B24',
    'L1ItemStreamer_L1_NIM_S9C1C19',
    'L1ItemStreamer_L1_NIM_S9C1C20',
    'L1ItemStreamer_L1_RD0_EMPTY',
    'L1ItemStreamer_L1_RD0_FILLED',
    'L1ItemStreamer_L1_RD0_FIRSTEMPTY',
    'L1ItemStreamer_L1_RD0_UNPAIRED',
    'L1ItemStreamer_L1_RD1_FILLED',
    'zerobias_noAlg',
    'L1ItemStreamer_L1_TRT',
    'L2IdScan_FS',
    'L2IdScan_FS_MB2',
    'L2SiTrack_FS',
    'L2SiTrack_FS_MB2',
    'PixelNoise',
    'SCTNoise',
    'EFInsideOut_FS',
    'EFInsideOut_FS_MB2',
    'EFTRTOnly_FS',
    'EFTRTOnly_FS_MB2',
    'InDetMon_FS',
    'SeededStreamerRNDM',
    'SeededStreamerWarmStart',
    'SeededStreamerZeroBias',
    'trk2_FullScan_IDSCAN_IDCalib',
    'trk2_FullScan_SiTrk_IDCalib',
    'trk2_IDSCAN_IDCalib',
    'trk2_SiTrk_IDCalib',
    'trk2_eGamma_IDSCAN_IDCalib',
    'trk2_eGamma_SiTrk_IDCalib',
    'trk6_IDSCAN_IDCalib',
    'trk6_SiTrk_IDCalib',
    'allID_IDCalib',
    'NIM_S8C2B21',
    'NIM_S8C2B22',
    'NIM_S8C2B23',
    'NIM_S8C2B24',
    'NIM_S8C2B25',
    'NIM_S8C2B26',
    'NIM_S8C2B27',
    'NIM_S9C0A20',
    'NIM_S9C0B21',
    'NIM_S9C0B22',
    'NIM_S9C0B23',
    'NIM_S9C0B24',
    'NIM_S9C1C19',
    'NIM_S9C1C20',
    'RD0_EMPTY',
    'RD0_FIRSTEMPTY',
    'RD0_UNPAIRED',
    'RD1_FILLED',
    'CosmicsAllTeEFID',
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo',
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo',
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo',
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo',
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics',
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField',
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel',
    'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel',
    
    'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel_IDCalib',
    
    ]
    
#beamspot#
beamspot = [    'vtxbeamspot_FSTracks',
                'vtxbeamspot_FSTracks_SiTrk',
                'vtxbeamspot_FSTracks_SiTrk_allL1',
                'vtxbeamspot_FSTracks_allL1',
                'vtxbeamspot_activeTE_SiTrk_peb',
                'vtxbeamspot_activeTE_SiTrk_peb_allL1',
                'vtxbeamspot_activeTE_peb',
                'vtxbeamspot_activeTE_peb_allL1',
                'vtxbeamspot_allTE_SiTrk_peb',
                'vtxbeamspot_allTE_SiTrk_peb_allL1',
                'vtxbeamspot_allTE_peb',
                'vtxbeamspot_allTE_peb_allL1',
                'BCM_AtoC',
                'BCM_AtoC_EMPTY',
                'BCM_CtoA',
                'BCM_CtoA_EMPTY',
                'BCM_HT',
                'BCM_HT_EMPTY',
                'BCM_MUL_1',
                'BCM_MUL_1_EMPTY',
                'BCM_MUL_2',
                'BCM_MUL_2_EMPTY',
                'BCM_MUL_3',
                'BCM_MUL_3_EMPTY',
                'BCM_MUL_4',
                'BCM_MUL_4_EMPTY',
                'BCM_MUL_5',
                'BCM_MUL_5_EMPTY',
                'BCM_MUL_6',
                'BCM_MUL_6_EMPTY',
                'BCM_Wide_EMPTY',
                'BGRP0',
                'BGRP1',
                'BPTX0_BGRP0',
                'BPTX0_BPTX1_BGRP0',
                'BPTX1_BGRP0',
                'CALREQ0',
                'CALREQ1',
                'CALREQ2',
                'L1A_Mon',
                'L1TRT_NoAlg',



                ]

bphysics = ['MU4_DiMu_MG_FS','MU4_DiMu_SiTrk_FS','MU4_DsPhiPi_SiTrk_FS','MU4_Jpsie5e3_SiTrk_FS',
            'MU4_DiMu_FS','MU4_Jpsimumu_FS','MU4_Upsimumu_FS','MU4_Bmumu_FS',
            'MU10_DsPhiPi','MU10_DsPhiPi_FS','MU4_DiMu_MG','MU4_DiMu_MG_FS ','MU4_Trk_Jpsi','MU4_Trk_Jpsi_FS',
            '2MU4_DiMu','2MU4_Jpsimumu','2MU4_Upsimumu','2MU4_Bmumu',
            'MU4_Jpsie5e3_FS','mu4_DsPhiPi_FS',
        #test -NB this does not match BPhysicsSlice.signatures but maybe we should standardise 
            'MU6_DsPhiPi', 'MU4_DsPhiPi', 'mu6_DsPhiPi', 'mu4_DsPhiPi',
            'MU6_DsPhiPi_FS', 'MU4_DsPhiPi_FS', 
            'mu6_DsPhiPi_FS','mu10_DsPhiPi', 'mu10_DsPhiPi_FS',
            'MU4_Jpsie5e3',
            'MU4_Jpsimumu', 'MU6_Jpsimumu','MU4_Upsimumu','MU6_Upsimumu',
            'MU4_Bmumu','MU6_Bmumu','MU4_DiMu','MU6_DiMu',
            'mu4_Jpsimumu','mu4_Upsimumu','mu4_DiMu',
            'mu6_Jpsimumu','mu6_Upsimumu','mu6_DiMu',
            'mu4_Bmumu','mu6_Bmumu','mu4_DiMu_FS','mu6_DiMu_FS',
            'MU6_Jpsimumu_FS','MU6_Upsimumu_FS',
            'MU6_Bmumu_FS','MU6_DiMu_FS',
            'mu4_Jpsimumu_FS', 'mu6_Jpsimumu_FS','mu4_Upsimumu_FS','mu6_Upsimumu_FS',
            'mu4_Bmumu_FS','mu6_Bmumu_FS',
            'MU6_BmumuX', 'MU4_BmumuX', 'mu6_BmumuX', 'mu4_BmumuX' ,
            '2mu6_Bmumu','2mu4_Bmumux','2mu6_Bmumux','2mu6_DiMu',
            '2mu6_Jpsimumu',
            '2mu6_Upsimumu',
            '2mu4_Upsimumu'
            #commissioning
            'MU4_DsPhiPi_SiTrk', 'MU4_Jpsie5e3_SiTrk', 'MU4_DiMu_SiTrk',
            '2mu4_DiMu_SiTrk',
            '2mu4_Bmumu','2mu4_DiMu','2mu4_Jpsimumu','2mu4_upsimumu',
             # more
            'mu4_DiMu_MG','mu4_DiMu_MG_FS','mu4_DiMu_SiTrk','mu4_DiMu_SiTrk_FS',
            'mu6_Jpsie5e3','mu6_Jpsie5e3_FS','mu6_Jpsie5e3_SiTrk','mu6_Jpsie5e3_SiTrk_FS',
            'mu6_Trk_Jpsi',
            'mu4_Trk_Jpsi',
            'mu4_Trk_Jpsi_FS',
            'mu4_Trk_Upsi_FS',
            '2mu4_DiMu_noVtx',
            '2mu4_Upsimumu',
            'MU4_DsPhiPi_SiTrk',
            'MU4_DiMu_FS_noOS',
            'MU4_DiMu_FS_noOS_passL2',
            'MU4_DiMu_noOS',
            'MU4_DiMu_noOS_passL2',
            'MU4_Trk_Jpsi_loose',
            'MU4_Trk_Jpsi_loose_FS',
            'MU4_Trk_Jpsi_loose_FS_passL2',
            'MU4_Trk_Jpsi_loose_passL2',
            'MU4_Trk_Upsi_loose_FS',
            'MU4_Trk_Upsi_loose_FS_passL2',
            'MU6_Jpsie5e3',
            'MU6_Jpsie5e3_FS',
            'MU6_Jpsie5e3_SiTrk_FS',
            'MU6_Trk_Jpsi_loose',
            'MU6_Trk_Jpsi_loose_FS',
            'MU6_Trk_Upsi_loose_FS',
            'mu6_Trk_Jpsi_loose',
            'mu6_Trk_Jpsi_loose_FS',
            'mu6_Trk_Upsi_loose_FS',
            'mu4_MSonly_Trk_Jpsi_loose',
            'mu4_MSonly_Trk_Upsi_loose_FS',
            'mu4mu6_DiMu',
            'mu6_DiMu_FS_noOS',
            'mu4_Trk_Jpsi_loose',
            'mu4_Trk_Upsi_loose_FS',
            '2mu4_DiMu_noOS',
            '2mu4_DiMu_noOS_passL2',
            '2mu4_DiMu_noVtx_noOS',
            '2mu4_DiMu_noVtx_noOS_passL2',
            '2mu6_DiMu_noVtx_noOS',
            'mu4_DiMu_noOS',
            'mu4mu6_Bmumu',
            'mu4mu6_Jpsimumu',
            'mu4mu6_Upsimumu',
            'MU4_DiMu_FS_noRoI',
            'MU4_DiMu_noRoI',
            'mu4mu6_BmumuX',
            'mu4mu6_Jpsimumu_IDTrkNoCut',


            ]

#minbias#
minbias  =[

    'PreS_loose',
    'PreS_tight',
    'mbSpTrkVtxMh',
    'mbTE20_eff',
    'mbMbts_1_1_unpaired_iso_NoAlg',
    'mbMbts_4_4_unpaired_iso_NoAlg',
    'mbMbts_2_SpTrkMh120',
    'ZDC','ZDC_A','ZDC_AND','ZDC_AND_EMPTY','ZDC_A_EMPTY',
           'ZDC_C','ZDC_AND','ZDC_C_EMPTY','ZDC_EMPTY',  
           'LUCID',
           'MBTS_1',
           'MBTS_1_1',
           'MBTS_2',
           'MbRndm','MbSpTrk','Mbts_2','Lucid_Mbts_2','Zdc1','MbTrt',
           'MbSp', 'MbLUCID_Sp', 'LUCID_Mbts_2','mbZdc',
           'mbRndm','mbSpTrk','mbSp_fwd','mbSp_oneD','mbTrtTrk',
           'mbLucid','mbMbts_1','mbMbts_1_1','mbMbts_1_1_NoCut_Time','mbMbts_1_NoCut_Time',
           'mbMbts_2','mbMbts_2_NoCut_Time',
           'mbBcm','mbID_BM_calib','mbID_LD_calib','mbID_MS_calib','mbID_ZC_calib','mbID_calib',
           'mbRmMbts_1',
           'mbRmMbts_2',
           'mbSp',
           'mbSpLowPtTrk',
           'mb_BM_calib',
           'mb_LD_calib',
           'mb_M2_calib',
           'mb_MS_calib',
           'mb_RM_calib',
           'mb_ZC_calib',
           'allL1_noHLT',
           'BCM_Wide',
           'RD0_FILLED',
           'allL1_noHLT',
           'mbMbts_1_NoCut_Time_unpaired',
           'mbRmMbts_1_1',
           'mbRmMbts_1_1_NoCut_Time',
           'mbRmMbts_1_NoCut_Time',
           'mbRmMbts_1_NoCut_Time_unpaired',
           'mbRmMbts_1_unpaired',
           'mbRmMbts_2_NoCut_Time',
           'mbSpTrk_cosmic',
           'mbSpTrk_unpaired',
           'mbTrtTrk_unpaired',
           'mb_M1_calib',
           'mb_M1_calib_unpaired',
           'mb_M2_calib_unpaired',
           'mb_MU',
           'mb_RM_calib_cosmic',
           'mb_RM_calib_unpaired',
           'mb_allL1_calib',
           'mbBcm_AtoC',
           'mbBcm_AtoC_cosmic',
           'mbBcm_ht',
           'mbBcm_ht_cosmic',
           'mbBcm_mul_1',
           'mbBcm_mul_1_cosmic',
           'mbBcm_mul_2',
           'mbBcm_mul_2_cosmic',
           'mbBcm_mul_3',
           'mbBcm_mul_3_cosmic',
           'mbBcm_mul_4',
           'mbBcm_mul_4_cosmic',
           'mbBcm_mul_5',
           'mbBcm_mul_5_cosmic',
           'mbBcm_mul_6',
           'mbBcm_mul_6_cosmic',
           'mbLucid_a',
           'mbLucid_a_c',
           'mbLucid_a_c_cosmic',
           'mbLucid_a_c_unpaired',
           'mbLucid_a_cosmic',
           'mbLucid_c',
           'mbLucid_c_cosmic',
           'mbLucid_comm',
           'mbLucid_comm_cosmic',
           'mbMbts_1_1_firstempty',
           'mbMbts_1_1_unpaired',
           'mbMbts_1_1_unpaired1',
           'mbMbts_1_1_unpaired2',
           'mbMbts_1_empty',
           'mbMbts_1_firstempty',
           'mbMbts_1_unpaired1',
           'mbMbts_1_unpaired2',
           'mbMbts_2_empty',
           'mbMbts_2_firstempty',
           'mbMbts_2_unpaired1',
           'mbMbts_2_unpaired2',
           'mbMbts_4_4_unpaired1',
           'mbMbts_4_4_unpaired2',
           'mbMbts_a',
           'mbMbts_c',
           'mbSpTrkMh_MB1',
           'mbSpTrkMh_MB2',
           'mbZdc_a',
           'mbZdc_a_c',
           'mbZdc_a_c_cosmic',
           'mbZdc_a_cosmic',
           'mbZdc_and',
           'mbZdc_and_cosmic',
           'mbZdc_c',
           'mbZdc_c_cosmic',
           'mb_BM_eff',
           'mb_LD_A_eff_unpaired',
           'mb_LD_C_eff_unpaired',
           'mb_LD_eff',
           'mb_M1_eff',
           'mb_M1_eff_cosmic',
           'mb_M1_eff_unpaired',
           'mb_M2_eff',
           'mb_M2_eff_unpaired',
           'mb_M4_4_eff',
           'mb_M4_4_eff_firstempty',
           'mb_M4_4_eff_unpaired',
           'mb_RM_eff',
           'mb_RM_eff_cosmic',
           'mb_RM_eff_unpaired',
           'mb_ZC_eff',
           'L1ItemStreamer_L1_L1A_Mon',
           'L1ItemStreamer_L1_LUCID',
           'L1ItemStreamer_L1_LUCID_A',
           'L1ItemStreamer_L1_LUCID_A_C',
           'L1ItemStreamer_L1_LUCID_A_C_EMPTY',
           'L1ItemStreamer_L1_LUCID_A_C_UNPAIRED',
           'L1ItemStreamer_L1_LUCID_A_EMPTY',
           'L1ItemStreamer_L1_LUCID_A_UNPAIRED',
           'L1ItemStreamer_L1_LUCID_C',
           'L1ItemStreamer_L1_LUCID_COMM',
           'L1ItemStreamer_L1_LUCID_COMM_EMPTY',
           'L1ItemStreamer_L1_LUCID_C_EMPTY',
           'L1ItemStreamer_L1_LUCID_C_UNPAIRED',
           'L1ItemStreamer_L1_LUCID_EMPTY',
           'L1ItemStreamer_L1_MBTSA0',
           'L1ItemStreamer_L1_MBTSA1',
           'L1ItemStreamer_L1_MBTSA10',
           'L1ItemStreamer_L1_MBTSA11',
           'L1ItemStreamer_L1_MBTSA12',
           'L1ItemStreamer_L1_MBTSA13',
           'L1ItemStreamer_L1_MBTSA14',
           'L1ItemStreamer_L1_MBTSA15',
           'L1ItemStreamer_L1_MBTSA2',
           'L1ItemStreamer_L1_MBTSA3',
           'L1ItemStreamer_L1_MBTSA4',
           'L1ItemStreamer_L1_MBTSA5',
           'L1ItemStreamer_L1_MBTSA6',
           'L1ItemStreamer_L1_MBTSA7',
           'L1ItemStreamer_L1_MBTSA8',
           'L1ItemStreamer_L1_MBTSA9',
           'L1ItemStreamer_L1_MBTSC0',
           'L1ItemStreamer_L1_MBTSC1',
           'L1ItemStreamer_L1_MBTSC10',
           'L1ItemStreamer_L1_MBTSC11',
           'L1ItemStreamer_L1_MBTSC12',
           'L1ItemStreamer_L1_MBTSC13',
           'L1ItemStreamer_L1_MBTSC14',
           'L1ItemStreamer_L1_MBTSC15',
           'L1ItemStreamer_L1_MBTSC2',
           'L1ItemStreamer_L1_MBTSC3',
           'L1ItemStreamer_L1_MBTSC4',
           'L1ItemStreamer_L1_MBTSC5',
           'L1ItemStreamer_L1_MBTSC6',
           'L1ItemStreamer_L1_MBTSC7',
           'L1ItemStreamer_L1_MBTSC8',
           'L1ItemStreamer_L1_MBTSC9',
           'L1ItemStreamer_L1_MBTS_1',
           'L1ItemStreamer_L1_MBTS_1_1',
           'L1ItemStreamer_L1_MBTS_1_1_Col',
           'L1ItemStreamer_L1_MBTS_1_1_EMPTY',
           'L1ItemStreamer_L1_MBTS_1_1_FIRSTEMPTY',
           'L1ItemStreamer_L1_MBTS_1_1_UNPAIRED',
           'L1ItemStreamer_L1_MBTS_1_1_UNPAIRED1',
           'L1ItemStreamer_L1_MBTS_1_1_UNPAIRED2',
           'L1ItemStreamer_L1_MBTS_1_Col',
           'L1ItemStreamer_L1_MBTS_1_EMPTY',
           'L1ItemStreamer_L1_MBTS_1_FIRSTEMPTY',
           'L1ItemStreamer_L1_MBTS_1_UNPAIRED',
           'L1ItemStreamer_L1_MBTS_1_UNPAIRED1',
           'L1ItemStreamer_L1_MBTS_1_UNPAIRED2',
           'L1ItemStreamer_L1_MBTS_2',
           'L1ItemStreamer_L1_MBTS_2_Col',
           'L1ItemStreamer_L1_MBTS_2_EMPTY',
           'L1ItemStreamer_L1_MBTS_2_FIRSTEMPTY',
           'L1ItemStreamer_L1_MBTS_2_UNPAIRED',
           'L1ItemStreamer_L1_MBTS_2_UNPAIRED1',
           'L1ItemStreamer_L1_MBTS_2_UNPAIRED2',
           'L1ItemStreamer_L1_MBTS_4_4',
           'L1ItemStreamer_L1_MBTS_4_4_FIRSTEMPTY',
           'L1ItemStreamer_L1_MBTS_4_4_UNPAIRED',
           'L1ItemStreamer_L1_MBTS_4_4_UNPAIRED1',
           'L1ItemStreamer_L1_MBTS_4_4_UNPAIRED2',
           'L1ItemStreamer_L1_MBTS_A',
           'L1ItemStreamer_L1_MBTS_C',
           'L1ItemStreamer_L1_ZB_4MBTS_A',
           'L1ItemStreamer_L1_ZDC',
           'L1ItemStreamer_L1_ZDC_A',
           'L1ItemStreamer_L1_ZDC_AND',
           'L1ItemStreamer_L1_ZDC_AND_EMPTY',
           'L1ItemStreamer_L1_ZDC_A_C',
           'L1ItemStreamer_L1_ZDC_A_C_EMPTY',
           'L1ItemStreamer_L1_ZDC_A_EMPTY',
           'L1ItemStreamer_L1_ZDC_C',
           'L1ItemStreamer_L1_ZDC_C_EMPTY',
           'L1ItemStreamer_L1_ZDC_EMPTY',
           'L1ItemStreamer_L1_ZDC_UNPAIRED',
           'SeededStreamerBCM',
           'SeededStreamerBCM_LUCID_ZDC',
           'SeededStreamerLUCID',
           'SeededStreamerMBTS',
           'SeededStreamerMBTS_ZDC',
           'SeededStreamerMinBias',
           'LUCID_A',
           'LUCID_A_C',
           'LUCID_A_C_EMPTY',
           'LUCID_A_C_UNPAIRED',
           'LUCID_A_EMPTY',
           'LUCID_A_UNPAIRED',
           'LUCID_C',
           'LUCID_COMM',
           'LUCID_COMM_EMPTY',
           'LUCID_C_EMPTY',
           'LUCID_C_UNPAIRED',
           'LUCID_EMPTY',
           'MBTSA0',
           'MBTSA1',
           'MBTSA10',
           'MBTSA11',
           'MBTSA12',
           'MBTSA13',
           'MBTSA14',
           'MBTSA15',
           'MBTSA2',
           'MBTSA3',
           'MBTSA4',
           'MBTSA5',
           'MBTSA6',
           'MBTSA7',
           'MBTSA8',
           'MBTSA9',
           'MBTSC0',
           'MBTSC1',
           'MBTSC10',
           'MBTSC11',
           'MBTSC12',
           'MBTSC13',
           'MBTSC14',
           'MBTSC15',
           'MBTSC2',
           'MBTSC3',
           'MBTSC4',
           'MBTSC5',
           'MBTSC6',
           'MBTSC7',
           'MBTSC8',
           'MBTSC9',
           'MBTS_1_1_Col',
           'MBTS_1_1_EMPTY',
           'MBTS_1_1_FIRSTEMPTY',
           'MBTS_1_1_UNPAIRED',
           'MBTS_1_1_UNPAIRED1',
           'MBTS_1_1_UNPAIRED2',
           'MBTS_1_Col',
           'MBTS_1_EMPTY',
           'MBTS_1_FIRSTEMPTY',
           'MBTS_1_UNPAIRED',
           'MBTS_1_UNPAIRED1',
           'MBTS_1_UNPAIRED2',
           'MBTS_2_Col',
           'MBTS_2_EMPTY',
           'MBTS_2_FIRSTEMPTY',
           'MBTS_2_UNPAIRED',
           'MBTS_2_UNPAIRED1',
           'MBTS_2_UNPAIRED2',
           'MBTS_4_4',
           'MBTS_4_4_FIRSTEMPTY',
           'MBTS_4_4_UNPAIRED',
           'MBTS_4_4_UNPAIRED1',
           'MBTS_4_4_UNPAIRED2',
           'MBTS_A',
           'MBTS_C',
           'ZB_4MBTS_A',
           'ZDC_A_C',
           'ZDC_A_C_EMPTY',
           'ZDC_UNPAIRED',
           'mbSpBg_unpaired',
           'mb_ZC_eff_unpaired',
           'L1ItemStreamer_L1_ZB',
           'mb_BCM_AC_CA_eff',
           'mb_BCM_HT_eff',
           'mb_BCM_Wide_eff',
           'L1ItemStreamer_L1_RD1_EMPTY',
           'L1MinBias_NoAlg',
           'l2j25_jetNoCut_MV_MbSpTrk',
           'l2j30_jetNoCut_MV_MbSpTrk',
           'mbLucidComm_cosmic_NoAlg',
           'mbLucid_A_eff_unpaired',
           'mbLucid_C_eff_unpaired',
           'mbLucid_eff',
           'mbMbts_1_eff',
           'mbMbts_1_eff_unpaired_iso',
           'mbMbts_1_empty_NoAlg',
           'mbMbts_2_SpTrkMh',
           'mbRd0_eff_unpaired_iso',
           'mbRd1_eff',
           'mbSpBg_unpaired_iso',
           'mbSpTrk_unpaired_iso',
           'mbZdc_eff',
           'mbZdc_eff_unpaired',
           'rd0_empty_NoAlg',
           'rd0_filled_NoAlg',
           'rd1_empty_NoAlg',
           'zerobias_NoAlg',

           'LUCID_A_C_UNPAIRED_ISO',
           'LUCID_A_C_UNPAIRED_NONISO',
           'MBTS_1_1_UNPAIRED_ISO',
           'MBTS_1_UNPAIRED_ISO',
           'MBTS_2_UNPAIRED_ISO',
           'MBTS_2_UNPAIRED_NONISO',
           'MBTS_4_4_UNPAIRED_ISO',

           'ZB',
           'RD0_UNPAIRED_ISO',
           'RD1_EMPTY',

           'BCM_AC_CA_BGRP0',
           'BCM_HT_BGRP0',
           'BCM_Wide_BGRP0',
           'mbMbts_2_Monitoring',
           'Standby',
 ]

#
# keep track of evolution
#
dict={}
dictold={'tau15'    : 'tau12_loose' ,
         'tau20i'  : 'tau16i_loose', 
         'tau25i'  : 'tau20i_loose',          
         'tau35i'  : 'tau29i_loose',         
         'tau45'   : 'tau38_loose',
         'tau45i'  : 'tau38i_loose',  
         'tau60'   : 'tau50_loose',          
         'tau100'  : 'tau84_loose',           
         'tau150'  : 'tau125_loose',
         'tau12_medium'    : 'tau12_loose' ,
         'tau16i_medium'  : 'tau16i_loose', 
         'tau20i_medium'  : 'tau20i_loose',          
         'tau29i_medium'  : 'tau29i_loose',         
         'tau38_medium'   : 'tau38_loose',
         'tau38i_medium'  : 'tau38i_loose',  
         'tau60i_medium'   : 'tau50_loose',          
         'tau100_medium'  : 'tau84_loose',           
         'tau150_medium'  : 'tau125_loose',
         'tau15i_tau45' : 'tau10i_tau38',
         'tau15i_tau35i' : 'tau12i_tau29i',
         'tau15i_tau45'  : 'tau12i_tau38',
         'e5'     : 'e5_medium',
         'e7'     : 'e7_medium',
         'e10'    : 'e10_medium',
         'e12'    : 'e12_medium',
         'e15'    : 'e15_medium',
         'e15'    : 'e15i_medium',
         'e25i'    : 'e25i_medium1',
         'e5_e7'   : 'e5_e7_medium',
         'e5_e10'  : 'e5_e10_medium',
         '2e15' : '2e15_medium '   }



class trigtable:

    def __init__(self):

        #self.level = glevel
        self.matrix  = []
        self.prflag  = []

    def addline(self,line):
        self.matrix.append(line)
        self.prflag.append(True)

    def sort(self):
        self.matrix.sort()

    def prmat(self):
        for n,l in enumerate(self.matrix):
            if self.prflag[n]:
                prline(l)


    def prdiff(self,tlist):
 
        ref  = -1
        test = []
        for i,x in enumerate(tlist):

            if x.cflag == 'ref':
                if ref == -1:
                    ref =  i
                else: 
                    print 'warning, two references'
                    stop  # stop if more than one reference
            if x.cflag == 'test':
                test.append(i)

        diffMatrix = []
        undefinedMatrix = []

        for n,l in enumerate(self.matrix):
            out_of_tolerance = False
            undefined        = False
            ref_element = l[ref+1]
            ratmax = 0
            for t in test:
                t_element = l[t+1]
                #print l[0],ref_element[0], ref_element[1], t_element[0],t_element[1]
                if (( t_element[0] > 0)  or (ref_element[0] > 0 )) and (ref_element[0] != 'E'):
                    #print t_element[0]
                    #print ref_element[0]
                    rat =  abs( t_element[0]/(ref_element[0]+0.01) - 1.0)
                    if rat > ratmax:
                       ratmax = rat
                    diff  = (ref_element[0] - t_element[0] ) 
                    errsq = (ref_element[1]*ref_element[1] + t_element[1]*t_element[1])
                    if errsq > 0:
                        chi   =  diff*diff/errsq
                    else:
                        chi = diff*diff
                    if chi > 2.0*2.0 :
                        out_of_tolerance = True
                    # print l[0],ref_element[0], ref_element[1], t_element[0],t_element[1]
                else:
                    undefined = True

            if out_of_tolerance:
                diffMatrix += [ [ratmax, chi, l] ]

            if undefined:
                undefinedMatrix += [l]
#                prline(l)

        diffMatrix.sort()
        diffMatrix.reverse()

        print 
        print "The following triggers are more than 2 sigma different"
        print 

        for  x in diffMatrix:
            prline(x[2],x[0]) 

        print 
        print "The following triggers were not defined in all releases tested or have zero triggers passing"
        print 
        for l in undefinedMatrix:
            prline(l)


    def prslice(self,category):
        prlist = []
        for j in category:
            for n,l in enumerate(self.matrix):
                trig = l[0]
                if trig[3:] == j:
                    self.prflag[n] = False 
                    prlist.append(l)
                    #prline(l)
        prlist.sort()
        for l in prlist:
            prline(l) 

    def listUnassigned(self):
        for n,l in enumerate(self.matrix):
            if self.prflag[n]:       
              print "'%s'," % l[0][3:]

class triglist:
    def __init__(self, cflag, dir, tag, pu=0, sn="16-00-00", model="ATLAS-GEO", cms=7, total=0):
        global glevel
        global grates
        global gdir

        self.cflag = cflag     # flag for comparison
        self.dir    = dir      # director
        self.tag = tag      #  tag  
        self.pu    = pu        # pile up 
        self.model = model     # geometry model
        self.sn    = sn        # serial number
        self.cms   = cms       # cms 
        self.total = total     # total events
 
     

        #print "glevel in triglist init",glevel
        if self.tag == "":
            dirName = self.dir+'/'
        else:
            dirName = self.dir+'/'+self.tag+'/'
  

        if gdir != "":
           dirName = gdir+dirName 
           print "final directoroy:",dirName

        self.f      = open(dirName+glevel+'.txt','r') 

        try:
            self.g      = open(dirName+'/totalEvents.txt','r')
            totalLine  = self.g.read().split()
            totalFile = eval(totalLine[len(totalLine)-1])
            print "totalFile:  ",totalFile
        except:
            print "WARNING:   could not read total events"
            totalFile  = 1

        if self.total == 0:
            self.total = totalFile
        else:
            if self.total != totalFile:
                print "WARNING:  event count mismatch:", self.total, totalFile

    def printit(self):
        print  'Direcotry: ', self.dir

    def save(self,num):
        global grates
        self.num = num
        self.frac  = float(num)/float(self.total)
        if grates:
            #   0.048 1e24 * 1e33 /1e3  =
            if self.pu != 0: 
                self.error = (48000/(self.pu))*sqrt(self.frac*(1-self.frac)/float(self.total))
                self.frac  = (48000/(self.pu))*self.frac
            else:
                self.error = (48000)*sqrt(self.frac*(1-self.frac)/float(self.total))
                self.frac  = (48000)*self.frac
        else:
            self.error = 1000*sqrt(self.frac*(1-self.frac)/float(self.total))
            self.frac  = 1000*self.frac

    def match(self,refkey):
       global glevel
       global grates

       self.frac  = -1.0
       self.error = -1.0
       self.f.seek(0)
       for newline in self.f:
          if newline.find(glevel) > -1:
             newkey, newcount = newline.split()
             if refkey == newkey:
                self.num = newcount
                self.frac = float(newcount)/float(self.total)
                # for pileup in MinBias events are generated with pileup n-1...
                if grates:
                    if self.pu != 0: 
                        self.error = (48000/(self.pu))*sqrt(self.frac*(1-self.frac)/float(self.total))
                        self.frac  = (48000/(self.pu))*self.frac
                    else:
                        self.error = (48000)*sqrt(self.frac*(1-self.frac)/float(self.total))
                        self.frac  = (48000)*self.frac
                else:
                    self.error = 1000*sqrt(self.frac*(1-self.frac)/float(self.total))
                    self.frac  = 1000*self.frac
             elif dict.has_key(newkey) and dict[newkey] == refkey:
                self.num = newcount
                self.frac = float(newcount)/float(self.total)
                if grates:
                   if self.pu != 0: 
                        self.error = (48000/(self.pu))*sqrt(self.frac*(1-self.frac)/float(self.total))
                        self.frac  = (48000/(self.pu))*self.frac
                   else:
                        self.error = (48000)*sqrt(self.frac*(1-self.frac)/float(self.total))
                        self.frac  = (48000)*self.frac
                else:
                    self.error = 1000*sqrt(self.frac*(1-self.frac)/float(self.total))
                    self.frac  = 1000*self.frac
             elif dict.has_key(refkey) and dict[ refkey ] == newkey:
                self.num = newcount
                self.frac = float(newcount)/float(self.total)
                if grates:
                   if self.pu != 0: 
                        self.error = (48000/(self.pu))*sqrt(self.frac*(1-self.frac)/float(self.total))
                        self.frac  = (48000/(self.pu))*self.frac
                   else:
                        self.error = (48000)*sqrt(self.frac*(1-self.frac)/float(self.total))
                        self.frac  = (48000)*self.frac
                else:
                    self.error = 1000*sqrt(self.frac*(1-self.frac)/float(self.total))
                    self.frac  = 1000*self.frac

def prtitle(tlists):

    print 32*'*',
    for x in tlists:
        print 16*'*',
    print
    print '# CMS   '.ljust(32),
    for x in tlists:
        print "%16d" % x.cms,
    print
    print '# Pileup'.ljust(32), 
    for x in tlists:
        print "         n=%5.2f" % x.pu,
    print
    print '# Prod'.ljust(32), 
    for x in tlists:
        print x.tag.rjust(16),
    print
    print '# Series1'.ljust(32), 
    for x in tlists:
        print x.model.rjust(16),
    print
    print '# Series2'.ljust(32),
    for x in tlists:
        print x.sn.rjust(16),
    print
    print '## Release'.ljust(32),
    for x in tlists:
        print x.dir.rjust(16),
    print
    print 32*'*',
    for x in tlists:
        print 16*'*',
    print

def prline(l,rdiff=0.0):
    trig = l[0]
    for i,m in enumerate(l):
        if i == 0:
            type = "" 
            for pt in  TriggerGroups["Primary"]:
              if pt == trig[3:]:
                 type = "P"
            for pt in  TriggerGroups["Supporting"]:
              if pt == trig[3:]:
                 type = "S"
            for pt in  TriggerGroups["Backup"]:
              if pt == trig[3:]:
                 type = "B"
            for pt in  TriggerGroups["Calibration"]:
              if pt == trig[3:]:
                 type = "B"
            if type != "":
              print m.ljust(30),type,
            else:
              print m.ljust(32),
        else: 
            if m[0] != -1: 
                print ' %15.1f' % m[0],  
            else:
                print '                ',
    if rdiff == 0.0:
       print 
    elif rdiff > 1000.0:
       print  '     -' 
    else:
       print '   %3.2f' % rdiff


def main():

    global glevel
    global grates
    global gdir

    # parse input command

    from optparse import OptionParser
    parser = OptionParser(usage = "usage: %prog [options]",
                          description = "Makes a table of releases and shows trigger counts\
                                         Releases and tags are selected by triglist.  This can \
                                         be entered on the command line or in the code.")

    parser.add_option("-l","--level",
	              action = "store", default ="L1", type="string",
	              help = "select level for trigger counts (L1, L2, EF)")

    parser.add_option("-t","--tlist",
	              action = "store", default =[], type="string",
	              help = 'list of triggers to compare:                             \
[triglist( "test","16.0.2.6", "e603_s932_r1787", 2.2),\
triglist( "ref" ,"16.0.2.6", "e603_s932_r1812", 2.2 )  ]')

    parser.add_option("-r","--rate",
                      action = "store_true", default = False,
	              help = "gives rate in kHz assuming L = 1e33")

    parser.add_option("-d","--dir",
	              action = "store", default ="", type="string",
	              help = "base directory for samples")


    (opts, args) = parser.parse_args()

    params = {'level'  : opts.level}
    params = {'tlist'  : opts.tlist}
    params = {'rate'   : opts.rate}
    params = {'dir'    : opts.dir}

    glevel = opts.level
    print "comparing level:",glevel

    grates = opts.rate

    if grates:
       print "output is in kHz at L = 1e33 assuming a cross-section of 0.0484 barns" 

    gdir = opts.dir
    if gdir != "":
        print"setting base directory to:",gdir

    doAll = True
#
#  infor on tags can be found here:
#
# https://twiki.cern.ch/twiki/bin/view/Atlas/MCTriggerDB#15_6_3_6_15_6_3_9_References
# https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasGeomDBTags
# http://www-f9.ijs.si/atlpy/atlprod/prodtag/?q=r520
#
    if  opts.tlist != [] :
        print "executing:",opts.tlist
        exec "tlists = "+opts.tlist
    else :
        tlists = [

            # examples for top samples 

            #scout including RPC timing
            #triglist( "test","16.0.2.6", "e603_s932_r1823",2.2),


            #fast MC 
            triglist( "test","16.0.2.3", "e603_a119",0,"16-00-00","ATLAS-GEO",7),
            # muon fix
            #triglist( "test","16.0.3.1", "e603_s932_r1933",0,"16-00-00","ATLAS-GEO",7 ),

            #Double Trains with 9 BCseparation, 150 ns bunch spacing within train, <n_MB> = 2.2, 150 ns cavern background, 
            #triglist( "test","16.0.2.6", "e603_s932_r1787",2.2,"16-00-00","ATLAS-GEO",7,0),
            # 50 ns constant bunch spacing,<n_MB> = 2.2, 150 ns cavern background
            #triglist( "ref","16.0.2.6", "e603_s932_r1812",2.2,"16-00-00","ATLAS-GEO",7,0)

            #150ns bunch spacing 25ns cavern background
            #triglist( "ref","16.0.2.4", "e603_s932_r1764",2.2,"16-00-00","ATLAS-GEO",7,0),
            # reference, no pileup
            triglist( "ref","16.0.2.3", "e603_s932_r1649",0,"16-00-00","ATLAS-GEO",7,10000 ),

            # minbias examples

            #triglist("test", "16.0.2.6mb", "e603_s932_r1824", 2.2,"16-00-00","ATLAS-GEO",7),
            #triglist("test", "16.0.2.6mb", "e603_s932_r1788", 2.2,"16-00-00","ATLAS-GEO",7),
            #triglist("ref", "16.0.2.6mb", "e603_s932_r1804", 2.2,"16-00-00","ATLAS-GEO",7),

            #both pileup and minbias are from same pythia production -- but different values of e603 for signal and e577 for pileup
            # xsec is 4.8445E+07nb   so maximal rate is  4.8445E+07*1.0e-9*1e-24*1e32 = 4.8e6 script gives rate per 1000, so we need
            # need to divide by 4844 to get rate at 1e32, 2*4844 = 9688 for 2e32       
            # 9 BC separation, 150 ns bunch spacing within train, <n_MB> = 2.2, no cavern
            #triglist("test", "16.0.2.4mb", "e603_s932_r1768", 2.2,"16-00-00","ATLAS-GEO",7,100000),
            # reference 
            #triglist("ref" , "16.0.1.6mb", "e603_s932_r1621",  0,"16-00-00","ATLAS-GEO",7),

            # nightly example
            #triglist("test", "devval", "9dec10", 0,"16-00-00","ATLAS-GEO",7,1000 ),
            #triglist("test", "16.1.X-VAL", "9dec10", 0,"16-00-00","ATLAS-GEO",7,1000 ),
            #triglist("ref", "16.0.X.Y-VAL", "9dec10",  0,"16-00-00","ATLAS-GEO",7, 1000),

             ]



#   
#   now read the files and produe the matrix
#   
    tt = trigtable()
    tt.matrix = []
# 
    for i,x in enumerate(tlists):
        if  i == 0 or doAll :
            x.f.seek(0)
            for refline in x.f:
                if refline.find(glevel) > -1:
                    refkey,refcount = refline.split()
                    x.save(refcount)
                    saveit = True
                    for j,y in enumerate(tlists): 
                        #  check if we already saved this line
                        if j != i:  
                            y.match(refkey)
                            if j < i:
                                if y.frac >= 0:
                                    saveit = False
                    if saveit:
                        line  = [refkey]
                        for z in tlists: 
                            line.append([z.frac,z.error])
                        tt.addline(line)
    tt.sort()

    prtitle(tlists)

#    tt.prdiff(tlists)

    print 
    print "Electron/photon slice" 
    print 
    tt.prslice(electron)
#

    print 
    print "muon slice"
    print 
    tt.prslice(muon)


    print 
    print "jet slice"
    print 
    tt.prslice(jet)

    print 
    print "bjet slice"
    print 
    tt.prslice(bjet)

    print 
    print "missing et slice"
    print 
    tt.prslice(xe)


    print 
    print "tau slice"
    print 
    tt.prslice(tau)

    print 
    print "mixed slices"
    print 
    tt.prslice(mixed)

    print 
    print  "Bphyiscs slice"
    print 
    tt.prslice(bphysics)

    print 
    print "min bias slice"
    print 
    tt.prslice(minbias)

    print 
    print "two photon"
    print 
    tt.prslice(twophoton)

    print 
    print "exotic"
    print 
    tt.prslice(exotic)

    print 
    print "calibration"
    print 
    tt.prslice(calibration)

    print 
    print "beamspot"
    print 
    tt.prslice(beamspot)


    print 
    print "unclassified triggers"
    print 
    tt.prmat()

    tt.listUnassigned()

    prtitle(tlists)
    tt.prdiff(tlists)
  
main()


