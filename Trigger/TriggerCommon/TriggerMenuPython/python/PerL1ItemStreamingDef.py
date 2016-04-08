# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
#
# SliceDef file for HLT signatures used for streaming
# of each L1 trigger separately W. Fedorko 2010/01/28
#
#########################################################################

from TriggerMenuPython.HltConfig import *
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

from TriggerJobOpts.TriggerFlags import TriggerFlags

class L2EFChain_L1ItemStreamer(L2EFChainDef):
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, inputTEs, config, streams, prescale=1, pass_through=0):
        self.streams=streams
        L2EFChainDef.__init__(self, sig_id,
                              "L2_"+sig_id, chain_counter,
                              l2_lower_chain_name,
                              "EF_"+sig_id, chain_counter,
                              inputTEs, config,
                              prescale, pass_through)
        mlog = logging.getLogger( 'StreamingDef.py:L2EFChain_L1ItemStreamer' )

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

    def defineSignatures(self):
        pass

    def defineStreamGroupTriggerType(self):
        self.groups = ['RATE:L1ItemStreamers']
        for mystream in self.streams:
            self.groups.append('RATE:L1ItemStreamers_'+mystream)
        self.physics_streams=self.streams;
        self.calib_streams = []
        self.trigger_type = []
        pass

class L1ItemStreamerMaker:
    _chains = []
    

    def appitemstr(self,l1item,str):
        if self.itemstrdict.has_key(l1item):
            if str not in self.itemstrdict[l1item]:
                self.itemstrdict[l1item].append(str)
        else:
            self.itemstrdict[l1item]=[str]
    
    def __init__(self,triggerPythonConfig=None):
        self.itemstrdict={}
        self.tpc = triggerPythonConfig
        if not L1ItemStreamerMaker._chains:
            
            
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

            em_type  = TT_calo | TT_phys | TT_caloem
            cl_type  = TT_calo | TT_phys
            mb_type  = TT_minb | TT_phys
            rd_type  = TT_rand | TT_phys
            bx_type  = TT_bptx | TT_phys
            zb_type  = TT_zerobs | TT_phys
            
            # streaming for beam items
            calo_seeds     =[ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if "EMPTY" not in x or "FIRSTEMPTY" in x]
            em_seeds       =[ x for x in self.tpc.Lvl1ItemByTriggerType(em_type, em_type) if "EMPTY" not in x or "FIRSTEMPTY" in x]
            minb_seeds_noncosmic     =[ x for x in self.tpc.Lvl1ItemByTriggerType(mb_type, mb_type) if "EMPTY" not in x or 'FIRSTEMPTY' in x]
            
            # Random and bptx and zerobias items
            rand_seeds     =[ x for x in self.tpc.Lvl1ItemByTriggerType(rd_type, rd_type)]
            bptx_seeds     =[ x for x in self.tpc.Lvl1ItemByTriggerType(bx_type, bx_type)]
            zerob_seeds    =[ x for x in self.tpc.Lvl1ItemByTriggerType(zb_type, zb_type) if "RD1_EMPTY" not in x]

            # Collect L1 items with matching trigger type AND which have empty in names
            minb_cosmic =[ x for x in self.tpc.Lvl1ItemByTriggerType(mb_type, mb_type) if "FIRSTEMPTY" not in x and 'EMPTY' in x]
            calo_cosmic =[ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if "FIRSTEMPTY" not in x and 'EMPTY' in x]
            calo_cosmic.append('L1_RD1_EMPTY')
            em_cosmic = [ x for x in self.tpc.Lvl1ItemByTriggerType(cl_type, cl_type) if "FIRSTEMPTY" not in x and 'EMPTY' in x \
                           and ('L1_EM' in x or 'L1_2EM' in x)]

            # Collect L1 muon items with TGC OR RPC trigger types
            tgc_type    = TT_tgc    | TT_phys
            rpcout_type = TT_rpcout | TT_phys
            rpcin_type  = TT_rpcin  | TT_phys             

            # make list of L1 items
            muon_cosmic = [ x for x in self.tpc.Lvl1ItemByTriggerType(tgc_type,    tgc_type)    if "FIRSTEMPTY" not in x and 'EMPTY' in x]
            
            muon_cosmic += [ x for x in self.tpc.Lvl1ItemByTriggerType(rpcout_type, rpcout_type)]

            # remove duplicate entries and make string of comma separated items
            muon_cosmic = list(set(muon_cosmic))
            mlog = logging.getLogger( 'PerL1ItemStreamingDef.py:L1ItemStreamerMaker' )
            muon_seeds  = [ x for x in self.tpc.Lvl1ItemByTriggerType(tgc_type,   tgc_type)    if 'EMPTY' not in x or "FIRSTEMPTY" in x]
            muon_seeds += [ x for x in self.tpc.Lvl1ItemByTriggerType(rpcin_type, rpcin_type)  if 'EMPTY' not in x or "FIRSTEMPTY" in x]
            # remove duplicate entries and make string of comma separated items            
            muon_seeds = list(set(muon_seeds))
            
            
            for l1item in rand_seeds:
                self.appitemstr(l1item,'RNDM')
            for l1item in zerob_seeds:
                self.appitemstr(l1item,'ZeroBias')
            for l1item in calo_seeds:
                self.appitemstr(l1item,'L1Calo')
            for l1item in em_seeds:
                self.appitemstr(l1item,'L1CaloEM')
            for l1item in muon_seeds:
                self.appitemstr(l1item,'MuonswBeam')
            for l1item in minb_seeds_noncosmic:
                self.appitemstr(l1item,'MinBias')
            for l1item in minb_cosmic:
                self.appitemstr(l1item,'background')
            for l1item in muon_cosmic:
                self.appitemstr(l1item,'CosmicMuons')
            for l1item in calo_cosmic:
                self.appitemstr(l1item,'CosmicCalo')
#            for l1item in em_cosmic:
#                self.appitemstr(l1item,'CosmicCalo')
            
                
            
            

            L1ItemStreamerMaker._chains=[]

            current_counter=4050
            #the counters for the seeded streamers run
            #from 4000 to 4028 so start at 4050

            for l1item in self.itemstrdict.keys():
                if TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus() or \
                       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
                    #streams=self.itemstrdict['L1_MBTS_1_1']
                    streams=['MinBias']
                    if ('_EMPTY') in l1item:
                        streams=self.itemstrdict[l1item]
                else:
                    streams=self.itemstrdict[l1item]
                    
                L1ItemStreamerMaker._chains.append(
                    L2EFChain_L1ItemStreamer('L1ItemStreamer_'+l1item,
                                             current_counter,
                                             l1item,
                                             '',
                                             L2EFChain_L1ItemStreamer.Config(L2EFChain_L1ItemStreamer.L2Config(),
                                                                             L2EFChain_L1ItemStreamer.EFConfig()),
                                             streams))
                current_counter=current_counter+1
                

            
            

            

            
                                             
            
