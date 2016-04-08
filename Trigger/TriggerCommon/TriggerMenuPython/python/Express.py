# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Express stream addition  """

__author__  = 'B.Demirkoz'
__version__="$Revision: 1.19 $"
__doc__="Addition of express stream tags and prescales to the list of chains"

from TriggerJobOpts.TriggerFlags  import TriggerFlags
from AthenaCommon.Logging import logging

log = logging.getLogger('TriggerMenuPython.Express.py')

def addExpressStream(triggerPythonConfig):
    Names = {}
    menu_name = TriggerFlags.triggerMenuSetup()
    if menu_name.startswith('lumi1E31') or menu_name.startswith('MC_lumi1E31') or menu_name.startswith('Physics_lumi1E31'):
        Names = ExpressNames1E31
    elif menu_name.startswith('lumi1E32') or menu_name.startswith('MC_lumi1E32') or menu_name.startswith('Physics_lumi1E32'):
        Names = ExpressNames1E32
    elif menu_name.startswith('Physics_pp_v1') or menu_name.startswith('MC_pp_v1') or menu_name.startswith('Test_pp_v1'):
        Names = ExpressNamesPPV1
    elif menu_name.startswith('Physics_pp_v2') or menu_name.startswith('MC_pp_v2'):
        Names = ExpressNamesPPV2
    elif menu_name.startswith('L1_alfa_v2'):
        Names = ExpressNamesAlfaV2
    elif menu_name.startswith('L1_pp_v3') or menu_name.startswith('Physics_pp_v3') or menu_name.startswith('MC_pp_v3'):
        Names = ExpressNamesPPV3
    elif menu_name.startswith('L1_pp_v4') or menu_name.startswith('Physics_pp_v4') or menu_name.startswith('MC_pp_v4'):
        Names = ExpressNamesPPV4
    elif menu_name.startswith('Cosmic'):
        #assignCosmicExpressStream(triggerPythonConfig,'LowLumi')
        Names = ExpressNamesCosmic
    elif menu_name.startswith('InitialBeam_v1') \
             or menu_name.startswith('InitialBeam_v2')\
             or menu_name.startswith('MC_InitialBeam_v1')\
             or menu_name.startswith('MC_InitialBeam_v2'):
        Names = ExpressNamesInitialBeam
    elif menu_name.startswith('InitialBeam_v3') \
             or menu_name.startswith('MC_InitialBeam_v3'):
        Names = ExpressNamesInitialBeamV3
    elif menu_name.startswith('InitialBeam_HI_v1') \
             or menu_name.startswith('MC_InitialBeam_HI_v1'):
        Names = ExpressNamesInitialBeamHIV1
        RareNames = RareNamesInitialBeamHIV1
    elif menu_name.startswith('Physics_HI_v1') \
             or menu_name.startswith('MC_HI_v1'):
        Names = ExpressNamesHIV1
        RareNames = RareNamesHIV1
    elif menu_name.startswith('Physics_HI_v2') \
             or menu_name.startswith('MC_HI_v2'):
        Names = ExpressNamesHIV2
        RareNames = RareNamesHIV2


        #assign only the rare stream for HI
        for chainName, chainPrescale in RareNames.iteritems():
            if chainName not in triggerPythonConfig.allChains.keys():
                log.warning('Rare stream: Signature %s not registered to TriggerPythonConfig' % chainName)
                continue

            #savannah
            l2chain = triggerPythonConfig.allChains[chainName][0]
            l2chain.addStreamTag('rare', type='physics', obeyLB='yes')
            if  len(triggerPythonConfig.allChains[chainName]) ==2:
                efchain = triggerPythonConfig.allChains[chainName][1]
                efchain.addStreamTag('rare', type='physics', obeyLB='yes', prescale=1)


    for chainName, chainPrescale in Names.iteritems():
        if chainName not in triggerPythonConfig.allChains.keys():
            log.warning('Signature %s not registered to TriggerPythonConfig' % chainName)
            continue

        #savannah
        #l2chain = triggerPythonConfig.allChains[chainName][0]
        #l2chain.addStreamTag('express', type='express', obeyLB='yes')

        #print 'ARGH', chainName,  len(triggerPythonConfig.allChains[chainName])
        #for c in  triggerPythonConfig.allChains[chainName] :
        #    print 'ARGH ', c.chain_name

        if  len(triggerPythonConfig.allChains[chainName]) ==2:
            efchain = triggerPythonConfig.allChains[chainName][1]
            efchain.addStreamTag('express', type='express', obeyLB='yes', prescale=chainPrescale)
        elif len(triggerPythonConfig.allChains[chainName]) == 1: #this is needed for hi jets
            efchain = triggerPythonConfig.allChains[chainName][0]
            efchain.addStreamTag('express', type='express', obeyLB='yes', prescale=chainPrescale)

