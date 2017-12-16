#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.dom.minidom

HypoTypes = (
    'TrigBjetHypo', 
    'TrigLeptonJetHypo', 
    'TrigL2DiMuXHypo', 
    'TrigL2BMuMuXHypo', 
    'TrigL2BMuMuHypo', 
    'TrigEFBMuMuXHypo', 
    'TrigEFBMuMuHypo', 
    'TrigL2DiMuHypo', 
    'TrigEFDiMuHypo', 
    'TrigL2DsPhiPiHypo', 
    'TrigEFDsPhiPiHypo', 
    'TrigL2JpsieeHypo', 
    'TrigEFJpsieeHypo', 
    'TrigL2BgammaXHypo', 
    'TrigEFBgammaXHypo', 
    'TrigL2CaloHypo', 
    'TrigL2IDCaloHypo', 
    # 'TrigL2IDCaloHypoMoni', 
    'TrigL2PhotonHypo', 
    'TrigL2DielectronMassHypo', 
    'TrigEFCaloHypo', 
    'TrigEFEgammaHypo', 
    'TrigEFPhotonHypo', 
    'TrigEFTrackHypo', 
    'TrigEFJetHypo', 
    'TrigL2JetHypo', 
    'CosmicMuonHypo', 
    'CosmicTrtHypo', 
    'CombinedIDMuonHypo', 
    'CombinedIDHalfTracksHypo', 
    'TrigEFMissingETHypo', 
    'TrigL2MissingETHypo', 
    'TrigRingerNeuralHypo', 
    'MufastHypo', 
    'MucombHypo', 
    'MuisoHypo', 
    'TrigMooreHypo', 
    'TrigMuonEFSegmentFinderHypo', 
    'TrigMuonEFTrackBuilderHypo', 
    'TrigMuonEFExtrapolatorHypo', 
    'TrigMuonEFCombinerHypo', 
    'TrigL2DimuonMassHypo', 
    'StauHypo', 
    'TileMuHypo', 
    'T2CaloTauHypo', 
    'T2IDTauHypo', 
    'T2TauHypo', 
    'EFIDTauHypo', 
    'EFTauHypo', 
    )

class ChainSigList:
    TheMap = {
        'EF_e6_medium1'     : 'e',
        'EF_g25'            : 'g',
        'EF_tau12_loose'    : 'tau',
        'EF_mu15i_loose'    : 'mui', 
        'EF_te650'          : 'xe',
        'EF_J350'           : 'j',
        'EF_b23'            : 'b',
        #
        'EF_e5_e10_medium'  : 'e_e',
        'EF_e20_g20'        : 'e_g',
        'EF_3e15_medium'    : '3e',
        'EF_3g10'           : '3g',
        'EF_mu4_mu6'        : 'mu_mu',
        'EF_3mu6'           : '3mu',
        'EF_3J180'          : '3j',
        'EF_4J95'           : '4j',
        'EF_2FJ70'          : '2fj',
        'EF_3b18_4L1J18'    : '3b_4L1J',
        #
        'EF_2e10_mu6'       : '2e_mu',
        'EF_e10_mu6'        : 'e_mu',
        'EF_2mu6_e10'       : '2mu_e',
        'EF_tau16i_2j70'    : 'taui_2j',
        'EF_tau16i_3j23'    : 'taui_3j',
        'EF_j70_xe30'       : 'j_xe',
        'EF_tau12_xe20'     : 'tau_xe',
        'EF_tau20i_b18'     : 'taui_b',
        'EF_2g10_mu6'       : '2g_mu',
        'EF_mu20_xe30'      : 'mu_xe',
        'EF_MU4_Jpsie5e3_FS': 'MU4_Jpsiee_FS',
        'EF_2MU4_Upsimumu'  : '2MU_Upsimumu',
        'EF_j42_xe30_mu15'  : 'j_xe_mu',
        'EF_j42_xe30_e15i'  : 'j_xe_ei',
        'EF_4j23_e15i'      : '4j_ei',
        'EF_4j23_mu15'      : '4j_mu',
        'EF_tau16i_4j23_WO' : 'taui_4j_WO',
        'EF_e20_xe15'       : 'e_xe',
        'EF_mu4_j10'        : 'mu_j',
        'EF_g25_xe30'       : 'g_xe',
        'EF_tau12_b23_j42'  : 'tau_b_j',
        'EF_2g17i_L33'      : '2gi',
        'EF_tau16i_j70_WO'  : 'taui_j_WO',
        'EF_2j42_xe30'      : '2j_xe',
        'EF_tau12_tau29i_PT': 'tau_taui_PT',
        'EF_MU4_Upsimumu_FS': 'MU_Upsimumu_FS',
        'EF_mu4_DsPhiPi_FS' : 'mu4_DsPhiPi_FS',
        'EF_2b23_3L1J23_passHLT': '2b_3L1J_passHLT',
        'EF_tau20i_e10'     : 'taui_e',
        'EF_tau16i_mu10'    : 'taui_mu',
        'EF_tau20i_2b23'    : 'taui_2b',
        'EF_MbTrk'          : 'MbTrk',
        'EF_2mu6_g10'       : '2mu_g',
    }
    def findSigName(chain_type):
        used = gChainList.ChainType2Name[chain_type]
        for a in used:
            for (k, v) in ChainSigList.TheMap.iteritems():
                if k == a: return v
        return ''
    findSigName = staticmethod(findSigName)

