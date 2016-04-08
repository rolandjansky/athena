# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Combined trigger slice signatures  """

__author__  = 'B.Demirkoz, O.Igonkina'
__version__="$Revision: 1.86 $"
__doc__="Implementation of Combined  Slice signatures "


from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

from string import atoi
from AthenaCommon.Logging import logging        
from TriggerMenuPython.TriggerPythonConfig import *
#from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import *

from TriggerMenuPython.ChainTemplate import *

fexes = FreeStore()
hypos = FreeStore()




from TrigJetHypo.TrigEFDPhiMetJetAllTEConfig import *

ef_DPhiMetJet = TrigEFDPhiMetJetAllTE()
ef_DPhiMetJet_05_2j30 = EFDPhiMetJet_2J30("EFDPhiMetJet_2J30_DPhi05",dPhiCut=0.5)
ef_DPhiMetJet_10_2j30 = EFDPhiMetJet_2J30("EFDPhiMetJet_2J30_DPhi10",dPhiCut=1.0)
ef_DPhiMetJet_10_2j45 = EFDPhiMetJet_2J45("EFDPhiMetJet_2J45_DPhi10",dPhiCut=1.0)
ef_DPhiMetJet_07_2j10 = EFDPhiMetJet_2J10("EFDPhiMetJet_2J10_DPhi07", dPhiCut=0.7)
ef_DPhiMetJet_20_2j10 = EFDPhiMetJet_2J10("EFDPhiMetJet_2J10_DPhi20", dPhiCut=2.0)
ef_DPhiMetJet_20_2j15 = EFDPhiMetJet_2J15("EFDPhiMetJet_2J15_DPhi20", dPhiCut=2.0)
ef_DPhiMetJet_07_100j10 = EFDPhiMetJet_100J10("EFDPhiMetJet_100J10", dPhiCut=0.7)




from TrigGenericAlgs.TrigGenericAlgsConfig import OverlapRemovalConfig
OverlapRemoval_afterL1_TauJet = OverlapRemovalConfig("OverlapRemoval_afterL1_TauJet", MinCentDist=0.2 )
OverlapRemoval_afterEF_TauJet = OverlapRemovalConfig("OverlapRemoval_afterEF_TauJet", MinCentDist=0.2 )

#for the bject mu-jet chain (id 284 below)
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Jet
#

from TrigBjetHypo.TrigBjetFexConfig  import *
from TrigBjetHypo.TrigBjetHypoConfig import *
#from TrigBjetHypo.TrigBjetFexConfig  import L2BjetFex
#from TrigBjetHypo.TrigBjetFexConfig  import EFBjetFex
from TrigBjetHypo.TrigBjetHypoConfig import *
from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConfig import L2HistoPrmVtx_SiTrack
from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConfig import EFHistoPrmVtx_Jet
#
from InDetTrigRecExample.EFInDetConfig import *
fexes.ef_bjet_tracks = TrigEFIDInsideOut_Bjet().getSequence()
#old
#fexes.l2_bjet = L2BjetFex()
#fexes.ef_bjet = EFBjetFex()

if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()):
    fexes.l2_bjet = getBjetFexInstance("L2","2011","SiTrack") 
    fexes.ef_bjet = getBjetFexInstance("EF","2011","EFID")
else:
    fexes.l2_bjet = getBjetFexInstance("L2","2012","SiTrack")
    fexes.ef_bjet = getBjetFexInstance("EF","2012","EFID")

#
fexes.l2_HistoPrmVtx_SiTrack = L2HistoPrmVtx_SiTrack()
fexes.ef_HistoPrmVtx_Jet = EFHistoPrmVtx_Jet()
#
#hypos.l2_BjetHypo_NoCut = L2BjetHypo_NoCut()
#hypos.ef_BjetHypo_NoCut = EFBjetHypo_NoCut()

hypos.l2_BjetHypo_NoCut = getBjetHypoNoCutInstance("L2") 
hypos.ef_BjetHypo_NoCut = getBjetHypoNoCutInstance("EF") 

from TrigBjetHypo.TrigLeptonJetFexAllTEConfig    import *
from TrigBjetHypo.TrigLeptonJetMatchAllTEConfig  import *

if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()):
    fexes.ef_LeptonJetFexAllTE = EFLeptonJetFexAllTE_CloseBy()
else:
    fexes.ef_LeptonJetFexAllTE_R  = getLeptonJetMatchAllTEInstance("CloseBy","R")
    fexes.ef_LeptonJetFexAllTE_RZ = getLeptonJetMatchAllTEInstance("CloseBy","RZ")    


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
                TE=triggerPythonConfig.addSequence("J10",[TrigSiTrack_Jet(),fexes.l2_HistoPrmVtx_SiTrack,fexes.l2_bjet, hypos.l2_BjetHypo_NoCut],"L2_Bjet_NoCut")
                l2chain.addHLTSignature(TE)

       
        # identical for EF as for L2
        efchain = HLTChain(self.ef_chain_name, self.chain_counter,
                           self.l2_chain_name, "EF", str(self.ef_prescale),
                           str(self.ef_pass_through))
        if TriggerFlags.doEF():
            for eachstream in self.streams:
                efchain.addStreamTag(eachstream)

            efchain.addGroup(self.groupEF)

            if self.doOrderEF == False:
                efchain.mergeAndAppendChains(efparts)
            elif self.doOrderEF == True:
                efchain.mergeAndAppendChains_Order(efparts)
   

            ## Removes overlap between combined signature
            if( self.removeOverlap ) :
                if 0 not in [ len(c.siglist) for c in efparts ]:
                    TES = []
                    [ TES.extend(c.siglist[-1].tes) for c in efparts ]
                    mlog.debug("Using this TES as input to overlap-removal sequence: "+str(TES))
                    #print "EF printout: "
                    #print TES
                
                    TE=triggerPythonConfig.addSequence(TES,OverlapRemoval_afterEF_TauJet,"EF"+self.sig_id+"nooverlap")
                    efchain.addHLTSignature(TE)


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
                    elif (self.dPhiCut == "dPhi_10_2j30"):
                        TE=triggerPythonConfig.addSequence(TES,ef_DPhiMetJet_10_2j30,"EF_"+self.sig_id+"dPhiMetJet")
                        efchain.addHLTSignature(TE)
                    elif (self.dPhiCut == "dPhi_10_2j45"):
                        tes1 = TES[1:]   # The EF input TE must be in the order X + MET +JET
                        tes2 = TES[0:1]  # so that tes1= MET +JET, tes2= X                   
                        TE=triggerPythonConfig.addSequence(tes1,ef_DPhiMetJet_10_2j45,"EF_"+self.sig_id+"dPhiMetJet")
                        efchain.addHLTSignature([TE])
                    elif (self.dPhiCut == "dPhi_07_2j10"):
                        tes1 = TES[0:2] # for e+xs+j triggers
                        tes2 = TES[2:]
                        TE=triggerPythonConfig.addSequence(tes1,ef_DPhiMetJet_07_2j10,"EF_"+self.sig_id+"dPhiMetJet")
                        efchain.addHLTSignature(tes2+[TE])
                    elif (self.dPhiCut == "dPhi_20_2j10"):
                        tes1 = TES[0:2] # for e+xs+j triggers
                        tes2 = TES[2:]
                        TE=triggerPythonConfig.addSequence(tes1,ef_DPhiMetJet_20_2j10,"EF_"+self.sig_id+"dPhiMetJet")
                        efchain.addHLTSignature(tes2+[TE])
                    elif (self.dPhiCut == "dPhi_20_2j15"):
                        #tes1 = TES[0:2] # for e+xs+j triggers
                        #tes2 = TES[2:]
                        tes1 = TES[1:]   
                        tes2 = TES[0:1]
                        #print 'DPHI ARGH: '+str(tes1)+str(tes2)
                        TE=triggerPythonConfig.addSequence(tes1,ef_DPhiMetJet_20_2j15,"EF_"+self.sig_id+"dPhiMetJet")
                        efchain.addHLTSignature([TE])
                    elif (self.dPhiCut == "dPhi_07_100j10"):
                        tes1 = TES[0:2]
                        tes2 = TES[2:]
                        TE=triggerPythonConfig.addSequence(tes1,ef_DPhiMetJet_07_100j10,"EF_"+self.sig_id+"dPhiMetJet")
                        efchain.addHLTSignature(tes2+[TE])


            if( self.doBjetMuonJet ):
                #TES = []
                #[ TES.extend(c.siglist[-1].tes) for c in efparts ]
                TE=triggerPythonConfig.addSequence("L2_Bjet_NoCut",fexes.ef_bjet_tracks + [fexes.ef_HistoPrmVtx_Jet,fexes.ef_bjet, hypos.ef_BjetHypo_NoCut],"EF_Bjet_NoCut")
                efchain.addHLTSignature(TE)
                                                                      
        # register chains to be eventually put into the output XML
        triggerPythonConfig.registerHLTChain(self.sig_id, l2chain) 
        triggerPythonConfig.registerHLTChain(self.sig_id, efchain)
        pass
    
class CombinedChain:
    def __init__(self, sig_id, chain_counter, L1_chain_name, EF_ingredients, stream, 
                  prescale_L2=1, prescale_EF=1, pass_throughL2=0, pass_throughEF=0, removeOverlap = False ):

        self.sig_id = sig_id
        self.EF_ingredients = EF_ingredients
        self.chain_counter = str(chain_counter)
        self.L1_chain_name =  L1_chain_name
        self.L2_chain_name =  "L2_"+str(sig_id)
        self.EF_chain_name =  "EF_"+str(sig_id)
        self.prescale_L2   = str(prescale_L2)
        self.prescale_EF   = str(prescale_EF)
        self.pass_throughL2  = str(pass_throughL2)
        self.pass_throughEF  = str(pass_throughEF)
        self.stream        = []
        self.stream        += stream
        self.group         = []
        self.removeOverlap = removeOverlap  # can pass here the HYPO to remove L1 overlaps 

    def generateMenu(self, triggerPythonConfig):
        
        mlog.debug("generating config for: "+self.sig_id)

 
        ## find chains objects given names of EF
        efparts = [ triggerPythonConfig.getHLTChain(i) for i in self.EF_ingredients ]
        if None in  efparts:  ## some action if simple chains are not found
            mlog.warning("Basic chains used to built combined chain: "+self.sig_id+" are not present.")
            mlog.warning("These were found:  "+str([c.chain_name for c in  efparts if c != None]))
            mlog.warning("These were needed: "+str(self.EF_ingredients ) )
            return 

        ## take the name of L2 part from EF part
        l2parts = [ triggerPythonConfig.getHLTChain(i.lower_chain_name)  for i in efparts ]
        if None in l2parts:  ## some action if simple chains are not found
            mlog.warning("Basic chains used to built combined chain: "+self.sig_id+" are not present.")
            mlog.warning("These were found:  "+str([c.chain_name for c in  l2parts if c != None ]))
            mlog.warning("These were needed: "+str( [i.lower_chain_name  for i in efparts]))
            return 

        l2chain = HLTChain(self.L2_chain_name, self.chain_counter,self.L1_chain_name,  
                           "L2", self.prescale_L2, self.pass_throughL2)
        for eachstream in self.stream:
            l2chain.addStreamTag(eachstream)   ## add stream tags
            
        l2chain.addGroup(self.group)   ## add monitoring group tags
        if TriggerFlags.doLVL2():

        ## we should put here a possibility to remove Overlaps, as soon as necessary algo is in place
            if( self.removeOverlap ) :
                #mlog.warning("removeOverlap for "+self.sig_id+" is not yet instrumented!") 
                #else:
                l2_init_tes = []
                #mlog.info( 'l2parts: ' +  str(l2parts))
                #for c in l2parts:
                #    mlog.info( 'l2parts chains sigs: ' + str(c.siglist))
                [ l2_init_tes.extend(c.siglist[0].tes) for c in l2parts ]
                #print l2_init_tes
                inputs = []
                for tes in l2_init_tes:
                    for s in triggerPythonConfig.theSeqLists:
                        if( s.output ==tes ):
                            inputs.extend(s.input)

                #print inputs
                TE = triggerPythonConfig.addSequence(inputs,OverlapRemoval_afterL1_TauJet,"L2"+self.sig_id+"nooverlap")
                l2chain.addHLTSignature(TE)

        
            l2chain.mergeAndAppendChains(l2parts)  ## here the complexity is hidden, chains are inspected and signatures merged

        #if not TriggerFlags.doLVL2():
        #    l2chain = HLTChain(self.L2_chain_name, self.chain_counter,self.L1_chain_name,  
        #                       "L2", self.prescale_L2, self.pass_throughL2)

        #if self.l2_topologicals[self.sig_id] is not None: ## here topological algo is added at the end
        #    TES = []
        #    [TES.extend(c.siglist[-1].tes) for c in l2parts]
        #    mlog.info("Using this TES as input to toplogical sequence: "+str(TES))
        #    triggerPythonConfig.addSequence(TES,  self.l2_topologicals[self.sig_id], self.sig_id)
        #    l2chain.addHLTSignature(self.sig_id)


        ## identical for EF as for L2
        efchain = HLTChain(self.EF_chain_name, self.chain_counter,
                           self.L2_chain_name, "EF", self.prescale_EF, self.pass_throughEF)
        
        if TriggerFlags.doEF():
            for eachstream in self.stream:
                efchain.addStreamTag(eachstream)   ## add stream tags

            efchain.addGroup(self.group)   ## add monitoring group tags
            efchain.mergeAndAppendChains(efparts) ## creating EF chain by merging appropriate efchains
            
            ## Removes overlap between combined signature
            if( self.removeOverlap ) :

                if 0 not in [ len(c.siglist) for c in efparts ]:
                    TES = []
                    [ TES.extend(c.siglist[-1].tes) for c in efparts ]
                    mlog.debug("Using this TES as input to overlap-removal sequence: "+str(TES))
                                    
                    TE=triggerPythonConfig.addSequence(TES,OverlapRemoval_afterEF_TauJet,"EF"+self.sig_id+"nooverlap")
                    efchain.addHLTSignature(TE)
                

        
        #if self.ef_topologicals[self.sig_id] is not None:
        #    TES = []
        #    triggerPythonConfig.addSequence(efchain.siglist[-1].tes,  self.ef_topologicals[self.sig_id], self.sig_id)
        #    efchain.addHLTSignature(self.sig_id)

        ## register chains to be eventually put into the output XML
        triggerPythonConfig.registerHLTChain(self.sig_id, l2chain) 
        triggerPythonConfig.registerHLTChain(self.sig_id, efchain)


from TrigEgammaMuonCombHypo.TrigL2ElectronMuonAngleHypoConfig import *
from TrigEgammaMuonCombHypo.TrigEFElectronMuonAngleHypoConfig import *

fexes.l2_emuAngleFex = TrigL2ElectronMuonAngleFex()
fexes.ef_emuAngleFex = TrigEFElectronMuonAngleFex()
#
hypos.l2_e5_medium_mu6_topo_loose  = TrigL2ElectronMuonAngleHypo_loose()
hypos.l2_e5_medium_mu6_topo_medium = TrigL2ElectronMuonAngleHypo_medium()
hypos.l2_e10_medium_mu6_topo_medium = TrigL2ElectronMuonAngleHypo_medium()
hypos.ef_e5_medium_mu6_topo_loose  = TrigEFElectronMuonAngleHypo_loose()
hypos.ef_e5_medium_mu6_topo_medium = TrigEFElectronMuonAngleHypo_medium()
hypos.ef_e10_medium_mu6_topo_medium = TrigEFElectronMuonAngleHypo_medium()
hypos.l2_e12T_medium_mu6_topo_medium = TrigL2ElectronMuonAngleHypo_medium()
hypos.ef_e12T_medium_mu6_topo_medium = TrigEFElectronMuonAngleHypo_medium()
hypos.l2_e12Tvh_medium_mu6_topo_medium = TrigL2ElectronMuonAngleHypo_medium()
hypos.l2_e12Tvh_medium1_mu6_topo_medium = TrigL2ElectronMuonAngleHypo_medium()
hypos.l2_e12Tvh_medium1_L2StarB_mu6_topo_medium = TrigL2ElectronMuonAngleHypo_medium() 
hypos.ef_e12Tvh_medium_mu6_topo_medium = TrigEFElectronMuonAngleHypo_medium()
hypos.ef_e12Tvh_medium1_mu6_topo_medium = TrigEFElectronMuonAngleHypo_medium()
hypos.ef_e12Tvh_medium1_L2StarB_mu6_topo_medium = TrigEFElectronMuonAngleHypo_medium() 
#
hypos.l2_e5_medium_mu4_topoAngle_medium = TrigL2ElectronMuonAngleHypo_e5mu4_medium()
hypos.l2_e5_tight_mu4_topoAngle_medium = TrigL2ElectronMuonAngleHypo_e5mu4_medium()
hypos.l2_e5_medium_mu4_topoAngle_tight = TrigL2ElectronMuonAngleHypo_e5mu4()
hypos.l2_e5_tight_mu4_topoAngle_tight = TrigL2ElectronMuonAngleHypo_e5mu4()
hypos.ef_e5_medium_mu4_topoAngle_medium = TrigEFElectronMuonAngleHypo_e5mu4_medium()
hypos.ef_e5_tight_mu4_topoAngle_medium = TrigEFElectronMuonAngleHypo_e5mu4_medium()
hypos.ef_e5_medium_mu4_topoAngle_tight = TrigEFElectronMuonAngleHypo_e5mu4()
hypos.ef_e5_tight_mu4_topoAngle_tight = TrigEFElectronMuonAngleHypo_e5mu4()

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

from TrigEgammaMuonCombHypo.TrigL2PhotonMuonAngleHypoConfig import *
from TrigEgammaMuonCombHypo.TrigEFPhotonMuonAngleHypoConfig import *

fexes.l2_gmuAngleFex = TrigL2PhotonMuonAngleFex()
fexes.ef_gmuAngleFex = TrigEFPhotonMuonAngleFex()
#OI here we want separate instances for each chain that we can monitor the decision
hypos.l2_mu10_g10_loose_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu10_g10_loose_TauMass")
hypos.l2_mu20_tight_g5_loose_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu20_tight_g5_loose_TauMass")
hypos.l2_mu4T_g20Tvh_loose_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu4T_g20Tvh_loose_TauMass")
#hypos.l2_mu4T_g22Tvh_loose_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu4T_g22Tvh_loose_TauMass")

hypos.l2_mu10i_g10_loose_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu10i_g10_loose_TauMass")
hypos.l2_mu10i_loose_g12Tvh_loose_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu10i_loose_g12Tvh_loose_TauMass")
hypos.l2_mu20i_tight_g5_loose_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu20i_tight_g5_loose_TauMass")
hypos.l2_mu4Ti_g20Tvh_loose_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu4Ti_g20Tvh_loose_TauMass")

hypos.l2_mu10i_g10_medium_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu10i_g10_medium_TauMass")
hypos.l2_mu10i_loose_g12Tvh_medium_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu10i_loose_g12Tvh_medium_TauMass")
hypos.l2_mu20i_tight_g5_medium_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu20i_tight_g5_medium_TauMass")
hypos.l2_mu4Ti_g20Tvh_medium_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu4Ti_g20Tvh_medium_TauMass")


hypos.ef_mu10_g10_loose_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu10_g10_loose_TauMass")
hypos.ef_mu20_tight_g5_loose_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu20_tight_g5_loose_TauMass")
hypos.ef_mu4T_g20Tvh_loose_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu4T_g20Tvh_loose_TauMass")
#hypos.ef_mu4T_g22Tvh_loose_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu4T_g22Tvh_loose_TauMass")

hypos.ef_mu10i_g10_loose_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu10i_g10_loose_TauMass")
hypos.ef_mu10i_loose_g12Tvh_loose_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu10i_loose_g12Tvh_loose_TauMass")
hypos.ef_mu20i_tight_g5_loose_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu20i_tight_g5_loose_TauMass")
hypos.ef_mu4Ti_g20Tvh_loose_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu4Ti_g20Tvh_loose_TauMass")

hypos.ef_mu10i_g10_medium_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu10i_g10_medium_TauMass")
hypos.ef_mu10i_loose_g12Tvh_medium_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu10i_loose_g12Tvh_medium_TauMass")
hypos.ef_mu20i_tight_g5_medium_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu20i_tight_g5_medium_TauMass")
hypos.ef_mu4Ti_g20Tvh_medium_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu4Ti_g20Tvh_medium_TauMass")



hypos.l2_mu20i_tight_g5_mugamma_TauMass  = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu20i_tight_g5_mugamma_TauMass")
hypos.ef_mu20i_tight_g5_mugamma_TauMass  = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu20i_tight_g5_mugamma_TauMass")

hypos.l2_mu10i_g10_mugamma_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu10i_g10_mugamma_TauMass")
hypos.ef_mu10i_g10_mugamma_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu10i_g10_mugamma_TauMass")

hypos.l2_mu10i_loose_g12Tvh_mugamma_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu10i_loose_g12Tvh_mugamma_TauMass") 
hypos.ef_mu10i_loose_g12Tvh_mugamma_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu10i_loose_g12Tvh_mugamma_TauMass") 

hypos.l2_mu4Ti_g20Tvh_mugamma_TauMass = TrigL2PhotonMuonAngleHypo_tau("TrigL2PhotonMuonAngleHypo_mu4Ti_g20Tvh_mugamma_TauMass")
hypos.ef_mu4Ti_g20Tvh_mugamma_TauMass = TrigEFPhotonMuonAngleHypo_tau("TrigEFPhotonMuonAngleHypo_mu4Ti_g20Tvh_mugamma_TauMass")


class PhotonMuonTopoAngle(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig, l2merge=None, efmerge=None, l2_hypo=None, ef_hypo=None):
        ChainTemplate.__init__(self, signature, l2=l2, ef=ef)
        
        self.group         = []
        self.addGroups('RATE:MuonPhoton').addGroups('BW:Muon')
        self.addStreams('Muons')
        
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
        self.addSequence(self.l2merge, [fexes.l2_gmuAngleFex, self.l2_hypo], \
                         self.l2.chain_name).addSignature('L2')
        self.addSequence(self.efmerge, [fexes.ef_gmuAngleFex, self.ef_hypo], \
                         self.ef.chain_name).addSignature('EF')

    def generateMenu(self, tpc):
        if not self.seq_generated:
            try:
                self.mergeThem()
                self.seq_generated = True
            except AttributeError:
                print 'something went wrong in combined chains'

    pass







class CombinedChain2_mujet:
    def __init__(self, sig_id, chain_counter, streams, groupL2, groupEF,
                 l2_chain_name, l2_ingredients, l1_item, l2_prescale, l2_pass_through,
                 ef_chain_name, ef_ingredients, ef_prescale=1, ef_pass_through=0,
                 doOrderL2=False, doOrderEF=False):
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
        self.doOrderL2 = doOrderL2
        self.doOrderEF = doOrderEF
         

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
            if self.doOrderL2 == False:    
                l2chain.mergeAndAppendChains(l2parts)
            elif self.doOrderL2 == True:
                l2chain.mergeAndAppendChains_Order(l2parts)

       
        # identical for EF as for L2
        efchain = HLTChain(self.ef_chain_name, self.chain_counter,
                           self.l2_chain_name, "EF", str(self.ef_prescale),
                           str(self.ef_pass_through))
        if TriggerFlags.doEF():
            for eachstream in self.streams:
                efchain.addStreamTag(eachstream)

            efchain.addGroup(self.groupEF)

            if self.doOrderEF == False:
                efchain.mergeAndAppendChains(efparts)
            elif self.doOrderEF == True:
                efchain.mergeAndAppendChains_Order(efparts)

            if 'matchedZ' in self.ef_chain_name:
                ef_LeptonJetMatching = eval('fexes.ef_LeptonJetFexAllTE_RZ')
            elif 'matched' in self.ef_chain_name:
                ef_LeptonJetMatching = eval('fexes.ef_LeptonJetFexAllTE_R')                


            TE=triggerPythonConfig.addSequence(self.ef_ingredients, ef_LeptonJetMatching, self.ef_chain_name)
            efchain.addHLTSignature(TE)

                                                                     
        # register chains to be eventually put into the output XML
        triggerPythonConfig.registerHLTChain(self.sig_id, l2chain) 
        triggerPythonConfig.registerHLTChain(self.sig_id, efchain)
        pass


from TrigTauHypo.T2TauTauCombConfig import *
from TrigTauHypo.EFTauTauCombConfig import *
fexes.l2_tautauCombFex = T2TauTauCombFex()
fexes.ef_tautauCombFex = EFTauTauCombFex()
###########################################################################################
class TauTauTopoComb(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig, l2merge=None, efmerge=None, l2_hypo=None, ef_hypo=None):
        ChainTemplate.__init__(self, signature, l2=l2, ef=ef)

        self.addGroups('RATE:MultiTau').addGroups('BW:Tau')
        self.addStreams('JetTauEtmiss')

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
            # in generateMenu()
            self.mergeThem()
            self.seq_generated = True
        except AttributeError:
            # Do it later in generateMenu()
            self.seq_generated = False

    def mergeThem(self):
        self.merge(efnames=self.efmerge, l2names=self.l2merge, onlyEF=True)
        self.addSequence(self.l2merge, [fexes.l2_tautauCombFex, self.l2_hypo], \
                         self.l2.chain_name).addSignature('L2')
        self.addSequence(self.efmerge, [fexes.ef_tautauCombFex, self.ef_hypo], \
                         self.ef.chain_name).addSignature('EF')
    def generateMenu(self, tpc):
        if not self.seq_generated:
            try:
                self.mergeThem()
                self.seq_generated = True
            except AttributeError:
                print 'something went wrong in combined chains'

    pass


 

hypos.l2_tau18Ti_loose2_e18vh_medium1_deta18  = T2TauTauCombHypo_tight("T2TauTauCombHypo_tau18e18_deta18")
hypos.ef_tau18Ti_loose2_e18vh_medium1_deta18  = EFTauTauCombHypo_tight("EFTauTauCombHypo_tau18e18_deta18")

hypos.l2_tau18Ti_loose2_e18vh_medium1_deta20  = T2TauTauCombHypo_medium("T2TauTauCombHypo_tau18e18_deta20")
hypos.ef_tau18Ti_loose2_e18vh_medium1_deta20  = EFTauTauCombHypo_medium("EFTauTauCombHypo_tau18e18_deta20")

hypos.l2_tau18Ti_loose2_e18vh_medium1_deta25  = T2TauTauCombHypo_loose("T2TauTauCombHypo_tau18e18_deta25")
hypos.ef_tau18Ti_loose2_e18vh_medium1_deta25  = EFTauTauCombHypo_loose("EFTauTauCombHypo_tau18e18_deta25")

hypos.l2_tau20_loose2_mu15_deta18  = T2TauTauCombHypo_tight("T2TauTauCombHypo_tau20mu15_deta18")
hypos.ef_tau20_loose2_mu15_deta18  = EFTauTauCombHypo_tight("EFTauTauCombHypo_tau20mu15_deta18")

hypos.l2_tau20_loose2_mu15_deta20  = T2TauTauCombHypo_medium("T2TauTauCombHypo_tau20mu15_deta20")
hypos.ef_tau20_loose2_mu15_deta20  = EFTauTauCombHypo_medium("EFTauTauCombHypo_tau20mu15_deta20")

hypos.l2_tau20_loose2_mu15_deta25  = T2TauTauCombHypo_loose("T2TauTauCombHypo_tau20mu15_deta25")
hypos.ef_tau20_loose2_mu15_deta25  = EFTauTauCombHypo_loose("EFTauTauCombHypo_tau20mu15_deta25")