#this is a first iteration of the cosmic menu. It will be deleted once we converge.
def assignCosmicExpressStream(triggerPythonConfig,name):
    #print 'igb assigning express to Cosmic2009_vX'
    #print menu_name

    if name is 'LowLumi':
        streams=['IDCosmic', 'RPCwBeam', 'TGCwBeam', 'L1Calo', 'L1CaloEM', 'RNDM', ]

        for name, chains in triggerPythonConfig.allChains.iteritems():
            for c in chains:
                if c.isL2():
                    for s in c.stream_tag:
                        #print 'igb L2 stream tag: ', s[0]
                        expressAssign=False
                        if s[0] in streams:
                            expressAssign=True
                        elif s[0] is 'express' and s[1] is 'express':
                            expressAssign=False
                    if expressAssign:
                        c.addStreamTag('express', type='express', obeyLB='no')
                        #print 'igb++ L2 stream tag: ', c.stream_tag

                elif c.isEF():
                    for s in c.stream_tag:
                        #print 'igb EF stream tag: ', s[0]
                        if s[0] in streams:
                            if s[0] is 'RPCwBeam':
                                c.addStreamTag('express', type='express', obeyLB='no', prescale=50)
                            elif s[0] is 'TGCwBeam':
                                c.addStreamTag('express', type='express', obeyLB='no', prescale=25)
                            elif s[0] is 'L1CaloEM':
                                c.addStreamTag('express', type='express', obeyLB='no', prescale=1)
                            elif s[0] is 'RNDM':
                                c.addStreamTag('express', type='express', obeyLB='no', prescale=7)
                            elif s[0] is 'L1Calo':
                                #print 'chain: ', c.chain_name
                                if c.chain_name.find('j50') > -1:
                                    c.addStreamTag('express', type='express', obeyLB='no', prescale=2)
                                else:
                                    c.addStreamTag('express', type='express', obeyLB='no', prescale=5)
                            elif s[0] is 'IDCosmic':
                                #print 'chain: ', c.chain_name
                                if c.chain_name.find('SiTrack')>-1 or c.chain_name.find('IdScan')>-1 or c.chain_name.find('IDSCAN')>-1:
                                    c.addStreamTag('express', type='express', obeyLB='no', prescale=1)
                                else:
                                    c.addStreamTag('express', type='express', obeyLB='no', prescale=25)
                            #print 'igb++ EF stream tag: ', c.stream_tag


#   Item name            | Prescale
#----------------------------------------------------------
ExpressNames1E31= {
    # egamma
    '2e5_medium'         :        4,
    'e10_medium'         :       40,
    'e20_loose'          :       10,
    'e20_medium'         :        1,
    'e25_medium'         :        1,
    'em105_passHLT'      :        1,
    'g20_loose'          :       40,
    # muon
    'mu4_j10'            :        1,
    'mu10'               :       14,
    'mu20'               :        1,
    'mu20_passHLT'       :      100,
    '2mu10'              :        1,
    '2mu20'              :        1,
    'mu4_Jpsimumu'       :        1,
    'mu4_Upsimumu'       :        1,
    'mbSpTrk'            :        3,
    'j23'                :        2,
    'j120'               :       10,
    # New tau items, prescales not final
    'tau12_loose_PT'     :        1,
    'tau16_loose'        :        1,
    'trk9_loose'         :        1,
    'tau50_loose'        :        1,
    # MET and total energy
    'g25_loose_xe30'     :        1,
    'xe20'               :       10,
    'xe35_loose'         :        1,
    'te150_medium'       :       40,
    'te300_loose'        :        1,
    'allL1_NoAlg'        :      100,
    }

ExpressNames1E32= {
    'e12_medium'         :        1,
    'e20_medium'         :        1,
    'e25_medium'         :        1,
    '2e20_loose'         :        1,
    'g25_loose_xe30'     :        1,
    'g25i_loose'         :        4,
    'g55_tight'          :        1,
    'em20_passHLT'       :       10,
    'g150'               :        1,
    'mu4_j10'            :        1,
    'mu10'               :       14,
    'mu20'               :       10,
    'mu20_passHLT'       :     1000,
    '2mu10'              :       10,
    '2mu20'              :        1,
    'mu4_Jpsimumu'       :        1,
    'mu4_Upsimumu'       :        1,
    'mbSpTrk'            :        3,
    'j23'                :        2,
    'j120'               :       10,
    'tau12_loose'        :        5,
    'xe80_medium'        :       10,
    'te350_medium'       :       10
    }

ExpressNamesPPV1= {
    'mu4'                    :   1,
    'mu6_IDTrkNoCut'         :   1,
    '2mu4'                   :   1,
    '2mu10'                  :   1,
    'mu10'                   :   1,
    'mu20_MSonly'            :   1,
    'mu6'                    :   1,
    'mu10_IDTrkNoCut'        :   1,
    '2mu4_Jpsimumu'          :   1,
    '2mu4_Upsimumu'          :   1,
    'mu15'                   :   1,
    'mu30_MSonly'            :   1,
    'g10_loose'              :   1,
    'e15_loose'              :   1,
    'e15_loose_IDTrkNoCut'   :   1,
    '2e3_loose'              :   1,
    '2e5_tight'              :   1,
    'e10_NoCut'              :   1,
    'e20_loose'              :   1,
    'g20_loose'              :   1,
    'e20_loose_IDTrkNoCut'   :   1,
    '2e10_loose'             :   1,
    'e20_loose_xe20_noMu'    :   1,
    '2e10_medium'            :   1,
    'e20_loose_xe30_noMu'    :   1,
    'j20_jetNoEF'            :   1,
    'j30_jetNoEF'            :   1,
    'L1J115_NoAlg'           :   1,
    'b10'                    :   1,
    'b10_IDTrkNoCut'         :   1,
    'mu4_L1J5_matched'       :   1,
    'tauNoCut'               :   1,
    'tau12_IDTrkNoCut'       :   1,
    'tau50_IDTrkNoCut'       :   1,
    'tau12_loose'            :   1,
    'tau50_loose'            :   1,
    'tau16_loose'            :   1,
    'tau84_loose'            :   1,
    'xe20_noMu'              :   1,
    'te250'                  :   1,
    'InDetMon_FS'            :   1,
    'mbRd1_eff'              :   1,
    'mbZdc_eff'              :   1,
    'L2IdScan_FS'            :   1,
    'L2SiTrack_FS'           :   1,
    'EFInsideOut_FS'         :   1,
    'EFTRTOnly_FS'           :   1,
    'rd0_empty_NoAlg'        :   1,
    ### cosmic
    'tauNoCut_cosmic'                                    :    1,
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'              :    1,
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'        :    1,
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'             :    1,
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'       :    1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'                 :    1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'            :    1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField'    :    1,
    'CosmicsAllTeEFID'                                   :    1,
    'j5_empty_NoAlg'                                     :    1,
    'j10_empty_NoAlg'                                     :    1,
    'j50_cosmic_jetNoCut'                                :    1,
    'mu4_MSonly_cosmic'                                  :    1,
 }

