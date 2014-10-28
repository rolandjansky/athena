# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Combined trigger slice signatures  """

__author__  = 'B.Demirkoz, O.Igonkina'
__version__="$Revision: 1.86 $"
__doc__="Implementation of Combined  Slice signatures "


from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.Lvl1Flags  import Lvl1Flags

from TriggerMenu.menu.HLTObjects import HLTChain,HLTSequence

from string import atoi
from AthenaCommon.Logging import logging        
from TriggerMenu.menu.TriggerPythonConfig import *
#from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import *

from TriggerMenu.menu.ChainTemplate import *

fexes = FreeStore()
hypos = FreeStore()

from TrigJetHypo.TrigEFDPhiMetJetAllTEConfig import *
from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConfig import L2HistoPrmVtx_SiTrack
from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConfig import EFHistoPrmVtx_Jet
from InDetTrigRecExample.EFInDetConfig import *

fexes.l2_HistoPrmVtx_SiTrack = L2HistoPrmVtx_SiTrack()
fexes.ef_HistoPrmVtx_Jet = EFHistoPrmVtx_Jet()

mlog = logging.getLogger( 'Combined.py' )  ##get the logger



class CombinedChain2:
    def __init__(self, sig_id, chain_counter, streams, groupL2, groupEF,
                 l2_chain_name, l2_ingredients, l1_item, l2_prescale, l2_pass_through,
                 ef_chain_name, ef_ingredients, ef_prescale=1, ef_pass_through=0,
                 removeOverlap=False, dPhiMetJet=False, dPhiCut=None, doOrderL2=False, doOrderEF=False,doBjetMuonJet=False):
        self.sig_id = sig_id
        self.chain_counter = str(chain_counter)
        self.streams = streams
        self.groupL2 = groupL2
        self.groupEF = groupEF
        self.l2_chain_name = l2_chain_name
        self.l2_ingredients = l2_ingredients
        self.l1_item = l1_item
        self.l2_prescale = l2_prescale
        self.l2_pass_through = l2_pass_through
        self.ef_chain_name = ef_chain_name
        self.ef_ingredients = ef_ingredients
        self.ef_prescale = ef_prescale
        self.ef_pass_through = ef_pass_through
        self.removeOverlap = removeOverlap
        self.dPhiMetJet = dPhiMetJet
        self.dPhiCut = dPhiCut
        self.doOrderL2 = doOrderL2
        self.doOrderEF = doOrderEF
        self.doBjetMuonJet = doBjetMuonJet
         

    def generateMenu(self, triggerPythonConfig):
        mlog.debug("generating config for: "+self.sig_id)
 
        # find chains objects given names of EF
        efparts = [ triggerPythonConfig.getHLTChain(i) \
                    for i in self.ef_ingredients ]
        if None in  efparts:
            mlog.warning("Basic chains used to built combined chain: " 
                         + self.sig_id+" are not present.")
            mlog.warning("These were found:  "
                         + str([c.chain_name for c in  efparts if c != None]))
            mlog.warning("These were needed: "
                         + str(self.ef_ingredients))
            return 

        # find chains objects given names of L2
        if len(self.l2_ingredients) is 0:  ## infer LVL2 parts from EF if self.l2_ingredients is empty
            mlog.debug( "Infering LVL2 parts from EF parts")
            l2parts = [ triggerPythonConfig.getHLTChain(i.lower_chain_name)  for i in efparts ]
        else:
            # take the name of L2 part from EF part
            l2parts = [ triggerPythonConfig.getHLTChain(i) \
                        for i in self.l2_ingredients ]            
        if None in l2parts:
            mlog.warning("Basic chains used to built combined chain: "
                         + self.sig_id+" are not present.")
            mlog.warning("These were found:  "
                         + str([c.chain_name for c in l2parts if c!=None]))
            mlog.warning("These were needed: "
                         + str(self.l2_ingredients))
            return


        l2chain = triggerPythonConfig.getHLTChain(self.l2_chain_name)
        # print 'L2 chain for sig %s is ' % self.sig_id, l2chain
        l2chain_exists = True
        if l2chain == None:
            l2chain = HLTChain(self.l2_chain_name, self.chain_counter,
                               self.l1_item, "L2",
                               str(self.l2_prescale), str(self.l2_pass_through))
            l2chain_exists = False
            for eachstream in self.streams:
                l2chain.addStreamTag(eachstream)
            l2chain.addGroup(self.groupL2)

        if TriggerFlags.doLVL2() and not l2chain_exists:
            if ( self.removeOverlap ) :
                l2_init_tes = []
                [ l2_init_tes.extend(c.siglist[0].tes) for c in l2parts ]
                mlog.debug('creating L2 chain for sig: ', self.sig_id)
                #print l2_init_tes
                inputs = []
                for tes in l2_init_tes:
                    for s in triggerPythonConfig.theSeqLists:
                        if( s.output ==tes ):
                            inputs.extend(s.input)
                # print inputs
                TE = triggerPythonConfig.addSequence(inputs,OverlapRemoval_afterL1_TauJet,"L2"+self.sig_id+"nooverlap")
                l2chain.addHLTSignature(TE)
                
            if self.doOrderL2 == False:    
                l2chain.mergeAndAppendChains(l2parts)
            elif self.doOrderL2 == True:
                l2chain.mergeAndAppendChains_Order(l2parts)
                
            if( self.doBjetMuonJet ):
                TE=triggerPythonConfig.addSequence("J10",[TrigSiTrack_Jet(),
                                                          fexes.l2_HistoPrmVtx_SiTrack,fexes.l2_bjet, 
                                                          hypos.l2_BjetHypo_NoCut],"L2_Bjet_NoCut")
                l2chain.addHLTSignature(TE)

       
        # identical for EF as for L2
        efchain = HLTChain(self.ef_chain_name, self.chain_counter,
                           self.l2_chain_name, "EF", str(self.ef_prescale),
                           str(self.ef_pass_through))
        if TriggerFlags.doEF():
            for eachstream in self.streams:
                efchain.addStreamTag(eachstream)

            efchain.addGroup(self.groupEF)

            #difference to CC: no option for ordering
            if self.doOrderEF == False:
                efchain.mergeAndAppendChains(efparts)
            elif self.doOrderEF == True:
                efchain.mergeAndAppendChains_Order(efparts)
   

            ## Removes overlap between combined signature
            if( self.removeOverlap ) : #same as CombinedChain
                if 0 not in [ len(c.siglist) for c in efparts ]:
                    TES = []
                    [ TES.extend(c.siglist[-1].tes) for c in efparts ]
                    mlog.debug("Using this TES as input to overlap-removal sequence: "+str(TES))
                
                    TE=triggerPythonConfig.addSequence(TES,OverlapRemoval_afterEF_TauJet,"EF"+self.sig_id+"nooverlap")
                    efchain.addHLTSignature(TE)

            ###
            #addition to CombinedChain
            ###
            if( self.dPhiMetJet ) :
                if 0 not in [ len(c.siglist) for c in efparts ]:
                    TES = []
                    [ TES.extend(c.siglist[-1].tes) for c in efparts ]
                    mlog.debug("Using this TES as input to dPhiMetJet sequence: "+str(TES))
                    #print "EF printout: "
                    #print TES

                    if (self.dPhiCut == None):
                        TE=triggerPythonConfig.addSequence(TES,ef_DPhiMetJet,"EF_"+self.sig_id+"dPhiMetJet")
                        efchain.addHLTSignature(TE)
                    elif (self.dPhiCut == "dPhi_05_2j30"):
                        TE=triggerPythonConfig.addSequence(TES,ef_DPhiMetJet_05_2j30,"EF_"+self.sig_id+"dPhiMetJet")
                        efchain.addHLTSignature(TE)

            if( self.doBjetMuonJet ):
                TE=triggerPythonConfig.addSequence("L2_Bjet_NoCut",fexes.ef_bjet_tracks + 
                                                   [fexes.ef_HistoPrmVtx_Jet,fexes.ef_bjet, 
                                                    hypos.ef_BjetHypo_NoCut],"EF_Bjet_NoCut")
                efchain.addHLTSignature(TE)
            ###
            ###
                                                             
        # register chains to be eventually put into the output XML
        triggerPythonConfig.registerHLTChain(self.sig_id, l2chain) 
        triggerPythonConfig.registerHLTChain(self.sig_id, efchain)
        pass

 