class SeqList:
    def __init__(self):
        self.TheList = [] # SequenceDef
    def findSequence(self, outputTE):
        tmp = filter(lambda x: x.outputTE==outputTE, self.TheList)
        if len(tmp)>0:
            return tmp[0]
        return None
    def addSequence(self, seq):
        s = self.findSequence(seq.outputTE)
        if s:
            #print 'Sequence with outputTE %s already exist' % seq.outputTE
            pass
        else:
            self.TheList.append(seq)
gSeqList = SeqList()

class ChainList:
    def __init__(self):
        self.L2List = [] # ChainDef
        self.EFList = [] # ChainDef 
        self.ChainType2Level = {} # ChainType: 'L2'|'EF'
        self.ChainType2Name = {} # ChainType: [chain names]
    def findChain(self, chain_name):
        for c in self.L2List:
            if c.chain_name == chain_name:
                return c
        for c in self.EFList:
            if c.chain_name == chain_name:
                return c
        return None
    def findL2ChainSeedingEF(self, efchain):
        for c in self.L2List:
            if c.chain_name == efchain.lower_chain_name:
                return c
        return None
    def addChain(self, level, chain):
        ctype = chain.chainType()
        if level=='L2':
            self.L2List.append(chain)
        elif level=='EF':
            self.EFList.append(chain)
        if ctype in self.ChainType2Name.keys():
            self.ChainType2Name[ctype].append(chain.chain_name)
        else:
            self.ChainType2Level[ctype] = level
            self.ChainType2Name[ctype] = [chain.chain_name]
    def dump(self):
        print 'from TriggerMenu.HltConfig import *\n\n'
        l2chain_types, efchain_types = [], []
        for (ctype, level) in self.ChainType2Level.iteritems():
            if level=='L2': l2chain_types.append(ctype)
            elif level=='EF': efchain_types.append(ctype)
        for c3 in efchain_types:
            names = self.ChainType2Name[c3]
            l2chain, efchain = None, None
            for efname in names:
                efchain = gChainList.findChain(efname)
                if efchain.lower_chain_name != '':
                    l2chain = gChainList.findChain(efchain.lower_chain_name)
                if l2chain!=None and efchain!=None: break
            l2efchain = L2EFChainDef(l2chain, efchain)
            c2 = l2chain.chainType()
            print '#----------------------------------------------------'
            print '# L2 ChainType: %s' % c2
            print '# EF ChainType: %s' % c3
            print '# Used in %s' % str(names)
            l2efchain.dumpDef()
            print '#----------------------------------------------------'
        pass
gChainList = ChainList()

class SignatureDef:
    def __init__(self, tes):
        self.TEs = tes
        
class ChainDef:
    def __init__(self, level, name, lower_chain_name, sigs):
        self.chain_name = name
        self.lower_chain_name = lower_chain_name
        self.level = level
        self.signatures = sigs
        pass
    def chainType(self):
        """
        Returns the chain type which is an ordered list of sequence types"""
        t = ''
        for (isig, sig) in enumerate(self.signatures):
            seq_multi = {}
            for te in sig:
                s = gSeqList.findSequence(te)
                if s:
                    seq_type = s.seqType()
                    if seq_type in seq_multi.keys():
                        seq_multi[seq_type] += 1
                    else:
                        seq_multi[seq_type] = 0
            tmp = seq_multi.keys()
            tmp.sort()
            for s in tmp:
                t += '%s(step%d,x%d)' % (s, isig, seq_multi[s])
        return t
    def dump(self):
        print 'Chain: %s' % self.chainType()