ExpressNamesPPV2= {
    ### cosmic
    'tauNoCut_cosmic'                                    :    1,
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'              :    1,
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'        :    1,
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'             :    1,
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'       :    1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'                 :    1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'            :    1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField'    :    1,
    'CosmicsAllTeEFID'                                   :    1,
    'mu4_MSonly_cosmic'                                  :    1,
    ### collision (#rate as requested in 78046)
    '2mu4_Jpsimumu_IDTrkNoCut'                                :   1,   #0.1
    '2mu13_Zmumu_IDTrkNoCut'                                :   1,   #0.1
    '2mu4_Upsimumu'                                            :   1,   #0.1
    'mu15'                                                     :   1,   #0.4
    '2mu10'                                                    :   1,   #0.3
    '2mu10_loose'                                              :   1,   #0.3
    'mu15_mu10_EFFS'                                           :   1,   #0.4
    'mu20_muCombTag_NoEF'                                      :   1,   #0.4
    'mu24_muCombTag_NoEF_tight'                                :   1,   #0.4
    'mu40_muCombTag_NoEF'                                      :   1,   #0.4
    'mu20_IDTrkNoCut'                                          :   1,   #0.4
    '2e5_tight'                                                :   1,   #0
    '2e10_medium'                                              :   1,   #0.4
    '2e12_medium'                                              :   1,   #0.4
    '2e15_medium'                                              :   1,   #0.4
    'e15_medium_xe40_noMu'                                     :   1,   #0.4
    'e15_medium_xe50_noMu'                                     :   1,   #0.4
    'mu15_mu10_EFFS_medium'                                    :   1,   #0.4
    '2mu10'                                                    :   1,   #0.4
    'e22_medium_IDTrkNoCut'                                    :   1,   #0.4
    #'e20_loose_xe30'                                          :   1,   #0.4
    'g20_loose'                                                :   1,   #0.4
    'e20_medium_IDTrkNoCut'                                    :   1,   #0.5
    #'Zee_monitor'                                             :   1,   #0
    'j30_a4tc_EFFS'                                              :   1,   #0.4
    'fj30_a4tc_EFFS'                                             :   1,   #0.4
    'j240_a4tc_EFFS'                                             :   1,   #0.4
    'mu4_L1J10_matched'                                        :   1,   #0.4
    'b10_IDTrkNoCut'                                           :   1,   #0.4
    #'tau29_loose'                                             :   1,   #0.4
    #'tau84_loose'                                             :   1,   #0.4
    'tau16_IDTrkNoCut'                                         :   1,   #0.4
    'tau50_IDTrkNoCut'                                         :   1,   #0.4
    'tauNoCut'                                                 :   1,   #0.4
    'tau16_medium'                                             :   1,   #0.4
    'tauNoCut_L1TAU50'                                         :   1,   #0.4
    'xe20_noMu'                                                :   1,   #0.25
    'InDetMon_FS'                                              :   1,   #0.4
    'rd0_empty_NoAlg'                                          :   1,   #0.5
    'rd0_filled_NoAlg'                                         :   1,   #1
    'L1J10_firstempty_NoAlg'                                   :   1,
    'L1J30_firstempty_NoAlg'                                   :   1,
    #'L1_J10_FIRSTEMPY'                                        :   1,   #1
    #'L1_J30_FIRSTEMPY'                                        :   1,   #1
    'L1Calo_NoAlg'                                             :   0,
    'L1Muon_NoAlg'                                             :   0,
    'L1MinBias_NoAlg'                                          :   0,
    'PreS_L1J30_empty'                                         :   0,
    'PreS_L1J30_firstempty'                                    :   0,
 }


ExpressNamesPPV3= {
    ### cosmic
    'tauNoCut_cosmic'                                    :    1,
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'              :    1,
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'        :    1,
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'             :    1,
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'       :    1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'                 :    1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'            :    1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField'    :    1,
    'CosmicsAllTeEFID'                                   :    1,
    'mu4_MSonly_cosmic'                                  :    1,
    ### collision (#rate as requested in 78046)
    '2mu4T_Jpsimumu_IDTrkNoCut'                                :   1,   #0.1
    '2mu13_Zmumu_IDTrkNoCut'                                :   1,   #0.1
    '2mu4T_Upsimumu'                                            :   1,   #0.1
    'mu15'                                                     :   1,   #0.4
    '2mu10'                                                    :   1,   #0.3
    '2mu10_loose'                                              :   1,   #0.3
    'mu15_mu10_EFFS'                                           :   1,   #0.4
    'mu20_muCombTag_NoEF'                                      :   1,   #0.4
    'mu20_IDTrkNoCut'                                          :   1,   #0.4
    '2e5_tight'                                                :   1,   #0
    '2e10_medium'                                              :   1,   #0.4
    '2e12_medium'                                              :   1,   #0.4
    '2e15vh_medium'                                              :   1,   #0.4
    'e15vh_medium_xe40_noMu'                                     :   1,   #0.4
    'e15vh_medium_xe50_noMu'                                     :   1,   #0.4
    'mu15_mu10_EFFS_medium'                                    :   1,   #0.4
    '2mu10'                                                    :   1,   #0.4
    'e22_medium_IDTrkNoCut'                                    :   1,   #0.4
    #'e20_loose_xe30'                                          :   1,   #0.4
    'g20_loose'                                                :   1,   #0.4
    'e22vh_medium_IDTrkNoCut'                                    :   1,   #0.5
    #'Zee_monitor'                                             :   1,   #0
    'j30_a4tc_EFFS'                                              :   1,   #0.4
    'fj30_a4tc_EFFS'                                             :   1,   #0.4
    'j240_a4tc_EFFS'                                             :   1,   #0.4
    'mu4_L1J10_matched'                                        :   1,   #0.4
    'b10_IDTrkNoCut'                                           :   1,   #0.4
    #'tau29_loose'                                             :   1,   #0.4
    #'tau84_loose'                                             :   1,   #0.4
    'tau16_IDTrkNoCut'                                         :   1,   #0.4
    'tau50_IDTrkNoCut'                                         :   1,   #0.4
    'tauNoCut'                                                 :   1,   #0.4
    'tau16_medium'                                             :   1,   #0.4
    'tauNoCut_L1TAU50'                                         :   1,   #0.4
    'xe20_noMu'                                                :   1,   #0.25
    'InDetMon_FS'                                              :   1,   #0.4
    'rd0_empty_NoAlg'                                          :   1,   #0.5
    'rd0_filled_NoAlg'                                         :   1,   #1
    'L1J10_firstempty_NoAlg'                                   :   1,
    'L1J30_firstempty_NoAlg'                                   :   1,
    #'L1_J10_FIRSTEMPY'                                        :   1,   #1
    #'L1_J30_FIRSTEMPY'                                        :   1,   #1
    'L1Calo_NoAlg'                                             :   0,
    'L1Muon_NoAlg'                                             :   0,
    'L1MinBias_NoAlg'                                          :   0,
    'PreS_L1J30_empty'                                         :   0,
    'PreS_L1J30_firstempty'                                    :   0,
    'tau29_IDTrkNoCut'                                         :   1,
    'tau125_IDTrkNoCut'                                        :   1,
    '2e5_tight_Jpsi'                                           :   1,
    'tau20_medium1'                                            :   1,
 }
