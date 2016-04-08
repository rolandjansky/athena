# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
#
# SliceDef file for HLT signatures used for streaming based on TT
#
#########################################################################

from TriggerMenuPython.HltConfig import *
from TriggerMenuPython.Lvl1Flags import Lvl1Flags


L1CaloItemList_EM_V2 = []

for item in Lvl1Flags.items():
    if 'L1_EM' in item:
        L1CaloItemList_EM_V2.append(item)


class L2EFChain_SeededStreamer(L2EFChainDef):

    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, inputTEs, config,
                 prescale=1, pass_through=0):
        self._supported = ["SeededStreamerRNDM",       
                           "SeededStreamerBPTX",       
                           "SeededStreamerL1Calo",
                           "SeededStreamerL1CaloEMFilter_V2",
                           "SeededStreamerL1CaloEM",
                           "SeededStreamerEMtoL1Calo",
                           "SeededStreamerL1CaloTau",
                           "SeededStreamerL1CaloJet",
                           "SeededStreamerL1CaloMET",
                           "SeededStreamerL1CaloXS",
                           "SeededStreamerMinBias",
                           "SeededStreamerMBTS",
                           "SeededStreamerBCM_LUCID_ZDC",
                           "SeededStreamerMBTS_ZDC",
                           "SeededStreamerBCM",
                           "SeededStreamerLUCID",
                           "SeededStreamerCosmicMuons",
                           "SeededStreamerMuonswBeam",
                           "SeededStreamerMinBiasFilter",
                           "SeededStreamerWarmStart",
                           "SeededStreamerCosmicCalo",
                           "SeededStreamerCosmicCaloEM",                           
                           "SeededStreamerCosmicMinBias",
                           "SeededStreamerZeroBias",
                           "L1Calo_NoAlg",
                           "L1Muon_NoAlg",
                           "L1MinBias_NoAlg",
                           "Standby",
                           "SeededStreamerExpressPhysicsHI",
		 	   "SeededStreamerExpressStandbyHI",
                           "Background",
                           "Background_RD0_UNPAIRED_ISO",
                           "Background_RD0_UNPAIRED_NONISO",
                           "Background_RD0_EMPTY",
                           ]
        
        if sig_id not in self._supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self._supported) )

        L2EFChainDef.__init__(self, sig_id,
                              "L2_"+sig_id, chain_counter,
                              l2_lower_chain_name,
                              "EF_"+sig_id, chain_counter,
                              inputTEs, config,
                              prescale, pass_through)
        mlog = logging.getLogger( 'StreamingDef.py:L2EFChain_SeededStreamer' )
        mlog.verbose('in L2EFChain_Streamer constructor for %s' % sig_id)

        
    class L2Config:
        def __init__(self):
            pass
       
    class EFConfig:
        def __init__(self):
            pass

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig
            
    def defineSequences(self, config):
        # L2 sequences
        L2seqname='L2Streamer_%s' % self.sig_id
        self.addL2Sequence(self.inputTEs,
                           [],L2seqname)
        #
        # EF sequences
        #EFseqname='EFStreamer_%s' % self.sig_id
        #self.addEFSequence([L2seqname],
        #                   [],
        #                   EFseqname)
        pass

    def defineSignatures(self):
        pass

    def defineStreamGroupTriggerType(self):
        self.groups = ['RATE:SeededStreamers','BW:Other']
        pass
    
    def getL1SeedList():
        return l2chain.lower_chain_name.replace(' ', '').split(',')