TauElectrons_eIdScan = []


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    TauElectrons_eIdScan += [
        CombinedChain('tau16_loose_e10_loose',   '213', 'L1_2TAU6_EM5',       ['EF_tau16_loose','EF_e10_loose'],
                      ['combined','comb__taue'], prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        CombinedChain('tau16_loose_e15_loose',    '6923', 'L1_2TAU6_EM10',      ['EF_tau16_loose','EF_e15_loose'],
                      ['combined','comb__taue'], prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        CombinedChain('tau29_loose_e15_tight', 4305, 'L1_2TAU11_EM10', ['EF_tau29_loose', 'EF_e15_tight'],
                      ['combined', 'comb__taue']),
        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.NoVHThresholdMenus(): 
    TauElectrons_eIdScan += [
        CombinedChain('tau16_loose_e15_medium',    '247', 'L1_2TAU6_EM10',     ['EF_e15_medium','EF_tau16_loose'],
                      ['combined','comb__taue'], prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
#        CombinedChain('tau20_loose_e15_medium', 308, 'L1_2TAU8_EM10', ['EF_tau20_loose', 'EF_e15_medium'],
#                      ['combined', 'comb__taue']),
#        CombinedChain('tau16_medium_e15_tight', 4307, 'L1_2TAU6_EM10', ['EF_tau16_medium', 'EF_e15_tight'],
#                      ['combined', 'comb__taue']), 
        CombinedChain('tau16_medium_e15_medium', 4308, 'L1_2TAU6_EM10', ['EF_tau16_medium', 'EF_e15_medium'],
                      ['combined', 'comb__taue']), 
        CombinedChain('tau16_loose_e15_tight', 945, 'L1_2TAU6_EM10', ['EF_tau16_loose', 'EF_e15_tight'],
                      ['combined', 'comb__taue']),
        CombinedChain('tau20_medium_e15_medium', 16, 'L1_2TAU8_EM10', ['EF_tau20_medium', 'EF_e15_medium'],
                      ['combined', 'comb__taue']), 
        CombinedChain('tau20T_medium_e15_medium', 241, 'L1_2TAU11_EM10', ['EF_tau20T_medium', 'EF_e15_medium'],
                      ['combined', 'comb__taue']),
        CombinedChain('tau20_medium_e15_tight', 3458, 'L1_2TAU8_EM10', ['EF_tau20_medium', 'EF_e15_tight'],
                      ['combined', 'comb__taue']), 
        CombinedChain('tau20T_medium_e15_tight', 3459, 'L1_2TAU11_EM10', ['EF_tau20T_medium', 'EF_e15_tight'],
                      ['combined', 'comb__taue']), 
        
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.VHThresholdMenus(): 
    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
        TauElectrons_eIdScan += [    
            CombinedChain('tau20i_medium_e15vh_medium', 245, 'L1_2TAU8I_EM10VH', ['EF_tau20i_medium', 'EF_e15vh_medium'],
                          ['combined', 'comb__taue']),    
            CombinedChain('tau20_medium_e15vh_medium', 16, 'L1_2TAU8_EM10VH', ['EF_tau20_medium', 'EF_e15vh_medium'],
                          ['combined', 'comb__taue']),
            CombinedChain('tau50_medium_e15vh_medium', 326, 'L1_TAU30_EM10VH', ['EF_tau50_medium', 'EF_e15vh_medium'],
                          ['combined', 'comb__taue']), 
            CombinedChain('tau20T_medium_e15vh_medium', 241, 'L1_2TAU11_EM10VH', ['EF_tau20T_medium', 'EF_e15vh_medium'],
                          ['combined', 'comb__taue']), 
            #        CombinedChain('tau20v_medium_e15vh_medium', 542, 'L1_2TAU8V_EM10VH', ['EF_tau20v_medium', 'EF_e15vh_medium'],
            #                      ['combined', 'comb__taue']), 

        ####
            ]

    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
        TauElectrons_eIdScan += [    
            CombinedChain('tau20Ti_medium1_e18vh_medium1', 200, 'L1_2TAU11I_EM14VH', ['EF_tau20Ti_medium1', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']), 
            
            CombinedChain('tau20Ti_medium_e18vh_medium1', 3189, 'L1_2TAU11I_EM14VH', ['EF_tau20Ti_medium', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']),
            
            #CombinedChain('tau20T_medium_e15vh_medium1', 202, 'L1_2TAU11_EM10VH', ['EF_tau20T_medium', 'EF_e15vh_medium1'],
            #              ['combined', 'comb__taue']),

            CombinedChain('tau20T_medium1_e15vh_medium1', 202, 'L1_2TAU11_EM10VH', ['EF_tau20T_medium1', 'EF_e15vh_medium1'],
                          ['combined', 'comb__taue']),           
            
            CombinedChain('tau50_medium1_e18vh_medium1', 6913, 'L1_2TAU11_TAU20_EM14VH', ['EF_tau50_medium1', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']),
            
            CombinedChain('tau38T_medium_e18vh_medium1', 6924, 'L1_2TAU11_TAU20_EM14VH', ['EF_tau38T_medium', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']),

            CombinedChain('tau38T_medium1_e18vh_medium1', 205, 'L1_2TAU11_TAU20_EM14VH', ['EF_tau38T_medium1', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']),
            
            CombinedChain('tau38T_medium1_llh_e18vh_medium1', 163, 'L1_2TAU11_TAU20_EM14VH', ['EF_tau38T_medium1_llh', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']),
            
            CombinedChain('tau50_medium_e15vh_medium1', 6926, 'L1_2TAU11_TAU20_EM10VH', ['EF_tau50_medium1', 'EF_e15vh_medium1'],
                          ['combined', 'comb__taue']),
            
            ###Added for sample T production for Physics groups to give feedback
            CombinedChain('tau20Ti_medium1_llh_e18vh_medium1', 207, 'L1_2TAU11I_EM14VH', ['EF_tau20Ti_medium1_llh', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']), 

            #Chains with looser pT thresholds at L2 and EF
            CombinedChain('tau20Ti_medium1_L2loose_e18vh_medium1', 6899, 'L1_2TAU11I_EM14VH', ['EF_tau20Ti_medium1_L2loose', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']), 
            CombinedChain('tau18Ti_medium1_L2loose_e18vh_medium1', 557, 'L1_2TAU11I_EM14VH', ['EF_tau18Ti_medium1_L2loose', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']), 
            

            CombinedChain('tau18Ti_loose3_e18vh_medium1', 6888, 'L1_2TAU11I_EM14VH', ['EF_tau18Ti_loose3', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']), 
            CombinedChain('tau18Ti_loose2_e18vh_medium1', 395, 'L1_2TAU11I_EM14VH', ['EF_tau18Ti_loose2', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']), 

            TauTauTopoComb('tau18Ti_loose2_e18vh_medium1_deta18',
                           l2=Chain(name='L2_tau18Ti_loose2_e18vh_medium1_deta18', counter=717, seed='L1_2TAU11I_EM14VH'),
                           efmerge=['EF_tau18Ti_loose2', 'EF_e18vh_medium1'], l2merge=['L2_tau18Ti_loose2', 'L2_e18vh_medium1']),
            
            TauTauTopoComb('tau18Ti_loose2_e18vh_medium1_deta20',
                           l2=Chain(name='L2_tau18Ti_loose2_e18vh_medium1_deta20', counter=583, seed='L1_2TAU11I_EM14VH'),
                           efmerge=['EF_tau18Ti_loose2', 'EF_e18vh_medium1'], l2merge=['L2_tau18Ti_loose2', 'L2_e18vh_medium1']),
            
            TauTauTopoComb('tau18Ti_loose2_e18vh_medium1_deta25',
                           l2=Chain(name='L2_tau18Ti_loose2_e18vh_medium1_deta25', counter=591, seed='L1_2TAU11I_EM14VH'),
                           efmerge=['EF_tau18Ti_loose2', 'EF_e18vh_medium1'], l2merge=['L2_tau18Ti_loose2', 'L2_e18vh_medium1']),
            ##L2 pass through test chain
            CombinedChain('tau20Ti_loose1_L2NoCut_e18vh_medium1', 5521, 'L1_2TAU11I_EM14VH', ['EF_tau20Ti_loose1_L2NoCut', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']), 
            CombinedChain('tau20Ti_loose2_L2NoCut_e18vh_medium1', 5522, 'L1_2TAU11I_EM14VH', ['EF_tau20Ti_loose2_L2NoCut', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']), 
            CombinedChain('tau20Ti_medium1_L2NoCut_e18vh_medium1', 5523, 'L1_2TAU11I_EM14VH', ['EF_tau20Ti_medium1_L2NoCut', 'EF_e18vh_medium1'],
                          ['combined', 'comb__taue']), 
            

            
            
            
            ]
    
for item in TauElectrons_eIdScan :
    if hasattr(item, 'group'):  item.group += [ "RATE:ElectronTau",'BW:Tau']

TauMuons = [
    CombinedChain('tau20_medium_mu15', 834, 'L1_TAU8_MU10', [ 'EF_mu15', 'EF_tau20_medium' ],
                  ['combined', 'comb__taumu']),
    CombinedChain('tau20T_medium_mu15', 677, 'L1_TAU11_MU10', [ 'EF_mu15', 'EF_tau20T_medium' ],
                  ['combined', 'comb__taumu']), 
    CombinedChain('tau20_medium1_mu15i', 153, 'L1_TAU8_MU10', [ 'EF_mu15i', 'EF_tau20_medium1' ],
                  ['combined', 'comb__taumu']), 
    CombinedChain('tau20T_medium1_mu15i', 154, 'L1_TAU11_MU10', [ 'EF_mu15i', 'EF_tau20T_medium1' ],
                  ['combined', 'comb__taumu']),

    ##L2 pass through test chain
    CombinedChain('tau20T_loose1_L2NoCut_mu15', 5524, 'L1_TAU11_MU10', [ 'EF_mu15', 'EF_tau20T_loose1_L2NoCut' ],
                  ['combined', 'comb__taumu']), 
    CombinedChain('tau20T_loose2_L2NoCut_mu15', 5525, 'L1_TAU11_MU10', [ 'EF_mu15', 'EF_tau20T_loose2_L2NoCut' ],
                  ['combined', 'comb__taumu']), 
    CombinedChain('tau20T_medium1_L2NoCut_mu15', 5526, 'L1_TAU11_MU10', [ 'EF_mu15', 'EF_tau20T_medium1_L2NoCut' ],
                  ['combined', 'comb__taumu']), 

    ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():    
    TauMuons += [
        CombinedChain('tau20_medium1_mu15', 837, 'L1_TAU8_MU10', [ 'EF_mu15', 'EF_tau20_medium1' ],
                      ['combined', 'comb__taumu']),

        CombinedChain('tau20_medium1_llh_mu15', 193, 'L1_TAU8_MU10', [ 'EF_mu15', 'EF_tau20_medium1_llh' ],
                      ['combined', 'comb__taumu']),

        CombinedChain('tau20_medium1_mu18', 836, 'L1_TAU8_MU10', [ 'EF_mu18', 'EF_tau20_medium1' ],
                      ['combined', 'comb__taumu']),
        
        #Chains with looser pT thresholds at L2 and EF
        CombinedChain('tau20_medium1_L2loose_mu15', 559, 'L1_TAU8_MU10', [ 'EF_mu15', 'EF_tau20_medium1_L2loose' ],
                      ['combined', 'comb__taumu']),
        CombinedChain('tau18_medium1_L2loose_mu15', 3971, 'L1_TAU8_MU10', [ 'EF_mu15', 'EF_tau18_medium1_L2loose' ],
                      ['combined', 'comb__taumu']),

        CombinedChain('tau20_loose3_mu15', 5172, 'L1_TAU8_MU10', [ 'EF_mu15', 'EF_tau20_loose3' ],
                      ['combined', 'comb__taumu']),
        CombinedChain('tau20_loose2_mu15', 396, 'L1_TAU8_MU10', [ 'EF_mu15', 'EF_tau20_loose2' ],
                      ['combined', 'comb__taumu']),

        TauTauTopoComb('tau20_loose2_mu15_deta18',
                       l2=Chain(name='L2_tau20_loose2_mu15_deta18', counter=450, seed='L1_TAU8_MU10'),
                       efmerge=['EF_tau20_loose2', 'EF_mu15'], l2merge=['L2_tau20_loose2', 'L2_mu15']),
        
        TauTauTopoComb('tau20_loose2_mu15_deta20',
                       l2=Chain(name='L2_tau20_loose2_mu15_deta20', counter=566, seed='L1_TAU8_MU10'),
                       efmerge=['EF_tau20_loose2', 'EF_mu15'], l2merge=['L2_tau20_loose2', 'L2_mu15']),
        
        TauTauTopoComb('tau20_loose2_mu15_deta25',
                       l2=Chain(name='L2_tau20_loose2_mu15_deta25', counter=580, seed='L1_TAU8_MU10'),
                       efmerge=['EF_tau20_loose2', 'EF_mu15'], l2merge=['L2_tau20_loose2', 'L2_mu15']),
        


        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():    
    TauMuons += [
        CombinedChain('tau20i_medium_mu15', 716, 'L1_TAU8I_MU10', [ 'EF_mu15', 'EF_tau20i_medium' ],
                      ['combined', 'comb__taumu']), 
        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():    
    TauMuons += [
        #    CombinedChain('tau16_loose_mu10i_loose',  '3101', 'L1_TAU6_MU10', ['EF_mu10', 'EF_tau16i_loose'], ['combined','comb__taumu'], prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        #    CombinedChain('tau16i_loose_mu10i_loose',  '561', 'L1_TAU6I_MU10', ['EF_mu10i_loose','EF_tau16i_loose'], ['combined','comb__taumu'], prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        #    CombinedChain('tau16i_loose_mu15i_loose',  '3100', 'L1_TAU9I_MU15', ['EF_mu15i_loose', 'EF_tau16i_loose'], ['combined','comb__taumu'], prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        
        CombinedChain('tau16_loose_mu10',  '207', 'L1_TAU6_MU10', ['EF_mu10','EF_tau16_loose'],
                      ['combined','comb__taumu'], prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        CombinedChain('tau16_loose_mu15',  '238', 'L1_TAU6_MU10', ['EF_mu15','EF_tau16_loose'],
                      ['combined','comb__taumu'], prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        CombinedChain('tau20_loose_mu15', 4310, 'L1_TAU8_MU10', [ 'EF_mu15' , 'EF_tau20_loose'],
                      ['combined', 'comb__taumu']), 
        CombinedChain('tau16_medium_mu15', 4311, 'L1_TAU6_MU10', [ 'EF_mu15', 'EF_tau16_medium'],
                      ['combined', 'comb__taumu']), 
        CombinedChain('tau16_medium_mu10', 201, 'L1_TAU6_MU10', [ 'EF_mu10', 'EF_tau16_medium'],
                      ['combined', 'comb__taumu']), 
        CombinedChain('tau29_loose_mu15', 4309, 'L1_TAU11_MU10', [ 'EF_mu15', 'EF_tau29_loose'],
                      ['combined', 'comb__taumu']),
        ]

for item in TauMuons:
    if hasattr(item, 'group'):  item.group += [ "RATE:MuonTau",'BW:Tau']

TauMuons_tauSiTrk = [
]

for item in TauMuons_tauSiTrk :
    item.group += [ "RATE:MuonTau",'BW:Tau']


TauMu_VBF = [
    CombinedChain2('tau20_medium1_mu15_vbf', 8001, ['combined', 'comb__taumu'], [], [],
#                   'L2_tau20_medium1_mu15_vbf', ['L2_tau20_medium1_mu15', 'L2_j0_a4TTem_j0_c4cchad_mjj0deta25_L1TAU8MU10'], 'L1_TAU8_MU10', 1, 0,
#                   'EF_tau20_medium1_mu15_vbf', ['EF_tau20_medium1_mu15', 'EF_j0_a4tchad_mjj400deta28_L2FS_l2deta2p5_L1TAU8MU10' ], doOrderL2=True, doOrderEF=True),
                   'L2_tau20_medium1_mu15_vbf', ['L2_tau20_medium1_mu15', 'L2_2j15_a4TTem_deta25_L1TAU8MU10'], 'L1_TAU8_MU10', 1, 0,
                   'EF_tau20_medium1_mu15_vbf', ['EF_tau20_medium1_mu15', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L1TAU8MU10' ], doOrderL2=True, doOrderEF=True),

    CombinedChain2('tau20_loose3_mu15_vbf', 8003, ['combined', 'comb__taumu'], [], [],
                   #'L2_tau20_loose3_mu15_vbf', ['L2_tau20_loose3_mu15', 'L2_tau20_loose3', 'L2_j0_a4TTem_j0_c4cchad_mjj0deta25_L1TAU8MU10'], 'L1_TAU8_MU10', 1, 0,
                   #'EF_tau20_loose3_mu15_vbf', ['EF_tau20_loose3_mu15', 'EF_tau20_loose3', 'EF_j0_a4tchad_mjj400deta28_L2FS_l2deta2p5_L1TAU8MU10' ], doOrderL2=True, doOrderEF=True),
                   'L2_tau20_loose3_mu15_vbf', ['L2_tau20_loose3_mu15', 'L2_2j15_a4TTem_deta25_L1TAU8MU10'], 'L1_TAU8_MU10', 1, 0,
                   'EF_tau20_loose3_mu15_vbf', ['EF_tau20_loose3_mu15', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L1TAU8MU10' ], doOrderL2=True, doOrderEF=True),

    CombinedChain2('tau20_loose2_mu15_vbf', 8004, ['combined', 'comb__taumu'], [], [],
                   #'L2_tau20_loose2_mu15_vbf', ['L2_tau20_loose2_mu15', 'L2_j0_a4TTem_j0_c4cchad_mjj0deta25_L1TAU8MU10'], 'L1_TAU8_MU10', 1, 0,
                   #'EF_tau20_loose2_mu15_vbf', ['EF_tau20_loose2_mu15', 'EF_j0_a4tchad_mjj400deta28_L2FS_l2deta2p5_L1TAU8MU10' ], doOrderL2=True, doOrderEF=True),
                   'L2_tau20_loose2_mu15_vbf', ['L2_tau20_loose2_mu15', 'L2_2j15_a4TTem_deta25_L1TAU8MU10'], 'L1_TAU8_MU10', 1, 0,
                   'EF_tau20_loose2_mu15_vbf', ['EF_tau20_loose2_mu15', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L1TAU8MU10' ], doOrderL2=True, doOrderEF=True),

]

for item in TauMu_VBF :
    item.groupL2 +=  [ "RATE:MuonTau",'BW:Tau']
    item.groupEF +=  [ "RATE:MuonTau",'BW:Tau']

TauEl_VBF = [
    CombinedChain2('tau18Ti_loose3_e18vh_medium1_vbf', 8005, ['combined', 'comb__taue'], [], [],
                   'L2_tau18Ti_loose3_e18vh_medium1_vbf', ['L2_tau18Ti_loose3_e18vh_medium1', 'L2_2j15_a4TTem_deta25_L12TAU11IEM14VH'], 'L1_2TAU11I_EM14VH', 1, 0,
                   'EF_tau18Ti_loose3_e18vh_medium1_vbf', ['EF_tau18Ti_loose3_e18vh_medium1', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU11IEM14VH' ], doOrderL2=True, doOrderEF=True),

    CombinedChain2('tau18Ti_loose2_e18vh_medium1_vbf', 403, ['combined', 'comb__taue'], [], [],
                   'L2_tau18Ti_loose2_e18vh_medium1_vbf', ['L2_tau18Ti_loose2_e18vh_medium1', 'L2_2j15_a4TTem_deta25_L12TAU11IEM14VH'], 'L1_2TAU11I_EM14VH', 1, 0,
                   'EF_tau18Ti_loose2_e18vh_medium1_vbf', ['EF_tau18Ti_loose2_e18vh_medium1', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU11IEM14VH' ], doOrderL2=True, doOrderEF=True),

    
]

for item in TauEl_VBF :
    item.groupL2 += [ "RATE:ElectronTau",'BW:Tau']
    item.groupEF += [ "RATE:ElectronTau",'BW:Tau']

TauTau_VBF = [
    ##This should go in MultiTau.py
    CombinedChain2('tau27Ti_loose3_tau18Ti_loose3_vbf', 8007, ['tauEtmiss'], [], [],
                   #'L2_tau27Ti_loose3_tau18Ti_loose3_vbf',['L2_tau27Ti_loose3_tau18Ti_loose3','L2_j0_a4TTem_j0_c4cchad_mjj0deta25_L12TAU11ITAU15'],'L1_2TAU11I_TAU15', 1, 0,
                   #'EF_tau27Ti_loose3_tau18Ti_loose3_vbf',['EF_tau27Ti_loose3_tau18Ti_loose3','EF_j0_a4tchad_mjj400deta28_L2FS_l2deta2p5_L12TAU11ITAU15' ],doOrderL2=True, doOrderEF=True),
                   'L2_tau27Ti_loose3_tau18Ti_loose3_vbf', ['L2_tau27Ti_loose3_tau18Ti_loose3', 'L2_2j15_a4TTem_deta25_L12TAU11ITAU15'], 'L1_2TAU11I_TAU15', 1, 0,
                   'EF_tau27Ti_loose3_tau18Ti_loose3_vbf', ['EF_tau27Ti_loose3_tau18Ti_loose3', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU11ITAU15' ], doOrderL2=True, doOrderEF=True),


    CombinedChain2('tau27Ti_loose2_tau18Ti_loose2_vbf', 431, ['tauEtmiss'], [], [],
                   #'L2_tau27Ti_loose2_tau18Ti_loose2_vbf',['L2_tau27Ti_loose2_tau18Ti_loose2', 'L2_j0_a4TTem_j0_c4cchad_mjj0deta25_L12TAU11ITAU15'], 'L1_2TAU11I_TAU15', 1, 0,
                   #'EF_tau27Ti_loose2_tau18Ti_loose2_vbf',['EF_tau27Ti_loose2_tau18Ti_loose2', 'EF_j0_a4tchad_mjj400deta28_L2FS_l2deta2p5_L12TAU11ITAU15' ],doOrderL2=True,doOrderEF=True),
                   'L2_tau27Ti_loose2_tau18Ti_loose2_vbf', ['L2_tau27Ti_loose2_tau18Ti_loose2', 'L2_2j15_a4TTem_deta25_L12TAU11ITAU15'], 'L1_2TAU11I_TAU15', 1, 0,
                   'EF_tau27Ti_loose2_tau18Ti_loose2_vbf', ['EF_tau27Ti_loose2_tau18Ti_loose2', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU11ITAU15' ], doOrderL2=True, doOrderEF=True),
    
    CombinedChain2('tau27T_loose2_tau18T_loose2_vbf_fj', 349, ['tauEtmiss'], [], [],
                   'L2_tau27T_loose2_tau18T_loose2_vbf_fj', ['L2_tau27Ti_loose2_tau18Ti_loose2', 'L2_2j15_a4TTem_deta25_L12TAU8TAU15FJ15'], 'L1_2TAU8_TAU15_FJ15', 1, 0,
                   'EF_tau27T_loose2_tau18T_loose2_vbf_fj', ['EF_tau27Ti_loose2_tau18Ti_loose2', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU8TAU15FJ15' ], doOrderL2=True, doOrderEF=True),
    
    CombinedChain2('tau27T_loose2_tau18T_loose2_vbf_cf', 444, ['tauEtmiss'], [], [],
                   'L2_tau27T_loose2_tau18T_loose2_vbf_cf', ['L2_tau27Ti_loose2_tau18Ti_loose2', 'L2_2j15_a4TTem_deta25_L12TAU8TAU15J15CF'], 'L1_2TAU8_TAU15_J15CF', 1, 0,
                   'EF_tau27T_loose2_tau18T_loose2_vbf_cf', ['EF_tau27Ti_loose2_tau18Ti_loose2', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU8TAU15J15CF' ], doOrderL2=True, doOrderEF=True),
    
]

for item in TauTau_VBF :
    item.groupL2 += ["RATE:MultiTau",'BW:Tau']
    item.groupEF += ["RATE:MultiTau",'BW:Tau']    

TauJetsEMScale = [


    CombinedChain2('tau29_loose_j45_a4tc_EFFS_xe40_noMu', 5313, ['tauEtmiss'], [],[],\
                   'L2_tau29_loose_j40_xe20_noMu', ['L2_xe20_noMu','L2_tau29_loose', 'L2_j40'],
                   'L1_TAU11_2J10_J20_XE20', 1, 0,
                   'EF_tau29_loose_j45_a4tc_EFFS_xe40_noMu', ['EF_xe40_noMu','EF_tau29_loose','EF_j45_a4tc_EFFS'], doOrderL2=True, doOrderEF=True),
    CombinedChain2('tau12_j75_a4tc_EFFS_xe40_loose_noMu', '4062', ['combined', 'comb__taujmet'], [],[],\
                   'L2_tau12_j70_xe20_loose_noMu',              [], 'L1_TAU6_J50_XE20', 1, 0, \
                   'EF_tau12_j75_a4tc_EFFS_xe40_loose_noMu',      ['EF_xe40_loose_noMu','EF_tau12_loose','EF_j75_a4tc_EFFS'], 1, 0),

    CombinedChain2('tau16_j75_a4tc_EFFS_xe40_loose_noMu', '4063', ['combined', 'comb__taujmet'], [],[],\
                   'L2_tau16_j70_xe20_loose_noMu',              [], 'L1_TAU6_J50_XE20', 1, 0, \
                   'EF_tau16_j75_a4tc_EFFS_xe40_loose_noMu',      ['EF_xe40_loose_noMu','EF_tau16_loose','EF_j75_a4tc_EFFS'], 1, 0),
                   ]

for item in TauJetsEMScale:
    item.groupL2 += [ "RATE:JetTau",'BW:Tau']
    item.groupEF += [ "RATE:JetTau",'BW:Tau']



JetBjetsEMScale = []

for item in JetBjetsEMScale :
    item.groupL2 += [ "RATE:SingleBjet", "BW:Bjets"]
    item.groupEF += [ "RATE:SingleBjet", "BW:Bjets"]


JetBjets = [
#L1_J75 -> L2_2b50_loose_j140_c4cchad -> EF_2b55_loose_j145_j55_a4tchad
#L1_J75 -> L2_2b50_loose_j140_c4cchad -> EF_2b45_loose_j145_j45_a4tchad
#L1_3J15_J50 -> L2_2b30_loose_j105_2j30_c4cchad -> EF_2b35_loose_j110_2j35_a4tchad 

    CombinedChain2('b35_medium_j35_a4tchad_vbf_3L1J15_FJ15', 6,['combined', 'jets'], [], [],
                   'L2_b30_medium_j30_c4cchad_vbf_3L1J15_FJ15',['L2_b30_medium_j30_c4cchad_3L1J15_FJ15','L2_2j15_a4TTem_deta25_L13J15FJ15'],'L1_3J15_FJ15', 1, 0,
                   'EF_b35_medium_j35_a4tchad_vbf_3L1J15_FJ15',['EF_b35_medium_j35_a4tchad_3L1J15_FJ15','EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L13J15FJ15' ],doOrderL2=True, doOrderEF=True),

    CombinedChain2('b35_medium_j35_a4tchad_L2FS_vbf_3L1J15_FJ15', 12,['combined', 'jets'], [], [],
                   'L2_b30_medium_j30_c4cchad_L2FS_vbf_3L1J15_FJ15',['L2_b30_medium_j30_a4TTem_3L1J15_FJ15','L2_2j15_a4TTem_deta25_L13J15FJ15'],'L1_3J15_FJ15', 1, 0,
                   'EF_b35_medium_j35_a4tchad_L2FS_vbf_3L1J15_FJ15',['EF_b35_medium_j35_a4tchad_L2FS_3L1J15_FJ15', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L13J15FJ15' ],doOrderL2=True, doOrderEF=True),


    CombinedChain2('b35_NoCut_j35_a4tchad_vbf_3L1J15_FJ15', 201,['combined', 'jets'], [], [],
                   'L2_b30_NoCut_j30_a4tchad_vbf_3L1J15_FJ15',['L2_b30_NoCut_j30_c4cchad_3L1J15_FJ15','L2_2j15_a4TTem_deta25_L13J15FJ15'],'L1_3J15_FJ15', 1, 0,
                   'EF_b35_NoCut_j35_a4tchad_vbf_3L1J15_FJ15',['EF_b35_NoCut_j35_a4tchad_3L1J15_FJ15', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L13J15FJ15' ],doOrderL2=True, doOrderEF=True),

    CombinedChain2('b35_NoCut_j35_a4tchad_L2FS_vbf_3L1J15_FJ15', 214,['combined', 'jets'], [], [],
                   'L2_b30_NoCut_j30_a4tchad_L2FS_vbf_3L1J15_FJ15',['L2_b30_NoCut_j30_a4TTem_3L1J15_FJ15','L2_2j15_a4TTem_deta25_L13J15FJ15'],'L1_3J15_FJ15', 1, 0,
                   'EF_b35_NoCut_j35_a4tchad_L2FS_vbf_3L1J15_FJ15',['EF_b35_NoCut_j35_a4tchad_L2FS_3L1J15_FJ15', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L13J15FJ15' ],doOrderL2=True, doOrderEF=True),


    CombinedChain2('b35_medium_j35_a4tchad_vbf_2L1FJ15', 254,['combined', 'jets'], [], [],
                   'L2_b30_medium_j30_c4cchad_vbf_2L1FJ15',['L2_b30_medium_j30_c4cchad_2L1FJ15','L2_2j15_a4TTem_deta25_2L1FJ15'],'L1_2FJ15', 1, 0,
                   'EF_b35_medium_j35_a4tchad_vbf_2L1FJ15',['EF_b35_medium_j35_a4tchad_2L1FJ15', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_2L1FJ15' ],doOrderL2=True, doOrderEF=True),


    CombinedChain2('b35_medium_j35_a4tchad_L2FS_vbf_2L1FJ15', 256,['combined', 'jets'], [], [],
                   'L2_b30_medium_j30_c4cchad_L2FS_vbf_2L1FJ15',['L2_b30_medium_j30_a4TTem_2L1FJ15','L2_2j15_a4TTem_deta25_2L1FJ15'],'L1_2FJ15', 1, 0,
                   'EF_b35_medium_j35_a4tchad_L2FS_vbf_2L1FJ15',['EF_b35_medium_j35_a4tchad_L2FS_2L1FJ15', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_2L1FJ15' ],doOrderL2=True, doOrderEF=True),


    CombinedChain2('b35_NoCut_j35_a4tchad_vbf_2L1FJ15', 281,['combined', 'jets'], [], [],
                   'L2_b30_NoCut_j30_a4tchad_vbf_2L1FJ15',['L2_b30_NoCut_j30_c4cchad_2L1FJ15','L2_2j15_a4TTem_deta25_2L1FJ15'],'L1_2FJ15', 1, 0,
                   'EF_b35_NoCut_j35_a4tchad_vbf_2L1FJ15',['EF_b35_NoCut_j35_a4tchad_2L1FJ15', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_2L1FJ15' ],doOrderL2=True, doOrderEF=True),

    CombinedChain2('b35_NoCut_j35_a4tchad_L2FS_vbf_2L1FJ15', 324,['combined', 'jets'], [], [],
                   'L2_b30_NoCut_j30_a4tchad_L2FS_vbf_2L1FJ15',['L2_b30_NoCut_j30_a4TTem_2L1FJ15','L2_2j15_a4TTem_deta25_2L1FJ15'],'L1_2FJ15', 1, 0,
                   'EF_b35_NoCut_j35_a4tchad_L2FS_vbf_2L1FJ15',['EF_b35_NoCut_j35_a4tchad_L2FS_2L1FJ15', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_2L1FJ15' ],doOrderL2=True, doOrderEF=True),

    ]

for item in JetBjets:
    item.groupL2 += [ "RATE:SingleBjet", "BW:Bjets"]
    item.groupEF += [ "RATE:SingleBjet", "BW:Bjets"]
            
if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()):
    PhotonBjets = [
        CombinedChain('g40_loose_b10_medium',    '217', 'L1_EM30', ['EF_g40_loose', 'EF_b10_medium'], ['egamma'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        CombinedChain('g40_tight_b15_medium',    '2000', 'L1_EM30', ['EF_g40_tight', 'EF_b15_medium'], ['egamma'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        CombinedChain('g40_tight_b10_medium',    '13', 'L1_EM30', ['EF_g40_tight', 'EF_b10_medium'], ['egamma'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        ]

    for item in PhotonBjets :
        item.group += [ "RATE:PhotonBjet", "BW:Egamma"]
else:
    PhotonBjets = []

TauMETs = []
TauMETs_v2v3 = [
    CombinedChain('tau16_loose_xe20_noMu',      '226', 'L1_TAU6_XE10',  ['EF_tau16_loose',  'EF_xe20_noMu'], ['tauEtmiss'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),    
    CombinedChain('tau16_loose_xe25_noMu',      '700', 'L1_TAU6_XE10',  ['EF_tau16_loose',  'EF_xe25_noMu'], ['tauEtmiss'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    CombinedChain('tau16_loose_xe30_noMu',      '702', 'L1_TAU6_XE10', ['EF_tau16_loose', 'EF_xe30_noMu_L1XE10'], ['tauEtmiss'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
#    CombinedChain('tau50_loose_xe55',          '102', 'L1_TAU40_XE40', ['EF_tau50_loose', 'EF_xe55'], ['tauEtmiss'], 
#                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    ]
    
for item in TauMETs+TauMETs_v2v3:
    item.group += [ "RATE:METTau",'BW:Tau']

TauMET_IdScan = []
TauMET_IdScan_v2v3 = [
    CombinedChain('tau16_medium_xe22_noMu',      '723', 'L1_TAU6_XE10',  ['EF_tau16_medium',  'EF_xe22_noMu'], ['tauEtmiss'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    CombinedChain('tau16_medium_xe25_noMu',      '724', 'L1_TAU6_XE10',  ['EF_tau16_medium',  'EF_xe25_noMu'], ['tauEtmiss'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    CombinedChain('tau16_medium_xe25_tight_noMu','725', 'L1_TAU6_XE10',  ['EF_tau16_medium',  'EF_xe25_tight_noMu_L1XE10'], ['tauEtmiss'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),    
]

for item in TauMET_IdScan+TauMET_IdScan_v2v3 :
    item.group += [ "RATE:METTau",'BW:Tau']

TauMET2s = [


    CombinedChain2('tau29T_medium_xe40_tight', 6341, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium_xe35_tight', [], 'L1_TAU15_XE35', 1, 0,
                   'EF_tau29T_medium_xe40_tight', ['EF_tau29T_medium','EF_xe40_tight'], 1, 0, doOrderL2=True,doOrderEF=True),
    
    CombinedChain2('tau29T_medium1_xe40_tight', 55, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium1_xe35_tight', [], 'L1_TAU15_XE35', 1, 0,
                   'EF_tau29T_medium1_xe40_tight', [ 'EF_tau29T_medium1','EF_xe40_tight'], 1, 0, doOrderL2=True,doOrderEF=True),
    
    CombinedChain2('tau29T_medium_xe45_tight', 6956, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium_xe40_tight', [], 'L1_TAU15_XE40', 1, 0,
                   'EF_tau29T_medium_xe45_tight', [ 'EF_tau29T_medium','EF_xe45_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    CombinedChain2('tau29T_medium1_xe45_tight', 57, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium1_xe40_tight', [], 'L1_TAU15_XE40', 1, 0,
                   'EF_tau29T_medium1_xe45_tight', [ 'EF_tau29T_medium1','EF_xe45_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    CombinedChain2('tau29Ti_medium_xe40_tight', 6985, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_medium_xe35_tight', [], 'L1_TAU15I_XE35', 1, 0,
                   'EF_tau29Ti_medium_xe40_tight', [ 'EF_tau29Ti_medium','EF_xe40_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    CombinedChain2('tau29Ti_medium1_xe40_tight', 186, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_medium1_xe35_tight', [], 'L1_TAU15I_XE35', 1, 0,
                   'EF_tau29Ti_medium1_xe40_tight', [ 'EF_tau29Ti_medium1','EF_xe40_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    CombinedChain2('tau29Ti_medium_xe45_tight', 6987, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_medium_xe40_tight', [], 'L1_TAU15I_XE40', 1, 0,
                   'EF_tau29Ti_medium_xe45_tight', [ 'EF_tau29Ti_medium','EF_xe45_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    CombinedChain2('tau29Ti_medium1_xe45_tight', 188, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_medium1_xe40_tight', [], 'L1_TAU15I_XE40', 1, 0,
                   'EF_tau29Ti_medium1_xe45_tight', [ 'EF_tau29Ti_medium1','EF_xe45_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    CombinedChain2('tau29Ti_medium1_llh_xe45_tight', 176, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_medium1_llh_xe40_tight', [], 'L1_TAU15I_XE40', 1, 0,
                   'EF_tau29Ti_medium1_llh_xe45_tight', ['EF_tau29Ti_medium1_llh','EF_xe45_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    CombinedChain2('tau38T_medium1_xe45_tight', 189, ['tauEtmiss'], [], [],
                   'L2_tau38T_medium1_xe40_tight', [], 'L1_TAU20_XE40', 1, 0,
                   'EF_tau38T_medium1_xe45_tight', ['EF_tau38T_medium1','EF_xe45_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    ## added for sampe T
    CombinedChain2('tau29Ti_medium1_llh_xe40_tight', 209, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_medium1_llh_xe35_tight', [], 'L1_TAU15I_XE35', 1, 0,
                   'EF_tau29Ti_medium1_llh_xe40_tight', ['EF_tau29Ti_medium1_llh','EF_xe40_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    ##New
    CombinedChain2('tau29Ti_medium1_xe55_tclcw', 168, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_medium1_xe40', [], 'L1_TAU15I_XE35', 1, 0,
                   'EF_tau29Ti_medium1_xe55_tclcw', [ 'EF_tau29Ti_medium1','EF_xe55_tclcw'], 1, 0, doOrderL2=True,doOrderEF=True),
    ##L2 pass through test chain
    CombinedChain2('tau29Ti_loose1_L2NoCut_xe55_tclcw', 5529, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_loose1_L2NoCut_xe40', [], 'L1_TAU15I_XE35', 1, 0,
                   'EF_tau29Ti_loose1_L2NoCut_xe55_tclcw', [ 'EF_tau29Ti_loose1_L2NoCut','EF_xe55_tclcw'], 1, 0, doOrderL2=True,doOrderEF=True),
    CombinedChain2('tau29Ti_loose2_L2NoCut_xe55_tclcw', 5527, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_loose2_L2NoCut_xe40', [], 'L1_TAU15I_XE35', 1, 0,
                   'EF_tau29Ti_loose2_L2NoCut_xe55_tclcw', [ 'EF_tau29Ti_loose2_L2NoCut','EF_xe55_tclcw'], 1, 0, doOrderL2=True,doOrderEF=True),
    CombinedChain2('tau29Ti_medium1_L2NoCut_xe55_tclcw', 5528, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_medium1_L2NoCut_xe40', [], 'L1_TAU15I_XE35', 1, 0,
                   'EF_tau29Ti_medium1_L2NoCut_xe55_tclcw', [ 'EF_tau29Ti_medium1_L2NoCut','EF_xe55_tclcw'], 1, 0, doOrderL2=True,doOrderEF=True),
    
    ##New
    CombinedChain2('tau38T_medium1_xe55_tclcw', 904, ['tauEtmiss'], [], [],
                   'L2_tau38T_medium1_xe40', [], 'L1_TAU20_XE35', 1, 0,
                   'EF_tau38T_medium1_xe55_tclcw', [ 'EF_tau38T_medium1','EF_xe55_tclcw'], 1, 0, doOrderL2=True,doOrderEF=True),
    
    CombinedChain2('tau29Ti_medium1_xe50_tclcw_tight', 905, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_medium1_xe35_tight', [], 'L1_TAU15I_XE35', 1, 0,
                   'EF_tau29Ti_medium1_xe50_tclcw_tight', [ 'EF_tau29Ti_medium1','EF_xe50_tclcw_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    CombinedChain2('tau38T_medium1_xe50_tclcw_tight', 906, ['tauEtmiss'], [], [],
                   'L2_tau38T_medium1_xe35_tight', [], 'L1_TAU20_XE35', 1, 0,
                   'EF_tau38T_medium1_xe50_tclcw_tight', [ 'EF_tau38T_medium1','EF_xe50_tclcw_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    CombinedChain2('tau29T_medium1_xe50_tclcw_tight', 892, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium1_xe35_tight', [], 'L1_TAU15_XE35', 1, 0,
                   'EF_tau29T_medium1_xe50_tclcw_tight', [ 'EF_tau29T_medium1','EF_xe50_tclcw_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    
    CombinedChain2('tau29T_medium1_xe55_tclcw', 893, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium1_xe40', [], 'L1_TAU15_XE35', 1, 0,
                   'EF_tau29T_medium1_xe55_tclcw', [ 'EF_tau29T_medium1','EF_xe55_tclcw'], 1, 0, doOrderL2=True,doOrderEF=True),


    CombinedChain2('tau29T_medium1_xe55_tclcw_tight', 894, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium1_xe40_tight', [], 'L1_TAU15_XE40', 1, 0,
                   'EF_tau29T_medium1_xe55_tclcw_tight', [ 'EF_tau29T_medium1','EF_xe55_tclcw'], 1, 0, doOrderL2=True,doOrderEF=True),

    ###
    
    CombinedChain2('tau29Ti_medium1_xe55_tclcw_tight', 169, ['tauEtmiss'], [], [],
                   'L2_tau29Ti_medium1_xe40_tight', [], 'L1_TAU15I_XE40', 1, 0,
                   'EF_tau29Ti_medium1_xe55_tclcw_tight', [ 'EF_tau29Ti_medium1','EF_xe55_tclcw'], 1, 0, doOrderL2=True,doOrderEF=True),

    CombinedChain2('tau38T_medium1_xe55_tclcw_tight', 170, ['tauEtmiss'], [], [],
                   'L2_tau38T_medium1_xe40_tight', [], 'L1_TAU20_XE40', 1, 0,
                   'EF_tau38T_medium1_xe55_tclcw_tight', [ 'EF_tau38T_medium1','EF_xe55_tclcw_tight'], 1, 0, doOrderL2=True,doOrderEF=True),
    
    CombinedChain2('tau38T_medium1_xe40_tight', 167, ['tauEtmiss'], [], [],
                   'L2_tau38T_medium1_xe35_tight', [], 'L1_TAU20_XE35', 1, 0,
                   'EF_tau38T_medium1_xe40_tight', [ 'EF_tau38T_medium1','EF_xe40_tight'], 1, 0, doOrderL2=True,doOrderEF=True),
    

    #Chains with looser pT thresholds at L2 and EF
    CombinedChain2('tau29T_medium1_L2loose_xe50_tclcw_tight', 3920, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium1_L2loose_xe35_tight', [], 'L1_TAU15_XE35', 1, 0,
                   'EF_tau29T_medium1_L2loose_xe50_tclcw_tight', [ 'EF_tau29T_medium1_L2loose','EF_xe50_tclcw_tight'], 1, 0, doOrderL2=True,doOrderEF=True),
    CombinedChain2('tau27T_medium1_L2loose_xe50_tclcw_tight', 558, ['tauEtmiss'], [], [],
                   'L2_tau27T_medium1_L2loose_xe35_tight', [], 'L1_TAU15_XE35', 1, 0,
                   'EF_tau27T_medium1_L2loose_xe50_tclcw_tight', [ 'EF_tau27T_medium1_L2loose','EF_xe50_tclcw_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    CombinedChain2('tau27T_loose3_xe50_tclcw_tight', 6271, ['tauEtmiss'], [], [],
                   'L2_tau27T_loose3_xe35_tight', [], 'L1_TAU15_XE35', 1, 0,
                   'EF_tau27T_loose3_xe50_tclcw_tight', [ 'EF_tau27T_loose3','EF_xe50_tclcw_tight'], 1, 0, doOrderL2=True,doOrderEF=True),

    ]

TauMET2s_v2v3 = [
    CombinedChain2('2tau29_loose1_EFxe15_noMu', '4055', ['tauEtmiss'], [],[],\
                   'L2_2tau29_loose1_EFxe15_noMu', [], 'L1_2TAU11', 1, 0,
                   'EF_2tau29_loose1_EFxe15_noMu', ['EF_tau29_loose1']*2+['EF_xe15_noL2'], 1, 0), 
    CombinedChain2('2tau29_loose1_EFxe20_noMu', '4056', ['tauEtmiss'], [],[],\
                   'L2_2tau29_loose1_EFxe20_noMu', [], 'L1_2TAU11', 1, 0,
                   'EF_2tau29_loose1_EFxe40_noMu', ['EF_tau29_loose1']*2+['EF_xe20_noL2'], 1, 0), 
    CombinedChain2('tau29_medium_xe35_noMu', 70, ['tauEtmiss'], [], [],
                   'L2_tau29_medium_xe25_noMu', [], 'L1_TAU11_XE20', 1, 0,
                   'EF_tau29_medium_xe35_noMu', [ 'EF_xe35_noMu', 'EF_tau29_medium'], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('tau29_medium_xe40_loose_noMu', 71, ['tauEtmiss'], [], [],
                   'L2_tau29_medium_xe30_loose_noMu', [], 'L1_TAU11_XE20', 1, 0,
                   'EF_tau29_medium_xe40_loose_noMu', ['EF_xe40_loose_noMu', 'EF_tau29_medium'], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('tau29_medium_xs80_loose_noMu', 5170, ['tauEtmiss'], [], [],
                   'L2_tau29_medium_xs50_noMu_1', [], 'L1_TAU11_XS30', 1, 0,
                   'EF_tau29_medium_xs80_loose_noMu', [ 'EF_xs80_loose_noMu','EF_tau29_medium'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('tau29_medium_xs70_noMu', 595, ['tauEtmiss'], [], [],
                   'L2_tau29_medium_xs50_noMu', [], 'L1_TAU11_XS35', 1, 0, 
                   'EF_tau29_medium_xs70_noMu', ['EF_xs70_noMu', 'EF_tau29_medium'], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('tau29_loose_xs75_noMu', 746, ['tauEtmiss'], [], [],
                   'L2_tau29_loose_xs50_noMu', [], 'L1_TAU11_XS35', 1, 0, 
                   'EF_tau29_loose_xs75_noMu', ['EF_xs75_noMu', 'EF_tau29_loose'], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('tau29_medium_xs75_noMu', 868, ['tauEtmiss'], [], [],
                   'L2_tau29_medium_xs50_noMu', [], 'L1_TAU11_XS35', 1, 0, 
                   'EF_tau29_medium_xs75_noMu', ['EF_xs75_noMu', 'EF_tau29_medium'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('tau29T_medium_xs70_noMu', 4331, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium_xs50_noMu_1', ['L2_xs50_noMu', 'L2_tau29T_medium'], 'L1_TAU15_XS35', 1, 0, 
                   'EF_tau29T_medium_xs70_noMu', ['EF_xs70_noMu', 'EF_tau29T_medium'], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('tau29T_loose_xs75_noMu', 4330, ['tauEtmiss'], [], [],
                   'L2_tau29T_loose_xs50_noMu', ['L2_xs50_noMu', 'L2_tau29T_loose'], 'L1_TAU15_XS35', 1, 0, 
                   'EF_tau29T_loose_xs75_noMu', ['EF_xs75_noMu', 'EF_tau29T_loose'], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('tau29T_medium_xs75_noMu', 800, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium_xs50_noMu', ['L2_xs50_noMu', 'L2_tau29T_medium'], 'L1_TAU15_XS35', 1, 0, 
                   'EF_tau29T_medium_xs75_noMu', ['EF_xs75_noMu', 'EF_tau29T_medium'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('tau29T_medium_xs75_loose_noMu', 34 , ['tauEtmiss'], [], [],
                   'L2_tau29T_medium_xs35_noMu', [], 'L1_TAU15_XS35', 1, 0, 
                   'EF_tau29T_medium_xs75_loose_noMu', ['EF_xs75_loose_noMu', 'EF_tau29T_medium'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('tau29_loose_xs65_loose_noMu', 608, ['tauEtmiss'], [], [],
                   'L2_tau29_loose_xs40_noMu', [], 'L1_TAU11_XS35', 1, 0, 
                   'EF_tau29_loose_xs65_loose_noMu', ['EF_xs65_loose_noMu', 'EF_tau29_loose' ], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('tau29_loose_xs70_loose_noMu', 609, ['tauEtmiss'], [], [],
                   'L2_tau29_loose_xs35_noMu', [], 'L1_TAU11_XS30', 1, 0, 
                   'EF_tau29_loose_xs70_loose_noMu', ['EF_xs70_loose_noMu', 'EF_tau29_loose'], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('tau29_loose_xs75_loose_noMu', 610, ['tauEtmiss'], [], [],
                   'L2_tau29_loose_xs35_noMu', [], 'L1_TAU11_XS30', 1, 0, 
                   'EF_tau29_loose_xs75_loose_noMu', ['EF_xs75_loose_noMu','EF_tau29_loose'], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('tau29_loose_xs80_loose_noMu', 607, ['tauEtmiss'], [], [],
                   'L2_tau29_loose_xs35_noMu', [], 'L1_TAU11_XS30', 1, 0, 
                   'EF_tau29_loose_xs80_loose_noMu', ['EF_xs80_loose_noMu', 'EF_tau29_loose' ], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('tau29_medium_xs45_loose_noMu_3L1J10', 5172, ['tauEtmiss'], [], [],
                   'L2_tau29_medium_xs20_noMu_3L1J10', [], 'L1_TAU11_XE10_3J10', 1, 0,
                   'EF_tau29_medium_xs45_loose_noMu_3L1J10', ['EF_xs45_loose_noMu_L1XE10', 'EF_tau29_medium'], 1, 0, doOrderL2=True),
    CombinedChain2('tau29_loose_xs45_loose_noMu_3L1J10', 229, ['tauEtmiss'], [], [],
                   'L2_tau29_loose_xs20_noMu_3L1J10', [], 'L1_TAU11_XE10_3J10', 1, 0,
                   'EF_tau29_loose_xs45_loose_noMu_3L1J10', ['EF_xs45_loose_noMu_L1XE10', 'EF_tau29_loose'], 1, 0, doOrderL2=True),
    CombinedChain2('tau29_loose1_xs45_loose_noMu_3L1J10', 306, ['tauEtmiss'], [], [],
                   'L2_tau29_loose1_xs20_noMu_3L1J10', [], 'L1_TAU11_XE10_3J10', 1, 0,
                   'EF_tau29_loose1_xs45_loose_noMu_3L1J10', ['EF_xs45_loose_noMu_L1XE10', 'EF_tau29_loose1'], 1, 0, doOrderL2=True),
    CombinedChain2('tau29_loose_xs60_loose_noMu_3L1J10', 6072, ['tauEtmiss'], [], [],
                   'L2_tau29_loose_xs20_noMu_3L1J10', [], 'L1_TAU11_XE10_3J10', 1, 0,
                   'EF_tau29_loose_xs60_loose_noMu_3L1J10', ['EF_xs60_loose_noMu_L1XE10', 'EF_tau29_loose'], 1, 0, doOrderL2=True),

    CombinedChain2('tau29_medium1_xs45_loose_noMu_3L1J10', 359, ['tauEtmiss'], [], [],
                   'L2_tau29_medium1_xs20_noMu_3L1J10', [], 'L1_TAU11_XE10_3J10', 1, 0,
                   'EF_tau29_medium1_xs45_loose_noMu_3L1J10', ['EF_xs45_loose_noMu_L1XE10', 'EF_tau29_medium1'], 1, 0, doOrderL2=True),

    CombinedChain2('tau29T_medium1_xs45_loose_noMu_3L1J10', 254, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium1_xs20_noMu_3L1J10', [], 'L1_TAU15_XE10_3J10', 1, 0,
                   'EF_tau29T_medium1_xs45_loose_noMu_3L1J10', ['EF_xs45_loose_noMu_L1XE10', 'EF_tau29T_medium1'], 1, 0, doOrderL2=True),

    CombinedChain2('tau29T_medium_xe35_noMu_3L1J10', 197, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium_xe25_noMu_3L1J10', [], 'L1_TAU15_XE25_3J10', 1, 0,
                   'EF_tau29T_medium_xe35_noMu_3L1J10', ['EF_xe35_noMu', 'EF_tau29T_medium'], 1, 0, doOrderL2=True),

    CombinedChain2('tau29T_medium_xs45_loose_noMu_3L1J10', 42, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium_xs20_noMu_3L1J10', [], 'L1_TAU15_XE10_3J10', 1, 0,
                   'EF_tau29T_medium_xs45_loose_noMu_3L1J10', ['EF_xs45_loose_noMu_L1XE10', 'EF_tau29T_medium'], 1, 0, doOrderL2=True),

    CombinedChain2('tau29T_medium_xs45_noMu_3L1J10', 44, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium_xs25_noMu_3L1J10', [], 'L1_TAU15_XS25_3J10', 1, 0,
                   'EF_tau29T_medium_xs45_noMu_3L1J10', ['EF_xs45_medium_noMu', 'EF_tau29T_medium'], 1, 0, doOrderL2=True),
    
    CombinedChain2('tau29T_medium1_xe35_noMu_3L1J10', 45, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium1_xe25_noMu_3L1J10', [], 'L1_TAU15_XE25_3J10', 1, 0,
                   'EF_tau29T_medium1_xe35_noMu_3L1J10', ['EF_xe35_noMu', 'EF_tau29T_medium1'], 1, 0, doOrderL2=True),

    CombinedChain2('tau29T_medium1_xs45_noMu_3L1J10', 103, ['tauEtmiss'], [], [],
                   'L2_tau29T_medium1_xs25_noMu_3L1J10', [], 'L1_TAU15_XS25_3J10', 1, 0,
                   'EF_tau29T_medium1_xs45_noMu_3L1J10', ['EF_xs45_medium_noMu', 'EF_tau29T_medium1'], 1, 0, doOrderL2=True),
    ]

for item in TauMET2s+TauMET2s_v2v3:
    item.groupL2 += ["RATE:METTau",'BW:Tau']
    item.groupEF += ["RATE:METTau",'BW:Tau']

TauMET2_IdScan = []
TauMET2_IdScan_v2v3 = []

for item in TauMET2_IdScan+TauMET2_IdScan_v2v3:
    item.groupL2 += ["RATE:METTau",'BW:Tau']
    item.groupEF += ["RATE:METTau",'BW:Tau']

# to be l2star for v4
ElectronMETs_eIdScan = [
    CombinedChain2('e24vh_medium1_EFxe40', '374', ['combined','comb__emet'], [], [],
                   'L2_e24vh_medium1_EFxe40', ['L2_e24vh_medium1', 'L2_noL2'], 'L1_EM18VH', 1, 0, 
                   'EF_e24vh_medium1_EFxe40', ['EF_e24vh_medium1', 'EF_xe40_noL2'], 1, 0),  
    
    CombinedChain2('e24vh_medium1_EFxe30', '373', ['combined','comb__emet'], [], [],
                   'L2_e24vh_medium1_EFxe30', ['L2_e24vh_medium1', 'L2_noL2'], 'L1_EM18VH', 1, 0, 
                   'EF_e24vh_medium1_EFxe30', ['EF_e24vh_medium1', 'EF_xe30_noL2'], 1, 0),

    CombinedChain2('e24vh_medium1_EFxe35_tclcw', '406', ['combined','comb__emet'], [], [],
                   'L2_e24vh_medium1_EFxe35', ['L2_e24vh_medium1', 'L2_noL2'], 'L1_EM18VH', 1, 0, 
                   'EF_e24vh_medium1_EFxe35_tclcw', ['EF_e24vh_medium1', 'EF_xe35_tclcw_noL2'], 1, 0),

    
    CombinedChain2('e24vh_medium1_EFxe35_tcem', '413', ['combined','comb__emet'], [], [],
                   'L2_e24vh_medium1_EFxe35', ['L2_e24vh_medium1', 'L2_noL2'], 'L1_EM18VH', 1, 0, 
                   'EF_e24vh_medium1_EFxe35_tcem', ['EF_e24vh_medium1', 'EF_xe35_tcem_noL2'], 1, 0),
    
    CombinedChain2('e24vh_medium1_EFxe30_tcem', '819', ['combined','comb__emet'], [], [],
                   'L2_e24vh_medium1_EFxe30', ['L2_e24vh_medium1', 'L2_noL2'], 'L1_EM18VH', 1, 0, 
                   'EF_e24vh_medium1_EFxe30_tcem', ['EF_e24vh_medium1', 'EF_xe30_tcem_noL2'], 1, 0),


    ###W T&P triggers
    CombinedChain2('e13_etcutTrk_xs60_dphi2j15xe20', '266', ['combined','comb__emet'], [], [],\
                   'L2_e13_etcutTrk_xs45', ['L2_e13_etcutTrk','L2_xs45'], 'L1_EM6_XS45', 1, 0,
                   'EF_e13_etcutTrk_xs60_dphi2j15xe20', ['EF_e13_etcutTrk','EF_xs60','EF_j15_a4tchad'], 1, 0,
                   #'L2_e13_etcutTrk_xs45', ['L2_e13_etcutTrk','L2_xs45_unseeded'], 'L1_EM6_XS45', 1, 0,
                   #'EF_e13_etcutTrk_xs60_dphi2j15xe20', ['EF_e13_etcutTrk','EF_xs60_unseeded','EF_j15_a4tchad'], 1, 0,
                   dPhiMetJet=True, dPhiCut="dPhi_20_2j15",doOrderL2=True, doOrderEF=True),

    ##CombinedChain2('e15_etcutTrk_xs60_dphi2j15xe20', '267', ['combined','comb__emet'], [], [],\
    ##               'L2_e15_etcutTrk_xs45', ['L2_xs45','L2_e15_etcutTrk'], 'L1_EM12_XS45', 1, 0,
    ##               'EF_e15_etcutTrk_xs60_dphi2j15xe20', ['EF_xs60','EF_j15_a4tchad','EF_e15_etcutTrk'], 1, 0,
    ##               dPhiMetJet=True, dPhiCut="dPhi_20_2j15",doOrderL2=True, doOrderEF=True),

    CombinedChain2('e13_etcutTrk_xs60', '268', ['combined','comb__emet'], [], [],\
                   'L2_e13_etcutTrk_xs45', ['L2_e13_etcutTrk','L2_xs45'], 'L1_EM6_XS45', 1, 0,
                   'EF_e13_etcutTrk_xs60', ['EF_e13_etcutTrk','EF_xs60'], 1, 0 ,doOrderL2=True, doOrderEF=True),

    
    ##CombinedChain2('e15_etcutTrk_xs60', '269', ['combined','comb__emet'], [], [],\
    ##               'L2_e15_etcutTrk_xs45', ['L2_xs45','L2_e15_etcutTrk'], 'L1_EM12_XS45', 1, 0,
    ##               'EF_e15_etcutTrk_xs60', ['EF_xs60','EF_e15_etcutTrk'], 1, 0,doOrderL2=True, doOrderEF=True),

    ## #95562
    ## L1_EM6_XS50 -> L2_e13_etcutTrk_xs50 -> EF_e13_etcutTrk_xs45+METiso
    ## L1_EM6_XS50 -> L2_e13_etcutTrk_xs50 -> EF_e13_etcutTrk_xs50+METiso
    ## L1_EM6_XS55 -> L2_e13_etcutTrk_xs55 -> EF_e13_etcutTrk_xs45+METiso
    ## L1_EM6_XS60 -> L2_e13_etcutTrk_xs60 -> EF_e13_etcutTrk_xs45+METiso
    ## L1_EM6_XS60 -> L2_e13_etcutTrk_xs60 -> EF_e13_etcutTrk_xs45
    ## L1_EM16V_XS45 -> L2_e20_etcutTrk_xs45 -> EF_e20_etcutTrk_xs45+METiso 
    
    CombinedChain2('e13_etcutTrk_xs45_L1XS50_dphi2j15xe20', '1000', ['combined','comb__emet'], [], [],\
                   'L2_e13_etcutTrk_xs50', ['L2_e13_etcutTrk','L2_xs50'], 'L1_EM6_XS50', 1, 0,
                   'EF_e13_etcutTrk_xs45_L1XS50_dphi2j15xe20', ['EF_e13_etcutTrk','EF_xs45_L1XS50','EF_j15_a4tchad'], 1, 0,
                   dPhiMetJet=True, dPhiCut="dPhi_20_2j15",doOrderL2=True, doOrderEF=True),

    CombinedChain2('e13_etcutTrk_xs50_L1XS50_dphi2j15xe20', '11', ['combined','comb__emet'], [], [],\
                   'L2_e13_etcutTrk_xs50', ['L2_e13_etcutTrk','L2_xs50'], 'L1_EM6_XS50', 1, 0,
                   'EF_e13_etcutTrk_xs50_L1XS50_dphi2j15xe20', ['EF_e13_etcutTrk','EF_xs50_L1XS50','EF_j15_a4tchad'], 1, 0,
                   dPhiMetJet=True, dPhiCut="dPhi_20_2j15",doOrderL2=True, doOrderEF=True),

    CombinedChain2('e13_etcutTrk_xs45_L1XS55_dphi2j15xe20', '933', ['combined','comb__emet'], [], [],\
                   'L2_e13_etcutTrk_xs55', ['L2_e13_etcutTrk','L2_xs55'], 'L1_EM6_XS55', 1, 0,
                   'EF_e13_etcutTrk_xs45_L1XS55_dphi2j15xe20', ['EF_e13_etcutTrk','EF_xs45_L1XS55','EF_j15_a4tchad'], 1, 0,
                   dPhiMetJet=True, dPhiCut="dPhi_20_2j15",doOrderL2=True, doOrderEF=True),

    CombinedChain2('e13_etcutTrk_xs45_L1XS60_dphi2j15xe20', '781', ['combined','comb__emet'], [], [],\
                  'L2_e13_etcutTrk_xs60', ['L2_e13_etcutTrk','L2_xs60'], 'L1_EM6_XS60', 1, 0,
                  'EF_e13_etcutTrk_xs45_L1XS60_dphi2j15xe20', ['EF_e13_etcutTrk','EF_xs45_L1XS60','EF_j15_a4tchad'], 1, 0,
                  dPhiMetJet=True, dPhiCut="dPhi_20_2j15",doOrderL2=True, doOrderEF=True),
    
    CombinedChain2('e13_etcutTrk_xs45_L1XS60', '782', ['combined','comb__emet'], [], [],\
                   'L2_e13_etcutTrk_xs60', ['L2_e13_etcutTrk','L2_xs60'], 'L1_EM6_XS60', 1, 0,
                   'EF_e13_etcutTrk_xs45_L1XS60', ['EF_e13_etcutTrk','EF_xs45_L1XS60'], 1, 0 ,doOrderL2=True, doOrderEF=True),
    
    CombinedChain2('e20_etcutTrk_xs45_L1XS45_dphi2j15xe20', '783', ['combined','comb__emet'], [], [],\
                   'L2_e20_etcutTrk_xs45', ['L2_e20_etcutTrk','L2_xs45'], 'L1_EM16V_XS45', 1, 0,
                   'EF_e20_etcutTrk_xs45_L1XS45_dphi2j15xe20', ['EF_e20_etcutTrk','EF_xs45_L1XS45','EF_j15_a4tchad'], 1, 0,
                   dPhiMetJet=True, dPhiCut="dPhi_20_2j15",doOrderL2=True, doOrderEF=True),
    
    
    ##Supporting/fallback trigger
    CombinedChain2('e20_etcutTrk_xs60_dphi2j15xe20', '270', ['combined','comb__emet'], [], [],\
                   'L2_e20_etcutTrk_xs45', ['L2_e20_etcutTrk','L2_xs45'], 'L1_EM16V_XS45', 1, 0,
                   'EF_e20_etcutTrk_xs60_dphi2j15xe20', ['EF_e20_etcutTrk','EF_xs60','EF_j15_a4tchad'], 1, 0,
                   dPhiMetJet=True, dPhiCut="dPhi_20_2j15",doOrderL2=True, doOrderEF=True),
    
    CombinedChain2('e20_etcutTrk_xe30_dphi2j15xe20', '271', ['combined','comb__emet'], [], [],\
                   'L2_e20_etcutTrk_xe25', ['L2_e20_etcutTrk','L2_xe25'], 'L1_EM16V_XE20', 1, 0,
                   'EF_e20_etcutTrk_xe30_dphi2j15xe20', ['EF_e20_etcutTrk','EF_xe30','EF_j15_a4tchad'], 1, 0,
                   dPhiMetJet=True, dPhiCut="dPhi_20_2j15",doOrderL2=True, doOrderEF=True),
    ]

ElectronMETs_eIdScan_v2v3 = [
    CombinedChain('e20_loose_xe20', '231', 'L1_EM14_XE10', ['EF_e20_loose', 'EF_xe20'], ['combined','comb__emet'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    CombinedChain('e20_loose_xe20_noMu', '589', 'L1_EM14_XE10', ['EF_e20_loose', 'EF_xe20_noMu'], ['combined','comb__emet'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0', pass_throughEF='0'), 
    CombinedChain('e20_loose_xe30', '233', 'L1_EM14_XE20', ['EF_e20_loose', 'EF_xe30'], ['combined','comb__emet'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    CombinedChain('e20_loose_xe30_noMu', '124', 'L1_EM14_XE20', ['EF_e20_loose', 'EF_xe30_noMu'], ['combined','comb__emet'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    CombinedChain('e15_tight_xe30_noMu',       '753', 'L1_EM10_XE20', ['EF_e15_tight',  'EF_xe30_noMu'], ['combined','comb__emet'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 
    
    CombinedChain2('e18_medium_xe30_noMu', '775', ['combined','comb__emet'], [], [],
                   'L2_e18_medium_xe20_noMu', ['L2_e18_medium','L2_xe20_noMu_L1EM14XE20'], 'L1_EM14_XE20', 1, 0,
                   'EF_e18_medium_xe30_noMu', ['EF_e18_medium', 'EF_xe30_noMu_L1EM14XE20'], 1, 0),
    ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.NoVHThresholdMenus():
    ElectronMETs_eIdScan_v2v3 += [
        CombinedChain2('e15_medium_xe30_noMu', '752', ['combined','comb__emet'], [], [],
                       'L2_e15_medium_xe20_noMu', ['L2_e15_medium','L2_xe20_noMu'], 'L1_EM10_XE20', 1, 0, 
                       'EF_e15_medium_xe30_noMu', ['EF_e15_medium', 'EF_xe30_noMu'], 1, 0), 
        CombinedChain2('e15_medium_xe35_noMu', '750', ['combined','comb__emet'], [], [],
                       'L2_e15_medium_xe25_noMu', ['L2_e15_medium','L2_xe25_noMu'], 'L1_EM10_XE20', 1, 0, 
                       'EF_e15_medium_xe35_noMu', ['EF_e15_medium', 'EF_xe35_noMu'], 1, 0), 
        CombinedChain2('e15_medium_xe35_loose_noMu', '757', ['combined','comb__emet'], [], [],
                       'L2_e15_medium_xe20_noMu', ['L2_e15_medium','L2_xe20_noMu_L1EM14XE20'], 'L1_EM10_XE20', 1, 0, 
                       'EF_e15_medium_xe35_loose_noMu', ['EF_e15_medium', 'EF_xe35_noMu_L1EM10XE20'], 1, 0), 
        
        CombinedChain('e15_medium_xe40_loose_noMu','751', 'L1_EM10_XE20', ['EF_e15_medium', 'EF_xe40_noMu'],
                      ['combined','comb__emet'], prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 

        CombinedChain2('e15_medium_xe40_noMu', '378', ['combined','comb__emet'], [], [],
                       'L2_e15_medium_xe30_noMu', ['L2_e15_medium','L2_xe30_noMu'], 'L1_EM10_XE30', 1, 0, 
                       'EF_e15_medium_xe40_noMu', ['EF_e15_medium', 'EF_xe40_noMu'], 1, 0),
        CombinedChain2('e15_medium_xe50_noMu', '379', ['combined','comb__emet'], [], [],
                       'L2_e15_medium_xe35_noMu', ['L2_e15_medium','L2_xe35_noMu'], 'L1_EM10_XE30', 1, 0, 
                       'EF_e15_medium_xe50_noMu', ['EF_e15_medium', 'EF_xe50_noMu'], 1, 0),        
        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.VHThresholdMenus():
    ElectronMETs_eIdScan_v2v3 += [
        CombinedChain2('e15vh_medium_xe40_noMu', '378', ['combined','comb__emet'], [], [],
                       'L2_e15vh_medium_xe30_noMu', ['L2_e15vh_medium','L2_xe30_noMu'], 'L1_EM10VH_XE30', 1, 0, 
                       'EF_e15vh_medium_xe40_noMu', ['EF_e15vh_medium', 'EF_xe40_noMu'], 1, 0),

        CombinedChain2('e15vh_medium_xe50_noMu', '379', ['combined','comb__emet'], [], [],
                       'L2_e15vh_medium_xe35_noMu', ['L2_e15vh_medium','L2_xe35_noMu'], 'L1_EM10VH_XE35', 1, 0, 
                       'EF_e15vh_medium_xe50_noMu', ['EF_e15vh_medium', 'EF_xe50_noMu'], 1, 0),  

        CombinedChain2('e22vh_medium1_EFxe20_noMu', '370', ['combined','comb__emet'], [], [],
                       'L2_e22vh_medium1_EFxe20_noMu', ['L2_e22vh_medium1', 'L2_noL2_noMu'], 'L1_EM16VH', 1, 0, 
                       'EF_e22vh_medium1_EFxe20_noMu', ['EF_e22vh_medium1', 'EF_xe20_noL2_noMu'], 1, 0),  

        CombinedChain2('e22vh_medium1_EFxe30_noMu', '371', ['combined','comb__emet'], [], [],
                       'L2_e22vh_medium1_EFxe30_noMu', ['L2_e22vh_medium1', 'L2_noL2_noMu'], 'L1_EM16VH', 1, 0, 
                       'EF_e22vh_medium1_EFxe30_noMu', ['EF_e22vh_medium1', 'EF_xe30_noL2_noMu'], 1, 0),  

        CombinedChain2('e22vh_medium1_EFxe40_noMu', '372', ['combined','comb__emet'], [], [],
                       'L2_e22vh_medium1_EFxe40_noMu', ['L2_e22vh_medium1', 'L2_noL2_noMu'], 'L1_EM16VH', 1, 0, 
                       'EF_e22vh_medium1_EFxe40_noMu', ['EF_e22vh_medium1', 'EF_xe40_noL2_noMu'], 1, 0),  
        ]


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    ElectronMETs_eIdScan_v2v3 += [
        CombinedChain2('e13_etcut_xs60_noMu', '232', ['combined','comb__emet'], [], [],\
                       'L2_e13_etcut_xs45_noMu', ['L2_xs45_noMu_L1EM10XS45','L2_e13_etcut'], 'L1_EM10_XS45', 1, 0,
                       'EF_e13_etcut_xs60_noMu', ['EF_xs60_noMu_L1EM10XS45','EF_e13_etcut'], 1, 0, doOrderL2=True, doOrderEF=True),
        CombinedChain2('e20_etcut_xs60_noMu', '504', ['combined','comb__emet'], [], [],\
                       'L2_e20_etcut_xs45_noMu', ['L2_xs45_noMu_unseeded','L2_e20_etcut'], 'L1_EM14', 1, 0,
                       'EF_e20_etcut_xs60_noMu', ['EF_xs60_noMu_unseeded','EF_e20_etcut'], 1, 0, doOrderL2=True, doOrderEF=True),
        CombinedChain2('e13_etcutTRT_xs60_noMu', '2810', ['combined','comb__emet'], [], [],\
                       'L2_e13_etcut_xs45_noMu', ['L2_xs45_noMu_L1EM10XS45','L2_e13_etcut'], 'L1_EM10_XS45', 1, 0,
                       'EF_e13_etcutTRT_xs60_noMu', ['EF_xs60_noMu_L1EM10XS45','EF_e13_etcutTRT'], 1, 0, doOrderL2=True, doOrderEF=True),
        CombinedChain2('e13_etcut_xs70_noMu', '230', ['combined','comb__emet'], [], [],\
                       'L2_e13_etcut_xs45_noMu', ['L2_xs45_noMu_L1EM10XS45','L2_e13_etcut'], 'L1_EM10_XS45', 1, 0,
                       'EF_e13_etcut_xs70_noMu', ['EF_xs70_noMu_L1EM10XS45', 'EF_e13_etcut'], 1, 0, doOrderL2=True, doOrderEF=True),
##         CombinedChain2('e13_etcutTrk_xs60_noMu', '232', ['combined','comb__emet'], [], [],\
##                        'L2_e13_etcutTrk_xs45_noMu', ['L2_xs45_noMu_L1EM10XS45','L2_e13_etcutTrk'], 'L1_EM10_XS45', 1, 0,
##                        'EF_e13_etcutTrk_xs60_noMu', ['EF_xs60_noMu_L1EM10XS45','EF_e13_etcutTrk'], 1, 0, doOrderL2=True, doOrderEF=True),
        CombinedChain2('e22_etcut_xs60_noMu', '505', ['combined','comb__gmet'], [], [],\
                       'L2_e22_etcut_xs45_noMu', ['L2_xs45_noMu_unseeded','L2_e22_etcut'], 'L1_EM16', 1, 0,
                       'EF_e22_etcut_xs60_noMu', ['EF_xs60_noMu_unseeded','EF_e22_etcut'], 1, 0, doOrderL2=True, doOrderEF=True),
        ]
    
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
    ElectronMETs_eIdScan_v2v3 += [
        CombinedChain2('e13_etcut_xs60_noMu', '232', ['combined','comb__emet'], [], [],\
                       'L2_e13_etcut_xs45_noMu', ['L2_xs45_noMu_unseeded','L2_e13_etcut_EM7'], 'L1_EM7_XS45', 1, 0,
                       'EF_e13_etcut_xs60_noMu', ['EF_xs60_noMu_unseeded','EF_e13_etcut_EM7'], 1, 0, doOrderL2=True, doOrderEF=True),
        CombinedChain2('e13_etcutTrk_xs60_noMu', '232', ['combined','comb__emet'], [], [],\
                       'L2_e13_etcutTrk_xs45_noMu', ['L2_xs45_noMu_unseeded','L2_e13_etcutTrk_EM7'], 'L1_EM7_XS45', 1, 0,
                       'EF_e13_etcutTrk_xs60_noMu', ['EF_xs60_noMu_unseeded','EF_e13_etcutTrk_EM7'], 1, 0, doOrderL2=True, doOrderEF=True),
        ]



for item in ElectronMETs_eIdScan+ElectronMETs_eIdScan_v2v3 :
    if issubclass(item.__class__, CombinedChain2):
        item.groupL2 += [ "RATE:ElectronMET",'BW:Egamma']
        item.groupEF += [ "RATE:ElectronMET",'BW:Egamma']
    else:
        item.group += [ "RATE:ElectronMET",'BW:Egamma']

MuonMETs =[
    CombinedChain2('mu24_tight_EFxe40', '877', ['combined','comb__mumet'], [], [],\
                   'L2_mu24_tight_EFxe40', ['L2_mu24_tight','L2_noL2'], 'L1_MU15', 1, 0,
                   'EF_mu24_tight_EFxe40', ['EF_mu24_tight', 'EF_xe40_noL2'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('2mu8_EFxe30', '603', ['combined','comb__mumet'], [], [],\
                   'L2_2mu8_EFxe30', ['L2_2mu8','L2_noL2'], 'L1_2MU6', 1, 0,
                   'EF_2mu8_EFxe30', ['EF_2mu8', 'EF_xe30_noL2'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('2mu4T_xe60', '601', ['combined','comb__mumet'], [], [],\
                   'L2_2mu4T_xe60', ['L2_2mu4T','L2_xe45'], 'L1_2MU4_XE40', 1, 0,
                   'EF_2mu4T_xe60', ['EF_2mu4T', 'EF_xe60'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('2mu4T_xe50_tclcw', '640', ['combined','comb__mumet'], [], [],\
                   'L2_2mu4T_xe35', ['L2_2mu4T','L2_xe35'], 'L1_2MU4_XE30', 1, 0,
                   'EF_2mu4T_xe50_tclcw', ['EF_2mu4T', 'EF_xe50_tclcw'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('2mu4T_xe60_tclcw', '602', ['combined','comb__mumet'], [], [],\
                   'L2_2mu4T_xe45', ['L2_2mu4T','L2_xe45'], 'L1_2MU4_XE40', 1, 0,
                   'EF_2mu4T_xe60_tclcw', ['EF_2mu4T', 'EF_xe60_tclcw'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('2mu8_EFxe30_tclcw', '604', ['combined','comb__mumet'], [], [],\
                   'L2_2mu8_EFxe30', ['L2_2mu8','L2_noL2'], 'L1_2MU6', 1, 0,
                   'EF_2mu8_EFxe30_tclcw', ['EF_2mu8', 'EF_xe30_tclcw_noL2'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('2mu8_EFxe40_tclcw', '760', ['combined','comb__mumet'], [], [],\
                   'L2_2mu8_EFxe40', ['L2_2mu8','L2_noL2'], 'L1_2MU6', 1, 0,
                   'EF_2mu8_EFxe40_tclcw', ['EF_2mu8', 'EF_xe40_tclcw_noL2'], 1, 0, doOrderL2=True, doOrderEF=True),


    ##CombinedChain2('2mu8_EFxe30_tclcw', '604', ['combined','comb__mumet'], [], [],\
    ##               'L2_2mu4T_xe45', ['L2_2mu4T','L2_xe45'], 'L1_2MU4_XE40', 1, 0,
    ##               'EF_2mu4T_xe60_tclcw', ['EF_2mu4T', 'EF_xe60_tclcw'], 1, 0, doOrderL2=True, doOrderEF=True),
    ##


    ]    


MuonMETs_v2v3 = [
    CombinedChain('mu15_EFxe45', '631', 'L1_MU10', ['EF_mu15', 'EF_xe45_noL2_noMu'], ['combined', 'comb__mumet' ], prescale_L2='1', prescale_EF='1', pass_throughL2='0', pass_throughEF='0'),

    CombinedChain2('mu15_xe30_noMu', '403', ['combined','comb__mumet'], [], [],\
                   'L2_mu15_xe20_noMu', ['L2_mu15','L2_xe20_noMu_L1MU10XE20'], 'L1_MU10_XE20', 1, 0,
                   'EF_mu15_xe30_noMu', ['EF_mu15', 'EF_xe30_noMu_L1MU10XE20'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('mu15_xe35_noMu', '404', ['combined','comb__mumet'], [], [],\
                   'L2_mu15_xe20_noMu', ['L2_mu15','L2_xe20_noMu_L1MU10XE20'], 'L1_MU10_XE20', 1, 0,
                   'EF_mu15_xe35_noMu', ['EF_mu15', 'EF_xe35_noMu_L1MU10XE20'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('mu15_medium_EFxs65_noMu', '6076', ['combined','comb__mumet'], [], [],\
                   'L2_mu15_medium_EFxs65_noMu', ['L2_mu15_medium','L2_noL2_noMu'], 'L1_MU11', 1, 0,
                   'EF_mu15_medium_EFxs65_noMu', ['EF_mu15_medium', 'EF_xs65_noL2_noMu'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('mu15_medium_EFxs70_noMu', '6077', ['combined','comb__mumet'], [], [],\
                   'L2_mu15_medium_EFxs70_noMu', ['L2_mu15_medium','L2_noL2_noMu'], 'L1_MU11', 1, 0,
                   'EF_mu15_medium_EFxs70_noMu', ['EF_mu15_medium', 'EF_xs70_noL2_noMu'], 1, 0, doOrderL2=True, doOrderEF=True),
    
    CombinedChain2('mu15_medium_EFxs75_noMu', '6078', ['combined','comb__mumet'], [], [],\
                   'L2_mu15_medium_EFxs75_noMu', ['L2_mu15_medium','L2_noL2_noMu'], 'L1_MU11', 1, 0,
                   'EF_mu15_medium_EFxs75_noMu', ['EF_mu15_medium', 'EF_xs75_noL2_noMu'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('mu24_medium_EFxe40_noMu', '305', ['combined','comb__mumet'], [], [],\
                   'L2_mu24_medium_EFxe40_noMu', ['L2_mu24_medium','L2_noL2_noMu'], 'L1_MU11', 1, 0,
                   'EF_mu24_medium_EFxe40_noMu', ['EF_mu24_medium', 'EF_xe40_noL2_noMu'], 1, 0, doOrderL2=True, doOrderEF=True),


    # reverting Iwona's changes from July 27, 2011 (see below the commented lines, "attempt to fix harmless config warnings in HI")
    # as they overwrite the Physics/MC_pp_v2 menu
    # adding the three chains below to this MuonMETs again, commenting out the lines below
   
    
    CombinedChain2('2mu4T_EFxe20_noMu', '770', ['combined','comb__mumet'], [], [],\
                   'L2_2mu4T_EFxe20_noMu', ['L2_2mu4T','L2_noL2_noMu'], 'L1_2MU4', 1, 0,
                   'EF_2mu4T_EFxe20_noMu', ['EF_2mu4T','EF_xe20_noL2_noMu'], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('2mu4T_EFxe30_noMu', '771', ['combined','comb__mumet'], [], [],\
                   'L2_2mu4T_EFxe30_noMu', ['L2_2mu4T','L2_noL2_noMu'], 'L1_2MU4', 1, 0,
                   'EF_2mu4T_EFxe30_noMu', ['EF_2mu4T','EF_xe30_noL2_noMu'], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('2mu4T_xe30_noMu', '781', ['combined','comb__mumet'], [], [],\
                   'L2_2mu4T_xe20_noMu', ['L2_2mu4T','L2_xe20_noMu_unseeded'], 'L1_2MU4', 1, 0,
                   'EF_2mu4T_xe30_noMu', ['EF_2mu4T','EF_xe30_noMu_unseeded'], 1, 0, doOrderL2=True, doOrderEF=True),
    CombinedChain2('2mu4T_xe40_noMu', '773', ['combined','comb__mumet'], [], [],\
                   'L2_2mu4T_xe25_noMu', ['L2_2mu4T','L2_xe25_noMu_unseeded'], 'L1_2MU4', 1, 0,
                   'EF_2mu4T_xe40_noMu', ['EF_2mu4T','EF_xe40_noMu_unseeded'], 1, 0, doOrderL2=True, doOrderEF=True),
    ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    MuonMETs_v2v3 += [
        CombinedChain2('2mu4_EFxe20_noMu', '6178', ['combined','comb__mumet'], [], [],\
                       'L2_2mu4_EFxe20_noMu', ['L2_2mu4','L2_noL2_noMu'], 'L1_2MU0', 1, 0,
                       'EF_2mu4_EFxe20_noMu', ['EF_2mu4', 'EF_xe20_noL2_noMu'], 1, 0, doOrderL2=True, doOrderEF=True),
        
        CombinedChain2('2mu4_xe30_noMu', '6179', ['combined','comb__mumet'], [], [],\
                       'L2_2mu4_xe20_noMu', ['L2_2mu4','L2_xe20_noMu'], 'L1_2MU0_XE20', 1, 0,
                       'EF_2mu4_xe30_noMu', ['EF_2mu4', 'EF_xe30_noMu'], 1, 0, doOrderL2=True, doOrderEF=True),
    
        CombinedChain2('2mu4_xe40_noMu', '6180', ['combined','comb__mumet'], [], [],\
                       'L2_2mu4_xe25_noMu', ['L2_2mu4','L2_xe25_medium_noMu_L1XE25'], 'L1_2MU0_XE25', 1, 0,
                       'EF_2mu4_xe40_noMu', ['EF_2mu4', 'EF_xe40_medium_noMu_L1XE25'], 1, 0, doOrderL2=True, doOrderEF=True),
        ]
    
for item in MuonMETs+MuonMETs_v2v3 :
    if issubclass(item.__class__, CombinedChain2):
        item.groupL2 += [ "RATE:METMuon",'BW:Muon']
        item.groupEF += [ "RATE:METMuon",'BW:Muon']
    else:
        item.group += [ "RATE:METMuon",'BW:Muon']


ElectronJetsEMScale = [    

    CombinedChain2('g5_medium_2j5_a4tcem_EFFS_L1EM3', '916', ['combined','comb__ej'], [], [], 
                   'L2_g5_medium_2j5_a4tcem_EFFS_L1EM3', ['L2_g5_medium'], 'L1_EM3',1, 0, 
                   'EF_g5_medium_2j5_a4tcem_EFFS_L1EM3', ['EF_2j5_a4tcem_EFFS_L1EM3', 'EF_g5_medium'], 1, 0),
    
    CombinedChain2('g5_medium_2j10_a4tcem_EFFS_L1EM3', '101', ['combined','comb__ej'], [], [],
                   'L2_g5_medium_2j10_a4tcem_EFFS_L1EM3', ['L2_g5_medium'], 'L1_EM3',1, 0,
                   'EF_g5_medium_2j10_a4tcem_EFFS_L1EM3', ['EF_2j10_a4tcem_EFFS_L1EM3', 'EF_g5_medium'], 1, 0),

    CombinedChain2('g7_medium_2j7_a4tcem_EFFS_L1EM3', '102', ['combined','comb__ej'], [], [],
                   'L2_g7_medium_2j7_a4tcem_EFFS_L1EM3', ['L2_g7_medium'], 'L1_EM3',1, 0,
                   'EF_g7_medium_2j7_a4tcem_EFFS_L1EM3', ['EF_2j7_a4tcem_EFFS_L1EM3', 'EF_g7_medium'], 1, 0),

    CombinedChain2('g7_medium_2j10_a4tchad_EFFS_L1EM3', '106', ['combined','comb__ej'], [], [],
                   'L2_g7_medium_2j10_a4tchad_EFFS_L1EM3', ['L2_g7_medium'], 'L1_EM3',1, 0,
                   'EF_g7_medium_2j10_a4tchad_EFFS_L1EM3', ['EF_2j10_a4tchad_EFFS_L1EM3', 'EF_g7_medium'], 1, 0),

    CombinedChain2('g10_medium_2j10_a4tchad_EFFS_L1EM5', '109', ['combined','comb__ej'], [], [],
                   'L2_g10_medium_2j10_a4tchad_EFFS_L1EM5', ['L2_g10_medium'], 'L1_EM5',1, 0,
                   'EF_g10_medium_2j10_a4tchad_EFFS_L1EM5', ['EF_2j10_a4tchad_EFFS_L1EM5', 'EF_g10_medium'], 1, 0),

    CombinedChain2('g7_medium_2j10_a4hi_EFFS_L1EM3', '972', ['combined','comb__ej'], [], [],
                   'L2_g7_medium_2j10_a4hi_EFFS_L1EM3', ['L2_g7_medium'], 'L1_EM3',1, 0,
                   'EF_g7_medium_2j10_a4hi_EFFS_L1EM3', ['EF_2j10_a4hi_EFFS_L1EM3', 'EF_g7_medium'], 1, 0),

    CombinedChain2('g10_medium_2j10_a4hi_EFFS_L1EM5', '1001', ['combined','comb__ej'], [], [],
                   'L2_g10_medium_2j10_a4hi_EFFS_L1EM5', ['L2_g10_medium'], 'L1_EM5',1, 0,
                   'EF_g10_medium_2j10_a4hi_EFFS_L1EM5', ['EF_2j10_a4hi_EFFS_L1EM5', 'EF_g10_medium'], 1, 0),

    CombinedChain2('e20_loose_2j10_a4tc_EFFS', '916', ['combined','comb__ej'], [], [], 
                   'L2_e20_loose_2j10_a4tc_EFFS', ['L2_e20_loose'], 'L1_EM14',1, 0, 
                   'EF_e20_loose_2j10_a4tc_EFFS', ['EF_2j10_a4tc_EFFS', 'EF_e20_loose'], 1, 0),
    
    CombinedChain2('e20_loose_3j10_a4tc_EFFS', '917', ['combined','comb__ej'], [], [], 
                   'L2_e20_loose_3j10_a4tc_EFFS', ['L2_e20_loose'], 'L1_EM14',1, 0, 
                   'EF_e20_loose_3j10_a4tc_EFFS', ['EF_3j10_a4tc_EFFS', 'EF_e20_loose'], 1, 0),
    
    CombinedChain2('e20_loose_4j10_a4tc_EFFS', '918', ['combined','comb__ej'], [], [], 
                   'L2_e20_loose_4j10_a4tc_EFFS', ['L2_e20_loose'], 'L1_EM14',1, 0, 
                   'EF_e20_loose_4j10_a4tc_EFFS', ['EF_4j10_a4tc_EFFS', 'EF_e20_loose'], 1, 0),
    
    CombinedChain2('e22vh_loose_2j20_a4tc_EFFS', '916', ['combined','comb__ej'], [], [], 
                   'L2_e22vh_loose_2j20_a4tc_EFFS', ['L2_e22vh_loose'], 'L1_EM16VH',1, 0, 
                   'EF_e22vh_loose_2j20_a4tc_EFFS', ['EF_2j20_a4tc_EFFS', 'EF_e22vh_loose'], 1, 0),
    
    CombinedChain2('e22vh_loose_3j20_a4tc_EFFS', '917', ['combined','comb__ej'], [], [], 
                   'L2_e22vh_loose_3j20_a4tc_EFFS', ['L2_e22vh_loose'], 'L1_EM16VH',1, 0, 
                   'EF_e22vh_loose_3j20_a4tc_EFFS', ['EF_3j20_a4tc_EFFS', 'EF_e22vh_loose'], 1, 0),
    
    CombinedChain2('e22vh_loose_4j15_a4tc_EFFS', '911', ['combined','comb__ej'], [], [], 
                   'L2_e22vh_loose_4j15_a4tc_EFFS', ['L2_e22vh_loose'], 'L1_EM16VH',1, 0, 
                   'EF_e22vh_loose_4j15_a4tc_EFFS', ['EF_4j15_a4tc_EFFS', 'EF_e22vh_loose'], 1, 0),

    CombinedChain2('e22vh_loose_3L1J10', '17', ['combined','comb__ej'], [], [],
                   'L2_e22vh_loose_3L1J10', ['L2_e22vh_loose'], 'L1_EM12_3J10',1, 0,
                   'EF_e22vh_loose_3L1J10', ['EF_e22vh_loose'], 1, 0),
    
    CombinedChain2('e22vh_loose_4L1J10', '18', ['combined','comb__ej'], [], [],
                   'L2_e22vh_loose_4L1J10', ['L2_e22vh_loose'], 'L1_EM12_4J10',1, 0,
                   'EF_e22vh_loose_4L1J10', ['EF_e22vh_loose'], 1, 0),

]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.VHThresholdMenus():
    ElectronJetsEMScale += [

        CombinedChain2('e22vh_medium1_3j10_a4tc_EFFS', '931', ['combined','comb__ej'], [], [], 
                       'L2_e22vh_medium1', ['L2_e22vh_medium1'], 'L1_EM16VH',1, 0, 
                       'EF_e22vh_medium1_3j10_a4tc_EFFS', ['EF_3j10_a4tc_EFFS', 'EF_e22vh_medium1'], 1, 0),
        
        CombinedChain2('e22vh_medium1_4j10_a4tc_EFFS', '934', ['combined','comb__ej'], [], [], 
                       'L2_e22vh_medium1', ['L2_e22vh_medium1'], 'L1_EM16VH',1, 0, 
                       'EF_e22vh_medium1_4j10_a4tc_EFFS', ['EF_4j10_a4tc_EFFS', 'EF_e22vh_medium1'], 1, 0),
        
        CombinedChain2('e22vh_medium1_3j15_a4tc_EFFS', '935', ['combined','comb__ej'], [], [], 
                       'L2_e22vh_medium1', ['L2_e22vh_medium1'], 'L1_EM16VH',1, 0, 
                       'EF_e22vh_medium1_3j15_a4tc_EFFS', ['EF_3j15_a4tc_EFFS', 'EF_e22vh_medium1'], 1, 0),

        ]
    
for item in ElectronJetsEMScale :
    if issubclass(item.__class__, CombinedChain2):
        item.groupL2 += [ "RATE:ElectronJet",'BW:Egamma']
        item.groupEF += [ "RATE:ElectronJet",'BW:Egamma']
    else:
        item.group += [ "RATE:ElectronJet",'BW:Egamma']

ElectronJET_tcem = [    

    #92131
    #with L2 jets...
    #e45 loose1 j180
    CombinedChain2('j180_a10tcem_e45_loose1', '593', ['combined','comb__ej'], [], [], 
                   'L2_j100_a10tecm_e45_loose1', ['L2_e45_loose1', 'L2_j100_a10TTem'], 'L1_EM30',1, 0, 
                   'EF_j180_a10tcem_e45_loose1', ['EF_e45_loose1', 'EF_j180_a10tcem'], 1, 0, doOrderL2=True, doOrderEF=True),
        
    #e45 loose1 j240
    CombinedChain2('j240_a10tcem_e45_loose1', '596', ['combined','comb__ej'], [], [], 
                   'L2_j100_a10tcem_e45_loose1', ['L2_e45_loose1', 'L2_j100_a10TTem'], 'L1_EM30',1, 0, 
                   'EF_j240_a10tcem_e45_loose1', ['EF_e45_loose1', 'EF_j240_a10tcem'], 1, 0, doOrderL2=True, doOrderEF=True),

    #e60 loose1 j240
    CombinedChain2('j240_a10tcem_e60_loose1', '598', ['combined','comb__ej'], [], [], 
                   'L2_j100_a10tcem_e60_loose1', ['L2_e60_loose1', 'L2_j100_a10TTem'], 'L1_EM30',1, 0, 
                   'EF_j240_a10tcem_e60_loose1', ['EF_e60_loose1', 'EF_j240_a10tcem'], 1, 0, doOrderL2=True, doOrderEF=True),

    #without L2 jets below...
    #e45 loose1 j220
    CombinedChain2('j220_a10tcem_e45_loose1', '643', ['combined','comb__ej'], [], [], 
                   'L2_e45_loose1', ['L2_e45_loose1'], 'L1_EM30',1, 0, 
                   'EF_j220_a10tcem_e45_loose1', ['EF_j220_a10tcem', 'EF_e45_loose1'], 1, 0),

    #e45 etcut j240
    CombinedChain2('j240_a10tcem_e45_etcut', '644', ['combined','comb__ej'], [], [], 
                   'L2_e45_etcut', ['L2_e45_etcut'], 'L1_EM30',1, 0, 
                   'EF_j240_a10tcem_e45_etcut', ['EF_j240_a10tcem', 'EF_e45_etcut'], 1, 0),
    #e45 etcut j220
    CombinedChain2('j220_a10tcem_e45_etcut', '633', ['combined','comb__ej'], [], [], 
                   'L2_e45_etcut', ['L2_e45_etcut'], 'L1_EM30',1, 0, 
                   'EF_j220_a10tcem_e45_etcut', ['EF_j220_a10tcem', 'EF_e45_etcut'], 1, 0),
    #e60 etcut j240
    CombinedChain2('j240_a10tcem_e60_etcut', '5028', ['combined','comb__ej'], [], [], 
                   'L2_e60_etcut', ['L2_e60_etcut'], 'L1_EM30',1, 0, 
                   'EF_j240_a10tcem_e60_etcut', ['EF_j240_a10tcem', 'EF_e60_etcut'], 1, 0),
    #e60 etcut j220
    CombinedChain2('j220_a10tcem_e60_etcut', '5014', ['combined','comb__ej'], [], [], 
                   'L2_e60_etcut', ['L2_e60_etcut'], 'L1_EM30',1, 0, 
                   'EF_j220_a10tcem_e60_etcut', ['EF_j220_a10tcem', 'EF_e60_etcut'], 1, 0),
    #end 92131

    #Electron+Bjets
    CombinedChain2('e24vh_medium1_b35_mediumEF_j35_a4tchad', '158', ['combined','comb__ej'], [], [],
                   'L2_e24vh_medium1',  [], 'L1_EM18VH', 1, 0,
                   'EF_e24vh_medium1_b35_mediumEF_j35_a4tchad', ['EF_e24vh_medium1','EF_b35_mediumEF_j35_a4tchad'], 1, 0, doOrderEF=True),
    ]

for item in ElectronJET_tcem :
    if issubclass(item.__class__, CombinedChain2):
        item.groupL2 += [ "RATE:ElectronJet",'BW:Egamma']
        item.groupEF += [ "RATE:ElectronJet",'BW:Egamma']
    else:
        item.group += [ "RATE:ElectronJet",'BW:Egamma']


ElectronJET_tchad = [    

    #93947 for electron + jets backgrounds
    #e24_loose0_3j45_a4tchad_L2FS
    CombinedChain2('e24vh_loose0_3j45_a4tchad_L2FS', '704', ['combined','comb__ej'], [], [], 
                   'L2_e24vh_loose0_3j15_a4TTem',       ['L2_roi_dummy_3J15', 'L2_e24vh_loose0', 'L2_3j15_a4TTem'], 'L1_EM18VH',1, 0, 
                   'EF_e24vh_loose0_3j45_a4tchad_L2FS', ['EF_e24vh_loose0', 'EF_3j45_a4tchad_L2FS'], 1, 0, doOrderL2=True, doOrderEF=True),
    #e24_loose0_4j45_a4tchad_L2FS
    CombinedChain2('e24vh_loose0_4j45_a4tchad_L2FS', '705', ['combined','comb__ej'], [], [], 
                   'L2_e24vh_loose0_4j15_a4TTem',       ['L2_roi_dummy_3J15', 'L2_e24vh_loose0', 'L2_4j15_a4TTem'], 'L1_EM18VH',1, 0, 
                   'EF_e24vh_loose0_4j45_a4tchad_L2FS', ['EF_e24vh_loose0', 'EF_4j45_a4tchad_L2FS'], 1, 0, doOrderL2=True, doOrderEF=True),
    #e24_loose0_4j55_a4tchad_L2FS
    CombinedChain2('e24vh_loose0_4j55_a4tchad_L2FS', '706', ['combined','comb__ej'], [], [], 
                   'L2_e24vh_loose0_4j15_a4TTem',       ['L2_roi_dummy_3J15', 'L2_e24vh_loose0', 'L2_4j15_a4TTem'], 'L1_EM18VH',1, 0, 
                   'EF_e24vh_loose0_4j55_a4tchad_L2FS', ['EF_e24vh_loose0', 'EF_4j55_a4tchad_L2FS'], 1, 0, doOrderL2=True, doOrderEF=True),

    CombinedChain2('e18vh_medium1_vbf_fj', 519,['combined', 'comb__ej'], [], [],
                   'L2_e18vh_medium1_vbf_fj',['L2_e18vh_medium1','L2_2j15_a4TTem_deta25_L1EM14VHFJ15'],'L1_EM14VH_FJ15', 1, 0,
                   'EF_e18vh_medium1_vbf_fj',['EF_e18vh_medium1', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L1EM14VHFJ15' ],doOrderL2=True, doOrderEF=True),
    
    CombinedChain2('e18vh_medium1_vbf_cf', 520,['combined', 'comb__ej'], [], [],
                   'L2_e18vh_medium1_vbf_cf',['L2_e18vh_medium1','L2_2j15_a4TTem_deta25_L1EM14VHJ15CF'],'L1_EM14VH_J15CF', 1, 0,
                   'EF_e18vh_medium1_vbf_cf',['EF_e18vh_medium1', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L1EM14VHJ15CF' ],doOrderL2=True, doOrderEF=True),
    
    CombinedChain2('e18vh_medium1_vbf_2L1TAU11I_EM14VH', 564, ['combined', 'comb__ej'], [], [],
                   'L2_e18vh_medium1_vbf_2L1TAU11I_EM14VH', ['L2_e18vh_medium1', 'L2_2j15_a4TTem_deta25_L12TAU11IEM14VH'], 'L1_2TAU11I_EM14VH', 1, 0,
                   'EF_e18vh_medium1_vbf_2L1TAU11I_EM14VH', ['EF_e18vh_medium1', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU11IEM14VH' ], doOrderL2=True, doOrderEF=True),
    
    CombinedChain2('e12Tvh_loose1_vbf_L1TAU11I_EM10VH_FJ15', 523,['combined', 'comb__ej'], [], [],
                   'L2_e12Tvh_loose1_vbf_L1TAU11I_EM10VH_FJ15',['L2_e12Tvh_loose1','L2_2j15_a4TTem_deta25_L12TAU8TAU11IEM10VHFJ15'],'L1_2TAU8_TAU11I_EM10VH_FJ15', 1, 0,
                   'EF_e12Tvh_loose1_vbf_L1TAU11I_EM10VH_FJ15',['EF_e12Tvh_loose1', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU8TAU11IEM10VHFJ15' ],doOrderL2=True, doOrderEF=True),
    
    CombinedChain2('e12Tvh_loose1_vbf_L1TAU11I_EM10VH_J15CF', 592,['combined', 'comb__ej'], [], [],
                   'L2_e12Tvh_loose1_vbf_L1TAU11I_EM10VH_J15CF',['L2_e12Tvh_loose1','L2_2j15_a4TTem_deta25_L12TAU8TAU11IEM10VHJ15CF'],'L1_2TAU8_TAU11I_EM10VH_J15CF', 1, 0,
                   'EF_e12Tvh_loose1_vbf_L1TAU11I_EM10VH_J15CF',['EF_e12Tvh_loose1', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L12TAU8TAU11IEM10VHJ15CF' ],doOrderL2=True, doOrderEF=True),
    
]
for item in ElectronJET_tchad :
    if issubclass(item.__class__, CombinedChain2):
        item.groupL2 += [ "RATE:ElectronJet",'BW:Egamma']
        item.groupEF += [ "RATE:ElectronJet",'BW:Egamma']
    else:
        item.group += [ "RATE:ElectronJet",'BW:Egamma']


MuonJetsEMScale = [
  CombinedChain2('mu4_j30', '987', ['combined','jets'], [], [],
                 'L2_mu4_j25', ['L2_mu4', 'L2_j25' ], 'L1_MU0_J10', 1, 0,
                 'EF_mu4_j30', ['EF_mu4', 'EF_j30'], 1, 0),

  CombinedChain2('mu4_j30_jetNoCut', '943', ['combined','jets'], [], [],
                 'L2_mu4_j25_jetNoCut', ['L2_mu4', 'L2_j25_jetNoCut' ], 'L1_MU0_J10', 1, 0,
                 'EF_mu4_j30_jetNoCut', ['EF_mu4', 'EF_j30_jetNoCut'], 1, 0),

  CombinedChain2('mu4_j30_jetNoEF', '943', ['combined','jets'], [], [],
                 'L2_mu4_j25', ['L2_mu4', 'L2_j25' ], 'L1_MU0_J10', 1, 0,
                 'EF_mu4_j30_jetNoEF', ['EF_mu4', 'EF_j30_jetNoEF'], 1, 0),
  
  CombinedChain2('mu15_j90_a4EFFS',   '3235', ['combined', 'comb__muj'], [], [],
                 'L2_mu15_j90_a4EFFS',   ['L2_mu15'], 'L1_MU10', 1, 0,
                 'EF_mu15_j90_a4tc_EFFS', ['EF_mu15', 'EF_j90_a4tc_EFFS'], 1, 0),

  CombinedChain2('mu18_medium_2j20_a4tc_EFFS', '301', ['combined', 'comb__muj'], [], [],
                 'L2_mu18_medium_2j20_a4tc_EFFS',   ['L2_mu18_medium'], 'L1_MU11', 1, 0,
                 'EF_mu18_medium_2j20_a4tc_EFFS', ['EF_mu18_medium', 'EF_2j20_a4tc_EFFS'], 1, 0),

  CombinedChain2('mu18_medium_3j20_a4tc_EFFS', '302', ['combined', 'comb__muj'], [], [],
                 'L2_mu18_medium_3j20_a4tc_EFFS',   ['L2_mu18_medium'], 'L1_MU11', 1, 0,
                 'EF_mu18_medium_3j20_a4tc_EFFS', ['EF_mu18_medium', 'EF_3j20_a4tc_EFFS'], 1, 0),

  CombinedChain2('mu18_medium_4j15_a4tc_EFFS', '304', ['combined', 'comb__muj'], [], [],
                 'L2_mu18_medium_4j15_a4tc_EFFS',   ['L2_mu18_medium'], 'L1_MU11', 1, 0,
                 'EF_mu18_medium_4j15_a4tc_EFFS', ['EF_mu18_medium', 'EF_4j15_a4tc_EFFS'], 1, 0),

  CombinedChain2('mu18_medium_4j20_a4tc_EFFS', '303', ['combined', 'comb__muj'], [], [],
                 'L2_mu18_medium_4j20_a4tc_EFFS',   ['L2_mu18_medium'], 'L1_MU11', 1, 0,
                 'EF_mu18_medium_4j20_a4tc_EFFS', ['EF_mu18_medium', 'EF_4j20_a4tc_EFFS'], 1, 0),

  ]




MuonJPsi = [
    #E.P.
    #CombinedChain2('mu18_L2_2mu4T_Jpsimumu', '7960', ['combined','comb__multimu'], [], [],
    #               'L2_mu18_2mu4T_Jpsimumu',    ['L2_mu18','L2_2mu4T_Jpsimumu'], 'L1_2MU4_MU6', 1, 0,
    #              'EF_mu18_L2_2mu4T_Jpsimumu',    ['EF_mu18'], 1, 0),

    CombinedChain2('mu18_tight_L2_2mu4T_Jpsimumu', '873', ['combined','comb__multimu'], [], [],
                   'L2_mu18_tight_2mu4T_Jpsimumu',    ['L2_2mu4T_Jpsimumu','L2_mu18_tight'], 'L1_MU15', 1, 0,
                   'EF_mu18_tight_L2_2mu4T_Jpsimumu',    ['EF_mu18_tight'], 1, 0, doOrderL2=True),

    CombinedChain2('mu18_tight_L2_2mu4T_Jpsimumu_L2StarB', '323', ['combined','comb__multimu'], [], [],
                   'L2_mu18_tight_2mu4T_Jpsimumu_L2StarB',    ['L2_2mu4T_Jpsimumu_L2StarB','L2_mu18_tight'], 'L1_MU15', 1, 0,
                   'EF_mu18_tight_L2_2mu4T_Jpsimumu_L2StarB',    ['EF_mu18_tight'], 1, 0, doOrderL2=True),

]

for item in MuonJPsi:
     if issubclass(item.__class__, CombinedChain2):
         item.groupL2 += [ "RATE:MuonJPsi",'BW:Muon']
         item.groupEF += [ "RATE:MuonJPsi",'BW:Muon']
         print 'MEOW stream ', item.streams
     else:
         item.group +=  [ "RATE:MuonJPsi",'BW:Muon']
         print 'MEOW stream ', item.streams

           
         
        

MuonJets = [
    
    CombinedChain2('mu24_tight_2j35_a4tchad', '796', ['combined','comb__muj'], [], [],
                   'L2_mu24_tight_2j35_a4tchad',    ['L2_mu24_tight'], 'L1_MU15', 1, 0,
                   'EF_mu24_tight_2j35_a4tchad',    ['EF_mu24_tight', 'EF_2j35_a4tchad'], 1, 0),

    CombinedChain2('mu24_tight_3j35_a4tchad', '797', ['combined','comb__muj'], [], [],
                   'L2_mu24_tight_3j35_a4tchad',    ['L2_mu24_tight'], 'L1_MU15', 1, 0,
                   'EF_mu24_tight_3j35_a4tchad',    ['EF_mu24_tight', 'EF_3j35_a4tchad'], 1, 0),

    CombinedChain2('mu24_tight_3j45_a4tchad', '759', ['combined','comb__muj'], [], [],
                   'L2_mu24_tight_3j45_a4tchad',    ['L2_mu24_tight'], 'L1_MU15', 1, 0,
                   'EF_mu24_tight_3j45_a4tchad',    ['EF_mu24_tight', 'EF_3j45_a4tchad'], 1, 0),

    CombinedChain2('mu24_tight_4j35_a4tchad', '762', ['combined','comb__muj'], [], [],
                   'L2_mu24_tight_4j35_a4tchad',    ['L2_mu24_tight'], 'L1_MU15', 1, 0,
                   'EF_mu24_tight_4j35_a4tchad',    ['EF_mu24_tight', 'EF_4j35_a4tchad'], 1, 0),

    CombinedChain2('mu24_tight_4j45_a4tchad', '761', ['combined','comb__muj'], [], [],
                   'L2_mu24_tight_4j45_a4tchad',    ['L2_mu24_tight'], 'L1_MU15', 1, 0,
                   'EF_mu24_tight_4j45_a4tchad',    ['EF_mu24_tight', 'EF_4j45_a4tchad'], 1, 0),


    CombinedChain2('mu8_4j45_a4tchad_L2FS', '898', ['combined','comb__muj'], [], [],
                   'L2_mu8_4j15_a4TTem',    ['L2_mu8', 'L2_4j15_a4TTem'], 'L1_4J15', 1, 0,
                   'EF_mu8_4j45_a4tchad_L2FS',    ['EF_mu8', 'EF_4j45_a4tchad_L2FS'], 1, 0),

    CombinedChain2('2mu6i_DiMu_DY_2j25_a4tchad', '418', ['combined','comb__muj'], [], [],
                   'L2_2mu6i_DiMu_DY_2j25_a4tchad',    ['L2_2mu6i_DiMu_DY'], 'L1_2MU6', 1, 0,
                   'EF_2mu6i_DiMu_DY_2j25_a4tchad',    ['EF_2mu6i_DiMu_DY', 'EF_2j25_a4tchad'], 1, 0),

    CombinedChain2('2mu6i_DiMu_DY_noVtx_noOS_2j25_a4tchad', '419', ['combined','comb__muj'], [], [],
                   'L2_2mu6i_DiMu_DY_noVtx_noOS_2j25_a4tchad',    ['L2_2mu6i_DiMu_DY_noVtx_noOS'], 'L1_2MU6', 1, 0,
                   'EF_2mu6i_DiMu_DY_noVtx_noOS_2j25_a4tchad',    ['EF_2mu6i_DiMu_DY_noVtx_noOS', 'EF_2j25_a4tchad'], 1, 0),
                   
    CombinedChain2('mu24_tight_b35_mediumEF_j35_a4tchad', '662', ['combined','comb__muj'], [], [],
                   'L2_mu24_tight',  [], 'L1_MU15', 1, 0,
                   'EF_mu24_tight_b35_mediumEF_j35_a4tchad', ['EF_mu24_tight','EF_b35_mediumEF_j35_a4tchad'], 1, 0, doOrderEF=True),
    
    CombinedChain2('mu24_j65_a4tchad', '262', ['combined','comb__muj'], [], [],
                   'L2_mu24_j65_c4cchad',    ['L2_mu24', 'L2_j60_c4cchad'], 'L1_MU10_J20', 1, 0,
                   'EF_mu24_j65_a4tchad',    ['EF_mu24', 'EF_j65_a4tchad'], 1, 0),

    #96386
    CombinedChain2('xe60_tclcw_loose_b35_mediumEF_j35_a4tchad', '37', ['combined','comb__jmet'], [], [],
                   'L2_xe40',  [], 'L1_XE35', 1, 0,
                   'EF_xe60_tclcw_loose_b35_mediumEF_j35_a4tchad', ['EF_b35_mediumEF_j35_a4tchad','EF_xe60_tclcw_loose'], 1, 0, doOrderEF=True),                                          
    
    CombinedChain2('xe60T_tclcw_loose_b35_mediumEF_j35_a4tchad', '94', ['combined','comb__jmet'], [], [],
                   'L2_xe40T',  [], 'L1_XE35_BGRP7', 1, 0,
                   'EF_xe60T_tclcw_loose_b35_mediumEF_j35_a4tchad', ['EF_b35_mediumEF_j35_a4tchad','EF_xe60T_tclcw_loose'], 1, 0, doOrderEF=True),                                          
    
    CombinedChain2('mu13_vbf_fj', 21, ['combined', 'comb__muj'], [], [],
                   'L2_mu13_vbf_fj', ['L2_mu13', 'L2_2j15_a4TTem_deta25_L1MU10FJ15'], 'L1_MU10_FJ15', 1, 0,
                   'EF_mu13_vbf_fj', ['EF_mu13', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L1MU10FJ15' ], doOrderL2=True, doOrderEF=True),

    CombinedChain2('mu13_vbf_cf', 510, ['combined', 'comb__muj'], [], [],
                   'L2_mu13_vbf_cf', ['L2_mu13', 'L2_2j15_a4TTem_deta25_L1MU10J15CF'], 'L1_MU10_J15CF', 1, 0,
                   'EF_mu13_vbf_cf', ['EF_mu13', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L1MU10J15CF' ], doOrderL2=True, doOrderEF=True),

    CombinedChain2('mu15_vbf_L1TAU8_MU10', 415, ['combined', 'comb__muj'], [], [],
                   #                   'L2_mu15_vbf_L1TAU8_MU10', ['L2_mu15', 'L2_j0_a4TTem_j0_c4cchad_mjj0deta25_L1TAU8MU10'], 'L1_TAU8_MU10', 1, 0,
                   #                   'EF_mu15_vbf_L1TAU8_MU10', ['EF_mu15', 'EF_j0_a4tchad_mjj400deta28_L2FS_l2deta2p5_L1TAU8MU10' ], doOrderL2=True, doOrderEF=True),
                   'L2_mu15_vbf_L1TAU8_MU10', ['L2_mu15', 'L2_2j15_a4TTem_deta25_L1TAU8MU10'], 'L1_TAU8_MU10', 1, 0,
                   'EF_mu15_vbf_L1TAU8_MU10', ['EF_mu15', 'EF_2j25_a4tchad_eta50_mjj400deta28_L2FS_L1TAU8MU10' ], doOrderL2=True, doOrderEF=True),
    ]

for item in MuonJets+MuonJetsEMScale :
    if issubclass(item.__class__, CombinedChain2):
        item.groupL2 += [ "RATE:MuonJet",'BW:Muon']
        item.groupEF += [ "RATE:MuonJet",'BW:Muon']
    else:
        item.group += [ "RATE:MuonJet",'BW:Muon']

mu4_j10_forbjet = CombinedChain2('mu4_j10_a4tc_EFFS',   '215', ['jets'], [], [],
                                 'L2_mu4_j10_a4tc_EFFS',   ['L2_mu4'], 'L1_MU0', 1, 0,
                                 'EF_mu4_j10_a4tc_EFFS', ['EF_mu4', 'EF_j10_a4tc_EFFS'], 1, 0)
mu4_j10_forbjet.groupL2 += ['RATE:SingleBjet','BW:Bjets']
mu4_j10_forbjet.groupEF += ['RATE:SingleBjet','BW:Bjets']
MuonJetsEMScale.append(mu4_j10_forbjet)
 
ElectronMuons_eIdScan = [
    CombinedChain('e10_loose_mu6', '236', 'L1_EM5_MU6', ['EF_mu6', 'EF_e10_loose'], ['combined','comb__emu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    CombinedChain('e10_loose_mu10', '237', 'L1_EM5_MU10', ['EF_mu10', 'EF_e10_loose'], ['combined','comb__emu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    CombinedChain('2e10_medium_mu6', '755', 'L1_2EM5_MU6', ['EF_mu6', 'EF_e10_medium', 'EF_e10_medium'], ['combined','comb__emu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 
    CombinedChain('e10_medium_2mu6', '756', 'L1_EM5_2MU6', ['EF_mu6', 'EF_mu6', 'EF_e10_medium'], ['combined','comb__emu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 
    CombinedChain('e10_medium_mu6',  '540', 'L1_EM5_MU6', ['EF_mu6', 'EF_e10_medium'], ['combined','comb__emu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 
    CombinedChain('e5_medium_mu6',  '544', 'L1_EM3_MU6', ['EF_e5_medium', 'EF_mu6'], ['combined','comb__emu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 
    CombinedChain('e10_medium_mu10', '541', 'L1_EM5_MU10', ['EF_mu10', 'EF_e10_medium'], ['combined','comb__emu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

    CombinedChain('2mu6_e10_loose', '240', 'L1_EM5_2MU6', ['EF_mu6', 'EF_mu6', 'EF_e10_loose'], ['combined','comb__emu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

    CombinedChain('2mu4T_2e5_medium', '308', 'L1_2MU4', ['EF_mu4T', 'EF_mu4T', 'EF_e5_medium', 'EF_e5_medium'], ['combined','comb__emu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 

    ElectronMuonTopoAngle('e5_medium_mu6_topo_loose',
                          l2=Chain(name='L2_e5_medium_mu6_topo_loose', counter=531, seed='L1_EM3_MU6'), 
                          efmerge=['EF_e5_medium', 'EF_mu6'], l2merge=['L2_e5_medium', 'L2_mu6']),
    ElectronMuonTopoAngle('e5_medium_mu6_topo_medium',
                          l2=Chain(name='L2_e5_medium_mu6_topo_medium', counter=533, seed='L1_EM3_MU6'), 
                          efmerge=[ 'EF_mu6', 'EF_e5_medium'], l2merge=['L2_mu6','L2_e5_medium']),
    ElectronMuonTopoAngle('e10_medium_mu6_topo_medium',
                          l2=Chain(name='L2_e10_medium_mu6_topo_medium', counter=548, seed='L1_EM5_MU6'), 
                          efmerge=[ 'EF_mu6', 'EF_e10_medium'], l2merge=['L2_mu6','L2_e10_medium']),

    ]


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.NoVHThresholdMenus():
    ElectronMuons_eIdScan += [
        CombinedChain('e15_medium_mu6',  '542', 'L1_EM10_MU6', ['EF_mu6', 'EF_e15_medium'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 
        ElectronMuonTopoAngle('e12T_medium_mu6_topo_medium',
                              l2=Chain(name='L2_e12T_medium_mu6_topo_medium', counter=123, seed='L1_EM10_MU6'), 
                              efmerge=[ 'EF_mu6', 'EF_e12T_medium'], l2merge=['L2_mu6','L2_e12T_medium']),
        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.VHThresholdMenus():
    ElectronMuons_eIdScan += [
        ElectronMuonTopoAngle('e12Tvh_medium_mu6_topo_medium',
                              l2=Chain(name='L2_e12Tvh_medium_mu6_topo_medium', counter=123, seed='L1_EM10VH_MU6'), 
                              efmerge=[ 'EF_mu6', 'EF_e12Tvh_medium'], l2merge=['L2_mu6','L2_e12Tvh_medium']),

        CombinedChain('e12Tvh_medium_mu6',  '124', 'L1_EM10VH_MU6', ['EF_mu6', 'EF_e12Tvh_medium'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 

        CombinedChain('e22_etcut_mu10', '26', 'L1_EM5_MU10', ['EF_mu10', 'EF_e22_etcut_EM5'],['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('e10_etcut_mu10', '24', 'L1_EM5_MU10', ['EF_mu10', 'EF_e10_etcut'],['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    ElectronMuons_eIdScan += [
        CombinedChain('e5_medium_mu4', '341', 'L1_EM3_MU0', ['EF_mu4', 'EF_e5_medium'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),   
        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
    ElectronMuons_eIdScan += [
        CombinedChain('2mu4T_DiMu_e5_medium',  '247', 'L1_2MU4', ['EF_2mu4T_DiMu', 'EF_e5_medium'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 

        CombinedChain('e22vh_medium1_mu10_EFFS', '25', 'L1_EM16VH', ['EF_e22vh_medium1', 'EF_mu10_EFFS'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),        
        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
    # for v4 to be eL2Star
    ElectronMuons_eIdScan += [ 

        CombinedChain('2mu4T_DiMu_e5_tight1',  '239', 'L1_2MU4_EM3', ['EF_2mu4T_DiMu', 'EF_e5_tight1'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 

        CombinedChain('2mu4T_2e5_tight1',  '238', 'L1_2MU4_2EM3', ['EF_2mu4T', 'EF_2e5_tight1'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 


        CombinedChain('e7T_loose1_2mu6', '756', 'L1_EM6_2MU6', ['EF_2mu6', 'EF_e7T_loose1'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        CombinedChain('2e7T_loose1_mu6', '757', 'L1_2EM6_MU6', ['EF_mu6', 'EF_e7T_loose1', 'EF_e7T_loose1'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('e7T_medium1_2mu6', '16', 'L1_EM6_2MU6', ['EF_2mu6', 'EF_e7T_medium1'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        
        CombinedChain('e7T_medium1_L2StarB_2mu6', '61', 'L1_EM6_2MU6', ['EF_2mu6', 'EF_e7T_medium1_L2StarB'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        
        CombinedChain('2e7T_medium1_mu6', '17', 'L1_2EM6_MU6', ['EF_mu6', 'EF_e7T_medium1', 'EF_e7T_medium1'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('2e7T_medium1_L2StarB_mu6', '81', 'L1_2EM6_MU6', ['EF_mu6', 'EF_e7T_medium1_L2StarB', 'EF_e7T_medium1_L2StarB'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),


        
        CombinedChain('e12Tvh_loose1_mu8',  '3124', 'L1_EM10VH_MU6', ['EF_mu8', 'EF_e12Tvh_loose1'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 

        CombinedChain('e12Tvh_medium1_mu10',  '2124', 'L1_EM10VH_MU6', ['EF_mu10', 'EF_e12Tvh_medium1'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 
        
        ElectronMuonTopoAngle('e12Tvh_medium1_mu6_topo_medium',
                              l2=Chain(name='L2_e12Tvh_medium1_mu6_topo_medium', counter=538, seed='L1_EM10VH_MU6'), 
                              efmerge=[ 'EF_mu6', 'EF_e12Tvh_medium1'], l2merge=['L2_mu6','L2_e12Tvh_medium1']),

        ElectronMuonTopoAngle('e12Tvh_medium1_L2StarB_mu6_topo_medium',
                              l2=Chain(name='L2_e12Tvh_medium1_L2StarB_mu6_topo_medium', counter=709, seed='L1_EM10VH_MU6'), 
                              efmerge=[ 'EF_mu6', 'EF_e12Tvh_medium1_L2StarB'], l2merge=['L2_mu6','L2_e12Tvh_medium1_L2StarB']),
        
        CombinedChain('e12Tvh_medium1_mu6',  '3125', 'L1_EM10VH_MU6', ['EF_mu6', 'EF_e12Tvh_medium1'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 

        CombinedChain('e12Tvh_medium1_mu8',  '126', 'L1_EM10VH_MU6', ['EF_mu8', 'EF_e12Tvh_medium1'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('e12Tvh_medium1_L2StarB_mu8',  '294', 'L1_EM10VH_MU6', ['EF_mu8', 'EF_e12Tvh_medium1_L2StarB'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 

        CombinedChain('e24vhi_loose1_mu8',  '543', 'L1_EM18VH', ['EF_e24vhi_loose1', 'EF_mu8'], ['combined','comb__emu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 
        
        ]
    
for item in ElectronMuons_eIdScan :
    if hasattr(item, 'group'): item.group += [ "RATE:ElectronMuon",'BW:Egamma', 'BW:Muon']
    

MuonEles = [
    CombinedChain('mu18_tight_e7_medium1',  '542', 'L1_MU15', ['EF_mu18_tight', 'EF_e7_medium1'], ['combined','comb__mue'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    CombinedChain('mu18_tight_e7_medium1_L2StarB',  '35', 'L1_MU15', ['EF_mu18_tight', 'EF_e7_medium1_L2StarB'], ['combined','comb__mue'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 
    ]

for item in MuonEles:
    if hasattr(item, 'group'): item.group += [ "RATE:MuonElectron",'BW:Muon']



        
PhotonMETs = [
    CombinedChain2('g40_loose_EFxe50', '809', ['combined','comb__gmet'], [], [],\
                   'L2_g40_loose_EFxe50', ['L2_g40_loose','L2_noL2'], 'L1_EM30', 1, 0,
                   'EF_g40_loose_EFxe50', ['EF_g40_loose', 'EF_xe50_noL2'], 1, 0),


    CombinedChain2('g40_loose_L2EFxe50', '816', ['combined','comb__gmet'], [], [],\
                   'L2_g40_loose_L2xe35', ['L2_g40_loose','L2_xe35_unseeded'], 'L1_EM30', 1, 0,
                   'EF_g40_loose_L2EFxe50', ['EF_g40_loose', 'EF_xe50_unseeded'], 1, 0),


    CombinedChain2('g40_loose_L2EFxe60', '817', ['combined','comb__gmet'], [], [],\
                   'L2_g40_loose_L2xe45', ['L2_g40_loose','L2_xe45_unseeded'], 'L1_EM30', 1, 0,
                   'EF_g40_loose_L2EFxe60', ['EF_g40_loose', 'EF_xe60_unseeded'], 1, 0,doOrderL2=True, doOrderEF=True),

    CombinedChain2('g40_loose_L2EFxe60_tclcw', '607', ['combined','comb__gmet'], [], [],\
                   'L2_g40_loose_L2xe45', ['L2_g40_loose','L2_xe45_unseeded'], 'L1_EM30', 1, 0,
                   'EF_g40_loose_L2EFxe60_tclcw', ['EF_g40_loose', 'EF_xe60_tclcw_unseeded'], 1, 0,doOrderL2=True, doOrderEF=True),
    
]

PhotonMETs_v2v3 = [
    CombinedChain('g20_loose_xe20_noMu','6942', 'L1_EM14_XE10', ['EF_g20_loose', 'EF_xe20_noMu'], ['combined','comb__gmet'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),     

    CombinedChain('g20_etcut_xe30_noMu', '754', 'L1_EM14_XE20', ['EF_g20_etcut', 'EF_xe30_noMu'], ['combined','comb__gmet'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), #
    CombinedChain('g40_loose_xe40_noMu', '810', 'L1_EM14_XE20', ['EF_g40_loose', 'EF_xe40_noMu'], ['combined','comb__gmet'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), #should use EM30 but no such combined item
    CombinedChain('g40_loose_xs35_noMu', '816', 'L1_EM14_XE20', ['EF_g40_loose', 'EF_xs35_noMu'], ['combined','comb__gmet'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), #should use EM30 but no such combined item

    CombinedChain2('g40_loose_xe45_medium_noMu', '817', ['combined','comb__gmet'], [], [],\
                   'L2_g40_loose_xe30_medium_noMu', ['L2_g40_loose', 'L2_xe30_medium_noMu_noseed'], 'L1_EM30', 1, 0,
                   'EF_g40_loose_xe45_medium_noMu', ['EF_g40_loose', 'EF_xe45_medium_noMu_noseed'], 1, 0),
    CombinedChain2('g40_loose_xe55_medium_noMu', '819', ['combined','comb__gmet'], [], [],\
                   'L2_g40_loose_xe35_medium_noMu', ['L2_g40_loose', 'L2_xe35_medium_noMu_noseed'], 'L1_EM30', 1, 0,
                   'EF_g40_loose_xe55_medium_noMu', ['EF_g40_loose', 'EF_xe55_medium_noMu_noseed'], 1, 0),

    CombinedChain2('g17_etcut_EFxe20_noMu', '3255', ['combined','comb__gmet'], [], [],\
                   'L2_g17_etcut_EFxe20_noMu', ['L2_g17_etcut','L2_noL2_noMu'], 'L1_EM10', 1, 0,
                   'EF_g17_etcut_EFxe20_noMu', ['EF_g17_etcut', 'EF_xe20_noL2_noMu'], 1, 0),
    CombinedChain2('g17_etcut_EFxe30_noMu', '3256', ['combined','comb__gmet'], [], [],\
                   'L2_g17_etcut_EFxe30_noMu', ['L2_g17_etcut','L2_noL2_noMu'], 'L1_EM10', 1, 0,
                   'EF_g17_etcut_EFxe30_noMu', ['EF_g17_etcut', 'EF_xe30_noL2_noMu'], 1, 0),
    CombinedChain2('g27_etcut_EFxe30_noMu', '3270', ['combined','comb__gmet'], [], [],\
                   'L2_g27_etcut_EFxe30_noMu', ['L2_g27_etcut','L2_noL2_noMu'], 'L1_EM14', 1, 0,
                   'EF_g27_etcut_EFxe30_noMu', ['EF_g27_etcut', 'EF_xe30_noL2_noMu'], 1, 0),
    CombinedChain2('g40_loose_EFxe35_noMu', '807', ['combined','comb__gmet'], [], [],\
                   'L2_g40_loose_EFxe35_noMu', ['L2_g40_loose','L2_noL2_noMu'], 'L1_EM30', 1, 0,
                   'EF_g40_loose_EFxe35_noMu', ['EF_g40_loose', 'EF_xe35_noL2_noMu'], 1, 0),
    CombinedChain2('g40_loose_EFxe40_noMu', '808', ['combined','comb__gmet'], [], [],\
                   'L2_g40_loose_EFxe40_noMu', ['L2_g40_loose','L2_noL2_noMu'], 'L1_EM30', 1, 0,
                   'EF_g40_loose_EFxe40_noMu', ['EF_g40_loose', 'EF_xe40_noL2_noMu'], 1, 0),

    CombinedChain2('g13_etcut_xs60_noMu', '2806', ['combined','comb__gmet'], [], [],\
                   'L2_g13_etcut_xs45_noMu', ['L2_g13_etcut','L2_xs45_noMu_L1EM10XS45'], 'L1_EM10_XS45', 1, 0,
                   'EF_g13_etcut_xs60_noMu', ['EF_g13_etcut', 'EF_xs60_noMu_L1EM10XS45'], 1, 0),

    CombinedChain('g25_loose_xe30_noMu','590', 'L1_EM14_XE20', ['EF_g25_loose', 'EF_xe30_noMu'], ['combined','comb__gmet'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 
    CombinedChain('g20_loose_xe30_noMu','6924', 'L1_EM14_XE20', ['EF_g20_loose', 'EF_xe30_noMu'], ['combined','comb__gmet'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    ]

for item in PhotonMETs+PhotonMETs_v2v3 :
    if issubclass(item.__class__, CombinedChain2):
        item.groupL2 += [ "RATE:METPhoton",'BW:Egamma']
        item.groupEF += [ "RATE:METPhoton",'BW:Egamma']
    else:
        item.group += [ "RATE:METPhoton",'BW:Egamma']

PhotonElectrons = [

    #92866
    CombinedChain2('e18_medium1_g25_loose', '157', ['combined','comb__eg'], [], [],\
                   'L2_e18_medium1_g25_loose', ['L2_e18_medium1','L2_g25_loose'], 'L1_2EM12_EM16V', 1, 0,
                   'EF_e18_medium1_g25_loose', ['EF_e18_medium1','EF_g25_loose'], 1, 0),

    CombinedChain2('e18_medium1_g35_loose', '5021', ['combined','comb__eg'], [], [],\
                   'L2_e18_medium1_g35_loose', ['L2_e18_medium1','L2_g35_loose'], 'L1_2EM12_EM16V', 1, 0,
                   'EF_e18_medium1_g35_loose', ['EF_e18_medium1','EF_g35_loose'], 1, 0),

    CombinedChain2('e18_loose1_g35_loose', '5022', ['combined','comb__eg'], [], [],\
                   'L2_e18_loose1_g35_loose', ['L2_e18_loose1','L2_g35_loose'], 'L1_2EM12_EM16V', 1, 0,
                   'EF_e18_loose1_g35_loose', ['EF_e18_loose1','EF_g35_loose'], 1, 0),

    CombinedChain2('e18_medium1_g25_medium', '5023', ['combined','comb__eg'], [], [],\
                   'L2_e18_medium1_g25_medium', ['L2_e18_medium1','L2_g25_medium'], 'L1_2EM12_EM16V', 1, 0,
                   'EF_e18_medium1_g25_medium', ['EF_e18_medium1','EF_g25_medium'], 1, 0),
    
    CombinedChain2('e18_loose1_g25_medium', '609', ['combined','comb__eg'], [], [],\
                   'L2_e18_loose1_g25_medium', ['L2_e18_loose1','L2_g25_medium'], 'L1_2EM12_EM16V', 1, 0,
                   'EF_e18_loose1_g25_medium', ['EF_e18_loose1','EF_g25_medium'], 1, 0),
    
    CombinedChain2('e18_loose1_g35_medium', '5027', ['combined','comb__eg'], [], [],\
                   'L2_e18_loose1_g35_medium', ['L2_e18_loose1','L2_g35_medium'], 'L1_2EM12_EM16V', 1, 0,
                   'EF_e18_loose1_g35_medium', ['EF_e18_loose1','EF_g35_medium'], 1, 0),

    CombinedChain2('e18_medium1_g35_medium', '6000', ['combined','comb__eg'], [], [],\
                   'L2_e18_medium1_g35_medium', ['L2_e18_medium1','L2_g35_medium'], 'L1_2EM12_EM16V', 1, 0,
                   'EF_e18_medium1_g35_medium', ['EF_e18_medium1','EF_g35_medium'], 1, 0),
    

    ]

for item in PhotonElectrons :
    if issubclass(item.__class__, CombinedChain2):
        item.groupL2 += [ "RATE:ElectronPhoton",'BW:Egamma']
        item.groupEF += [ "RATE:ElectronPhoton",'BW:Egamma']
    else:
        item.group += [ "RATE:ElectronPhoton",'BW:Egamma']
        
    
PhotonMuons = [

    CombinedChain('2mu6_MSonly_g10_loose', '1020', 'L1_EM5_2MU6', ['EF_mu6_MSonly_wOvlpRm', 'EF_mu6_MSonly_wOvlpRm','EF_g10_loose'], ['combined','comb__gmu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

    CombinedChain('2mu6_MSonly_g10_loose_noOvlpRm', '1092', 'L1_EM5_2MU6', ['EF_mu6_MSonly', 'EF_mu6_MSonly','EF_g10_loose'], ['combined','comb__gmu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    
    CombinedChain('2mu6_MSonly_g10_loose_nonfilled', '471',
                  'L1_2MU6_EMPTY, L1_2MU6_FIRSTEMPTY, L1_2MU6_UNPAIRED_ISO, L1_2MU6_UNPAIRED_NONISO',
                  ['EF_mu6_MSonly_wOvlpRm', 'EF_mu6_MSonly_wOvlpRm','EF_g10_loose'], ['combined','comb__gmu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

    CombinedChain('2mu6_g10_loose', '245', 'L1_EM5_2MU6', ['EF_mu6', 'EF_mu6','EF_g10_loose'], ['combined','comb__gmu'],
                  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

    ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
    PhotonMuons += [
        CombinedChain('2mu10_MSonly_g10_loose', '1498', 'L1_EM5_2MU6', ['EF_mu10_MSonly_wOvlpRm', 'EF_mu10_MSonly_wOvlpRm','EF_g10_loose'], ['combined','comb__gmu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
    PhotonMuons += [
        CombinedChain('mu18_2g10_loose', '695', 'L1_2EM6_MU6',
                      ['EF_mu18', 'EF_g10_loose', 'EF_g10_loose'], ['combined','comb__gmu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('mu18_2g10_medium', '72', 'L1_2EM6_MU6',
                      ['EF_mu18', 'EF_g10_medium', 'EF_g10_medium'], ['combined','comb__gmu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('mu18_2g15_loose', '73', 'L1_2EM6_MU6',
                      ['EF_mu18', 'EF_g15_loose', 'EF_g15_loose'], ['combined','comb__gmu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('2mu10_MSonly_g10_loose', '149', 'L1_EM6_2MU6',
                      ['EF_mu10_MSonly_wOvlpRm', 'EF_mu10_MSonly_wOvlpRm','EF_g10_loose'], ['combined','comb__gmu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('2mu10_MSonly_g10_loose_EMPTY', '150', 'L1_2MU4_EMPTY',
                      ['EF_mu10_MSonly_wOvlpRm', 'EF_mu10_MSonly_wOvlpRm','EF_g10_loose'], ['combined','comb__gmu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        
        CombinedChain('2mu10_MSonly_g10_loose_UNPAIRED_ISO', '151', 'L1_2MU6_UNPAIRED_ISO',
                      ['EF_mu10_MSonly_wOvlpRm', 'EF_mu10_MSonly_wOvlpRm','EF_g10_loose'], ['combined','comb__gmu'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        ]

MuonPhotons=[]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
    MuonPhotons += [
        PhotonMuonTopoAngle('mu10_g10_loose_TauMass',
                            l2=Chain(name='L2_mu10_g10_loose_TauMass', counter=691, seed='L1_EM5_MU10'),
                            efmerge=[ 'EF_mu10', 'EF_g10_loose'], l2merge=['L2_mu10','L2_g10_loose']),
        
        PhotonMuonTopoAngle('mu20_tight_g5_loose_TauMass',
                            l2=Chain(name='L2_mu20_tight_g5_loose_TauMass', counter=692, seed='L1_MU15'),
                            efmerge=[ 'EF_mu20_tight', 'EF_g5_loose'], l2merge=['L2_mu20_tight','L2_g5_loose']),
        
        PhotonMuonTopoAngle('mu4T_g20Tvh_loose_TauMass',
                            l2=Chain(name='L2_mu4T_g20Tvh_loose_TauMass', counter=693, seed='L1_EM16VH'),
                            efmerge=[ 'EF_mu4T', 'EF_g20Tvh_loose'], l2merge=['L2_mu4T','L2_g20Tvh_loose']),
        
#        PhotonMuonTopoAngle('mu4T_g22Tvh_loose_TauMass',
#                            l2=Chain(name='L2_mu4T_g22Tvh_loose_TauMass', counter=694, seed='L1_EM18VH'),
#                            efmerge=[ 'EF_mu4T', 'EF_g22Tvh_loose'], l2merge=['L2_mu4T','L2_g22Tvh_loose']),
        
        CombinedChain('mu10_g10_loose', '248', 'L1_EM5_MU10',
                      ['EF_mu10', 'EF_g10_loose'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('mu20_tight_g5_loose', '697', 'L1_MU15',
                      ['EF_mu20_tight', 'EF_g5_loose'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('mu4T_g20Tvh_loose', '698', 'L1_EM16VH',
                      ['EF_mu4T', 'EF_g20Tvh_loose'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        
#        CombinedChain('mu4T_g22Tvh_loose', '699', 'L1_EM18VH',
#                      ['EF_mu4T', 'EF_g22Tvh_loose'], ['combined','comb__mug'],
#                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
                
        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
    MuonPhotons += [
        CombinedChain('mu18_g20vh_loose', '694', 'L1_EM10VH_MU6',
                      ['EF_mu18', 'EF_g20vh_loose'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('mu24_g20vh_loose', '842', 'L1_EM10VH_MU6',
                      ['EF_mu24', 'EF_g20vh_loose'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('mu24_g20vh_medium', '47', 'L1_EM10VH_MU6',
                      ['EF_mu24', 'EF_g20vh_medium'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        PhotonMuonTopoAngle('mu10i_g10_loose_TauMass',
                            l2=Chain(name='L2_mu10i_g10_loose_TauMass', counter=688, seed='L1_EM6_MU10'),
                            efmerge=[ 'EF_mu10i', 'EF_g10_loose'], l2merge=['L2_mu10','L2_g10_loose']),

        PhotonMuonTopoAngle('mu10i_g10_mugamma_TauMass',
                            l2=Chain(name='L2_mu10i_g10_mugamma_TauMass', counter=3688, seed='L1_EM6_MU10'),
                            efmerge=[ 'EF_mu10i', 'EF_g10_loose_TauMass'], l2merge=['L2_mu10','L2_g10_loose_TauMass']),


        PhotonMuonTopoAngle('mu10i_loose_g12Tvh_loose_TauMass',
                            l2=Chain(name='L2_mu10i_loose_g12Tvh_loose_TauMass', counter=698, seed='L1_EM10VH_MU6'),
                            efmerge=[ 'EF_mu10i_loose', 'EF_g12Tvh_loose'], l2merge=['L2_mu10_loose','L2_g12Tvh_loose']),

        PhotonMuonTopoAngle('mu10i_loose_g12Tvh_mugamma_TauMass',
                            l2=Chain(name='L2_mu10i_loose_g12Tvh_mugamma_TauMass', counter=3698, seed='L1_EM10VH_MU6'),
                            efmerge=[ 'EF_mu10i_loose', 'EF_g12Tvh_loose_TauMass'], l2merge=['L2_mu10_loose','L2_g12Tvh_loose_TauMass']),

        
        PhotonMuonTopoAngle('mu20i_tight_g5_loose_TauMass',
                            l2=Chain(name='L2_mu20i_tight_g5_loose_TauMass', counter=689, seed='L1_MU15'),
                            efmerge=[ 'EF_mu20i_tight', 'EF_g5_loose'], l2merge=['L2_mu20_tight','L2_g5_loose']), 

        PhotonMuonTopoAngle('mu20i_tight_g5_mugamma_TauMass',
                            l2=Chain(name='L2_mu20i_tight_g5_mugamma_TauMass', counter=6891, seed='L1_MU15'),
                            efmerge=[ 'EF_mu20i_tight', 'EF_g5_loose_TauMass'], l2merge=['L2_mu20_tight','L2_g5_loose_TauMass']), 

        
        PhotonMuonTopoAngle('mu4Ti_g20Tvh_loose_TauMass',
                            l2=Chain(name='L2_mu4Ti_g20Tvh_loose_TauMass', counter=6984, seed='L1_EM16VH_MU4'),
                            efmerge=[ 'EF_mu4Ti', 'EF_g20Tvh_loose'], l2merge=['L2_mu4T','L2_g20Tvh_loose']),

        PhotonMuonTopoAngle('mu4Ti_g20Tvh_mugamma_TauMass',
                            l2=Chain(name='L2_mu4Ti_g20Tvh_mugamma_TauMass', counter=3984, seed='L1_EM16VH_MU4'),
                            efmerge=[ 'EF_mu4Ti', 'EF_g20Tvh_loose_TauMass'], l2merge=['L2_mu4T','L2_g20Tvh_loose_TauMass']),


        PhotonMuonTopoAngle('mu10i_g10_medium_TauMass',
                            l2=Chain(name='L2_mu10i_g10_medium_TauMass', counter=697, seed='L1_EM6_MU10'),
                            efmerge=[ 'EF_mu10i', 'EF_g10_medium'], l2merge=['L2_mu10','L2_g10_medium']),
        
        PhotonMuonTopoAngle('mu10i_loose_g12Tvh_medium_TauMass',
                            l2=Chain(name='L2_mu10i_loose_g12Tvh_medium_TauMass', counter=630, seed='L1_EM10VH_MU6'),
                            efmerge=[ 'EF_mu10i_loose', 'EF_g12Tvh_medium'], l2merge=['L2_mu10_loose','L2_g12Tvh_medium']),
        
        PhotonMuonTopoAngle('mu20i_tight_g5_medium_TauMass',
                            l2=Chain(name='L2_mu20i_tight_g5_medium_TauMass', counter=699, seed='L1_MU15'),
                            efmerge=[ 'EF_mu20i_tight', 'EF_g5_medium'], l2merge=['L2_mu20_tight','L2_g5_medium']),
        
        PhotonMuonTopoAngle('mu4Ti_g20Tvh_medium_TauMass',
                            l2=Chain(name='L2_mu4Ti_g20Tvh_medium_TauMass', counter=693, seed='L1_EM16VH_MU4'),
                            efmerge=[ 'EF_mu4Ti', 'EF_g20Tvh_medium'], l2merge=['L2_mu4T','L2_g20Tvh_medium']),
        
        
        CombinedChain('mu10i_g10_loose', '1248', 'L1_EM6_MU10',
                      ['EF_mu10i', 'EF_g10_loose'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('mu10i_loose_g12Tvh_loose', '1249', 'L1_EM10VH_MU6',
                      ['EF_mu10i_loose', 'EF_g12Tvh_loose'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        
        CombinedChain('mu20i_tight_g5_loose', '1250', 'L1_MU15',
                      ['EF_mu20i_tight', 'EF_g5_loose'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        
        CombinedChain('mu4Ti_g20Tvh_loose', '1251', 'L1_EM16VH_MU4',
                      ['EF_mu4Ti', 'EF_g20Tvh_loose'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        CombinedChain('mu10i_g10_medium', '284', 'L1_EM6_MU10',
                      ['EF_mu10i', 'EF_g10_medium'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        
        CombinedChain('mu10i_loose_g12Tvh_medium', '595', 'L1_EM10VH_MU6',
                      ['EF_mu10i_loose', 'EF_g12Tvh_medium'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        
        CombinedChain('mu20i_tight_g5_medium', '645', 'L1_MU15',
                      ['EF_mu20i_tight', 'EF_g5_medium'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        
        CombinedChain('mu4Ti_g20Tvh_medium', '661', 'L1_EM16VH_MU4',
                      ['EF_mu4Ti', 'EF_g20Tvh_medium'], ['combined','comb__mug'],
                      prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        ]

for item in PhotonMuons :
    item.group += [ "RATE:MuonPhoton",'BW:Egamma']

for item in MuonPhotons :
    item.group += [ "RATE:MuonPhoton",'BW:Muon']

        
JetEMScaleMETs = [

    # jet+met+muon
    CombinedChain2('MU0_j45_a4tc_EFFS_xe45_loose_noMu', '598', ['combined','comb__mujmet'], [],[],
                   'L2_MU0_j40_xe20_loose_noMu', ['L2_j40', 'L2_xe20_loose_noMu_L1MU0J20XE20'], 'L1_MU0_J20_XE20', 1, 0, 
                   'EF_MU0_j45_a4tc_EFFS_xe45_loose_noMu', ['EF_xe45_loose_noMu_L1MU0J20XE20', 'EF_j45_a4tc_EFFS'], 1, 0),
    CombinedChain2('j80_a4tc_EFFS_xe60_noMu', '480', ['combined','comb__jmet'], [], [],
                   'L2_j75_xe40_noMu',         ['L2_xe40_noMu','L2_j75'], 'L1_J50_XE35', 1, 0,
                   'EF_j80_a4tc_EFFS_xe60_noMu',     ['EF_xe60_noMu','EF_j80_a4tc_EFFS'], 1, 0, doOrderEF=True),
    
    CombinedChain2('j75_a4tc_EFFS_xe40_loose_noMu', '392', ['combined','comb__jmet'], [], [],
                   'L2_j70_xe20_loose_noMu',         ['L2_xe20_loose_noMu','L2_j70'], 'L1_J50_XE20', 1, 0,
                   'EF_j75_a4tc_EFFS_xe40_loose_noMu',     ['EF_xe40_loose_noMu','EF_j75_a4tc_EFFS'], 1, 0, doOrderEF=True),

    CombinedChain2('j75_a4tc_EFFS_xe45_loose_noMu', '393', ['combined','comb__jmet'], [], [],
                   'L2_j70_xe20_loose_noMu',         ['L2_xe20_loose_noMu','L2_j70'], 'L1_J50_XE20', 1, 0,
                   'EF_j75_a4tc_EFFS_xe45_loose_noMu',     ['EF_xe45_loose_noMu','EF_j75_a4tc_EFFS'], 1, 0, doOrderEF=True),

    CombinedChain2('j75_a4tc_EFFS_xe55_loose_noMu', '481', ['combined','comb__jmet'], [], [],
                   'L2_j70_xe25_loose_noMu',         ['L2_xe25_loose_noMu','L2_j70'], 'L1_J50_XE30', 1, 0,
                   'EF_j75_a4tc_EFFS_xe55_loose_noMu',     ['EF_xe55_loose_noMu','EF_j75_a4tc_EFFS'], 1, 0, doOrderEF=True),

    CombinedChain2('j75_a4tc_EFFS_xe55_noMu', '482', ['combined','comb__jmet'], [], [],
                   'L2_j70_xe35_noMu',         ['L2_xe35_noMu','L2_j70'], 'L1_J50_XE35', 1, 0,
                   'EF_j75_a4tc_EFFS_xe55_noMu',     ['EF_xe55_noMu','EF_j75_a4tc_EFFS'], 1, 0, doOrderEF=True),

    CombinedChain2('j75_a4tc_EFFS_xe55_noMu_l2cleancons', '1342', ['combined','comb__jmet'], [], [],
                   'L2_j70_xe35_noMu_l2cleancons',         ['L2_xe35_noMu','L2_j70_l2cleancons'], 'L1_J50_XE35', 1, 0,
                   'EF_j75_a4tc_EFFS_xe55_noMu_l2cleancons',     ['EF_xe55_noMu','EF_j75_a4tc_EFFS_l2cleancons'], 1, 0, doOrderEF=True),
    
    CombinedChain2('j75_a4tc_EFFS_xe40_loose_noMu_dphijxe03', '440', ['combined','comb__jmet'], [], [],
                   'L2_j70_xe20_loose_noMu',                        ['L2_xe20_loose_noMu','L2_j70'], 'L1_J50_XE20', 1, 0,
                   'EF_j75_a4tc_EFFS_xe40_loose_noMu_dphijxe03',      ['EF_xe40_loose_noMu','EF_j75_a4tc_EFFS'], 1, 0, dPhiMetJet=True, doOrderEF=True),


    CombinedChain2('j55_a4tc_EFFS_xe55_medium_noMu_dphi2j30xe10_l2cleancons','1341', ['combined','comb__jmet'], [], [],
                   'L2_j50_xe35_medium_noMu_l2cleancons',            ['L2_xe35_medium_noMu','L2_j50_l2cleancons'], 'L1_J30_XE35', 1, 0,
                   'EF_j55_a4tc_EFFS_xe55_medium_noMu_dphi2j30xe10_l2cleancons',      ['EF_xe55_medium_noMu','EF_j55_a4tc_EFFS_l2cleancons'], 1, 0, dPhiMetJet=True, dPhiCut="dPhi_10_2j30",doOrderEF=True),
    
    CombinedChain2('j55_a4tc_EFFS_xe55_medium_noMu_dphi2j30xe10',         '29', ['combined','comb__jmet'], [], [],
                   'L2_j50_xe35_medium_noMu',                             ['L2_xe35_medium_noMu','L2_j50'], 'L1_J30_XE35', 1, 0,
                   'EF_j55_a4tc_EFFS_xe55_medium_noMu_dphi2j30xe10',      ['EF_xe55_medium_noMu','EF_j55_a4tc_EFFS'], 1, 0, dPhiMetJet=True, dPhiCut="dPhi_10_2j30",doOrderEF=True),

    CombinedChain2('j65_a4tc_EFFS_xe65_noMu_dphi2j30xe10',         '363', ['combined','comb__jmet'], [], [],
                   'L2_j60_xe45_noMu',                             ['L2_xe45_noMu','L2_j60'], 'L1_J30_XE40', 1, 0,
                   'EF_j65_a4tc_EFFS_xe65_noMu_dphi2j30xe10',      ['EF_xe65_noMu','EF_j65_a4tc_EFFS'], 1, 0, dPhiMetJet=True, dPhiCut="dPhi_10_2j30",doOrderEF=True),


    CombinedChain2('j75_j45_a4tc_EFFS_xe55_noMu', '924', ['combined','comb__jmet'], [], [],
                   'L2_j70_j40_xe35_noMu', ['L2_xe35_noMu','L2_j70','L2_j40'], 'L1_J50_XE35', 1, 0,
                   'EF_j75_j45_a4tc_EFFS_xe55_noMu', ['EF_xe55_noMu','EF_j75_a4tc_EFFS','EF_j45_a4tc_EFFS'], 1, 0, doOrderEF=True),

    CombinedChain2('j70_a4tc_EFFS_xe70_noMu_dphi2j30xe10','990', ['combined','comb__jmet'], [], [],
                   'L2_j65_xe50_noMu', ['L2_xe50_noMu','L2_j65'], 'L1_J30_XE40', 1, 0,
                   'EF_j70_a4tc_EFFS_xe70_noMu_dphi2j30xe10', ['EF_xe70_noMu','EF_j70_a4tc_EFFS'], 1, 0, dPhiMetJet=True, dPhiCut="dPhi_10_2j30", doOrderEF=True),
    
    #jet+met firstempty, empty
    CombinedChain2('j30_eta13_a4tc_EFFS_EFxe30_noMu_empty', '492', ['CosmicCalo'], [], [],
                   'L2_j30_eta13_a4tc_EFFS_EFxe30_noMu_empty',             [], 'L1_J10_EMPTY', 1, 0,
                   'EF_j30_eta13_a4tc_EFFS_EFxe30_noMu_empty',    ['EF_j30_eta13_a4tc_EFFS_empty','EF_xe30_noL2_noMu'], 1, 100),

    CombinedChain2('j30_eta13_a4tc_EFFS_EFxe30_noMu_firstempty', '493', ['JetTauEtmiss'], [], [],
                   'L2_j30_eta13_a4tc_EFFS_EFxe30_noMu_firstempty',             [], 'L1_J10_FIRSTEMPTY', 1, 0,
                   'EF_j30_eta13_a4tc_EFFS_EFxe30_noMu_firstempty',    ['EF_j30_eta13_a4tc_EFFS_firstempty','EF_xe30_noL2_noMu'], 1, 100),

    CombinedChain2('j50_eta13_a4tc_EFFS_EFxe50_noMu_empty', '494', ['CosmicCalo'], [], [],
                   'L2_j50_eta13_a4tc_EFFS_EFxe50_noMu_empty',             [], 'L1_J30_EMPTY', 1, 0,
                   'EF_j50_eta13_a4tc_EFFS_EFxe50_noMu_empty',    ['EF_j50_eta13_a4tc_EFFS_empty','EF_xe50_noL2_noMu'], 1, 10),

    CombinedChain2('j50_eta13_a4tc_EFFS_EFxe50_noMu_firstempty', '495', ['JetTauEtmiss'], [], [],
                   'L2_j50_eta13_a4tc_EFFS_EFxe50_noMu_firstempty',             [], 'L1_J30_FIRSTEMPTY', 1, 0,
                   'EF_j50_eta13_a4tc_EFFS_EFxe50_noMu_firstempty',    ['EF_j50_eta13_a4tc_EFFS_firstempty','EF_xe50_noL2_noMu'], 1, 10),

    CombinedChain2('j50_eta25_a4tc_EFFS_EFxe50_noMu_empty', '496', ['CosmicCalo'], [], [],
                   'L2_j50_eta25_a4tc_EFFS_EFxe50_noMu_empty',             [], 'L1_J30_EMPTY', 1, 0,
                   'EF_j50_eta25_a4tc_EFFS_EFxe50_noMu_empty',    ['EF_j50_eta25_a4tc_EFFS_empty','EF_xe50_noL2_noMu'], 1, 10),

    CombinedChain2('j50_eta25_a4tc_EFFS_EFxe50_noMu_firstempty', '497', ['JetTauEtmiss'], [], [],
                   'L2_j50_eta25_a4tc_EFFS_EFxe50_noMu_firstempty',             [], 'L1_J30_FIRSTEMPTY', 1, 0,
                   'EF_j50_eta25_a4tc_EFFS_EFxe50_noMu_firstempty',    ['EF_j50_eta25_a4tc_EFFS_firstempty','EF_xe50_noL2_noMu'], 1, 10),

    CombinedChain2('2j45_a4tc_EFFS_leadingmct100_xe40_medium_noMu','228', ['combined','comb__jmet'], [], [],
                   'L2_2j40_anymct100_xe20_medium_noMu', ['L2_2j40_anymct100', 'L2_xe20_medium_noMu' ], 'L1_2J20_XE20', 1, 0,
                   'EF_2j45_a4tc_EFFS_leadingmct100_xe40_medium_noMu', ['EF_xe40_medium_noMu','EF_2j45_a4tc_EFFS_leadingmct100'], 1, 0),

    CombinedChain2('2j55_a4tc_EFFS_leadingmct100_xe40_medium_noMu','281', ['combined','comb__jmet'], [], [],
                   'L2_2j50_anymct100_xe20_medium_noMu', ['L2_2j50_anymct100', 'L2_xe20_medium_noMu' ], 'L1_2J30_XE20', 1, 0,
                   'EF_2j55_a4tc_EFFS_leadingmct100_xe40_medium_noMu', ['EF_xe40_medium_noMu','EF_2j55_a4tc_EFFS_leadingmct100'], 1, 0),

    CombinedChain2('j100_a4tc_EFFS_EFxe70_noMu','7282', ['combined','comb__jmet'], [], [],
                   'L2_j95_EFxe70_noMu', ['L2_noL2_noMu','L2_j95'], 'L1_J75', 1, 0,
                   'EF_j100_a4tc_EFFS_EFxe70_noMu', ['EF_xe70_noL2_noMu','EF_j100_a4tc_EFFS'], 1, 0,doOrderEF=True),

    CombinedChain2('j100_2j30_a4tc_EFFS_EFxe70_noMu','289',['combined','comb__jmet'], [], [],
                   'L2_j95_EFxe70_noMu', ['L2_j95','L2_noL2_noMu'], 'L1_J75', 1, 0,
                   'EF_j100_2j30_a4tc_EFFS_EFxe70_noMu',['EF_xe70_noL2_noMu','EF_j100_2j30_a4tc_EFFS'], 1, 0,doOrderEF=True),

    CombinedChain2('j100_2j40_a4tc_EFFS_EFxe30_noMu_ht350','290',['combined','comb__jmet'], [], [],
                   'L2_j95_2j35_EFxe70_noMu', ['L2_j95_2j35','L2_noL2_noMu'], 'L1_J75', 1, 0,
                   'EF_j100_2j40_a4tc_EFFS_EFxe30_noMu_ht350',['EF_xe30_noL2_noMu','EF_j100_2j40_a4tc_EFFS_ht350'], 1, 0,doOrderEF=True),
    
    CombinedChain2('j75_a4tc_EFFS_xe65_noMu','295',['combined','comb__jmet'], [], [],
                   'L2_j70_xe40_noMu', ['L2_xe40_noMu','L2_j70'], 'L1_J50_XE35', 1, 0,
                   'EF_j75_a4tc_EFFS_xe65_noMu',['EF_xe65_noMu','EF_j75_a4tc_EFFS'], 1, 0,doOrderEF=True),

    CombinedChain2('j75_a4tc_EFFS_xe65_noMu_dphi2j30xe10',         '296', ['combined','comb__jmet'], [], [],
                   'L2_j70_xe40_noMu',                             ['L2_xe40_noMu','L2_j70'], 'L1_J50_XE35', 1, 0,
                   'EF_j75_a4tc_EFFS_xe65_noMu_dphi2j30xe10',      ['EF_xe65_noMu','EF_j75_a4tc_EFFS'], 1, 0, dPhiMetJet=True, dPhiCut="dPhi_10_2j30",doOrderEF=True),


    ###############
    
    ## jet + xs 
    CombinedChain2('j75_a4tc_EFFS_xs35_noMu', '4061', ['combined','comb__jmet'], [], [], \
                   'L2_j70_xs25_noMu',              [], 'L1_J50_XS25', 1, 0,\
                   'EF_j75_a4tc_EFFS_xs35_noMu',      ['EF_xs35_noMu', 'EF_j75_a4tc_EFFS'], 1, 0), 
    
    ]


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    JetEMScaleMETs += [
        CombinedChain2('mu4_j45_a4tc_EFFS_xe45_loose_noMu', '599', ['combined','comb__mujmet'], [],[],
                       'L2_mu4_j40_xe20_loose_noMu', ['L2_j40', 'L2_xe20_loose_noMu_L1MU0J20XE20', 'L2_mu4'], 'L1_MU0_J20_XE20', 1, 0, 
                       'EF_mu4_j45_a4tc_EFFS_xe45_loose_noMu', ['EF_xe45_loose_noMu_L1MU0J20XE20', 'EF_j45_a4tc_EFFS', 'EF_mu4'], 1, 0, doOrderL2=True, doOrderEF=True),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
    JetEMScaleMETs += [   
        CombinedChain2('mu4T_j45_a4tc_EFFS_xe45_loose_noMu', '599', ['combined','comb__mujmet'], [],[],
                       'L2_mu4T_j40_xe20_loose_noMu', ['L2_j40', 'L2_xe20_loose_noMu_L1MU4J20XE20', 'L2_mu4T'], 'L1_MU4_J20_XE20', 1, 0, 
                       'EF_mu4T_j45_a4tc_EFFS_xe45_loose_noMu', ['EF_xe45_loose_noMu_L1MU4J20XE20', 'EF_j45_a4tc_EFFS', 'EF_mu4T'], 1, 0, doOrderL2=True, doOrderEF=True),    


        CombinedChain2('mu4T_j45_a4tc_EFFS_xe55_noMu', '605', ['combined','comb__mujmet'], [],[],
                       'L2_mu4T_j40_xe35_noMu', ['L2_j40', 'L2_xe35_noMu_L1MU4J20XE35', 'L2_mu4T'], 'L1_MU4_J20_XE35', 1, 0, 
                       'EF_mu4T_j45_a4tc_EFFS_xe55_noMu', ['EF_xe55_noMu_L1MU4J20XE35', 'EF_j45_a4tc_EFFS', 'EF_mu4T'], 1, 0, doOrderL2=True, doOrderEF=True),    
        ]
    

for item in JetEMScaleMETs :
    if issubclass(item.__class__, CombinedChain2):
        if( "mu4T_j45" in item.sig_id ):
            item.groupL2 += [ "RATE:JetMET",'BW:Muon']
            item.groupEF += [ "RATE:JetMET",'BW:Muon']
        else:
            item.groupL2 += [ "RATE:JetMET",'BW:Jets']
            item.groupEF += [ "RATE:JetMET",'BW:Jets']
        if( "empty" in item.sig_id ):
            pass
        else:
            item.groupL2 += [ "RATE:SUSY"]
            item.groupEF += [ "RATE:SUSY"]
    else:
        if( "mu4T_j45" in item.sig_id ):
            item.group += [ "RATE:JetMET",'BW:Muon']
        else:
            item.group += [ "RATE:JetMET",'BW:Jets']
        if( "empty" in item.sig_id ):
            pass
        else:
            item.group+= [ "RATE:SUSY"]

JetMETs = [
    CombinedChain2('j110_a4tchad_xe65_tclcw', '3995', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe45',             [], 'L1_J50_XE40', 1, 0,
                   'EF_j110_a4tchad_xe65_tclcw',    ['EF_j110_a4tchad','EF_xe65_tclcw'], 1, 0, doOrderEF=True ),

    CombinedChain2('j110_a4tchad_xe75_tclcw', '89', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe45',             [], 'L1_J50_XE40', 1, 0,
                   'EF_j110_a4tchad_xe75_tclcw',    ['EF_j110_a4tchad','EF_xe75_tclcw'], 1, 0, doOrderEF=True ),

    CombinedChain2('j145_a4tchad_L2EFxe60_tclcw', '97', ['combined','comb__jmet'], [], [],
                   'L2_j140_c4cchad_L2xe45',             [], 'L1_J75', 1, 0,
                   'EF_j145_a4tchad_L2EFxe60_tclcw',    ['EF_j145_a4tchad','EF_xe60_tclcw_unseeded'], 1, 0, doOrderEF=True),

    CombinedChain2('j145_a4tchad_L2EFxe70_tclcw', '98', ['combined','comb__jmet'], [], [],
                   'L2_j140_c4cchad_L2xe45',             [], 'L1_J75', 1, 0,
                   'EF_j145_a4tchad_L2EFxe70_tclcw',    ['EF_j145_a4tchad','EF_xe70_tclcw_unseeded'], 1, 0, doOrderEF=True),

    CombinedChain2('j145_a4tchad_L2EFxe80_tclcw', '6943', ['combined','comb__jmet'], [], [],
                   'L2_j140_c4cchad_L2xe45',             [], 'L1_J75', 1, 0,
                   'EF_j145_a4tchad_L2EFxe80_tclcw',    ['EF_j145_a4tchad','EF_xe80_tclcw_unseeded'], 1, 0, doOrderEF=True),

    CombinedChain2('j145_a4tchad_L2EFxe90_tclcw', '44', ['combined','comb__jmet'], [], [],
                   'L2_j140_c4cchad_L2xe45',             [], 'L1_J75', 1, 0,
                   'EF_j145_a4tchad_L2EFxe90_tclcw',    ['EF_j145_a4tchad','EF_xe90_tclcw_unseeded'], 1, 0, doOrderEF=True),

    CombinedChain2('j170_a4tchad_EFxe50_tclcw', '6999', ['combined','comb__jmet'], [], [],
                   'L2_j165_c4cchad_EFxe50',  ['L2_j165_c4cchad','L2_noL2'], 'L1_J75', 1, 0,
                   'EF_j170_a4tchad_EFxe50_tclcw',    ['EF_j170_a4tchad','EF_xe50_tclcw_noL2'], 1, 0, doOrderEF=True), 

    CombinedChain2('j170_a4tchad_EFxe60_tclcw', '3910', ['combined','comb__jmet'], [], [],
                   'L2_j165_c4cchad_EFxe60',  ['L2_j165_c4cchad','L2_noL2'], 'L1_J75', 1, 0,
                   'EF_j170_a4tchad_EFxe60_tclcw',    ['EF_j170_a4tchad','EF_xe60_tclcw_noL2'], 1, 0, doOrderEF=True),

    CombinedChain2('j170_a4tchad_EFxe70_tclcw', '208', ['combined','comb__jmet'], [], [],
                   'L2_j165_c4cchad_EFxe70',  ['L2_j165_c4cchad','L2_noL2'], 'L1_J75', 1, 0,
                   'EF_j170_a4tchad_EFxe70_tclcw',    ['EF_j170_a4tchad','EF_xe70_tclcw_noL2'], 1, 0, doOrderEF=True),

    CombinedChain2('j170_a4tchad_EFxe80_tclcw', '283', ['combined','comb__jmet'], [], [],
                   'L2_j165_c4cchad_EFxe80',  ['L2_j165_c4cchad','L2_noL2'], 'L1_J75', 1, 0,
                   'EF_j170_a4tchad_EFxe80_tclcw',    ['EF_j170_a4tchad','EF_xe80_tclcw_noL2'], 1, 0, doOrderEF=True),

    
    CombinedChain2('j110_a4tchad_xe100_tclcw', '6988', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe45',  [], 'L1_J75_XE40', 1, 0,
                   'EF_j110_a4tchad_xe100_tclcw',  ['EF_j110_a4tchad','EF_xe100_tclcw_loose'], 1, 0, doOrderEF=True),

    CombinedChain2('j80_a4tchad_xe60_tclcw_loose', '8054', ['combined','comb__jmet'], [], [],
                   'L2_j75_c4cchad_xe40',  [], 'L1_J30_XE35', 1, 0,
                   'EF_j80_a4tchad_xe60_tclcw_loose',  ['EF_j80_a4tchad','EF_xe60_tclcw_loose'], 1, 0, doOrderEF=True),
    
    CombinedChain2('j80_a4tchad_xe70_tclcw_veryloose', '8055', ['combined','comb__jmet'], [], [],
                   'L2_j75_c4cchad_xe40',  [], 'L1_J30_XE35', 1, 0,
                   'EF_j80_a4tchad_xe70_tclcw_veryloose',  ['EF_j80_a4tchad','EF_xe70_tclcw_veryloose'], 1, 0, doOrderEF=True),


    CombinedChain2('j110_a4tchad_xe90_tclcw_loose', '58', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe45',  [], 'L1_J50_XE40', 1, 0,
                   'EF_j110_a4tchad_xe90_tclcw_loose',  ['EF_j110_a4tchad','EF_xe90_tclcw_loose'], 1, 0, doOrderEF=True),

    CombinedChain2('j80_a4tchad_xe100_tclcw_loose', '59', ['combined','comb__jmet'], [], [],
                   'L2_j75_c4cchad_xe55',  [], 'L1_J30_XE50', 1, 0,
                   'EF_j80_a4tchad_xe100_tclcw_loose',  ['EF_j80_a4tchad','EF_xe100_tclcw_loose'], 1, 0, doOrderEF=True),


    ##Stopped Hadrons Jets+MET

    CombinedChain2('j30_a4tcem_eta13_xe30_firstempty', '64', ['combined','comb__jmet'], [], [],
                   'L2_j30_a4tcem_eta13_xe30_firstempty',  ['L2_noL2'], 'L1_J10_FIRSTEMPTY', 1, 0,
                   'EF_j30_a4tcem_eta13_xe30_firstempty',  ['EF_j30_a4tcem_eta13_firstempty','EF_xe30'], 1, 0, doOrderEF=True),

    CombinedChain2('j50_a4tcem_eta13_xe50_firstempty', '65', ['combined','comb__jmet'], [], [],
                   'L2_j50_a4tcem_eta13_xe50_firstempty',  ['L2_noL2'], 'L1_J30_FIRSTEMPTY', 1, 0,
                   'EF_j50_a4tcem_eta13_xe50_firstempty',  ['EF_j50_a4tcem_eta13_firstempty','EF_xe50'], 1, 0, doOrderEF=True),
    
    CombinedChain2('j50_a4tcem_eta25_xe50_firstempty', '66', ['combined','comb__jmet'], [], [],
                   'L2_j50_a4tcem_eta25_xe50_firstempty',  ['L2_noL2'], 'L1_J30_FIRSTEMPTY', 1, 0,
                   'EF_j50_a4tcem_eta25_xe50_firstempty',  ['EF_j50_a4tcem_eta25_firstempty','EF_xe50'], 1, 0, doOrderEF=True),

    CombinedChain2('j30_a4tcem_eta13_xe30_empty', '67', ['combined','comb__jmet'], [], [],
                   'L2_j30_a4tcem_eta13_xe30_empty',  ['L2_noL2'], 'L1_J10_EMPTY', 1, 0,
                   'EF_j30_a4tcem_eta13_xe30_empty',  ['EF_j30_a4tcem_eta13_empty','EF_xe30'], 1, 0, doOrderEF=True),

    CombinedChain2('j50_a4tcem_eta13_xe50_empty', '68', ['combined','comb__jmet'], [], [],
                   'L2_j50_a4tcem_eta13_xe50_empty',  ['L2_noL2'], 'L1_J30_EMPTY', 1, 0,
                   'EF_j50_a4tcem_eta13_xe50_empty',  ['EF_j50_a4tcem_eta13_empty','EF_xe50'], 1, 0, doOrderEF=True),
    
    CombinedChain2('j50_a4tcem_eta25_xe50_empty', '69', ['combined','comb__jmet'], [], [],
                   'L2_j50_a4tcem_eta25_xe50_empty',  ['L2_noL2'], 'L1_J30_EMPTY', 1, 0,
                   'EF_j50_a4tcem_eta25_xe50_empty',  ['EF_j50_a4tcem_eta25_empty','EF_xe50'], 1, 0, doOrderEF=True),

    # Monojet
    CombinedChain2('j80_a4tchad_xe70_tclcw_dphi2j45xe10',         '70', ['combined','comb__jmet'], [], [],
                   'L2_j75_c4cchad_xe45',                         ['L2_j75_c4cchad','L2_xe45'], 'L1_J30_XE40', 1, 0,
                   'EF_j80_a4tchad_xe70_tclcw_dphi2j45xe10',      ['EF_j80_a4tchad','EF_xe70_tclcw','EF_j45_a4tchad'], 1, 0, dPhiMetJet=True, dPhiCut="dPhi_10_2j45",doOrderEF=True),

    CombinedChain2('j80_a4tchad_xe85_tclcw_dphi2j45xe10',         '71', ['combined','comb__jmet'], [], [],
                   'L2_j75_c4cchad_xe55',                         ['L2_j75_c4cchad','L2_xe55'], 'L1_J30_XE50', 1, 0,
                   'EF_j80_a4tchad_xe85_tclcw_dphi2j45xe10',      ['EF_j80_a4tchad','EF_xe85_tclcw','EF_j45_a4tchad'], 1, 0, dPhiMetJet=True, dPhiCut="dPhi_10_2j45",doOrderEF=True),

    CombinedChain2('j40_a4tchad_xe60_tclcw_loose_dphi2j45xe10_delayed',         '125', ['combined','haddelayed'], [], [],
                   'L2_xe40',      [], 'L1_XE35', 1, 0,
                   'EF_j40_a4tchad_xe60_tclcw_loose_dphi2j45xe10_delayed',      ['EF_j40_a4tchad','EF_xe60_tclcw_loose','EF_j45_a4tchad'], 1, 0, dPhiMetJet=True, dPhiCut="dPhi_10_2j45",doOrderEF=True),

    CombinedChain2('j40_a4tchad_xe60T_tclcw_loose_dphi2j45xe10_delayed',         '122', ['combined','haddelayed'], [], [],
                   'L2_xe40T',      [], 'L1_XE35_BGRP7', 1, 0,
                   'EF_j40_a4tchad_xe60T_tclcw_loose_dphi2j45xe10_delayed',      ['EF_j40_a4tchad','EF_xe60T_tclcw_loose','EF_j45_a4tchad'], 1, 0, dPhiMetJet=True, dPhiCut="dPhi_10_2j45",doOrderEF=True),

    
    ##Fat Jets + MET
    CombinedChain2('j180_a10tcem_EFxe50_tclcw', '6991', ['combined','comb__jmet'], [], [],
                   'L2_j165_c4cchad_EFxe50',  ['L2_j165_c4cchad','L2_noL2'], 'L1_J75', 1, 0,
                   'EF_j180_a10tcem_EFxe50_tclcw',  ['EF_j180_a10tcem','EF_xe50_tclcw_noL2'], 1, 0, doOrderEF=True),

    CombinedChain2('j180_a10tclcw_EFxe50_tclcw', '3991', ['combined','comb__jmet'], [], [],
                   'L2_j165_c4cchad_EFxe50',  ['L2_j165_c4cchad','L2_noL2'], 'L1_J75', 1, 0,
                   'EF_j180_a10tclcw_EFxe50_tclcw',  ['EF_j180_a10tclcw','EF_xe50_tclcw_noL2'], 1, 0, doOrderEF=True),

    CombinedChain2('j145_a10tcem_L2FS_L2xe60_tclcw', '3994', ['combined','comb__jmet'], [], [],
                   'L2_j100_a10TTem_L2xe45',  ['L2_j100_a10TTem','L2_xe45_unseeded'], 'L1_J75', 1, 0,
                   'EF_j145_a10tcem_L2FS_L2xe60_tclcw',  ['EF_j145_a10tcem','EF_xe60_tclcw_unseeded'], 1, 0, doOrderEF=True),


    ## mu+j+met
    CombinedChain2('mu24_j65_a4tchad_EFxe40_tclcw', '109', ['combined','comb__mujmet'], [], [],
                   'L2_mu24_j60_c4cchad_EFxe40',  ['L2_mu24','L2_j60_c4cchad','L2_noL2'], 'L1_MU10_J20', 1, 0,
                   'EF_mu24_j65_a4tchad_EFxe40_tclcw',  ['EF_mu24','EF_j65_a4tchad','EF_xe40_tclcw_noL2'], 1, 0, doOrderEF=True),

    CombinedChain2('mu24_j65_a4tchad_EFxe50_tclcw', '605', ['combined','comb__mujmet'], [], [],
                   'L2_mu24_j60_c4cchad_EFxe50',  ['L2_mu24','L2_j60_c4cchad','L2_noL2'], 'L1_MU10_J20', 1, 0,
                   'EF_mu24_j65_a4tchad_EFxe50_tclcw',  ['EF_mu24','EF_j65_a4tchad','EF_xe50_tclcw_noL2'], 1, 0, doOrderEF=True),

    CombinedChain2('mu24_j65_a4tchad_EFxe60_tclcw', '606', ['combined','comb__mujmet'], [], [],
                   'L2_mu24_j60_c4cchad_EFxe60',  ['L2_mu24','L2_j60_c4cchad','L2_noL2'], 'L1_MU10_J20', 1, 0,
                   'EF_mu24_j65_a4tchad_EFxe60_tclcw',  ['EF_mu24','EF_j65_a4tchad','EF_xe60_tclcw_noL2'], 1, 0, doOrderEF=True),


    CombinedChain2('mu4T_j65_a4tchad_xe60_tclcw_loose', '110', ['combined','comb__mujmet'], [], [],
                   'L2_mu4T_j60_c4cchad_xe40',  [], 'L1_MU4_J20_XE35', 1, 0,
                   'EF_mu4T_j65_a4tchad_xe60_tclcw_loose',  ['EF_mu4T','EF_j65_a4tchad','EF_xe60_tclcw_loose'], 1, 0, doOrderEF=True),

    CombinedChain2('mu4T_j65_a4tchad_xe70_tclcw_veryloose', '600', ['combined','comb__mujmet'], [], [],
                   'L2_mu4T_j60_c4cchad_xe40',  [], 'L1_MU4_J20_XE35', 1, 0,
                   'EF_mu4T_j65_a4tchad_xe70_tclcw_veryloose',  ['EF_mu4T','EF_j65_a4tchad','EF_xe70_tclcw_veryloose'], 1, 0, doOrderEF=True),


    CombinedChain2('mu24_j65_a4tchad_EFxe40', '263', ['combined','comb__mujmet'], [], [],
                   'L2_mu24_j60_c4cchad_xe35',    ['L2_mu24', 'L2_j60_c4cchad', 'L2_noL2'], 'L1_MU10_J20', 1, 0,
                   'EF_mu24_j65_a4tchad_EFxe40',    ['EF_mu24', 'EF_j65_a4tchad', 'EF_xe40'], 1, 0),

    ### mu+met "wMu" 

    CombinedChain2('mu24_j65_a4tchad_EFxe60wMu_tclcw', '813', ['combined','comb__mujmet'], [], [],
                   'L2_mu24_j60_c4cchad_EFxe60wMu',  ['L2_mu24','L2_j60_c4cchad','L2_noL2'], 'L1_MU10_J20', 1, 0,
                   'EF_mu24_j65_a4tchad_EFxe60wMu_tclcw',  ['EF_mu24','EF_j65_a4tchad','EF_xe60_tclcw_noL2_wMu'], 1, 0, doOrderEF=True),
    

    CombinedChain2('mu24_j65_a4tchad_EFxe40wMu_tclcw', '806', ['combined','comb__mujmet'], [], [],
                   'L2_mu24_j60_c4cchad_EFxe40wMu',  ['L2_mu24','L2_j60_c4cchad','L2_noL2'], 'L1_MU10_J20', 1, 0,
                   'EF_mu24_j65_a4tchad_EFxe40wMu_tclcw',  ['EF_mu24','EF_j65_a4tchad','EF_xe40_tclcw_noL2_wMu'], 1, 0, doOrderEF=True),
    

    CombinedChain2('2mu8_EFxe40wMu_tclcw', '807', ['combined','comb__mumet'], [], [],\
                   'L2_2mu8_EFxe40wMu', ['L2_2mu8','L2_noL2'], 'L1_2MU6', 1, 0,
                   'EF_2mu8_EFxe40wMu_tclcw', ['EF_2mu8', 'EF_xe40_tclcw_noL2_wMu'], 1, 0, doOrderL2=True, doOrderEF=True),
    

    
    ##ZH triggers 
    CombinedChain2('j80_a4tchad_xe55_tclcw', '3120', ['combined','comb__jmet'], [], [],
                   'L2_j75_c4cchad_xe40',  [], 'L1_J30_XE35', 1, 0,
                   'EF_j80_a4tchad_xe55_tclcw',    ['EF_j80_a4tchad','EF_xe55_tclcw'], 1, 0, doOrderEF=True),
    CombinedChain2('j80_a4tchad_xe60_tclcw', '3121', ['combined','comb__jmet'], [], [],
                   'L2_j75_c4cchad_xe45',  [], 'L1_J30_XE40', 1, 0,
                   'EF_j80_a4tchad_xe60_tclcw',    ['EF_j80_a4tchad','EF_xe60_tclcw'], 1, 0, doOrderEF=True),
    CombinedChain2('j80_a4tchad_xe70_tclcw', '3122', ['combined','comb__jmet'], [], [],
                   'L2_j75_c4cchad_xe55',  [], 'L1_J30_XE50', 1, 0,
                   'EF_j80_a4tchad_xe70_tclcw',    ['EF_j80_a4tchad','EF_xe70_tclcw'], 1, 0, doOrderEF=True),
    CombinedChain2('j110_a4tchad_xe55_tclcw', '123', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe40',  [], 'L1_J50_XE35', 1, 0,
                   'EF_j110_a4tchad_xe55_tclcw',    ['EF_j110_a4tchad','EF_xe55_tclcw'], 1, 0, doOrderEF=True),
    CombinedChain2('j110_a4tchad_xe60_tclcw', '610', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe45',  [], 'L1_J50_XE40', 1, 0,
                   'EF_j110_a4tchad_xe60_tclcw',    ['EF_j110_a4tchad','EF_xe60_tclcw'], 1, 0, doOrderEF=True),

    CombinedChain2('j40_j20_a4tchad_xe60_tclcw', '3017', ['combined','comb__jmet'], [], [],
                   'L2_xe40',  [], 'L1_XE35', 1, 0,
                   'EF_j40_j20_a4tchad_xe60_tclcw',    ['EF_j40_j20_a4tchad','EF_xe60_tclcw_loose'], 1, 0, doOrderEF=True),
    


    ##New Triggers 
    CombinedChain2('j80_a4tchad_xe70_tclcw_loose', '1210', ['combined','comb__jmet'], [], [],
                   'L2_j75_c4cchad_xe45',  [], 'L1_J30_XE40', 1, 0,
                   'EF_j80_a4tchad_xe70_tclcw_loose',    ['EF_j80_a4tchad','EF_xe70_tclcw_loose'], 1, 0, doOrderEF=True),

    
    CombinedChain2('j80_a4tchad_xe80_tclcw_loose', '1211', ['combined','comb__jmet'], [], [],
                   'L2_j75_c4cchad_xe45',  [], 'L1_J30_XE40', 1, 0,
                   'EF_j80_a4tchad_xe80_tclcw_loose',    ['EF_j80_a4tchad','EF_xe80_tclcw_loose'], 1, 0, doOrderEF=True),
    

    CombinedChain2('j80_a4tchad_xe100_tclcw_veryloose', '155', ['combined','comb__jmet'], [], [],
                   'L2_j75_c4cchad_xe45',  [], 'L1_J30_XE40', 1, 0,
                   'EF_j80_a4tchad_xe100_tclcw_veryloose',    ['EF_j80_a4tchad','EF_xe100_tclcw_veryloose'], 1, 0, doOrderEF=True),


    CombinedChain2('j110_a4tchad_xe60_tclcw_loose', '1213', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe40',  [], 'L1_J50_XE35', 1, 0,
                   'EF_j110_a4tchad_xe60_tclcw_loose',    ['EF_j110_a4tchad','EF_xe60_tclcw_loose'], 1, 0, doOrderEF=True),


    CombinedChain2('j110_a4tchad_xe70_tclcw_veryloose', '1214', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe40',  [], 'L1_J50_XE35', 1, 0,
                   'EF_j110_a4tchad_xe70_tclcw_veryloose',    ['EF_j110_a4tchad','EF_xe70_tclcw_veryloose'], 1, 0, doOrderEF=True),

    CombinedChain2('j110_a4tchad_xe90_tclcw_veryloose', '156', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe40',  [], 'L1_J50_XE35', 1, 0,
                   'EF_j110_a4tchad_xe90_tclcw_veryloose',    ['EF_j110_a4tchad','EF_xe90_tclcw_veryloose'], 1, 0, doOrderEF=True),


    CombinedChain2('j110_a4tchad_xe70_tclcw_loose', '1217', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe45',  [], 'L1_J50_XE45', 1, 0,
                   'EF_j110_a4tchad_xe70_tclcw_loose',    ['EF_j110_a4tchad','EF_xe70_tclcw_loose'], 1, 0, doOrderEF=True),

    CombinedChain2('j110_a4tchad_xe80_tclcw_loose', '1218', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe45',  [], 'L1_J50_XE45', 1, 0,
                   'EF_j110_a4tchad_xe80_tclcw_loose',    ['EF_j110_a4tchad','EF_xe80_tclcw_loose'], 1, 0, doOrderEF=True),

    CombinedChain2('j110_a4tchad_xe100_tclcw_veryloose', '599', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe45',  [], 'L1_J50_XE45', 1, 0,
                   'EF_j110_a4tchad_xe100_tclcw_veryloose',    ['EF_j110_a4tchad','EF_xe100_tclcw_veryloose'], 1, 0, doOrderEF=True),

    
    CombinedChain2('j110_a4tchad_xe50_tclcw', '1226', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe35',  [], 'L1_J50_XE30', 1, 0,
                   'EF_j110_a4tchad_xe50_tclcw',    ['EF_j110_a4tchad','EF_xe50_tclcw'], 1, 0, doOrderEF=True),


    CombinedChain2('j110_a4tchad_xe60_tclcw_veryloose', '1227', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe35',  [], 'L1_J50_XE30', 1, 0,
                   'EF_j110_a4tchad_xe60_tclcw_veryloose',    ['EF_j110_a4tchad','EF_xe60_tclcw_veryloose'], 1, 0, doOrderEF=True),

    CombinedChain2('j110_a4tclcw_xe100_tclcw_veryloose', '105', ['combined','comb__jmet'], [], [],
                   'L2_j105_c4cchad_xe45',  [], 'L1_J50_XE40', 1, 0,
                   'EF_j110_a4tclcw_xe100_tclcw_veryloose',    ['EF_j110_a4tclcw','EF_xe100_tclcw_veryloose'], 1, 0, doOrderEF=True),

    CombinedChain2('j145_a4tclcw_L2EFxe90_tclcw', '106', ['combined','comb__jmet'], [], [],
                   'L2_j140_c4cchad_L2xe45',             [], 'L1_J75', 1, 0,
                   'EF_j145_a4tclcw_L2EFxe90_tclcw',    ['EF_j145_a4tclcw','EF_xe90_tclcw_unseeded'], 1, 0, doOrderEF=True),


    CombinedChain2('j145_2j45_a4tchad_L2EFxe70_tclcw', '159', ['combined','comb__jmet'], [], [],
                   'L2_j140_c4cchad_L2xe40',  [], 'L1_J75', 1, 0,
                   'EF_j145_2j45_a4tchad_L2EFxe70_tclcw',    ['EF_j145_2j45_a4tchad','EF_xe70_tclcw_loose_unseeded'], 1, 0, doOrderEF=True),




    ]


for item in JetMETs :
    if issubclass(item.__class__, CombinedChain2):
        item.groupL2 += [ "RATE:JetMET",'BW:Jets']
        item.groupEF += [ "RATE:JetMET",'BW:Jets']


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
    BjetMETs = [

        CombinedChain2('b10_medium_EFxe25_noMu_L1JE100', '591', ['combined','comb__jmet'], [], [],
                       'L2_b10_medium_EFxe25_noMu_L1JE100', ['L2_b10_medium', 'L2_noL2_noMu'], 'L1_JE100', 1, 0,
                       'EF_b10_medium_EFxe25_noMu_L1JE100', ['EF_b10_medium', 'EF_xe25_noL2_noMu'], 1, 0),
        CombinedChain2('b10_medium_EFxe25_noMu_L1JE140', '592', ['combined','comb__jmet'], [], [],
                       'L2_b10_medium_EFxe25_noMu_L1JE140', ['L2_b10_medium', 'L2_noL2_noMu'], 'L1_JE140', 1, 0,
                       'EF_b10_medium_EFxe25_noMu_L1JE140', ['EF_b10_medium', 'EF_xe25_noL2_noMu'], 1, 0),
        CombinedChain2('b10_medium_EFxe25_noMu_L1_2J10J50', '593', ['combined','comb__jmet'], [], [],
                       'L2_b10_medium_EFxe25_noMu_L1_2J10J50', ['L2_b10_medium', 'L2_noL2_noMu'], 'L1_2J10_J50', 1, 0,
                       'EF_b10_medium_EFxe25_noMu_L1_2J10J50', ['EF_b10_medium', 'EF_xe25_noL2_noMu'], 1, 0),
        
        #EF_b15_medium_2j55_a4tc_EFFS_xe50_noMu (seeded by L1_2J30_XE20 for now)
        CombinedChain2('b15_medium_2j55_a4tc_EFFS_xe50_noMu', '584', ['combined','comb__jmet'], [], [],
                   'L2_b15_medium_2j50_xe30_noMu', ['L2_b15_medium_2j50', 'L2_xe30_noMu'], 'L1_2J30_XE20', 1, 0,
                       'EF_b15_medium_2j55_a4tc_EFFS_xe50_noMu', ['EF_b15_medium_2j55_a4tc_EFFS', 'EF_xe50_noMu'], 1, 0),
        
        
        #EF_2b15_medium_j75_j40_a4tc_EFFS_EFxe30_noMu
        CombinedChain2('2b15_medium_j75_j40_a4tc_EFFS_EFxe30_noMu', '580', ['combined','comb__jmet'], [], [],
                       'L2_2b15_medium_j70_2j35_EFxe30_noMu', ['L2_2b15_medium_j70_2j35_2L1J10', 'L2_noL2_noMu'], 'L1_2J10_J50', 1, 0,
                       'EF_2b15_medium_j75_j40_a4tc_EFFS_EFxe30_noMu', ['EF_2b15_medium_2L1J10','EF_j75_j40_a4tc_EFFS_2L1J10','EF_xe30_noMu'], 1, 0),
        
        CombinedChain2('b15_medium_j100_j40_a4tc_EFFS_EFxe30_noMu', '2580', ['combined','comb__jmet'], [], [],
                       'L2_b15_medium_j95_j35_EFxe30_noMu', ['L2_b15_medium_j95_j35', 'L2_noL2_noMu'], 'L1_J75', 1, 0,
                       'EF_b15_medium_j100_j40_a4tc_EFFS_EFxe30_noMu', ['EF_b15_medium_j100_j40_a4tc_EFFS', 'EF_xe30_noMu'], 1, 0),
        
        # To be seeded by L1_2J15_J50_XE35
        CombinedChain2('b15_medium_j75_j40_a4tc_EFFS_xe50_noMu', '2581', ['combined','comb__jmet'], [], [],
                       'L2_b15_medium_j70_j35_xe35_noMu', ['L2_b15_medium_j70_j35', 'L2_xe35_noMu'], 'L1_2J15_J50', 1, 0,
                       'EF_b15_medium_j75_j40_a4tc_EFFS_xe50_noMu', ['EF_b15_medium_j75_j40_a4tc_EFFS', 'EF_xe50_noMu'], 1, 0),
        
        CombinedChain2('b75_medium_j100_a4tc_EFFS_EFxe30_noMu', '2582', ['combined','comb__jmet'], [], [],
                       'L2_b75_medium_j95_EFxe30_noMu', ['L2_b75_medium_j95', 'L2_noL2_noMu'], 'L1_J75', 1, 0,
                       'EF_b75_medium_j100_a4tc_EFFS_EFxe30_noMu', ['EF_b75_medium_j100_a4tc_EFFS', 'EF_xe30_noMu'], 1, 0),
        
        CombinedChain2('b75_medium_j100_a4tc_EFFS_EFxe20_noMu_ht200', '2583', ['combined','comb__jmet'], [], [],
                       'L2_b75_medium_j95_EFxe20_noMu', ['L2_b75_medium_j95', 'L2_noL2_noMu'], 'L1_J75', 1, 0,
                       'EF_b75_medium_j100_a4tc_EFFS_EFxe20_noMu_ht200', ['EF_b75_medium_j100_a4tc_EFFS_ht200', 'EF_xe20_noMu'], 1, 0),
        
        ]

    for item in BjetMETs:
        item.groupL2 += [ "RATE:BjetMET", "RATE:SUSY", 'BW:Bjets']
        item.groupEF += [ "RATE:BjetMET","RATE:SUSY", 'BW:Bjets']

        

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():

    BjetMETs = [

        CombinedChain2('b180_loose_j180_a10tcem_L2j140_EFxe50_tclcw', '7591', ['combined','comb__jmet'], [], [],
                       'L2_b140_loose_j140_c4cchad_EFxe', ['L2_b140_loose_j140_c4cchad', 'L2_noL2'], 'L1_J75', 1, 0,
                       'EF_b180_loose_j180_a10tcem_L2j140_EFxe50_tclcw',['EF_b180_loose_j180_a10tcem_L2j140','EF_xe50_tclcw_noL2'], 1, 0, doOrderEF=True),
        
        ###L1_J75 -> L2_b140_medium_j140_c4cchad -> EF_b180_medium_j180_a10tcem_xe50_tclcw
        CombinedChain2('b180_medium_j180_a10tcem_L2j140_EFxe50_tclcw', '7592', ['combined','comb__jmet'], [], [],
                       'L2_b140_medium_j140_c4cchad_EFxe', ['L2_b140_medium_j140_c4cchad', 'L2_noL2'], 'L1_J75', 1, 0,
                       'EF_b180_medium_j180_a10tcem_L2j140_EFxe50_tclcw',['EF_b180_medium_j180_a10tcem_L2j140','EF_xe50_tclcw_noL2'], 1, 0, doOrderEF=True),

        ##Bjets + MET
        CombinedChain2('b55_mediumEF_j110_j55_a4tchad_xe60_tclcw', '616', ['combined','comb__jmet'], [], [],
                       'L2_j105_j50_c4cchad_xe40',  [], 'L1_J50_XE35', 1, 0,
                       'EF_b55_mediumEF_j110_j55_xe60_tclcw',    ['EF_b55_mediumEF_j110_j55_a4tchad','EF_xe60_tclcw_loose'], 1, 0, doOrderEF=True),
        
        CombinedChain2('b45_mediumEF_j110_j45_a4tchad_xe60_tclcw', '617', ['combined','comb__jmet'], [], [],
                       'L2_j105_j40_c4cchad_xe40',  [], 'L1_J50_XE35', 1, 0,
                       'EF_b45_mediumEF_j110_j45_xe60_tclcw',    ['EF_b45_mediumEF_j110_j45_a4tchad','EF_xe60_tclcw_loose'], 1, 0, doOrderEF=True),
        
        CombinedChain2('b80_loose_j80_a4tchad_xe70_tclcw', '618', ['combined','comb__jmet'], [], [],
                       'L2_b75_loose_j75_c4cchad_xe45',  [], 'L1_J30_XE40', 1, 0,
                       'EF_b80_loose_j80_a4tchad_xe70_tclcw',    ['EF_b80_loose_j80_a4tchad','EF_xe70_tclcw'], 1, 0, doOrderEF=True),

        
        
        CombinedChain2('b145_mediumEF_j145_a4tchad_L2EFxe60_tclcw', '619', ['combined','comb__jmet'], [], [],
                       'L2_j140_c4cchad_L2xe50',  ['L2_j140_c4cchad','L2_xe50_tight_unseeded'], 'L1_J75', 1, 0,
                       'EF_b145_mediumEF_j145_a4tchad_L2EFxe60_tclcw',    ['EF_b145_mediumEF_j145_a4tchad','EF_xe60_tclcw_tight_unseeded'], 1, 0, doOrderEF=True),

        CombinedChain2('2b45_loose_j145_j45_a4tchad_EFxe40_tclcw', '620', ['combined','comb__jmet'], [], [],
                       'L2_2b40_loose_j140_j40_c4cchad_EFxe',  ['L2_2b40_loose_j140_j40_c4cchad', 'L2_noL2'], 'L1_J75', 1, 0,
                       'EF_2b45_loose_j145_j45_a4tchad_EFxe40_tclcw',    ['EF_2b45_loose_j145_j45_a4tchad','EF_xe40_tclcw_noL2'], 1, 0, doOrderEF=True),
        
        ####L1_3J15_J50 -> L2_b30_loose_j105_2j30_c4cchad -> EF_b35_loose_j110_2j35_EFxe80_tclcw
        ####L1_J75 -> L2_j140_c4cchad_L2xe45 -> EF_b45_looseEF_j145_a4tchad_L2EFxe60_tclcw 

        CombinedChain2('b35_loose_j110_2j35_a4tchad_EFxe80_tclcw', '621', ['combined','comb__jmet'], [], [],
                       'L2_b30_loose_j105_2j30_c4cchad_EFxe',  ['L2_b30_loose_j105_2j30_c4cchad', 'L2_noL2'], 'L1_3J15_J50', 1, 0,
                       'EF_b35_loose_j110_2j35_a4tchad_EFxe80_tclcw', ['EF_b35_loose_j110_2j35_a4tchad','EF_xe80_tclcw_noL2'], 1, 0, doOrderEF=True),
        
        CombinedChain2('b45_looseEF_j145_a4tchad_L2EFxe60_tclcw', '622', ['combined','comb__jmet'], [], [],
                       'L2_j140_c4cchad_L2xe45',  ['L2_j140_c4cchad','L2_xe45_unseeded'], 'L1_J75', 1, 0,
                       'EF_b45_looseEF_j145_a4tchad_L2EFxe60_tclcw', ['EF_b45_looseEF_j145_a4tchad','EF_xe60_tclcw_unseeded'], 1, 0, doOrderEF=True),
        

        
        ##ZH version with b tag
        CombinedChain2('b80_loose_j80_a4tchad_xe55_tclcw', '611', ['combined','comb__jmet'], [], [],
                       'L2_b75_loose_j75_c4cchad_xe40',  [], 'L1_J30_XE35', 1, 0,
                       'EF_b80_loose_j80_a4tchad_xe55_tclcw',    ['EF_b80_loose_j80_a4tchad','EF_xe55_tclcw'], 1, 0, doOrderEF=True),
        
        CombinedChain2('b80_loose_j80_a4tchad_xe60_tclcw', '612', ['combined','comb__jmet'], [], [],
                       'L2_b75_loose_j75_c4cchad_xe45',  [], 'L1_J30_XE40', 1, 0,
                       'EF_b80_loose_j80_a4tchad_xe60_tclcw',    ['EF_b80_loose_j80_a4tchad','EF_xe60_tclcw'], 1, 0, doOrderEF=True),

        CombinedChain2('b80_loose_j80_a4tchad_xe60_tclcw_loose', '91', ['combined','comb__jmet'], [], [],
                       'L2_b75_loose_j75_c4cchad_xe40',  [], 'L1_J30_XE35', 1, 0,
                       'EF_b80_loose_j80_a4tchad_xe60_tclcw_loose',   ['EF_b80_loose_j80_a4tchad','EF_xe60_tclcw_loose'], 1, 0, doOrderEF=True),
       
        
        CombinedChain2('b80_loose_j80_a4tchad_xe75_tclcw', '613', ['combined','comb__jmet'], [], [],
                       'L2_b75_loose_j75_c4cchad_xe55',  [], 'L1_J30_XE50', 1, 0,
                       'EF_b80_loose_j80_a4tchad_xe75_tclcw',    ['EF_b80_loose_j80_a4tchad','EF_xe75_tight1_tclcw'], 1, 0, doOrderEF=True),

        CombinedChain2('b110_loose_j110_a4tchad_xe55_tclcw', '614', ['combined','comb__jmet'], [], [],
                       'L2_b105_loose_j105_c4cchad_xe40',  [], 'L1_J50_XE35', 1, 0,
                       'EF_b110_loose_j110_a4tchad_xe55_tclcw',    ['EF_b110_loose_j110_a4tchad','EF_xe55_tclcw'], 1, 0, doOrderEF=True),
        
        CombinedChain2('b110_loose_j110_a4tchad_xe60_tclcw', '615', ['combined','comb__jmet'], [], [],
                       'L2_b105_loose_j105_c4cchad_xe45',  [], 'L1_J50_XE40', 1, 0,
                       'EF_b110_loose_j110_a4tchad_xe60_tclcw',    ['EF_b110_loose_j110_a4tchad','EF_xe60_tclcw'], 1, 0, doOrderEF=True),
        
        #L1_J30_XE35 -> L2_b75_loose_j75_c4cchad_xe40 -> EF_b50_medium_j80_j50_a4tchad_xe55_tcem
        #L1_J30_XE40 -> L2_b75_loose_j75_c4cchad_xe45 -> EF_b50_medium_j80_j50_a4tchad_xe60_tcem
        #L1_J30_XE45 -> L2_b75_loose_j75_c4cchad_xe50 -> EF_b50_loose_j80_j50_a4tchad_xe65_tcem: L1_J30_XE50 -> L2_b75_loose_j75_c4cchad_xe55 -> EF_b50_loose_j80_j50_a4tchad_xe70_tcem
        


        ]

    for item in BjetMETs:
        item.groupL2 += [ "RATE:BjetMET", "RATE:SUSY", 'BW:Bjets']
        item.groupEF += [ "RATE:BjetMET","RATE:SUSY", 'BW:Bjets']
     


BjetMuonJets = [
    CombinedChain2('mu22_j30_a4tc_EFFS', '284', ['combined','comb__muj'], [], [],
                   'L2_mu22_j30_a4tc_EFFS', ['L2_mu22', 'L2_j25'], 'L1_MU10_J10', 1, 0,
                   'EF_mu22_j30_a4tc_EFFS', ['EF_mu22', 'EF_j30_a4tc_EFFS'], 1, 0, doBjetMuonJet=True),
    ]

for item in BjetMuonJets:
    item.groupL2 += [ "RATE:BjetMuonJet",'BW:Bjets']
    item.groupEF += [ "RATE:BjetMuonJet",'BW:Bjets']



mymujets = [
    CombinedChain2_mujet('mu4T_j15_a4tchad_matched', '369', ['combined', 'jets'], [], [],
                         'L2_mu4T', [], 'L1_MU4', 1, 0,
                         'EF_mu4T_j15_a4tchad_matched', ['EF_mu4T', 'EF_b15_NoCut_j15_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j25_a4tchad_matched', '407', ['combined', 'jets'], [], [],
                         'L2_mu4T', [], 'L1_MU4', 1, 0,
                         'EF_mu4T_j25_a4tchad_matched', ['EF_mu4T', 'EF_b25_NoCut_j25_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j35_a4tchad_matched', '409', ['combined', 'jets'], [], [],
                         'L2_mu4T', [], 'L1_MU4', 1, 0,
                         'EF_mu4T_j35_a4tchad_matched', ['EF_mu4T', 'EF_b35_NoCut_j35_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j45_a4tchad_matched', '417', ['combined', 'jets'], [], [],
                         'L2_mu4T_j40_c4cchad', ['L2_mu4T', 'L2_b40_NoCut_j40_c4cchad'], 'L1_MU4_J10', 1, 0,
                         'EF_mu4T_j45_a4tchad_matched', ['EF_mu4T', 'EF_b45_NoCut_j45_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j55_a4tchad_matched', '442', ['combined', 'jets'], [], [],
                         'L2_mu4T_j50_c4cchad', ['L2_mu4T', 'L2_b50_NoCut_j50_c4cchad'], 'L1_MU4_J15', 1, 0,
                         'EF_mu4T_j55_a4tchad_matched', ['EF_mu4T', 'EF_b55_NoCut_j55_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j65_a4tchad_matched', '455', ['combined', 'jets'], [], [],
                         'L2_mu4T_j60_c4cchad', ['L2_mu4T', 'L2_b60_NoCut_j60_c4cchad'], 'L1_MU4_J15', 1, 0,
                         'EF_mu4T_j65_a4tchad_matched', ['EF_mu4T', 'EF_b65_NoCut_j65_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j80_a4tchad_matched', '456', ['combined', 'jets'], [], [],
                         'L2_mu4T_j75_c4cchad', ['L2_mu4T', 'L2_b75_NoCut_j75_c4cchad'], 'L1_MU4_J30', 1, 0,
                         'EF_mu4T_j80_a4tchad_matched', ['EF_mu4T', 'EF_b80_NoCut_j80_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j110_a4tchad_matched', '457', ['combined', 'jets'], [], [],
                         'L2_mu4T_j105_c4cchad', [], 'L1_MU4_J50', 1, 0,
                         'EF_mu4T_j110_a4tchad_matched', ['EF_mu4T', 'EF_b110_NoCut_j110_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j145_a4tchad_matched', '458', ['combined', 'jets'], [], [],
                         'L2_mu4T_j140_c4cchad', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j145_a4tchad_matched', ['EF_mu4T', 'EF_b145_NoCut_j145_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j180_a4tchad_matched', '459', ['combined', 'jets'], [], [],
                         'L2_mu4T_j165_c4cchad', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j180_a4tchad_matched', ['EF_mu4T', 'EF_b180_NoCut_j180_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j220_a4tchad_matched', '463', ['combined', 'jets'], [], [],
                         'L2_mu4T_j165_c4cchad', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j220_a4tchad_matched', ['EF_mu4T', 'EF_b220_NoCut_j220_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j280_a4tchad_matched', '464', ['combined', 'jets'], [], [],
                         'L2_mu4T_j165_c4cchad', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j280_a4tchad_matched', ['EF_mu4T', 'EF_b280_NoCut_j280_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j360_a4tchad_matched', '465', ['combined', 'jets'], [], [],
                         'L2_mu4T_j165_c4cchad', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j360_a4tchad_matched', ['EF_mu4T', 'EF_b360_NoCut_j360_a4tchad'], 1, 0),

    # jetcalibdelayed
    CombinedChain2_mujet('mu4T_j110_a4tchad_matched_jetcalibdelayed', '5457', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T_j105_c4cchad', [], 'L1_MU4_J50', 1, 0,
                         'EF_mu4T_j110_a4tchad_matched_jetcalibdelayed', ['EF_mu4T', 'EF_b110_NoCut_j110_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j145_a4tchad_matched_jetcalibdelayed', '5458', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T_j140_c4cchad', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j145_a4tchad_matched_jetcalibdelayed', ['EF_mu4T', 'EF_b145_NoCut_j145_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j180_a4tchad_matched_jetcalibdelayed', '5459', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T_j165_c4cchad', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j180_a4tchad_matched_jetcalibdelayed', ['EF_mu4T', 'EF_b180_NoCut_j180_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j220_a4tchad_matched_jetcalibdelayed', '5463', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T_j165_c4cchad', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j220_a4tchad_matched_jetcalibdelayed', ['EF_mu4T', 'EF_b220_NoCut_j220_a4tchad'], 1, 0),
    
    CombinedChain2_mujet('mu4T_j15_a4tchad_matched_jetcalibdelayed', '5464', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T', [], 'L1_MU4', 1, 0,
                         'EF_mu4T_j15_a4tchad_matched_jetcalibdelayed', ['EF_mu4T', 'EF_b15_NoCut_j15_a4tchad'], 1, 0),
    
    CombinedChain2_mujet('mu4T_j15_a4tchad_matchedZ_jetcalibdelayed', '5465', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T', [], 'L1_MU4', 1, 0,
                         'EF_mu4T_j15_a4tchad_matchedZ_jetcalibdelayed', ['EF_mu4T', 'EF_b15_NoCut_j15_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j80_a4tchad_matched_jetcalibdelayed', '5466', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T_j75_c4cchad', ['L2_mu4T', 'L2_b75_NoCut_j75_c4cchad'], 'L1_MU4_J30', 1, 0,
                         'EF_mu4T_j80_a4tchad_matched_jetcalibdelayed', ['EF_mu4T', 'EF_b80_NoCut_j80_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j80_a4tchad_L2FS_matched_jetcalibdelayed', '5467', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T_j30_a4TTem', [], 'L1_MU4_J30', 1, 0,
                         'EF_mu4T_j80_a4tchad_L2FS_matched_jetcalibdelayed', ['EF_mu4T', 'EF_b80_NoCut_j80_a4tchad_L2FS'], 1, 0),
    


    ##mu-jet triggers with matching, seeding off L1_MU6_2J20
    CombinedChain2_mujet('mu4T_b55_medium_2j55_a4tchad_L1J20_matched', '2781', ['combined', 'jets'], [], [],
                         'L2_mu4T_b50_medium_2j50_c4cchad_L1J20', ['L2_mu4T', 'L2_b50_medium_2j50_c4cchad_L1J20'], 'L1_MU6_2J20', 1, 0,
                         'EF_mu4T_b55_medium_2j55_a4tchad_L1J20_matched', ['EF_mu4T', 'EF_b55_medium_2j55_a4tchad_L1J20'], 1, 0),

    CombinedChain2_mujet('mu4T_2b55_loose_2j55_a4tchad_L1J20_matched', '2782', ['combined', 'jets'], [], [],
                         'L2_mu4T_2b50_loose_2j50_c4cchad_L1J20', ['L2_mu4T', 'L2_2b50_loose_2j50_c4cchad_L1J20'], 'L1_MU6_2J20', 1, 0,
                         'EF_mu4T_2b55_loose_2j55_a4tchad_L1J20_matched', ['EF_mu4T', 'EF_2b55_loose_2j55_a4tchad_L1J20'], 1, 0),

    CombinedChain2_mujet('mu4T_b65_medium_2j65_a4tchad_matched', '2783', ['combined', 'jets'], [], [],
                         'L2_mu4T_b60_medium_2j60_c4cchad', ['L2_mu4T', 'L2_b60_medium_2j60_c4cchad'], 'L1_MU6_2J20', 1, 0,
                         'EF_mu4T_b65_medium_2j65_a4tchad_matched', ['EF_mu4T', 'EF_b65_medium_2j65_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu6T_2b55_medium_2j55_a4tchad_L1J20_matched', '3782', ['combined', 'jets'], [], [],
                         'L2_mu6T_2b50_medium_2j50_c4cchad_L1J20', ['L2_mu6T', 'L2_2b50_medium_2j50_c4cchad_L1J20'], 'L1_MU6_2J20', 1, 0,
                         'EF_mu6T_2b55_medium_2j55_a4tchad_L1J20_matched', ['EF_mu6T', 'EF_2b55_medium_2j55_a4tchad_L1J20'], 1, 0),

    

   ## mu-jet triggers with matching in deltaR and deltaZ

    CombinedChain2_mujet('mu4T_j15_a4tchad_matchedZ', '315', ['combined', 'jets'], [], [],
                         'L2_mu4T', [], 'L1_MU4', 1, 0,
                         'EF_mu4T_j15_a4tchad_matchedZ', ['EF_mu4T', 'EF_b15_NoCut_j15_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j25_a4tchad_matchedZ', '356', ['combined', 'jets'], [], [],
                         'L2_mu4T', [], 'L1_MU4', 1, 0,
                         'EF_mu4T_j25_a4tchad_matchedZ', ['EF_mu4T', 'EF_b25_NoCut_j25_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j35_a4tchad_matchedZ', '362', ['combined', 'jets'], [], [],
                         'L2_mu4T', [], 'L1_MU4', 1, 0,
                         'EF_mu4T_j35_a4tchad_matchedZ', ['EF_mu4T', 'EF_b35_NoCut_j35_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j45_a4tchad_matchedZ', '363', ['combined', 'jets'], [], [],
                         'L2_mu4T_j40_c4cchad', ['L2_mu4T', 'L2_b40_NoCut_j40_c4cchad'], 'L1_MU4_J10', 1, 0,
                         'EF_mu4T_j45_a4tchad_matchedZ', ['EF_mu4T', 'EF_b45_NoCut_j45_a4tchad'], 1, 0),

    CombinedChain2_mujet('mu4T_j55_a4tchad_matchedZ', '364', ['combined', 'jets'], [], [],
                         'L2_mu4T_j50_c4cchad', ['L2_mu4T', 'L2_b50_NoCut_j50_c4cchad'], 'L1_MU4_J15', 1, 0,
                         'EF_mu4T_j55_a4tchad_matchedZ', ['EF_mu4T', 'EF_b55_NoCut_j55_a4tchad'], 1, 0),

    #L1.5 jets for mu+jets
    CombinedChain2_mujet('mu4T_j45_a4tchad_L2FS_matched', '472', ['combined', 'jets'], [], [],
                         'L2_mu4T_j10_a4TTem', [], 'L1_MU4_J10', 1, 0,
                         'EF_mu4T_j45_a4tchad_L2FS_matched', ['EF_mu4T', 'EF_b45_NoCut_j45_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j55_a4tchad_L2FS_matched', '475', ['combined', 'jets'], [], [],
                         'L2_mu4T_j15_a4TTem', [], 'L1_MU4_J15', 1, 0,
                         'EF_mu4T_j55_a4tchad_L2FS_matched', ['EF_mu4T', 'EF_b55_NoCut_j55_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j65_a4tchad_L2FS_matched', '476', ['combined', 'jets'], [], [],
                         'L2_mu4T_j15_a4TTem', [], 'L1_MU4_J15', 1, 0,
                         'EF_mu4T_j65_a4tchad_L2FS_matched', ['EF_mu4T', 'EF_b65_NoCut_j65_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j80_a4tchad_L2FS_matched', '479', ['combined', 'jets'], [], [],
                         'L2_mu4T_j30_a4TTem', [], 'L1_MU4_J30', 1, 0,
                         'EF_mu4T_j80_a4tchad_L2FS_matched', ['EF_mu4T', 'EF_b80_NoCut_j80_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j110_a4tchad_L2FS_matched', '480', ['combined', 'jets'], [], [],
                         'L2_mu4T_j50_a4TTem', [], 'L1_MU4_J50', 1, 0,
                         'EF_mu4T_j110_a4tchad_L2FS_matched', ['EF_mu4T', 'EF_b110_NoCut_j110_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j145_a4tchad_L2FS_matched', '481', ['combined', 'jets'], [], [],
                         'L2_mu4T_j75_a4TTem', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j145_a4tchad_L2FS_matched', ['EF_mu4T', 'EF_b145_NoCut_j145_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j180_a4tchad_L2FS_matched', '482', ['combined', 'jets'], [], [],
                         'L2_mu4T_j75_a4TTem', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j180_a4tchad_L2FS_matched', ['EF_mu4T', 'EF_b180_NoCut_j180_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j220_a4tchad_L2FS_matched', '483', ['combined', 'jets'], [], [],
                         'L2_mu4T_j75_a4TTem', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j220_a4tchad_L2FS_matched', ['EF_mu4T', 'EF_b220_NoCut_j220_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j280_a4tchad_L2FS_matched', '484', ['combined', 'jets'], [], [],
                         'L2_mu4T_j75_a4TTem', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j280_a4tchad_L2FS_matched', ['EF_mu4T', 'EF_b280_NoCut_j280_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j360_a4tchad_L2FS_matched', '488', ['combined', 'jets'], [], [],
                         'L2_mu4T_j75_a4TTem', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j360_a4tchad_L2FS_matched', ['EF_mu4T', 'EF_b360_NoCut_j360_a4tchad_L2FS'], 1, 0),

    # jetcalibdelayed
    CombinedChain2_mujet('mu4T_j110_a4tchad_L2FS_matched_jetcalibdelayed', '5480', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T_j50_a4TTem', [], 'L1_MU4_J50', 1, 0,
                         'EF_mu4T_j110_a4tchad_L2FS_matched_jetcalibdelayed', ['EF_mu4T', 'EF_b110_NoCut_j110_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j145_a4tchad_L2FS_matched_jetcalibdelayed', '5481', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T_j75_a4TTem', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j145_a4tchad_L2FS_matched_jetcalibdelayed', ['EF_mu4T', 'EF_b145_NoCut_j145_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j180_a4tchad_L2FS_matched_jetcalibdelayed', '5482', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T_j75_a4TTem', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j180_a4tchad_L2FS_matched_jetcalibdelayed', ['EF_mu4T', 'EF_b180_NoCut_j180_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j220_a4tchad_L2FS_matched_jetcalibdelayed', '5483', ['combined', 'jetcalibdelayed'], [], [],
                         'L2_mu4T_j75_a4TTem', [], 'L1_J75', 1, 0,
                         'EF_mu4T_j220_a4tchad_L2FS_matched_jetcalibdelayed', ['EF_mu4T', 'EF_b220_NoCut_j220_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j45_a4tchad_L2FS_matchedZ', '469', ['combined', 'jets'], [], [],
                         'L2_mu4T_j10_a4TTem', [], 'L1_MU4_J10', 1, 0,
                         'EF_mu4T_j45_a4tchad_L2FS_matchedZ', ['EF_mu4T', 'EF_b45_NoCut_j45_a4tchad_L2FS'], 1, 0),

    CombinedChain2_mujet('mu4T_j55_a4tchad_L2FS_matchedZ', '471', ['combined', 'jets'], [], [],
                         'L2_mu4T_j15_a4TTem', [], 'L1_MU4_J15', 1, 0,
                         'EF_mu4T_j55_a4tchad_L2FS_matchedZ', ['EF_mu4T', 'EF_b55_NoCut_j55_a4tchad_L2FS'], 1, 0),

    
    
    ]

for item in mymujets:
    item.groupL2 += [ "RATE:BjetMuonJet",'BW:Bjets']
    item.groupEF += [ "RATE:BjetMuonJet",'BW:Bjets']


BjetMuonJets_noTRTMuons = [
    CombinedChain2('mu4_MSonly_j15_a2hi_EFFS_L1TE10', '837', ['combined','comb__muj'], [], [],
                   'L2_L1TE10_NoAlg', ['L2_mu4_MSonly_EFFS_L1TE10', 'L2_L1TE10_NoAlg'], 'L1_TE10', 1, 0,
                   'EF_mu4_MSonly_j15_a2hi_EFFS_L1TE10', ['EF_mu4_MSonly_EFFS_L1TE10', 'EF_j15_a2hi_EFFS_L1TE10'], 1, 0, doBjetMuonJet=False),
    ]

for item in BjetMuonJets_noTRTMuons:
    item.groupL2 += [ "RATE:BjetMuonJet",'BW:Bjets']
    item.groupEF += [ "RATE:BjetMuonJet",'BW:Bjets']

        
TripleSignatures_EMScale = []
_TripleSignatures_EMScale_JET_MET_MU = [
    ]

_TripleSignatures_EMScale_JET_MET_ELE = [ ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    _TripleSignatures_EMScale_JET_MET_ELE += [
        
        CombinedChain2('e13_etcut_xs60_noMu_dphi2j10xe07', '150', ['combined','comb__emet'], [], [],
                       'L2_e13_etcut_xs45_noMu', ['L2_xs45_noMu_L1EM10XS45','L2_e13_etcut'], 'L1_EM10_XS45', 1, 0,
                       'EF_e13_etcut_xs60_noMu_dphi2j10xe07', ['EF_xs60_noMu_L1EM10XS45','EF_j10_a4tc_EFFS','EF_e13_etcut'], 1, 0,
                       dPhiMetJet=True, dPhiCut="dPhi_07_2j10", doOrderL2=True, doOrderEF=True),
        CombinedChain2('e20_etcut_xs60_noMu_dphi2j10xe07', '506', ['combined','comb__emet'], [], [],
                       'L2_e20_etcut_xs45_noMu', ['L2_xs45_noMu_unseeded','L2_e20_etcut'], 'L1_EM14', 1, 0,
                       'EF_e20_etcut_xs60_noMu_dphi2j10xe07', ['EF_xs60_noMu_unseeded','EF_j10_a4tc_EFFS','EF_e20_etcut'], 1, 0,
                       dPhiMetJet=True, dPhiCut="dPhi_07_2j10", doOrderL2=True, doOrderEF=True),
        CombinedChain2('e13_etcut_xs60_noMu_dphi2j10xe20', '168', ['combined','comb__emet'], [], [],
                       'L2_e13_etcut_xs45_noMu', ['L2_xs45_noMu_unseeded','L2_e13_etcut'], 'L1_EM7_XS45', 1, 0,
                       'EF_e13_etcut_xs60_noMu_dphi2j10xe20', ['EF_xs60_noMu_unseeded','EF_j10_a4tc_EFFS','EF_e13_etcut'], 1, 0,
                       dPhiMetJet=True, dPhiCut="dPhi_20_2j10", doOrderL2=True, doOrderEF=True),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
    _TripleSignatures_EMScale_JET_MET_ELE += [
        CombinedChain2('e13_etcutTrk_xs60_noMu_dphi2j10xe07', '150', ['combined','comb__emet'], [], [],
                       'L2_e13_etcutTrk_xs45_noMu', ['L2_xs45_noMu_unseeded','L2_e13_etcutTrk_EM7'], 'L1_EM7_XS45', 1, 0,
                       'EF_e13_etcutTrk_xs60_noMu_dphi2j10xe07', ['EF_xs60_noMu_unseeded','EF_j10_a4tc_EFFS','EF_e13_etcutTrk_EM7'], 1, 0,
                       dPhiMetJet=True, dPhiCut="dPhi_07_2j10", doOrderL2=True, doOrderEF=True),

        CombinedChain2('e13_etcutTrk_xs45_noMu_dphi2j10xe20', '167', ['combined','comb__emet'], [], [], 
                       'L2_e13_etcutTrk_xs30_noMu', ['L2_xs30_noMu_unseeded','L2_e13_etcutTrk_EM7'], 'L1_EM7_XS30', 1, 0,
                       'EF_e13_etcutTrk_xs45_noMu_dphi2j10xe20', ['EF_xs45_noMu_unseeded','EF_j10_a4tc_EFFS','EF_e13_etcutTrk_EM7'], 1, 0,
                       dPhiMetJet=True, dPhiCut="dPhi_20_2j10", doOrderL2=True, doOrderEF=True),

        CombinedChain2('e13_etcutTrk_xs60_noMu_dphi2j10xe20', '168', ['combined','comb__emet'], [], [],
                       'L2_e13_etcutTrk_xs45_noMu', ['L2_xs45_noMu_unseeded','L2_e13_etcutTrk_EM7'], 'L1_EM7_XS45', 1, 0,
                       'EF_e13_etcutTrk_xs60_noMu_dphi2j10xe20', ['EF_xs60_noMu_unseeded','EF_j10_a4tc_EFFS','EF_e13_etcutTrk_EM7'], 1, 0,
                       dPhiMetJet=True, dPhiCut="dPhi_20_2j10", doOrderL2=True, doOrderEF=True),

        CombinedChain2('e20_etcutTrk_xs60_noMu_dphi2j10xe07', '506', ['combined','comb__emet'], [], [],
                       'L2_e20_etcutTrk_xs45_noMu', ['L2_xs45_noMu_unseeded','L2_e20_etcutTrk_EM12'], 'L1_EM12_XS45', 1, 0,
                       'EF_e20_etcutTrk_xs60_noMu_dphi2j10xe07', ['EF_xs60_noMu_unseeded','EF_j10_a4tc_EFFS','EF_e20_etcutTrk_EM12'], 1, 0,
                       dPhiMetJet=True, dPhiCut="dPhi_07_2j10", doOrderL2=True, doOrderEF=True),

        CombinedChain2('e20_etcutTrk_xe40_noMu_dphi2j10xe20', '169', ['combined','comb__emet'], [], [],
                       'L2_e20_etcutTrk_xe25_noMu', ['L2_xe25_noMu_unseeded','L2_e20_etcutTrk_EM12'], 'L1_EM12_XE30', 1, 0,
                       'EF_e20_etcutTrk_xe40_noMu_dphi2j10xe20', ['EF_xe40_noMu_unseeded','EF_j10_a4tc_EFFS','EF_e20_etcutTrk_EM12'], 1, 0,
                       dPhiMetJet=True, dPhiCut="dPhi_20_2j10", doOrderL2=True, doOrderEF=True),

        ]
    
_TripleSignatures_EMScale_TAU_ELE_JET = [
    # FIX L1 SEEDS FOR ALL THESE CHAINS (for MC_pp_v2)
    # Use L1_2TAU6_EM10_J5 DONE
    CombinedChain2('tau16_medium_e15_tight_j20', 4319, ['combined','comb__tauej'], [], [], \
                   'L2_tau16_medium_e15_tight_j15', ['L2_tau16_medium', 'L2_e15_tight', 'L2_j15'],
                   'L1_2TAU6_EM10_J5', 1, 0, \
                   'EF_tau16_medium_e15_tight_j20', ['EF_tau16_medium', 'EF_e15_tight', 'EF_j20_a4tc_EFFS'], 1, 0), 
    # Use L1_2TAU6_EM10_J10 DONE
    CombinedChain2('tau16_medium_e15_tight_j30', 4320, ['combined','comb__tauej'], [], [], \
                   'L2_tau16_medium_e15_tight_j15', ['L2_tau16_medium', 'L2_e15_tight', 'L2_j25'],
                   'L1_2TAU6_EM10_J10', 1, 0, \
                   'EF_tau16_medium_e15_tight_j30', ['EF_tau16_medium', 'EF_e15_tight', 'EF_j30_a4tc_EFFS'], 1, 0), 
    ]
_TripleSignatures_EMScale_TAU_MU_JET = [
    # FIX L1 SEEDS FOR ALL THESE CHAINS (for MC_pp_v2)
    # Use L1_2TAU6_MU10_J5 DONE
    CombinedChain2('tau16_medium_mu15_j20', 4321, ['combined','comb__taumuj'], [], [], \
                   'L2_tau16_medium_mu15_j15', [ 'L2_mu15','L2_tau16_medium', 'L2_j15'],
                   'L1_2TAU6_MU10_J5', 1, 0, \
                   'EF_tau16_medium_mu15_j20', [ 'EF_mu15','EF_tau16_medium', 'EF_j20_a4tc_EFFS'], 1, 0), 
    # Use L1_2TAU6_MU10_J10 DONE
    CombinedChain2('tau16_medium_mu15_j30', 4322, ['combined','comb__taumuj'], [], [], \
                   'L2_tau16_medium_mu15_j15', [ 'L2_mu15','L2_tau16_medium', 'L2_j25'],
                   'L1_2TAU6_MU10_J10', 1, 0, \
                   'EF_tau16_medium_mu15_j30', ['EF_mu15','EF_tau16_medium',  'EF_j30_a4tc_EFFS'], 1, 0), 
    ]

for item in _TripleSignatures_EMScale_JET_MET_ELE:
    if hasattr(item, 'group'):
        SIG=getattr(item, 'sig_id')
        if 'dphi' in SIG:
            item.group   += [ "RATE:JetMET", "RATE:ElectronMET","RATE:ElectronJet",'BW:Egamma']
        else:
            item.group   += [ "RATE:JetMET", "RATE:ElectronMET","RATE:ElectronJet","RATE:SUSY",'BW:Egamma']        
    else:
        SIG=getattr(item, 'ef_chain_name')
        if 'dphi' in SIG:
            item.groupL2 += [ "RATE:JetMET", "RATE:ElectronMET","RATE:ElectronJet",'BW:Egamma']
            item.groupEF += [ "RATE:JetMET", "RATE:ElectronMET","RATE:ElectronJet",'BW:Egamma']
        else:        
            item.groupL2 += [ "RATE:JetMET", "RATE:ElectronMET","RATE:ElectronJet","RATE:SUSY",'BW:Egamma']
            item.groupEF += [ "RATE:JetMET", "RATE:ElectronMET","RATE:ElectronJet","RATE:SUSY",'BW:Egamma']
    TripleSignatures_EMScale += [item]

for item in _TripleSignatures_EMScale_JET_MET_MU:
    if hasattr(item, 'group'):
        item.group   += [ "RATE:MuonJet", "RATE:METMuon","RATE:JetMet","RATE:SUSY",'BW:Muon']        
    else:
        item.groupL2 += [ "RATE:MuonJet", "RATE:METMuon","RATE:JetMet","RATE:SUSY",'BW:Muon']
        item.groupEF += [ "RATE:MuonJet", "RATE:METMuon","RATE:JetMet","RATE:SUSY",'BW:Muon']
    TripleSignatures_EMScale += [item] 
    
for item in _TripleSignatures_EMScale_TAU_ELE_JET:
    if hasattr(item, 'group'):
        item.group   += [ "RATE:ElectronJet", "RATE:ElectronMET","RATE:JetTau",'BW:Tau']
    else:
        item.groupL2 += [ "RATE:ElectronJet", "RATE:ElectronMET","RATE:JetTau",'BW:Tau']
        item.groupEF += [ "RATE:ElectronJet", "RATE:ElectronMET","RATE:JetTau",'BW:Tau']
    TripleSignatures_EMScale += [item] 
for item in _TripleSignatures_EMScale_TAU_MU_JET:
    if hasattr(item, 'group'):
        item.group   += [ "RATE:MuonJet", "RATE:MuonMET","RATE:JetTau",'BW:Tau']
    else:
        item.groupL2 += [ "RATE:MuonJet", "RATE:MuonMET","RATE:JetTau",'BW:Tau']
        item.groupEF += [ "RATE:MuonJet", "RATE:MuonMET","RATE:JetTau",'BW:Tau']
    TripleSignatures_EMScale += [item] 

del _TripleSignatures_EMScale_JET_MET_ELE
del _TripleSignatures_EMScale_JET_MET_MU
del _TripleSignatures_EMScale_TAU_ELE_JET
del _TripleSignatures_EMScale_TAU_MU_JET


JetGaps = [
    ]

for item in JetGaps :
    item.groupL2+=[ "RATE:MinbiasJet",'BW:Jets']
    item.groupEF+=[ "RATE:MinbiasJet",'BW:Jets']

JetEMScaleGaps = [
    ]

for item in JetEMScaleGaps :
    item.groupL2+=[ "RATE:MinbiasJet",'BW:Jets']
    item.groupEF+=[ "RATE:MinbiasJet",'BW:Jets'] 