ExpressNamesPPV4= {
    'j165_u0uchad_LArNoiseBurst'                               :   1,
    'j55_u0uchad_firstempty_LArNoiseBurst'                     :   1,
    'j55_u0uchad_empty_LArNoiseBurst'                          :   1,
    'xe45_LArNoiseBurst'                                       :   1,
    'xe55_LArNoiseBurst'                                       :   1,
    #'xe60_LArNoiseBurst'                                       :   1,
    'j35_u0uchad_empty_LArNoiseBurst'                          :   1,
    'j35_u0uchad_firstempty_LArNoiseBurst'                     :   1,
    'j80_u0uchad_LArNoiseBurstT'                               :   1,
    'mu18it_tight'                                             :   1,
    'mu24i_tight'                                              :   1,
    '2mu13'                                                    :   1,
    '2mu6_Jpsimumu'                                            :   1,
    '2mu6_Upsimumu'                                            :   1,
    'mu18_IDTrkNoCut_tight'                                    :   1,
    'mu22_IDTrkNoCut_tight'                                    :   1,
    'mu24_muCombTag_NoEF_tight'                                :   1,
    'e24vh_medium1'                                            :   1,
    '2e12Tvh_loose1'                                           :   1,
    'e24vh_tight1_e15_NoCut_Zee'                               :   1,
    'g30_loose_g20_loose'                                      :   1,
    'g35_loose_g25_loose'                                      :   1,
    'e24vh_medium1_IDTrkNoCut'                                 :   1,
    'tau20_medium1'                                            :   1,
    'tauNoCut'                                                 :   1,
    'tauNoCut_L1TAU40'                                         :   1,
    'tau29_IDTrkNoCut'                                         :   1,
    'tau125_IDTrkNoCut'                                        :   1,
    'j35_a4tchad'                                              :   1,
    'j180_a4tchad'                                             :   1,
    'fj55_a4tchad_L2FS'                                        :   1,
    'mu4T_j55_a4tchad_matched'                                 :   1,
    'b55_NoCut_j55_a4tchad'                                    :   1,
    'xe30'                                                     :   1,
    'xe80'                                                     :   1,
    'te1400'                                                   :   1,
    'InDetMon_FS'                                              :   1,
    'mbMbts_2_NoAlg'                                           :   1,
    'mbSpTrk'                                                  :   1,
    'rd0_empty_NoAlg'                                          :   1,
    'rd0_filled_NoAlg'                                         :   1,
    'L1J10_firstempty_NoAlg'                                   :   1,
    'L1J30_firstempty_NoAlg'                                   :   1,
    '2mu13_Zmumu_IDTrkNoCut'                                   :   1,
    '2mu4T_Jpsimumu_IDTrkNoCut'                                :   1,
    'mu4_L1MU11_MSonly_cosmic'                                 :   1,
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'             :   1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'                  :   1,
    'CosmicsAllTeEFID'                                         :   1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'                        :   1,
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'                   :   1,
    'tauNoCut_cosmic'                                          :   1,
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'                    :   1,
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'              :   1,
    }