class L2EFChainDef:
    def __init__(self, l2chain, efchain):
        self.l2chain = l2chain
        self.efchain = efchain
    def findAllHypos(self, sigs):
        hypo_algos = []
        for sig in self.l2chain.signatures:
            for te in sig:
                seq = gSeqList.findSequence(te)
                if seq:
                    hypos = seq.hypoTypes()
                    if len(hypos)>1:
                        print 'Number of Hypo for TE %s is %d' % \
                              (te, len(hypos))
                    elif len(hypos)==1:
                        hypo_algos.append(hypos[0])
        return hypo_algos
    def allHypoNames(self, hypos):
        hypo_multis = {}
        hypo_names = []
        for a in hypos:
            if a in hypo_multis.keys():
                hypo_multis[a] += 1
            else:
                hypo_multis[a] = 1
            hypo_names.append(a + '_' + str(hypo_multis[a]))
        return hypo_names
    def dumpConfig(self):
        def dumpConfig1(level, hypos):
            ket = '):'
            if len(hypos)>0: ket = ', '
            hypo_names = self.allHypoNames(hypos)
            print """
    class %sConfig:
        def __init__(self%s""" % (level, ket)
            for (i, a) in enumerate(hypo_names):
                if i!=len(hypos)-1:
                    print '                     %s, ' % a
                else:
                    print '                     %s):' % a
            for (i, a) in enumerate(hypo_names):
                print '            self.%s = %s' % (a, a)
            print '            pass'
        l2hypos = self.findAllHypos(self.l2chain.signatures)
        efhypos = self.findAllHypos(self.efchain.signatures)
        dumpConfig1('L2', l2hypos)
        dumpConfig1('EF', efhypos)
        print """
    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
"""
        pass
    def dumpSeqDef(self):
        print """
    def defineSequences(self):"""
        tes = []
        for sig in self.l2chain.signatures+self.efchain.signatures:
            for te in sig:
                seq = gSeqList.findSequence(te)
                if te in tes: continue
                else:
                    print '        self.addSequence(%s, %s, %s)' % \
                    (str(map(lambda x: str(x), seq.inputTEs)),
                    str(seq.algoInstanceList()),
                    "'"+seq.outputTE+"'")
                    tes.append(te)
        pass
    def dumpSigDef(self):
        print """
    def defineSignatures(self):"""
        for sig in self.l2chain.signatures:
            print '        self.addL2Signature(%s)' % \
            str(map(lambda x: str(x), sig))
        for sig in self.efchain.signatures:
            print '        self.addEFSignature(%s)' % \
            str(map(lambda x: str(x), sig))
        pass
    def dumpDef(self):
##         used = gChainList.ChainType2Name[self.efchain.chainType()]
##         if len(used)>0: print "USED        '%s': '%s', " % \
##            (used[0], used[0].replace('EF_', ''))
        chain_sig = ChainSigList.findSigName(self.efchain.chainType())
        print 'class L2EFChain_%s(L2EFChainDef): ' % chain_sig
        self.dumpConfig()
        print """
    def __init__(self, sig_id, 
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter, 
                 l2_inputTEs, config):
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter, 
                              l2_inputTEs, config)
"""
        self.dumpSeqDef()
        self.dumpSigDef()
        print """
    def defineStreamGroupTriggerType(self):
        self.physics_streams = []
        self.calib_streams = []
        self.groups = []
        self.trigger_type = []
    def defineTErenaming(self):
        pass
"""
        pass
    
        
class SequenceDef:
    def __init__(self, inputTEs, algos, outputTE):
        self.inputTEs = inputTEs
        self.outputTE = outputTE
        self.algos = algos
    def seqType(self):
        """
        Returns the sequence type which is an ordered list of
        algorithm types"""
        t = ''
        for a in self.algos:
            tmp = a.split('/')
            t += '->%s' % tmp[0]
        return t
    def algoTypes(self):
        algo_types = []
        for a in self.algos:
            tmp = a.split('/')
            algo_types.append(tmp[0])
        return algo_types
    def hypoTypes(self):
        hypo_types = []
        for a in self.algos:
            tmp = a.split('/')
            if tmp[0] in HypoTypes: hypo_types.append(tmp[0])
        return hypo_types
    def algoInstanceList(self):
        l = []
        for a in self.algoTypes():
            b = str(a + '()')
            b = b.replace(':', '_')
            l.append(b)
        return l
    def dump(self):
        print '%s <- %s' % (self.outputTE, str(self.inputTEs))
        for a in self.algos:
            print '  %s' % a
        print '  type=%s' % self.seqType()

class XML:
    def __init__(self, filename):
        self.doc = xml.dom.minidom.parse(filename)
    def getChains(self):
        chains = self.doc.getElementsByTagName('CHAIN')
        for c in chains:
            level = c.getAttribute('level')
            signatures = []
            sigs = c.getElementsByTagName('SIGNATURE')
            for s in sigs:
                tmp = []
                tes = s.getElementsByTagName('TRIGGERELEMENT')
                for te in tes:
                    tmp.append(te.getAttribute('te_name'))
                signatures.append(tmp)
            name = c.getAttribute('chain_name')
            lower_chain_name = c.getAttribute('lower_chain_name')
            chaindef = ChainDef(level, name, lower_chain_name, signatures)
            gChainList.addChain(level, chaindef)
    def getSequences(self):
        seqs = self.doc.getElementsByTagName('SEQUENCE')
        for s in seqs:
            input = s.getAttribute('input').split(' ')
            algos = s.getAttribute('algorithm').split(' ')
            output = s.getAttribute('output')
            ss = SequenceDef(input, algos, output)
            gSeqList.addSequence(ss)

    def dumpSequences(self):
        for s in gSeqList.TheList:
            s.dump()
    def dumpChains(self):
        for c in gChainList.L2List:
            c.dump()
        for c in gChainList.EFList:
            c.dump()
    
if __name__=='__main__':
    hltxml = '/afs/cern.ch/atlas/software/builds/AtlasTrigger/14.2.20/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_lumi1E31_14.2.20.xml'
    m = XML(hltxml)
    m.getSequences()
    m.getChains()
    # m.dumpSequences()
    # m.dumpChains()

    gChainList.dump()
    