class SeededStreamerMaker:
    _chains = []
    def __init__(self,triggerPythonConfig=None):
        self.tpc = triggerPythonConfig
        if not SeededStreamerMaker._chains:

            # Trigger type logic - matches Lvl1.py definitions
            TT_rand    = 1 << 0
            TT_bptx    = 1 << 1
            TT_calo    = 1 << 2        
            TT_tgc     = 1 << 3
            TT_rpcin   = 1 << 3 # -> same as tgc bit
            TT_minb    = 1 << 5
            TT_rpcout  = 1 << 6
            TT_phys    = 1 << 7
            TT_caloem  = 1 << 4
            TT_zerobs  = 1 << 1
            
            em_type  = TT_calo   | TT_phys | TT_caloem
            cl_type  = TT_calo   | TT_phys
            mb_type  = TT_minb   | TT_phys
            rd_type  = TT_rand   | TT_phys
            bx_type  = TT_bptx   | TT_phys        
            zb_type  = TT_zerobs | TT_phys

                        
            # streaming for beam items
            #calo_exceptions = set(["L1_EM14", "L1_2EM5", "L1_2EM3_EM7", "L1_2TAU6_EM10", "L1_2TAU8_TAU11", "L1_EM10", "L1_TAU11_XE20", "L1_EM10_XE20", "L1_TAU5"])
            calo_exceptions = set([])
            
            calo_seeds     =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if ("EMPTY" not in x or "FIRSTEMPTY" in x) and (x not in calo_exceptions)
                                       ])
            em_seeds       =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if ('L1_EM' in x or 'L1_2EM' in x or 'L1_3EM' in x ) \
                                       and ("EMPTY" not in x or "FIRSTEMPTY" in x) ])
            tau_seeds      =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if ("EMPTY" not in x or "FIRSTEMPTY" in x) and "TAU" in x])
            met_seeds      =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if ("EMPTY" not in x or "FIRSTEMPTY" in x) and ("XE" in x or "TE" in x or "XS" in x) ])
            met_xs_seeds      =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if ("EMPTY" not in x or "FIRSTEMPTY" in x) and ("XS" in x) ])
            jet_seeds      =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if ("EMPTY" not in x or "FIRSTEMPTY" in x) and "J" in x ])
            minb_seeds     =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(mb_type, mb_type)])
            mbts_seeds     =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(mb_type, mb_type) if "MBTS" in x ])
            non_mbts_seeds     =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(mb_type, mb_type) if "MBTS" not in x ])
            mbts_zdc_seeds =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(mb_type, mb_type) if ("MBTS" in x or "ZDC" in x) ])
            bcm_seeds =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(mb_type, mb_type) if "BCM" in x ])
            lucid_seeds =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(mb_type, mb_type) if "LUCID" in x ])

            # Random, zero bias random and bptx items
            rand_seeds     =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(rd_type, rd_type)])
            bptx_seeds     =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(bx_type, bx_type)])
            zerob_seeds    =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(zb_type, zb_type) if "RD1_EMPTY" not in x])
            
            # Collect L1 items with matching trigger type AND which have empty in names
            minb_cosmic =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(mb_type, mb_type) if "FIRSTEMPTY" not in x and 'EMPTY' in x])
#            calo_cosmic = 'L1_RD1_EMPTY,'
#            calo_cosmic +=','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if "FIRSTEMPTY" not in x and 'EMPTY' in x])
            calo_cosmic =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if "FIRSTEMPTY" not in x and 'EMPTY' in x])
            em_cosmic   =','.join([ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if "FIRSTEMPTY" not in x and 'EMPTY' in x \
                                    and ('L1_EM' in x or 'L1_2EM' in x)])
            bgrp1_seeds = ','.join([x for x in self.tpc.Lvl1ItemByThreshold('BGRP1')])

            from TriggerJobOpts.TriggerFlags import TriggerFlags
            
            standby_seeds    = ",".join([ x for x in TriggerFlags.Lvl1.items() if "_EMPTY" not in x and "CALREQ" not in x and "ZB" not in x])

            if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
                background_seeds = 'L1_EM3_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J10_UNPAIRED_NONISO,L1_TAU8_UNPAIRED_ISO,L1_FJ10_UNPAIRED_ISO,L1_MU0_UNPAIRED_ISO,L1_2MU6_UNPAIRED_ISO,L1_2MU6_UNPAIRED_NONISO,L1_BCM_AC_CA_BGRP0,L1_MBTS_2_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_ISO,L1_BCM_Wide_UNPAIRED_ISO,L1_ZDC_UNPAIRED_ISO,L1_LUCID_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_NONISO,L1_BCM_Wide_UNPAIRED_NONISO,L1_BCM_Wide_EMPTY,L1_ZDC_EMPTY,L1_ZDC_A_C_EMPTY,L1_LUCID_EMPTY,L1_LUCID_A_C_EMPTY,L1_BCM_AC_CA_BGRP0,L1_ZDC_A_C_UNPAIRED_ISO,L1_BCM_AC_CA_UNPAIRED_ISO'
            elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
                background_seeds = 'L1_EM3_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J10_UNPAIRED_NONISO,L1_TAU8_UNPAIRED_ISO,L1_FJ10_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_2MU6_UNPAIRED_ISO,L1_2MU6_UNPAIRED_NONISO,L1_BCM_AC_CA_BGRP0,L1_MBTS_2_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_ISO,L1_BCM_Wide_UNPAIRED_ISO,L1_ZDC_UNPAIRED_ISO,L1_LUCID_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_NONISO,L1_BCM_Wide_UNPAIRED_NONISO,L1_BCM_Wide_EMPTY,L1_ZDC_EMPTY,L1_ZDC_A_C_EMPTY,L1_LUCID_EMPTY,L1_LUCID_A_C_EMPTY,L1_BCM_AC_CA_BGRP0,L1_ZDC_A_C_UNPAIRED_ISO,L1_BCM_AC_CA_UNPAIRED_ISO'
            elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
                # savannah 91813