ExpressNamesInitialBeamHIV1= {
    #initial running
    'SeededStreamerExpressPhysicsHI'               : 10,
    'SeededStreamerExpressStandbyHI'               : 10,
    "SeededStreamerRNDM"                           : -1,
    "SeededStreamerL1Calo"                         : -1,
    "SeededStreamerMinBias"                        : -1,
    "SeededStreamerMBTS"                           : -1,
    "SeededStreamerBCM_LUCID_ZDC"                  : -1,
    "SeededStreamerMuonswBeam"                     : -1,
    #physics
    #'L1ItemStreamer_L1_FJ10'                       :  -1,
    #'L1ItemStreamer_L1_FJ95'                       :  -1,
    #'L1ItemStreamer_L1_J5'                         :  -1,
    #'L1ItemStreamer_L1_J55'                        :  -1,
    #'L1ItemStreamer_L1_EM2'                        :  -1,
    #'L1ItemStreamer_L1_EM5'                        :  -1,
    #'L1ItemStreamer_L1_2EM5'                       :  -1,
    #'L1ItemStreamer_L1_EM14'                       :  -1,
    #'L1ItemStreamer_L1_MU0'                        :  -1,
    #'L1ItemStreamer_L1_2MU0'                       :  -1,
    #'L1ItemStreamer_L1_2MU6'                       :  -1,
    #'L1ItemStreamer_L1_MU10'                       :  -1,
    #'L1ItemStreamer_L1_MU20'                       :  -1,
    #'L1ItemStreamer_L1_MBTS_4_4'                   : -20,
    #'L1ItemStreamer_L1_RD0_EMPTY'                  :  -2,
    'mbMbts_1_1'                                   : 100,
    'mbMbts_2_2'                                   : 100,
    'mbMbts_2_2_eff_HI'                            : 100,
    'mbMbts_4_4'                                   : 100,
    'mbMbts_4_4_eff_HI'                            : 100,
    'mbSp_HI'                                      : 2,
    ### cosmic
    'tauNoCut_cosmic'                                    :    -1,
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'              :    -1,
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'        :    -1,
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'             :    -1,
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'       :    -1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'                 :    -1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'            :    -1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField'    :    -1,
    'CosmicsAllTeEFID'                                   :    -1,
    'j5_empty_NoAlg'                                     :    -1,
    'j50_cosmic_jetNoCut'                                :    -1,
    'mu4_MSonly_cosmic'                                  :    -1,
 }

ExpressNamesHIV1= {
    #initial running
    "SeededStreamerRNDM"                           : -1,
    "SeededStreamerL1Calo"                         : -1,
    "SeededStreamerMinBias"                        : -1,
    "SeededStreamerMBTS"                           : -1,
    "SeededStreamerBCM_LUCID_ZDC"                  : -1,
    "SeededStreamerMuonswBeam"                     : -1,
    "L1ItemStreamer_L1_MBTS_1_1"                   : -1,
    #pPb
    'mbSpTrk_L1ZDC'                                : -1,
    'mbSpTrk_L1MBTS_2'                             : -1,
    'mbSpTrk_L1MBTS_1_1'                           : -1,
    'mbZdc_NoAlg'                                  : -1,
    #PbPb
    'rd0_empty_NoAlg'                              : 1,
    'mbSp_HI'                                      : 2,
    'mu10_MSonly'	                           : 1,
    'mu10_MSonly_EFFS_L1ZDC'                       : 1,
    'mu4_MSonly_L1TE50'                            : 1,
    '2mu4_MSonly'                                  : 1,
    '2mu4_MSonly_EFFS_L1ZDC'                       : 1,
    '2mu6T_MSonly'                                 : 1,
    'mu4_IDTrkNoCut'	                           : 1,
    'mu4T_IDTrkNoCut'                              : 1,
    'L1MU0_NoAlg'	                           : 1,
    'L1MU4_NoAlg'	                           : 1,
    'j15_a2hi_EFFS_L1ZDC'	                   : 1,
    'j20_a2hi_EFFS_L1ZDC'                          : 1,
    'j75_a2hi_EFFS_L1ZDC'                          : 1,
    'Background'                                   : 1,
    'mbZdc_a_c_eff_HI'                             : 1,
    'mbZdc_a_c_NoAlg'                              : 1,
    'mbZdc_a_c_L1VTE50_trk'                        : 1,
    'mbZdc_a_c_L1TE50_NoAlg'                       : 1,
    'L1TE50_NoAlg'                                 : 1,
    '2g5_loose'                                    : 1,
    'g5_loose'                                     : 1,
    'L1EM3_NoAlg'                                  : 1,
    'g10_loose'                                    : 1,
    'L1EM16_NoAlg'                                 : 1,
    'L1EM12_NoAlg'                                 : 1,
    'InDetMon_HI_FS'                               : 1,
    'mu4_MSonly_j15_a2hi_EFFS_matched'             : 1,
    ### cosmic
    'tauNoCut_cosmic'                                    :    -1,
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'              :    -1,
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'        :    -1,
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'             :    -1,
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'       :    -1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'                 :    -1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'            :    -1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField'    :    -1,
    'CosmicsAllTeEFID'                                   :    -1,
    'mu4_MSonly_cosmic'                                  :    -1,
 }