ElectronJET_tchad = [    
    
    CombinedChain2('e24vhi_medium1_mu6',  '125', ['combined','comb__emu'], [], [],
                   'L2_e24vhi_medium1_mu6', ['L2_mu6', 'L2_e24vhi_medium1'], 'L1_EM10VH_MU6', 1, 0,
                   'EF_e24vhi_medium1_mu6', ['EF_mu6', 'EF_e24vhi_medium1'], 1, 0, doOrderL2=False, doOrderEF=False),

    #CombinedChain2('e24vh_loose0_3j45_a4tchad_L2FS', '704', ['combined','comb__ej'], [], [], 
    #               'L2_e24vh_loose0_3j15_a4TTem',       ['L2_roi_dummy_3J15', 'L2_e24vh_loose0', 'L2_3j15_a4TTem'], 'L1_EM18VH',1, 0, 
    #               'EF_e24vh_loose0_3j45_a4tchad_L2FS', ['EF_e24vh_loose0', 'EF_3j45_a4tchad_L2FS'], 1, 0, doOrderL2=True, doOrderEF=True),
    ]

MuonJets = [
    #CombinedChain2('mu24_tight_2j35_a4tchad', '796', ['combined','comb__muj'], [], [],
    #               'L2_mu24_tight_2j35_a4tchad',    ['L2_mu24_tight'], 'L1_MU15', 1, 0,
    #               'EF_mu24_tight_2j35_a4tchad',    ['EF_mu24_tight', 'EF_2j35_a4tchad'], 1, 0),
    ]