#                background_seeds = 'L1_RD0_UNPAIRED_ISO,L1_RD0_EMPTY,L1_BCM_AC_CA_BGRP0,L1_BCM_AC_CA_UNPAIRED_ISO,L1_BCM_Wide_EMPTY,L1_BCM_Wide_UNPAIRED_ISO,L1_BCM_Wide_UNPAIRED_NONISO,L1_EM3_UNPAIRED_ISO,L1_FJ30_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J10_UNPAIRED_NONISO,L1_LUCID_A_C_EMPTY,L1_LUCID_A_C_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_NONISO,L1_LUCID_EMPTY,L1_LUCID_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_LUCID_COMM_UNPAIRED_ISO,L1_LUCID_COMM_EMPTY'
                background_seeds = 'L1_BCM_AC_CA_BGRP0,L1_BCM_AC_CA_UNPAIRED_ISO,L1_BCM_Wide_EMPTY,L1_BCM_Wide_UNPAIRED_ISO,L1_BCM_Wide_UNPAIRED_NONISO,L1_EM3_UNPAIRED_ISO,L1_FJ30_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J10_UNPAIRED_NONISO,L1_LUCID_A_C_EMPTY,L1_LUCID_A_C_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_NONISO,L1_LUCID_EMPTY,L1_LUCID_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_LUCID_COMM_UNPAIRED_ISO,L1_LUCID_COMM_EMPTY'
                #                L1_TAU8_UNPAIRED_ISO,
                #                L1_2MU6_UNPAIRED_ISO,
                #                L1_2MU6_UNPAIRED_NONISO,
                #                L1_MBTS_2_UNPAIRED_ISO,
                #                L1_ZDC_UNPAIRED_ISO,
                #                L1_ZDC_EMPTY,
                #                L1_ZDC_A_C_EMPTY,
                #                L1_ZDC_A_C_UNPAIRED_ISO,'
            elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus():
                 background_seeds = 'L1_EM3_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J10_UNPAIRED_NONISO,L1_MU0_UNPAIRED_ISO,L1_2MU6_UNPAIRED_ISO,L1_2MU6_UNPAIRED_NONISO,L1_BCM_AC_CA_BGRP0,L1_LUCID_A_C_UNPAIRED_ISO,L1_BCM_Wide_UNPAIRED_ISO,L1_ZDC_UNPAIRED_ISO,L1_LUCID_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_NONISO,L1_BCM_Wide_UNPAIRED_NONISO,L1_BCM_Wide_EMPTY,L1_ZDC_EMPTY,L1_ZDC_A_C_EMPTY,L1_LUCID_EMPTY,L1_LUCID_A_C_EMPTY,L1_BCM_AC_CA_BGRP0,L1_ZDC_A_C_UNPAIRED_ISO,L1_BCM_AC_CA_UNPAIRED_ISO'
            elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
                background_seeds = 'L1_EM3_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J10_UNPAIRED_NONISO,L1_MU0_UNPAIRED_ISO,L1_2MU6_UNPAIRED_ISO,L1_2MU6_UNPAIRED_NONISO,L1_BCM_AC_CA_BGRP0,L1_LUCID_A_C_UNPAIRED_ISO,L1_BCM_Wide_UNPAIRED_ISO,L1_ZDC_UNPAIRED_ISO,L1_LUCID_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_NONISO,L1_BCM_Wide_UNPAIRED_NONISO,L1_BCM_Wide_EMPTY,L1_ZDC_EMPTY,L1_ZDC_A_C_EMPTY,L1_LUCID_EMPTY,L1_LUCID_A_C_EMPTY,L1_BCM_AC_CA_BGRP0,L1_ZDC_A_C_UNPAIRED_ISO,L1_BCM_AC_CA_UNPAIRED_ISO,L1_FJ0_UNPAIRED_ISO,L1_FJ5_UNPAIRED_ISO,L1_LUCID_UNPAIRED_NONISO,L1_MU0_UNPAIRED_NONISO,L1_EM3_UNPAIRED_NONISO,L1_FJ10_UNPAIRED_NONISO,L1_BCM_AC_CA_UNPAIRED_NONISO'
            elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus():
                background_seeds = 'L1_BCM_AC_CA_BGRP0,L1_BCM_AC_CA_UNPAIRED_ISO,L1_BCM_Wide_EMPTY,L1_BCM_Wide_UNPAIRED_ISO,L1_BCM_Wide_UNPAIRED_NONISO,L1_EM3_UNPAIRED_ISO,L1_FJ30_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J10_UNPAIRED_NONISO,L1_LUCID_A_C_EMPTY,L1_LUCID_A_C_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_NONISO,L1_LUCID_EMPTY,L1_LUCID_UNPAIRED_ISO,L1_LUCID_COMM_UNPAIRED_ISO,L1_LUCID_COMM_EMPTY,L1_J10'                
            else:
                background_seeds = 'L1_EM2_UNPAIRED_ISO,L1_J5_UNPAIRED_ISO,L1_TAU5_UNPAIRED_ISO,L1_FJ10_UNPAIRED_ISO,L1_MU0_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_ISO,L1_BCM_AC_CA_BGRP0'


            background_unpaired_seeds = 'L1_RD0_UNPAIRED_ISO'
            background_unpaired_noniso_seeds = 'L1_RD0_UNPAIRED_NONISO'
            background_empty_seeds    = 'L1_RD0_EMPTY'
            
            
            expresshi_seeds  = ",".join([ x for x in TriggerFlags.Lvl1.items() if "_EMPTY" not in x \
                                         and "_FIRSTEMPTY" not in x and "_UNPAIRED" not in x \
                                         and "BGRP" not in x and "CALREQ" not in x \
                                         and "L1A_Mon" not in x and "L1_RD" not in x \
                                          and "L1_MBTS" not in x])
            
            # Collect L1 muon items with TGC OR RPC trigger types
            tgc_type    = TT_tgc    | TT_phys
            rpcout_type = TT_rpcout | TT_phys
            rpcin_type  = TT_rpcin  | TT_phys             

            # make list of L1 items
            muon_cosmic_list  = [ x for x in self.tpc.Lvl1ItemByTriggerType(tgc_type,    tgc_type)    if "FIRSTEMPTY" not in x and 'EMPTY' in x]
            muon_cosmic_list += [ x for x in self.tpc.Lvl1ItemByTriggerType(rpcout_type, rpcout_type)]

            # remove duplicate entries and make string of comma separated items
            muon_cosmic_list = list(set(muon_cosmic_list))
            muon_cosmic      = ','.join(muon_cosmic_list)

            muon_seeds_list  = [ x for x in self.tpc.Lvl1ItemByTriggerType(tgc_type,   tgc_type)    if 'EMPTY' not in x or "FIRSTEMPTY" in x]
            muon_seeds_list += [ x for x in self.tpc.Lvl1ItemByTriggerType(rpcin_type, rpcin_type)  if 'EMPTY' not in x or "FIRSTEMPTY" in x]

            # remove duplicate entries and make string of comma separated items            
            muon_seeds_list = list(set(muon_seeds_list))
            muon_seeds      = ','.join(muon_seeds_list)

            mlog = logging.getLogger( 'StreamingDef.py:SeededStreamerMaker' )
            mlog.info('Muon seeds: '+str(muon_seeds))
            mlog.info('Cosmic seeds: '+str(muon_cosmic))
            
            #note seeds of WarmStart depend on a menu whether FJ3 or FJ5 is present
            from TriggerJobOpts.TriggerFlags  import TriggerFlags
            menu_name=TriggerFlags.triggerMenuSetup()
            
            if menu_name.find('InitialBeam_v2') > -1 or  menu_name.find('Cosmic_v2') > -1  \
                   or menu_name.find('InitialBeam_v1') > -1 or  menu_name.find('Cosmic2009') > -1:
                
                WarmStart_seeds='L1_RD0_FILLED,L1_MU0_EMPTY,L1_RD0_EMPTY,L1_EM3,L1_J5,L1_FJ3'
            elif menu_name.find('Physics_pp_v1') > -1  \
                  or menu_name.find('Physics_pp_v2') > -1  \
                  or menu_name.find('Physics_pp_v3') > -1  \
                  or menu_name.find('Physics_pp_v4') > -1  \
                  or menu_name.find('MC_pp_v1') > -1 \
                  or menu_name.find('MC_pp_v2') > -1 \
                  or menu_name.find('MC_pp_v3') > -1 \
                  or menu_name.find('MC_pp_v4') > -1 \
                  or menu_name.find('Physics_default') > -1 \
                  or menu_name.find('MC_loose_default') > -1 \
                  or menu_name.find('MC_tight_default') > -1 \
                  :
                WarmStart_seeds='L1_RD0_FILLED,L1_MU0_EMPTY,L1_RD0_EMPTY,L1_EM3,L1_J5,L1_FJ10'
            else:
                WarmStart_seeds='L1_RD0_FILLED,L1_MU0_EMPTY,L1_RD0_EMPTY,L1_EM3,L1_J5,L1_FJ5'
            
            SeededStreamerMaker._chains = [
                L2EFChain_SeededStreamer("SeededStreamerRNDM", 4011, rand_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),
                
                L2EFChain_SeededStreamer("SeededStreamerBPTX", 4012, bptx_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),
                
                L2EFChain_SeededStreamer("SeededStreamerL1Calo", 4013, calo_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerL1CaloEM", 4014, em_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerEMtoL1Calo",4026,em_seeds,'',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                
                L2EFChain_SeededStreamer("SeededStreamerL1CaloTau",4015,tau_seeds,'',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerL1CaloJet",4016,jet_seeds,'',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerL1CaloMET",4017,met_seeds,'',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerL1CaloXS",4017,met_xs_seeds,'',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),
                
                L2EFChain_SeededStreamer("SeededStreamerL1CaloEMFilter_V2",4018,','.join(self.tpc.Lvl1ItemByTriggerType(0x94,0x94)),'',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),
                
                L2EFChain_SeededStreamer("SeededStreamerMinBias",4019, minb_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerMBTS_ZDC", 4032, mbts_zdc_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerMBTS", 4020, mbts_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),
                
                L2EFChain_SeededStreamer("SeededStreamerBCM_LUCID_ZDC", 4033, non_mbts_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerBCM", 4021, bcm_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerLUCID", 4031, lucid_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerCosmicMuons", 4023, muon_cosmic, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerMuonswBeam", 4024, muon_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerWarmStart",4025,WarmStart_seeds,'',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerCosmicCalo", 4027, calo_cosmic, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),
                
                L2EFChain_SeededStreamer("SeededStreamerCosmicCaloEM", 4029, em_cosmic, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),
                
                L2EFChain_SeededStreamer("SeededStreamerCosmicMinBias", 4028, minb_cosmic, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("SeededStreamerZeroBias",4030, zerob_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("L1Muon_NoAlg", 4034, muon_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),
                
                L2EFChain_SeededStreamer("L1Calo_NoAlg", 4035, calo_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer("L1MinBias_NoAlg",4036, minb_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer('Standby', 2500, standby_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer('SeededStreamerExpressPhysicsHI', 2501, expresshi_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())), 

		L2EFChain_SeededStreamer('SeededStreamerExpressStandbyHI', 2503, expresshi_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer('Background', 2502, background_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer('Background_RD0_UNPAIRED_ISO', 779, background_unpaired_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer('Background_RD0_UNPAIRED_NONISO', 699, background_unpaired_noniso_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                L2EFChain_SeededStreamer('Background_RD0_EMPTY', 780, background_empty_seeds, '',
                                         L2EFChain_SeededStreamer.Config(L2EFChain_SeededStreamer.L2Config(),
                                                                         L2EFChain_SeededStreamer.EFConfig())),

                ]


#SeededStreamingChains = SeededStreamerMaker._streams
#print 'WTF SeededStreaming chains are in StreamingDef', SeededStreamingChains
# Please do not add the streaming chains to SeededStreamerMaker._chains unless the chains are defined inside SeededStreamerMaker class.

from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter

l2_ALFASubDetListWriter = TrigSubDetListWriter("ALFASubDetListWriter")
l2_ALFASubDetListWriter.SubdetId = ['TDAQ_CTP','FORWARD_ALPHA']
l2_ALFASubDetListWriter.MaxRoIsPerEvent=1

class L2EFChain_ALFACalibration(L2EFChainDef):
    
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 inputTEs, config):
        
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              inputTEs, config)
        
        self.robWriter = []
        
    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix
            
    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix
            
    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
                                
    def defineStreamGroupTriggerType(self):
        self.physics_streams = []
        if 'ALFA' in self.sig_id:
            self.calib_streams = ['ALFACalib']
            self.groups = [ 'RATE:ALFACalibration','BW:Detector']

    def defineSequences(self, config):
        if ('ALFA') in self.sig_id:
            self.robWriter = [l2_ALFASubDetListWriter]
        else :
            mlog = logging.getLogger( 'CosmicDef.py:' )
            mlog.error("Can not configure"+self.sig_id+" unknown input")
            exit
            
        self.addL2Sequence(self.inputTEs,
                               self.robWriter,
                               'L2_')
            
    def defineSignatures(self):
        self.addL2Signature(['L2_'])
                
                
    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_':     'L2_Calib_' + self.sig_id,
            }
        pass
                
Config   = L2EFChain_ALFACalibration.Config
L2create = L2EFChain_ALFACalibration.L2Config
EFcreate = L2EFChain_ALFACalibration.EFConfig
                                                                                                    

# if "alfa_v1" in TriggerFlags.triggerMenuSetup():
#     alfa_seed = 'L1_ALFA_EMPTY,L1_ALFA_BGRP7,L1_ALFA_ELAST1,L1_ALFA_ELAST2,L1_ALFA_ELAST3,L1_ALFA_ELAST4,L1_ALFA_ELAST5,L1_ALFA_ELAST6,L1_ALFA_ELAST7,L1_ALFA_ELAST8,L1_ALFA_ELAST9,L1_ALFA_ELAST10,L1_ALFA_ELAST11,L1_ALFA_ELAST12,L1_ALFA_ELAST13,L1_ALFA_ELAST14,L1_ALFA_ELAST15,L1_ALFA_ELAST16,L1_ALFA_ELAST17,L1_ALFA_ELAST18,L1_ALFA_SDIFF1,L1_ALFA_SDIFF2,L1_ALFA_SDIFF3,L1_ALFA_SDIFF4,L1_ALFA_SDIFF5,L1_ALFA_SDIFF6,L1_ALFA_SDIFF7,L1_ALFA_SDIFF8,L1_ALFA_SHOW1,L1_ALFA_SHOW2,L1_ALFA_SHOW3,L1_ALFA_SHOW4,L1_ALFA_SHOW5,L1_ALFA_SHOW6,L1_ALFA_SYST1,L1_ALFA_SYST2,L1_ALFA_SYST3,L1_ALFA_SYST4,L1_ALFA_SHOWSYST1,L1_ALFA_SHOWSYST2,L1_ALFA_SHOWSYST3,L1_ALFA_SHOWSYST4,L1_ALFA_SHOWSYST5,L1_ALFA_SYST5,L1_ALFA_SYST6,L1_ALFA_SYST7,L1_ALFA_SYST8,L1_ALFA_SYST9,L1_ALFA_SYST10,L1_ALFA_SYST11,L1_ALFA_SYST12,L1_ALFA_SYST13,L1_ALFA_SYST14,L1_ALFA_SYST15,L1_ALFA_SYST16,L1_ALFA_SYST17,L1_ALFA_SYST18,L1_ALFA_SYST19,L1_ALFA_SYST20,L1_ALFA_ANY,L1_ALFA_ANY_EMPTY,L1_ALFA_ANY_FIRSTEMPTY,L1_ALFA_ANY_UNPAIRED_ISO,L1_ALFA_ANY_UNPAIRED_NONISO'
# else:
#     alfa_seed = 'L1_ALFA_EMPTY,L1_ALFA_BGRP7'

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
    alfa_seed = 'L1_ALFA_EMPTY,L1_ALFA_BGRP7,L1_ALFA_BGRP7_OD,L1_ALFA_EMPTY_OD'
elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus():
    alfa_seed = 'L1_ALFA_EMPTY,L1_ALFA_BGRP7,L1_ALFA_ELAST1,L1_ALFA_ELAST2,L1_ALFA_ELAST11,L1_ALFA_ELAST12,L1_ALFA_ELAST13,L1_ALFA_ELAST14,L1_ALFA_ELAST15,L1_ALFA_ELAST16,L1_ALFA_ELAST17,L1_ALFA_ELAST18,L1_ALFA_SDIFF1,L1_ALFA_SDIFF2,L1_ALFA_SDIFF3,L1_ALFA_SDIFF4,L1_ALFA_SDIFF5,L1_ALFA_SDIFF6,L1_ALFA_SDIFF7,L1_ALFA_SDIFF8,L1_ALFA_SHOW1,L1_ALFA_SHOW2,L1_ALFA_SHOW3,L1_ALFA_SHOW4,L1_ALFA_SYST1,L1_ALFA_SYST2,L1_ALFA_SYST3,L1_ALFA_SYST4,L1_ALFA_SHOWSYST1,L1_ALFA_SHOWSYST2,L1_ALFA_SHOWSYST3,L1_ALFA_SHOWSYST4,L1_ALFA_SHOWSYST5,L1_ALFA_SYST9,L1_ALFA_SYST10,L1_ALFA_SYST11,L1_ALFA_SYST12,L1_ALFA_SYST17,L1_ALFA_SYST18,L1_ALFA_ANY,L1_ALFA_ANY_EMPTY,L1_ALFA_ANY_FIRSTEMPTY,L1_ALFA_ANY_UNPAIRED_ISO,L1_ALFA_ANY_UNPAIRED_NONISO,L1_ALFA_B7L1U_OD,L1_ALFA_B7L1L_OD,L1_ALFA_A7L1U_OD,L1_ALFA_A7L1L_OD,L1_ALFA_A7R1U_OD,L1_ALFA_A7R1L_OD,L1_ALFA_B7R1U_OD,L1_ALFA_B7R1L_OD,L1_ALFA_B7L1_OD,L1_ALFA_A7L1_OD,L1_ALFA_A7R1_OD,L1_ALFA_B7R1_OD,L1_ALFA_EMPTY_OD,L1_ALFA_BGRP7_OD,L1_BGRP1_ALFA_BGT,L1_BGRP4_ALFA_BGT,L1_BGRP7_ALFA_BGT,L1_ALFA_B7L1U,L1_ALFA_B7L1L,L1_ALFA_A7L1U,L1_ALFA_A7L1L,L1_ALFA_A7R1U,L1_ALFA_A7R1L,L1_ALFA_B7R1U,L1_ALFA_B7R1L,L1_ALFA_B1_EMPTY,L1_ALFA_B2_EMPTY,L1_MBTS_2_A_ALFA_C,L1_MBTS_2_C_ALFA_A,L1_MBTS_2_ALFA,L1_LUCID_A_ALFA_C,L1_LUCID_C_ALFA_A,L1_LUCID_ALFA,L1_ZDC_A_ALFA_C,L1_ZDC_C_ALFA_A,L1_ZDC_ALFA'
else:
    alfa_seed = 'L1_ALFA_EMPTY,L1_ALFA_BGRP7'

ExseededStreamingChains = [
    L2EFChain_ALFACalibration("ALFA_Calib",
                              "L2_ALFA_Calib", 5112,  alfa_seed, '', None,[''],
                              Config(L2create(None),
                                     EFcreate(None))),
    ]

            