ExpressNamesHIV2= {
    "j10_u0uchad_LArNoiseBurst"                    : 1,
    "j10_u0uchad_firstempty_LArNoiseBurst"         : 1,
    "j10_u0uchad_empty_LArNoiseBurst"              : 1,
    #initial running
    "SeededStreamerRNDM"                           : -1,
    "SeededStreamerL1Calo"                         : -1,
    "SeededStreamerMinBias"                        : -1,
    "SeededStreamerMBTS"                           : -1,
    "SeededStreamerBCM_LUCID_ZDC"                  : -1,
    "SeededStreamerMuonswBeam"                     : -1,
    "L1MBTS_1_1_NoAlg"                             : -1,
    "L1MBTS_2_NoAlg"                               : -1,
    'L1ZDC_NoAlg'                                  : -1,
    'L1ZDC_A_NoAlg'                                : -1,
    'L1ZDC_C_NoAlg'                                : -1,
    'L1LHCF_NoAlg'                                 : -1,
    #pPb
    'mbSpTrk_L1ZDC'                                : -1,
    'mbSpTrk_L1MBTS_2'                             : -1,
    'mbSpTrk_L1MBTS_1_1'                           : -1,
    'mbSpTrk_L1TE0'                                : -1,
    'mbSpTrk_L1FJ0'                                : -1,
    'mbSpTrk_L1FJ0_A'                              : -1,
    'mbSpTrk_L1FJ0_C'                              : -1,
    'mbMbts_2'                                     : -1,
    'mbMbts_1_1'                                   : -1,
    'mbRd1_eff'                                    : -1,
    'rd0_filled_NoAlg'                             : -1,
    'rd0_empty_NoAlg'                              : 1,
    'mbSpTrk'                                      : 1,
    'mbSp_HI_L1BGRP1'                              : 2,
    'mbSpTrk_noiseSup'                             : -1,
    #high-pt physics
    'L1MU0_NoAlg'                                  : 1,
    'mu8'         	                           : 1,
    '2mu4'                                         : 1,
    'mu4_IDTrkNoCut'	                           : 1,
    'mu6_IDTrkNoCut'                               : 1,
    'mu8_IDTrkNoCut'                               : 1,
    'L1MU4_NoAlg'	                           : 1,
    # jets
    'j20_a4tchad_EFFS_L2mbMbts'	                   : 1,
    'j30_a4tchad_EFFS_L2mbMbts'                    : 1,
    'j20_a4hi_EFFS_L2mbMbts'                       : 1, 
    'j30_a4hi_EFFS_L2mbMbts'                       : 1, 
    'j50_a4tchad_EFFS_L1J10'                       : 1,
    'j60_a4tchad_EFFS_L1J15'                       : 1,
    'j75_a4tchad_EFFS_L1J15'                       : 1,
    'j80_a4tchad_EFFS_L1J15'                       : 1,
    'j50_a4hi_EFFS_L1J10'                          : 1, 
    'j60_a4hi_EFFS_L1J15'                          : 1, 
    'j75_a4hi_EFFS_L1J15'                          : 1, 
    'j80_a4hi_EFFS_L1J15'                          : 1,
    'fj15_a4tchad_EFFS_L1FJ0'                      : 1,
    'fj15_a4hi_EFFS_L1FJ0'                         : 1,
    'fj15_a4tchad_EFFS_L2mbMbts'                   : 1, 
    'fj15_a4hi_EFFS_L2mbMbts'                      : 1,
    '2j10_a4tchad_eta50_deta40_L2mbMbts'           : 1,
    '2j10_a4hi_eta50_deta40_L2mbMbts'              : 1,
    'L1TE65_NoAlg'                                 : 1,
    'L1TE90_NoAlg'                                 : 1,
    'L1TE100_NoAlg'                                : 1,
    #pp jets
    'j10_a4tchad_EFFS_L2mbMbts_L1RD0'	           : 1,
    'j15_a4tchad_EFFS_L2mbMbts_L1RD0'	           : 1,
    'j20_a4tchad_EFFS_L2mbMbts_L1RD0'	           : 1,
    'j30_a4tchad_EFFS_L2mbMbts_L1RD0'              : 1,
    'j10_a4tchad_EFFS_L2te_L1RD0'	           : 1,
    'j15_a4tchad_EFFS_L2te_L1RD0'	           : 1,
    'j20_a4tchad_EFFS_L2te_L1RD0'	           : 1,
    'j30_a4tchad_EFFS_L2te_L1RD0'                  : 1,
    'j10_a4tchad_EFFS_L1RD0'	                   : 1,
    'j15_a4tchad_EFFS_L1RD0'	                   : 1,
    'j20_a4tchad_EFFS_L1RD0'	                   : 1,
    'j30_a4tchad_EFFS_L1RD0'                       : 1,
    'fj10_a4tchad_EFFS_L2mbMbts_L1RD0'             : 1,
    'fj15_a4tchad_EFFS_L2mbMbts_L1RD0'             : 1,
    'fj10_a4tchad_EFFS_L2te_L1RD0'                 : 1,
    'fj15_a4tchad_EFFS_L2te_L1RD0'                 : 1,
    'fj10_a4tchad_EFFS_L1RD0'                      : 1,
    'fj15_a4tchad_EFFS_L1RD0'                      : 1,
    '2j10_a4tchad_eta50_deta40_L2mbMbts_L1RD0'     : 1,
    '2j10_a4tchad_eta50_deta40_L2te_L1RD0'         : 1,
    '2j10_a4tchad_eta50_deta40_L1RD0'              : 1,
    #
    'Background'                                   : 1,
    #
    'g15_loose'                                    : 1,
    'g15_medium'                                   : 1,
    'g20_medium'                                   : 1,
    'e15_loose1'                                   : 1,
    'L1EM16_NoAlg'                                 : 1,
    'L1EM12_NoAlg'                                 : 1,
    'e5_medium1_IDTrkNoCut'                        : 1,
    # p-p
    'mbSpTrkVtxMh_pp_trk30_L1TE0_pileupSup'        : 1,
    'mbSpTrkVtxMh_pp_trk50_L1TE0_pileupSup'        : 1,
    'mbSpTrkVtxMh_pp_trk60_L1TE0_pileupSup'        : 1,
    'mbSpTrkVtxMh_pp_trk70_L1TE0_pileupSup'        : 1,
    'mbSpTrkVtxMh_pp_trk50_L1TE10_pileupSup'       : 1,
    'mbSpTrkVtxMh_pp_trk60_L1TE10_pileupSup'       : 1,
    'mbSpTrkVtxMh_pp_trk70_L1TE10_pileupSup'       : 1,
    # p-Pb
    'mbSpTrkVtxMh_hip_trk100_L1TE10'               : 1,
    'mbSpTrkVtxMh_hip_trk110_L1TE10'               : 1,
    'mbSpTrkVtxMh_hip_trk120_L1TE10'               : 1,
    'mbSpTrkVtxMh_hip_trk130_L1TE10'               : 1,
    'mbSpTrkVtxMh_hip_trk140_L1TE20'               : 1,
    'mbSpTrkVtxMh_hip_trk150_L1TE20'               : 1,
    'mbSpTrkVtxMh_hip_trk155_L1TE20'               : 1,
    'mbSpTrkVtxMh_hip_trk160_L1TE20'               : 1,
    'mbSpTrkVtxMh_hip_trk170_L1TE35'               : 1,
    'mbSpTrkVtxMh_hip_trk175_L1TE35'               : 1,
    'mbSpTrkVtxMh_hip_trk180_L1TE35'               : 1,
    'mbSpTrkVtxMh_hip_trk190_L1TE35'               : 1,
    'mbSpTrkVtxMh_hip_trk140_L1TE50'               : 1,
    'mbSpTrkVtxMh_hip_trk150_L1TE50'               : 1,
    'mbSpTrkVtxMh_hip_trk155_L1TE50'               : 1,
    'mbSpTrkVtxMh_hip_trk160_L1TE50'               : 1,
    'mbSpTrkVtxMh_hip_trk170_L1TE50'               : 1,
    'mbSpTrkVtxMh_hip_trk175_L1TE50'               : 1,
    'mbSpTrkVtxMh_hip_trk180_L1TE50'               : 1,
    'mbSpTrkVtxMh_hip_trk190_L1TE50'               : 1,
    'mbSpTrkVtxMh_hip_trk175_L1MBTS'               : 1,
    'mbSpTrkVtxMh_hip_trk180_L1TE65'               : 1,
    'mbSpTrkVtxMh_hip_trk225_L1TE65'               : 1,
    'mbSpTrkVtxMh_hip_trk230_L1TE65'               : 1,
    'mbSpTrkVtxMh_hip_trk240_L1TE65'               : 1,
    'mbSpTrkVtxMh_hip_trk245_L1TE65'               : 1,
    'mbSpTrkVtxMh_hip_trk200_L1TE90'               : 1,
    'mbSpTrkVtxMh_hip_trk210_L1TE90'               : 1,
    'mbSpTrkVtxMh_hip_trk220_L1TE90'               : 1,
    'mbSpTrkVtxMh_hip_trk230_L1TE90'               : 1,
    'mbSpTrkVtxMh_hip_trk240_L1TE90'               : 1,
    'InDetMon_FS'                                  : 1,
    '2mu4_Jpsimumu_IDTrkNoCut'                     : 1,
    'e5_tight1_e5_NoCut'                           : 1,
    'mu4_j40_a4hi_EFFS_matched'                    : 1,
    ### cosmic
    'tauNoCut_cosmic'                                    :    -1,
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'              :    -1,
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'        :    -1,
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'             :    -1,
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'       :    -1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'                 :    -1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'            :    -1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField'    :    -1,
    'CosmicsAllTeEFID'                                   :    -1,
    'mu4_MSonly_cosmic'                                  :    -1,
 }