for item in ElectronJET_tchad :
    if issubclass(item.__class__, CombinedChain2):
        item.groupL2 += [ "RATE:ElectronJet",'BW:Egamma']
        item.groupEF += [ "RATE:ElectronJet",'BW:Egamma']
    else:
        item.group += [ "RATE:ElectronJet",'BW:Egamma']

for item in MuonJets :
    if issubclass(item.__class__, CombinedChain2):
        item.groupL2 += [ "RATE:MuonJet",'BW:Muon']
        item.groupEF += [ "RATE:MuonJet",'BW:Muon']
    else:
        item.group += [ "RATE:MuonJet",'BW:Muon']





    
################################################################################################################
from TrigEgammaMuonCombHypo.TrigL2ElectronMuonAngleHypoConfig import *
from TrigEgammaMuonCombHypo.TrigEFElectronMuonAngleHypoConfig import *
fexes.l2_emuAngleFex = TrigL2ElectronMuonAngleFex()
fexes.ef_emuAngleFex = TrigEFElectronMuonAngleFex()
hypos.l2_e24vhi_medium1_mu6_topo_medium  = TrigL2ElectronMuonAngleHypo_medium()

class ElectronMuonTopoAngle(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig, l2merge=None, efmerge=None, l2_hypo=None, ef_hypo=None):
        ChainTemplate.__init__(self, signature, l2=l2, ef=ef)

        self.addGroups('RATE:MultiElectron').addGroups('BW:Egamma')
        self.addStreams('egamma')

        self.seq_generated = False
        self.l2merge = l2merge
        self.efmerge = efmerge
        self.l2_hypo = l2_hypo
        self.ef_hypo = ef_hypo
        if l2_hypo == None and hasattr(hypos, 'l2_%s' % signature):
            self.l2_hypo = getattr(hypos, 'l2_%s' % signature)
        if ef_hypo == None and hasattr(hypos, 'ef_%s' % signature):
            self.ef_hypo = getattr(hypos, 'ef_%s' % signature)
            
        try:
            # Single electron chains are still configured in the old way.
            # So HLTChain objects may not be created yet. If so, try again
            # in generateMenu()
            self.mergeThem()
            self.seq_generated = True
        except AttributeError:
            # Do it later in generateMenu()
            self.seq_generated = False

    def mergeThem(self):
        self.merge(efnames=self.efmerge, l2names=self.l2merge, onlyEF=True)
        self.addSequence(self.l2merge, [fexes.l2_emuAngleFex, self.l2_hypo], \
                         self.l2.chain_name).addSignature('L2')
        self.addSequence(self.efmerge, [fexes.ef_emuAngleFex, self.ef_hypo], \
                         self.ef.chain_name).addSignature('EF')
    def generateMenu(self, tpc):
        if not self.seq_generated:
            try:
                self.mergeThem()
                self.seq_generated = True
            except AttributeError:
                print 'something went wrong in combined chains'
                
    pass



ElectronMuons_eIdScan = [
    ElectronMuonTopoAngle('e24vhi_medium1_mu6_topo_medium',
                          l2=Chain(name='L2_e24vhi_medium1_mu6_topo_medium', counter=123, seed='L1_EM10VH_MU6'), 
                          efmerge=[ 'EF_mu6', 'EF_e24vhi_medium1'], l2merge=['L2_mu6','L2_e24vhi_medium1']),

    #ElectronMuonTopoAngle('e12Tvh_medium_mu6_topo_medium',
    #                      l2=Chain(name='L2_e12Tvh_medium_mu6_topo_medium', counter=123, seed='L1_EM10VH_MU6'), 
    #                      efmerge=[ 'EF_mu6', 'EF_e12Tvh_medium'], l2merge=['L2_mu6','L2_e12Tvh_medium']),
    
    ]