ExpressNamesAlfaV2= {
    'j55_u0uchad_firstempty_LArNoiseBurst'                     :   1,
    'InDetMon_FS'                                              :   1,
    'j165_u0uchad_LArNoiseBurst'                               :   1,
    'L1J10_firstempty_NoAlg'                                   :   1,
    'j55_u0uchad_empty_LArNoiseBurst'                          :   1,
    'rd0_filled_NoAlg'                                         :   1,
    'mbMbts_2_NoAlg'                                        :   1,
    'mbMbts_1_NoAlg'                                        :   1,
    }

RareNamesInitialBeamHIV1= {
    }

RareNamesHIV1= {
    }

RareNamesHIV2= {
    }

ExpressNamesCosmic= {
    'CosmicsAllTeEFID'             		   :  1,
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo' :  1,
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'  :  1,
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'           :  1,
    'j50'                          		   :  2,
    'SeededStreamerCosmicMuons'                    : 35,
    'SeededStreamerL1Calo'                         :  5,
    'SeededStreamerCosmicCalo'                     :  5,
    'SeededStreamerL1CaloEM'                       :  1,
    'SeededStreamerCosmicCaloEM'                   :  1,
    'SeededStreamerRNDM'                           :  3,
    'j10_cosmic'                                   : 17,
    'j80_cosmic'                                   :  1,
    'mu10_MSonly_cosmic'                           :  3,
     }

ExpressNamesInitialBeam= {
    #for commissioning period
    #'SeededStreamerL1Calo'                         : 10,
    #'SeededStreamerMinBias'                        : 10,
     #'SeededStreamerL1CaloEM'                       : 10,
    #'SeededStreamerL1CaloJet'                      : 10,
    #'SeededStreamerL1CaloTau'                      : 10,
    #'SeededStreamerL1CaloMET'                      : 10,
    #'SeededStreamerRNDM'                           : 10,
    #'SeededStreamerMBTS'                           : 10,
    #'SeededStreamerBCM_LUCID_ZDC'                  : 10,
    #'SeededStreamerCosmicMuons'                    : 10,
    #for 900GeV collisions
    'SeededStreamerMuonswBeam'             	   :  1,
    'SeededStreamerL1CaloEM'                       :  1,
    'SeededStreamerL1CaloJet'                      :  1,
    'SeededStreamerL1CaloTau'                      :  1,
    'SeededStreamerL1CaloMET'                      :  4,
    'SeededStreamerRNDM'                           :  3,
    'SeededStreamerMBTS_ZDC'                       : 20,
    #Backup items
    'L1ItemStreamer_L1_J5'                         :  1,
    'L1ItemStreamer_L1_FJ18'                       :  1,
    'L1ItemStreamer_L1_J15'                        :  1,
    'L1ItemStreamer_L1_J55'                        :  1,
    'L1ItemStreamer_L1_J75'                        :  1,
    'L1ItemStreamer_L1_EM2'                        :  1,
    'L1ItemStreamer_L1_EM3'                        :  1,
    'L1ItemStreamer_L1_EM5'                        :  1,
    'L1ItemStreamer_L1_EM14'                       :  1,
    'L1ItemStreamer_L1_MU0'                        :  1,
    'L1ItemStreamer_L1_2MU0'                       :  1,
    'L1ItemStreamer_L1_MU10'                       :  1,
    'L1ItemStreamer_L1_MU20'                       :  1,
    'L1ItemStreamer_L1_XE10'                       :  4,
    'L1ItemStreamer_L1_TE10'                       :  4,
    'L1ItemStreamer_L1_TE100'                      :  1,
    'L1ItemStreamer_L1_TAU5'                       :  1,
    'L1ItemStreamer_L1_TAU6'                       :  1,
    'L1ItemStreamer_L1_MBTS_4_4'                   :  20,
    'L1ItemStreamer_L1_RD0_EMPTY'                  :  2,
    }

ExpressNamesInitialBeamV3= {
    #for commissioning period
    #'SeededStreamerL1Calo'                         : 10,
    #'SeededStreamerMinBias'                        : 10,
     #'SeededStreamerL1CaloEM'                       : 10,
    #'SeededStreamerL1CaloJet'                      : 10,
    #'SeededStreamerL1CaloTau'                      : 10,
    #'SeededStreamerL1CaloMET'                      : 10,
    #'SeededStreamerRNDM'                           : 10,
    #'SeededStreamerMBTS'                           : 10,
    #'SeededStreamerBCM_LUCID_ZDC'                  : 10,
    #'SeededStreamerCosmicMuons'                    : 10,
    #for 900GeV collisions
    'SeededStreamerMuonswBeam'             	   :   1,
    'SeededStreamerL1CaloEM'                       :  1,
    'SeededStreamerL1CaloJet'                      :  1,
    'SeededStreamerL1CaloTau'                      :  1,
    'SeededStreamerL1CaloMET'                      :  4,
    'SeededStreamerRNDM'                           :  3,
    'SeededStreamerMBTS_ZDC'                       : 20,
    #Backup items
    'L1ItemStreamer_L1_J5'                         :  1,
    'L1ItemStreamer_L1_FJ15'                       :  1,
    'L1ItemStreamer_L1_J15'                        :  1,
    'L1ItemStreamer_L1_J55'                        :  1,
    'L1ItemStreamer_L1_J75'                        :  1,
    'L1ItemStreamer_L1_EM2'                        :  1,
    'L1ItemStreamer_L1_EM3'                        :  1,
    'L1ItemStreamer_L1_EM5'                        :  1,
    'L1ItemStreamer_L1_2EM5'                       :  1,
    'L1ItemStreamer_L1_EM14'                       :  1,
    'L1ItemStreamer_L1_MU0'                        :  1,
    'L1ItemStreamer_L1_2MU0'                       :  1,
    'L1ItemStreamer_L1_2MU6'                       :  1,
    'L1ItemStreamer_L1_MU10'                       :  1,
    'L1ItemStreamer_L1_MU20'                       :  1,
    'L1ItemStreamer_L1_XE10'                       :  4,
    'L1ItemStreamer_L1_TE10'                       :  4,
    'L1ItemStreamer_L1_TE100'                      :  1,
    'L1ItemStreamer_L1_TAU5'                       :  1,
    'L1ItemStreamer_L1_TAU6'                       :  1,
    'L1ItemStreamer_L1_MBTS_4_4'                   :  20,
    'L1ItemStreamer_L1_RD0_EMPTY'                  :  2,
    'j10'					   :  0,
    'j40'                                          :  0,
    'j140'                                         :  0,
    'j200'                                         :  0,
#    'e3_NoCut'                                     :  0,
#    'g3_nocut'                                     :  0,
#    'e5_NoCut'                                     :  0,
#    'g5_nocut'                                     :  0,
#    'g5_loose'                                     :  0,
#    'e10_medium'                                   :  0,
#    'e10_loose'                                    :  0,
#    'g10_loose'                                    :  0,
#    'e20_loose_NoIDTrkCut'                         :  0,
#    'e5_NoCut_SiTrk'                               :  0,
#    'e5_NoCut_IdScan'                              :  0,
#    'e5_NoCut_TRT'                                 :  0,
#    'e5_loose_NoIDTrkCut'                          :  0,
    'e5_loose_NoIDTrkCut'                          :  0,
    'e5_NoCut_cells'                               :  0,
    'e10_loose_IdScan'                             :  0,
    'e10_medium_IdScan'                            :  0,
    'e10_NoCut'                                    :  0,
    'g10_loose'                                    :  0,
    'e20_loose_NoIDTrkCut'                         :  0,
    '2e3_loose_IdScan'                             :  0,
    'mu6'                                          :  0,
    'mu10_MSonly'                                  :  0,
    '2mu4_MSonly'                                  :  0,
    'mu4_tile'                                     :  0,
    'mu4'                                          :  0,
    'mu6_MSonly'                                   :  0,
    'mu10i_loose'                                  :  0,
    'mu20_MSonly'                                  :  0,
    'MU4_Trk_Jpsi'                                 :  0,
    'MU4_Trk_Upsi_loose_FS'                        :  0,
    'mu4_NoIDTrkCut'                               :  0,
    'mu10_NoIDTrkCut'                              :  0,
    'xe20_noMu'                                    :  0,
    'xe60_medium_noMu'                             :  0,
    'te90'                                         :  0,
    'te250_medium'                                 :  0,
    'tau12_loose_PT'                               :  0,
    'trk9_loose'                                   :  0,
    'tau12_loose'                                  :  0,
    'tauNoCut_SiTrk'                               :  0,
    'tauNoCut_cells'                               :  0,
    'tauNoCut_NoIDTrkCut'                          :  0,
    'tau16i_NoIDTrkCut'                            :  0,
    'mb_M4_4_eff'                                  :  0,
    'L2IdScan_FS'                                  :  0,
    'L2SiTrack_FS'                                 :  0,
    'EFInsideOut_FS'                               :  0,
    'EFTRTOnly_FS'                                 :  0,
    'InDetMon_FS'                                  :  0,
    'b10'                                          :  0,
    'b10_IDScan_noCut'                             :  0,
   }
