
ConfigureL2=True
ConfigureEF=True

include.block('TrigL2CosmicMuonHypo/TriggerConfig_Cosmic.py')

if (globals().has_key("doEFIDCosmics")):
    global doEFIDCosmics
else:
    doEFIDCosmics=False
    
if (globals().has_key("doIDHitsHypo")):
    global doIDHitsHypo
else:
    doIDHitsHypo=False

print "#################################################################################"
print " Trigger Configuration for Cosmics Slices:                                     "
print " doEFIDCosmics: ",doEFIDCosmics
print " doIDHitsHypo: ",doIDHitsHypo
print " ConfigureL2: ",ConfigureL2
print " ConfigureEF: ",ConfigureEF

print " CosmicL1Threshold: ",CosmicL1Threshold
print " CosmicL1Signature: ",CosmicL1Signature

print " CosmicL1TGCThreshold: ",CosmicL1TGCThreshold
print " CosmicL1TGCSignature: ",CosmicL1TGCSignature

print " CosmicL1StandaloneThreshold: ",CosmicL1StandaloneThreshold
print " CosmicL1StandaloneSignature: ",CosmicL1StandaloneSignature

print " CosmicL1CaloThreshold: ",CosmicL1CaloThreshold
print " CosmicL1CaloSignature: ",CosmicL1CaloSignature

print "CosmicStandaloneL2Prescale: ",CosmicStandaloneL2Prescale
print "CosmicStandaloneEFPrescale: ",CosmicStandaloneEFPrescale

print "TrigEGamma prescale factor:",TrigEGammaL2PrescaleFactor  
print "TrigEGamma passthrough:",TrigEGammaL2PassThrough    

print "#################################################################################"

# include('TrigL2CosmicMuonHypo/TriggerConfig_Cosmic_flags.py')

if TriggerFlags.doLVL1 or ConfigureL2:
    include('TrigL2CosmicMuonHypo/TriggerConfig_Cosmic_LVL1.py')

# create configurable instances ...

# from TrigL2CosmicCombined.TrigL2CosmicCombined_Config import CombinedIDHalfTracks_Cosmic
# htcombinedCosmics = CombinedIDHalfTracks_Cosmic() # here we created an instance
# 
# from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CombinedIDHalfTracksHypo_Cosmic
# htcombinedCosmicsHypo = CombinedIDHalfTracksHypo_Cosmic() # here we created an instance
from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo

print "DOEFIDCOSMICS/DOIDHITSHYPO: ",doEFIDCosmics,"/",doIDHitsHypo

def StreamTagName(TEName):
    #if (TEName==CosmicL1Threshold):             return 'Cosmics_'+TEName
    #elif (TEName==CosmicL1CaloThreshold):       return 'Cosmics_'+TEName
    #elif (TEName==CosmicL1StandaloneThreshold): return 'Cosmics_'+TEName
    #elif (TEName==CosmicL1TGCThreshold):        return 'Cosmics_'+TEName
    #else: return 'Cosmics'
    return 'Cosmics_'+TEName

if triggerMenu.signatureIsEnabled("cosmic_IDSCAN"):
    TE=CosmicL1Threshold
    L2chain = HLTChain( chain_name="L2_cosmic_IDSCAN", chain_counter="1900", \
                        lower_chain_name=CosmicL1Signature, level="L2",\
                        prescale="1", pass_through="0")
    L2chain.addTriggerTypeBit('1900')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    EFchain = HLTChain(chain_name='EF_cosmic_IDSCAN', chain_counter='1900',lower_chain_name='L2_cosmic_IDSCAN', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1900')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    from TrigEFIDCosmic.TrigEFIDCosmic_Config import *
    from TrigEFIDCosmic.IDHitsHypo_Config import *
    if (ConfigureL2):
        from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Cosmics
        idscanCosmics = TrigIDSCAN_Cosmics() # here we created an instance
        #idscanCosmics.OutputLevel=VERBOSE
        idscanCosmics.doTRTpropagation = True

        L2Seq=[idscanCosmics]
        if (doIDHitsHypo):
            IDHitsHypo_IDSCAN_L2=IDHitsHypo_IDSCAN_L2()
            L2Seq+=[IDHitsHypo_IDSCAN_L2]
        TE=triggerPythonConfig.addSequence(CosmicL1Threshold,L2Seq, 'idscan')
        
    L2chain.addHLTSignature(TE)

    if (ConfigureEF):
        if (doEFIDCosmics):
            TrigEFIDCosmicAlgo_IDSCAN_EF=TrigEFIDCosmicAlgo_IDSCAN_EF()
            EFSeq=[TrigEFIDCosmicAlgo_IDSCAN_EF]
            if (doIDHitsHypo):
                IDHitsHypo_EF=IDHitsHypo_IDSCAN_EF()
                EFSeq+=[IDHitsHypo_EF]
            TE=triggerPythonConfig.addSequence(TE,EFSeq, 'TrigEFIDCosmic_IDSCAN_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(L2chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled("cosmic_SiTrack"):
    TE=CosmicL1Threshold
    L2chain = HLTChain( chain_name="L2_cosmic_SiTrack", chain_counter="1901", \
                        lower_chain_name=CosmicL1Signature, level="L2", \
                        prescale="1", pass_through="0")
    L2chain.addTriggerTypeBit('1901')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    EFchain = HLTChain(chain_name='EF_cosmic_SiTrack', chain_counter='1901',lower_chain_name='L2_cosmic_SiTrack', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1901')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')


    from TrigEFIDCosmic.TrigEFIDCosmic_Config import *
    from TrigEFIDCosmic.IDHitsHypo_Config import *
    if (ConfigureL2 or ConfigureEF):
        from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Cosmics
        sitrackCosmics = TrigSiTrack_Cosmics() # here we created an instance
        sitrackCosmics.OutputLevel=DEBUG
        sitrackCosmics.Map_UsePixel = False
    
        L2Seq=[sitrackCosmics]
        if (doIDHitsHypo):
            IDHitsHypo_SiTrack_L2=IDHitsHypo_SiTrack_L2()
            L2Seq+=[IDHitsHypo_SiTrack_L2]
        TE=triggerPythonConfig.addSequence(CosmicL1Threshold,L2Seq, 'sitrack')
        
    L2chain.addHLTSignature(TE)

    if (ConfigureEF):
        if (doEFIDCosmics):
            TrigEFIDCosmicAlgo_SiTrack_EF=TrigEFIDCosmicAlgo_SiTrack_EF()
            EFSeq=[TrigEFIDCosmicAlgo_SiTrack_EF]
            if (doIDHitsHypo):
                IDHitsHypo_EF=IDHitsHypo_SiTrack_EF()
                EFSeq+=[IDHitsHypo_EF]
            TE=triggerPythonConfig.addSequence(TE,EFSeq, 'TrigEFIDCosmic_SiTrack_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(L2chain)
    triggerPythonConfig.addHLTChain(EFchain)

   #     IncludeTrigEFIDCosmic(TrigEFIDCosmicAlgo_SiTrack_EF,IDHitsHypo_SiTrack_EF,chain,'EFIDCosmicsSiTrack')

if triggerMenu.signatureIsEnabled('cosmic_trtxk'):
    TE=CosmicL1Threshold
    L2chain = HLTChain(chain_name='L2_cosmic_trtxk', chain_counter='1902', lower_chain_name=CosmicL1Signature,\
                       level='L2',prescale='1', pass_through='0')
    L2chain.addTriggerTypeBit('1902')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    EFchain = HLTChain(chain_name='EF_cosmic_trtxk', chain_counter='1902',lower_chain_name='L2_cosmic_trtxk', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1902')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    from TrigEFIDCosmic.TrigEFIDCosmic_Config import *
    from TrigEFIDCosmic.IDHitsHypo_Config import *
    if (ConfigureL2):
        from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Cosmics
        trtsegm  = TrigTRTSegFinder_Cosmics()
        trtsegm.RoIhalfWidthDeltaPhi = 3.14
        trtsegm.RoIhalfWidthDeltaEta = 3.

        #trtTRKSegMakerTool=trtsegm.SegmentsMakerTool
        #trtTRKSegMakerTool.RIOonTrackToolYesDr="TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool"
        #trtTRKSegMakerTool.RIOonTrackToolNoDr="TRT_DriftCircleOnTrackNoDriftTimeTool/TRT_DriftCircleOnTrackNoDriftTimeTool"

        L2Seq = [trtsegm]
        if (doIDHitsHypo):
            IDHitsHypo_TRTXK_L2=IDHitsHypo_TRTXK_L2("IDHitsHypo_TRTXK_L2")
            L2Seq+=[IDHitsHypo_TRTXK_L2]
        TE=triggerPythonConfig.addSequence(TE,L2Seq, 'trtseg')
    L2chain.addHLTSignature(TE)

    if (ConfigureEF):
        if (doEFIDCosmics):
            TrigEFIDCosmicAlgo_TRTXK_EF=TrigEFIDCosmicAlgo_TRTXK_EF()
            EFSeq=[TrigEFIDCosmicAlgo_TRTXK_EF]
            if (doIDHitsHypo):
                IDHitsHypo_TRTXK_EF=IDHitsHypo_TRTXK_EF("IDHitsHypo_TRTXK_EF")
                EFSeq+=[IDHitsHypo_TRTXK_EF]
            TE=triggerPythonConfig.addSequence(TE,EFSeq, 'TrigEFIDCosmic_TRTXK_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(L2chain)
    triggerPythonConfig.addHLTChain(EFchain)
    
if triggerMenu.signatureIsEnabled('cosmic_trtxk_TRTTrkHypo'):
    TE=CosmicL1Threshold
    chain = HLTChain('L2_cosmic_trtxk_TRTTrkHypo', '1914', CosmicL1Signature, level='L2',
                     prescale='1', pass_through='0')
    chain.addTriggerTypeBit('1914')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_trtxk_TRTTrkHypo', chain_counter='1914',lower_chain_name='L2_cosmic_trtxk_TRTTrkHypo', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1914')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Cosmics
        trtsegm  = TrigTRTSegFinder_Cosmics("trtsegm_TRTTrkHypo")
        trtsegm.RoIhalfWidthDeltaPhi = 3.14
        trtsegm.RoIhalfWidthDeltaEta = 3.
    
        #trtTRKSegMakerTool=trtsegm.SegmentsMakerTool
        #trtTRKSegMakerTool.RIOonTrackToolYesDr="TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool"
        #trtTRKSegMakerTool.RIOonTrackToolNoDr="TRT_DriftCircleOnTrackNoDriftTimeTool/TRT_DriftCircleOnTrackNoDriftTimeTool"

        from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic
        trthypoCosmics = CosmicTrtHypo_Cosmic("CosmicTrtHypo_Cosmic") # here we created an instance  
        
        TE=triggerPythonConfig.addSequence(CosmicL1Threshold, [trtsegm,trthypoCosmics], 'trtxk_TRTTrkHypo')
        chain.addHLTSignature(TE)
        #triggerPythonConfig.addSequence(CosmicL1Threshold, [trtsegm], 'trtseg_TRTTrkHypo')
        #TE=triggerPythonConfig.addSequence('trtseg_TRTTrkHypo', [trtxk], 'trtxk_TRTTrkHypo')
    chain.addHLTSignature(TE)

    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_trtxk_TRTTrkHypo")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'trtxk_TRTTrkHypo_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)
    
if triggerMenu.signatureIsEnabled("cosmic_muonOnly"):
    TE = CosmicL1Threshold
    chain = HLTChain(chain_name="L2_cosmic_muonOnly", chain_counter="1903", \
                     lower_chain_name=CosmicL1Signature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('1903')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_muonOnly', chain_counter='1903',lower_chain_name='L2_cosmic_muonOnly', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1903')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigL2CosmicMuon.TrigL2CosmicMuon_Config import TrigL2CosmicMuon_Cosmic
        from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import *

        l2_TrigL2CosmicMuon_Cosmic = TrigL2CosmicMuon_Cosmic("TrigL2CosmicMuon_Cosmic_muonOnly")
        l2_CosmicMuonHypo_Cosmic = CosmicMuonHypo_Cosmic("CosmicMuonHypo_Cosmic_muonOnly")

        TE=triggerPythonConfig.addSequence(CosmicL1Threshold,[l2_TrigL2CosmicMuon_Cosmic, \
                                               l2_CosmicMuonHypo_Cosmic], 'muonOnly')

    chain.addHLTSignature(TE)
    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_muonOnly")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'muonOnly_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled("cosmic_mufast"):
    TE = CosmicL1Threshold
    chain = HLTChain(chain_name="L2_cosmic_muFast", chain_counter="1916", \
                     lower_chain_name=CosmicL1Signature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('1916')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_mufast', chain_counter='1916',lower_chain_name='L2_cosmic_muFast', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1916')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    from TrigMuonHypo.TrigMuonHypoConfig import *

    if (ConfigureL2):
        from TrigmuFast.TrigmuFastConfig import *

        sequence = [ TrigmuFastConfig('900GeV') ]
        sequence += [ MufastHypoConfig('900GeV','2GeV') ]
	
        TE=triggerPythonConfig.addSequence(CosmicL1Threshold,sequence, \
                                                'L2_cosmic_muFast')

    chain.addHLTSignature(TE)
    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_muFast")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'muFast_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled("cosmic_mufast_muiso"):
    include ("TrigL2CosmicMuonHypo/LArCellBuilding.py")
    TE = CosmicL1Threshold
    chain = HLTChain(chain_name="L2_cosmic_muFast_muiso", chain_counter="1922", \
                     lower_chain_name=CosmicL1Signature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('1922')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_mufast_muiso', chain_counter='1922',lower_chain_name='L2_cosmic_muFast_muiso', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1922')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    from TrigMuonHypo.TrigMuonHypoConfig import *

    if (ConfigureL2):
        from TrigmuFast.TrigmuFastConfig import *

        sequence = [ TrigmuFastConfig('muiso_900GeV') ]
        sequence += [ MufastHypoConfig('muiso_900GeV','2GeV') ]

        TE=triggerPythonConfig.addSequence(TE,sequence, \
                                                'L2_cosmic_muFast_pre_muiso')
        chain.addHLTSignature(TE)

        from TrigmuIso.TrigmuIsoConfig import *
        sequence = [ muIsoConfig() ]
        sequence += [ MuisoHypoConfig() ]
	
        TE=triggerPythonConfig.addSequence(TE,sequence, \
                                                'L2_cosmic_muFast_muiso')

    chain.addHLTSignature(TE)
    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_muFast_muiso")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'muFast_muiso_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled("cosmic_moore_muon"):

    TE = CosmicL1Threshold
    L2chain = HLTChain(chain_name="L2_cosmic_moore_muon", chain_counter="1913", \
                     lower_chain_name=CosmicL1Signature, level="L2", \
                     prescale="1", pass_through="0")
    L2chain.addTriggerTypeBit('1913')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    efchain = HLTChain( chain_name="EF_cosmic_moore_muon", chain_counter="1913",\
                        lower_chain_name="L2_cosmic_moore_muon",\
                        level="EF", \
                        prescale='1',\
                        pass_through='0')

    efchain.addTriggerTypeBit('1913')
    efchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigL2CosmicMuon.TrigL2CosmicMuon_Config import TrigL2CosmicMuon_Cosmic
        from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import *

        l2_TrigL2CosmicMuon_Cosmic = TrigL2CosmicMuon_Cosmic("TrigL2CosmicMuon_Cosmic_moore_muon")
        l2_CosmicMuonHypo_Cosmic = CosmicMuonHypo_Cosmic("CosmicMuonHypo_Cosmic_moore_muon")

        TE=triggerPythonConfig.addSequence(TE,[l2_TrigL2CosmicMuon_Cosmic,l2_CosmicMuonHypo_Cosmic], 'moore_muon')
    L2chain.addHLTSignature(TE)

    if (ConfigureEF):
        from TrigMoore.TrigMooreConfig import *

        ef_TrigMooreConfig_CS = TrigMooreConfig_CS('Muon')
        TE=triggerPythonConfig.addSequence('moore_muon',[ef_TrigMooreConfig_CS], 'moore_muonef')
        include("TrigMoore/jobOfragment_TrigMoore.py")
    efchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(L2chain)
    triggerPythonConfig.addHLTChain(efchain)
        

if triggerMenu.signatureIsEnabled("cosmic_muonOnly_TGC"):
    TE=CosmicL1TGCThreshold
    chain = HLTChain(chain_name="L2_cosmic_muonOnly_TGC", chain_counter="1904", \
                     lower_chain_name=CosmicL1TGCSignature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('1904')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_muonOnly_TGC', chain_counter='1904',lower_chain_name='L2_cosmic_muonOnly_TGC', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1904')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigL2CosmicMuon.TrigL2CosmicMuon_Config import TrigL2CosmicMuon_Cosmic
        from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import *

        l2_TrigL2CosmicMuon_Cosmic = TrigL2CosmicMuon_Cosmic("TrigL2CosmicMuon_Cosmic_muonOnly_TGC")
        l2_CosmicMuonHypo_Cosmic = CosmicMuonHypo_Cosmic("CosmicMuonHypo_Cosmic_muonOnly_TGC")

        TE=triggerPythonConfig.addSequence(TE,[l2_TrigL2CosmicMuon_Cosmic, \
                                               l2_CosmicMuonHypo_Cosmic], 'muon_TGC')
    chain.addHLTSignature(TE)

    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_muonOnly_TGC")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'muonOnly_TGC_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled("cosmic_muon"):
    TE=CosmicL1Threshold
    L2chain = HLTChain(chain_name="L2_cosmic_muon", chain_counter="1905", \
                       lower_chain_name=CosmicL1Signature, level="L2", \
                       prescale="1", pass_through="0")
    L2chain.addTriggerTypeBit('1905')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    EFchain = HLTChain(chain_name='EF_cosmic_muon', chain_counter='1905',lower_chain_name='L2_cosmic_muon', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1905')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    from TrigEFIDCosmic.TrigEFIDCosmic_Config import *
    from TrigEFIDCosmic.IDHitsHypo_Config import *

    if (ConfigureL2):
        from TrigL2CosmicMuon.TrigL2CosmicMuon_Config import TrigL2CosmicMuon_Cosmic
        from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import *

        l2_TrigL2CosmicMuon_Cosmic = TrigL2CosmicMuon_Cosmic("TrigL2CosmicMuon_Cosmic_muon")
        l2_CosmicMuonHypo_Cosmic = CosmicMuonHypo_Cosmic("CosmicMuonHypo_Cosmic_muon")

        L2Seq = [l2_TrigL2CosmicMuon_Cosmic,l2_CosmicMuonHypo_Cosmic]
        if (doIDHitsHypo):
            IDHitsHypo_TRTXK_L2=IDHitsHypo_muon_L2()
            L2Seq+=[IDHitsHypo_muon_L2]
        TE=triggerPythonConfig.addSequence(TE,L2Seq, 'muon')
    L2chain.addHLTSignature(TE)

    if (ConfigureEF):
        if (doEFIDCosmics):
            TrigEFIDCosmicAlgo_EF=TrigEFIDCosmicAlgo_muon_EF()
            EFSeq=[TrigEFIDCosmicAlgo_EF]
            if (doIDHitsHypo):
                IDHitsHypo_EF=IDHitsHypo_muon_EF()
                EFSeq+=[IDHitsHypo_EF]
            TE=triggerPythonConfig.addSequence(TE,EFSeq, 'TrigEFIDCosmic_muon_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(L2chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled("cosmic_tileRodMu"):
    TE=CosmicL1Threshold
    chain = HLTChain(chain_name="L2_cosmic_tileRodMu", chain_counter="1906", \
                     lower_chain_name=CosmicL1Signature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('1906')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_tileRodMu', chain_counter='1906',lower_chain_name='L2_cosmic_tileRodMu', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1906')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigTileMuId.TrigTileRODMuAlgConfig import *
        l2_TrigTileRODMuAlg = TrigTileRODMuAlg_L2()
        TE=triggerPythonConfig.addSequence(CosmicL1Threshold,[l2_TrigTileRODMuAlg], 'tileRodMu')
    chain.addHLTSignature(TE)

    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_tileRodMu")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'TileRodMu_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)


if triggerMenu.signatureIsEnabled("cosmic_tileLookForMu"):
    TE=CosmicL1Threshold
    chain = HLTChain(chain_name="L2_cosmic_tileLookForMu", \
                     chain_counter="1907", \
                     lower_chain_name=CosmicL1Signature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('1907')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_tileLookForMu', chain_counter='1907',lower_chain_name='L2_cosmic_tileLookForMu', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1907')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigTileMuId.TrigTileLookForMuAlgConfig import *
        l2_TrigTileLookForMuAlg = TrigTileLookForMuAlg_L2()
        TE=triggerPythonConfig.addSequence(CosmicL1Threshold,[l2_TrigTileLookForMuAlg], \
                                        'tileLookForMu')
    chain.addHLTSignature(TE)

    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_tileLookForMu")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'TileLookForMu_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled('cosmic_egamma'):
    include ("TrigL2CosmicMuonHypo/LArCellBuilding.py")

    TE=CosmicL1Threshold
    chain1 = HLTChain(chain_name='L2_g10', chain_counter='1908',\
                      lower_chain_name=CosmicL1Signature, level='L2', \
                      prescale=TrigEGammaL2PrescaleFactor, pass_through=TrigEGammaL2PassThrough)
    chain1.addTriggerTypeBit('1908')
    chain1.addStreamTag('cosmics', type='physics', obeyLB='yes', prescale='1')

    chain2 = HLTChain(chain_name='EF_g10', chain_counter='1908',\
                      lower_chain_name='L2_g10', level='EF', \
                      prescale='1', pass_through='0')
    chain2.addTriggerTypeBit('1908')
    chain2.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
# Level 2 part

    if (ConfigureL2):
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
        l2_T2CaloEgamma_eGamma = T2CaloEgamma_eGamma()
        l2_T2CaloEgamma_eGamma.OutputLevel=DEBUG
        from TrigEgammaHypo.TrigL2PhotonHypoConfig import L2PhotonHypo_g10
        from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_g10
        l2_L2PhotonHypo_g10 = L2PhotonHypo_g10()
        l2_L2PhotonFex_g10 = L2PhotonFex_g10()

        l2_L2PhotonHypo_g10.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
        l2_L2PhotonHypo_g10.AcceptAll=False
        l2_L2PhotonFex_g10.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
        l2_L2PhotonFex_g10.AcceptAll=False

    # EF part
    if (ConfigureEF):
        from TrigEgammaRec.TrigEgammaRecConfig import TrigEgammaRec_eGamma ,TrigEgammaRec_NoIDEF_eGamma
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        from TrigEgammaHypo.TrigEFEgammaHypoConfig import *
        from CaloRec.CaloRecConf import CaloClusterBuilderSW
        trigslw = CaloClusterBuilderSW("trigslw")
        trigslw.e_threshold=-1.*GeV
        t1=TrigCaloClusterMaker_slw()
        #t1+=trigslw
        from TrigEgammaHypo.TrigEFPhotonHypoConfig import *
        EFPhotonHypo_g10().etCut = [-7.*GeV, -7.*GeV, -7.*GeV, -7.*GeV, -7.*GeV]
        EFPhotonHypo_g10().AcceptAll=False
    
    # sequences, signatures, chains
    if (ConfigureL2):
        from TrigEgammaRec.TrigEgammaRecConfig import TrigEgammaRec_eGamma ,TrigEgammaRec_NoIDEF_eGamma
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        from TrigEgammaHypo.TrigEFEgammaHypoConfig import *
        from CaloRec.CaloRecConf import CaloClusterBuilderSW
        TE=triggerPythonConfig.addSequence(TE, [l2_T2CaloEgamma_eGamma],'L2_g10cl')
        triggerPythonConfig.addSequence(TE, [l2_L2PhotonFex_g10,l2_L2PhotonHypo_g10], 'L2_g10')
        chain1.addHLTSignature( ['L2_g10cl'] )
        chain1.addHLTSignature( ['L2_g10'] )
    triggerPythonConfig.addHLTChain(chain1)


    if (ConfigureEF):
        triggerPythonConfig.addSequence('EF_g10calo', [TrigEgammaRec_NoIDEF_eGamma()
                                                       ,EFPhotonHypo_g10()],
                                        'EF_g10')
        triggerPythonConfig.addSequence('L2_g10', [TrigCaloCellMaker_eGamma(),
                                                   TrigCaloTowerMaker_eGamma(),
                                                   TrigCaloClusterMaker_slw()],
                                        'EF_g10calo')
        chain2.addHLTSignature( ['EF_g10calo'] )
        chain2.addHLTSignature( ['EF_g10'] )
    triggerPythonConfig.addHLTChain(chain2)
    
    # Some histogramming (lines to taken out when Denis will give me the Ok)
    if (ConfigureEF):
      from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram
      from TrigMonitorBase.TrigGenericMonitoringToolConfig import TrigGenericMonitoringToolConfig
      from TrigEgammaRec.TrigEgammaRecMonitoring import TrigEgammaRecValidationMonitoring
      t = TrigEgammaRec_NoIDEF_eGamma().AthenaMonTools[1]
      t.deleteHistogramOfVariable("Et_em")
      t.Histograms += [ defineHistogram('Et_em',type='TH1F',
             title="EFEgammaRec EM transverse energy; Et_em [MeV]",
             xbins=80,xmin=-0.5e3,xmax=2.5e3) ]
      t.deleteHistogramOfVariable("Ehad1")
      t.Histograms += [ defineHistogram('Ehad1',type='TH1F',
             title="EFEgammaRec Had energy 1; Had energy 1 [MeV]",
             xbins=80,xmin=-4e2,xmax=4e2) ]
      
if triggerMenu.signatureIsEnabled('cosmic_egamma_nohad'):
    TE=CosmicL1Threshold
    include ("TrigL2CosmicMuonHypo/LArCellBuilding.py")
    chain1 = HLTChain(chain_name='L2_g10_nohad', chain_counter='1915',\
                      lower_chain_name=CosmicL1Signature, level='L2', \
                      prescale=TrigEGammaL2PrescaleFactor, pass_through=TrigEGammaL2PassThrough)
    chain1.addTriggerTypeBit('1915')
    chain1.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    chainEF = HLTChain(chain_name='EF_Cosmic_Dummy_egamma', chain_counter='1915',\
                       lower_chain_name='L2_g10_nohad', level='EF', \
                       prescale='1', pass_through='0')
    chainEF.addTriggerTypeBit('1915')
    chainEF.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    # Level 2 part

    if (ConfigureL2):
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma_NoHad
        l2_T2CaloEgamma_eGamma = T2CaloEgamma_eGamma_NoHad()
        l2_T2CaloEgamma_eGamma.OutputLevel=DEBUG
        from TrigEgammaHypo.TrigL2PhotonHypoConfig import L2PhotonHypo_g10
        from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_g10
        l2_L2PhotonHypo_g10 = L2PhotonHypo_g10("L2PhotonHypo_g10_nohad")
        l2_L2PhotonFex_g10 = L2PhotonFex_g10("L2PhotonFex_g10_nohad")
        l2_L2PhotonHypo_g10.AcceptAll=True
        l2_L2PhotonHypo_g10.OutputLevel=DEBUG
        l2_L2PhotonFex_g10.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]

    # sequences, signatures, chains
    if (ConfigureL2):
        from TrigEgammaRec.TrigEgammaRecConfig import TrigEgammaRec_eGamma ,TrigEgammaRec_NoIDEF_eGamma
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        from TrigEgammaHypo.TrigEFEgammaHypoConfig import *
        from CaloRec.CaloRecConf import CaloClusterBuilderSW
        TE=triggerPythonConfig.addSequence(TE, [l2_T2CaloEgamma_eGamma],'L2_g10cl_nohad')
        chain1.addHLTSignature(TE)
        TE=triggerPythonConfig.addSequence(TE, [l2_L2PhotonFex_g10,
                                                     l2_L2PhotonHypo_g10], 'L2_g10_nohad')
        chain1.addHLTSignature(TE)
    triggerPythonConfig.addHLTChain(chain1)

    # EF part
    if (ConfigureEF):
        from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
        EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_egamma")
        TE=triggerPythonConfig.addSequence("L2_g10_nohad", [EF_Cosmic_Dummy_Algo], "EF_Cosmic_Dummy_NoHad")
        chainEF.addHLTSignature( ['EF_Cosmic_Dummy_NoHad'] )
    triggerPythonConfig.addHLTChain(chainEF)


    # Some histogramming
    if (ConfigureL2):
      from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram
      from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import TrigT2CaloEgammaValMonitoring
      t=l2_T2CaloEgamma_eGamma.AthenaMonTools[1]
      t.deleteHistogramOfVariable("Et")
      t.Histograms += [ defineHistogram('Et',type='TH1F',
             title="Et of Clusters; Et; number of RoIs",
             xbins=60,xmin=-1e3,xmax=5e3) ]
      from TrigEgammaHypo.TrigL2PhotonHypoMonitoring import TrigL2PhotonHypoValidationMonitoring
      t = l2_L2PhotonHypo_g10.AthenaMonTools[2]
      t.deleteHistogramOfVariable("PhEt")
      t.Histograms += [ defineHistogram('PhEt',type='TH1F',
             title="L2Photon Hypo Et; Ph_Et_{em} [MeV]",
             xbins=60,xmin=-1e3,xmax=5e3) ]

if triggerMenu.signatureIsEnabled('cosmic_tau'):
    TE=CosmicL1Threshold

    L2chain = HLTChain(chain_name='L2_tau10', chain_counter='1909',\
                      lower_chain_name=CosmicL1Signature, level='L2', \
                      prescale='1', pass_through='0')
    L2chain.addTriggerTypeBit('1909')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    triggerPythonConfig.addHLTChain(L2chain)

    EFchain = HLTChain(chain_name='EF_tau10', chain_counter='1909',lower_chain_name='L2_tau10', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1909')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    triggerPythonConfig.addHLTChain(EFchain)

    include ("TrigL2CosmicMuonHypo/LArCellBuilding.py")

    # Level 2 part
    if (ConfigureL2):
        from TrigT2CaloTau.TrigT2CaloTauConfig import T2CaloTau_Tau
        l2_T2CaloTau_Tau = T2CaloTau_Tau()
        l2_T2CaloTau_Tau.OutputLevel=DEBUG
        from TrigTauHypo.TrigTauHypoConfig_tau10 import T2CaloTauHypo_tau10
        #TrigCaloTauHypo_tau10.AcceptAll=True

        TE=triggerPythonConfig.addSequence(TE, [l2_T2CaloTau_Tau, T2CaloTauHypo_tau10()],'L2_tau10')
    L2chain.addHLTSignature(TE)

    # EF part
    # sequences, signatures, chains
    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_tau10")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'EF_tau10')
    EFchain.addHLTSignature(TE)
    
if triggerMenu.signatureIsEnabled('cosmic_dummy'):
    TE=CosmicL1Threshold
    chain = HLTChain(chain_name='L2_Cosmic_Dummy', chain_counter='1910',\
                     lower_chain_name=CosmicL1Signature, level='L2', \
                     prescale='1', pass_through='0')
    chain.addTriggerTypeBit('1910')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    chainEF = HLTChain(chain_name='EF_Cosmic_Dummy', chain_counter='1910',\
                       lower_chain_name='L2_Cosmic_Dummy', level='EF', \
                       prescale='1', pass_through='0')
    chainEF.addTriggerTypeBit('1910')
    chainEF.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    # sequences, signatures, chains

    # Level 2 part
    if (ConfigureL2):
        from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
        L2_Cosmic_Dummy_Algo = PESA__dummyAlgo("L2_Cosmic_DummyAlgo")

        TE=triggerPythonConfig.addSequence(TE, [L2_Cosmic_Dummy_Algo], "L2_Cosmic_Dummy_Signature")
        chain.addHLTSignature( ['L2_Cosmic_Dummy_Signature'] )
    triggerPythonConfig.addHLTChain(chain)
    
    # EF part
    if (ConfigureEF):
        from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
        EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo")
        TE=triggerPythonConfig.addSequence(TE, [EF_Cosmic_Dummy_Algo], "EF_Cosmic_Dummy")
        chainEF.addHLTSignature( TE )
    triggerPythonConfig.addHLTChain(chainEF)
        
if triggerMenu.signatureIsEnabled('cosmic_standalone_dummy'):
    
    # sequences, signatures, chains

    # Level 2 part
    TE=CosmicL1StandaloneThreshold
    chain = HLTChain(chain_name='L2_Cosmic_Standalone_Dummy', chain_counter='1917',\
                     lower_chain_name=CosmicL1StandaloneSignature, level='L2', \
                     prescale=CosmicStandaloneL2Prescale, pass_through='0')
    chain.addTriggerTypeBit('1917')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    chainEF = HLTChain(chain_name='EF_Cosmic_Standalone_Dummy', chain_counter='1917',\
                       lower_chain_name='L2_Cosmic_Standalone_Dummy', level='EF', \
                       prescale=CosmicStandaloneEFPrescale, pass_through='0')
    chainEF.addTriggerTypeBit('1917')
    chainEF.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
        L2_Cosmic_Dummy_Algo = PESA__dummyAlgo("L2_Cosmic_Standalone_DummyAlgo")

        TE=triggerPythonConfig.addSequence(TE, [L2_Cosmic_Dummy_Algo], "L2_Cosmic_Standalone_Dummy")
        chain.addHLTSignature(TE)
        triggerPythonConfig.addHLTChain(chain)
    
    # EF part
    if (ConfigureEF):
        from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
        EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_Standalone_DummyAlgo")
        TE=triggerPythonConfig.addSequence(TE, [EF_Cosmic_Dummy_Algo], "EF_Cosmic_Standalone_Dummy")
        chainEF.addHLTSignature( TE )
        triggerPythonConfig.addHLTChain(chainEF)
        
if triggerMenu.signatureIsEnabled('cosmic_standalone_empty'):
    # sequences, signatures, chains
    # Level 2 part
    TE=CosmicL1StandaloneThreshold
    if (ConfigureL2):
        chain = HLTChain(chain_name='L2_Cosmic_Standalone_Empty', chain_counter='1918',\
                         lower_chain_name=CosmicL1StandaloneSignature, level='L2', \
                         prescale=CosmicStandaloneL2Prescale, pass_through='0')
        chain.addTriggerTypeBit('1918')
        chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
        triggerPythonConfig.addHLTChain(chain)
    
    # EF part
    if (ConfigureEF):
        EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_Standalone_DummyAlgo_mt")
        chainEF = HLTChain(chain_name='EF_Cosmic_Standalone_Empty', chain_counter='1918',\
                          lower_chain_name='L2_Cosmic_Standalone_Empty', level='EF', \
                          prescale=CosmicStandaloneEFPrescale, pass_through='0')
        chainEF.addTriggerTypeBit('1918')
        chainEF.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
        triggerPythonConfig.addHLTChain(chainEF)
        
if triggerMenu.signatureIsEnabled('cosmic_METTest'):
    TE=CosmicL1Threshold
    l2chain = HLTChain( chain_name='L2_cosmic_xe10',chain_counter='1911',\
                        lower_chain_name=CosmicL1Signature,\
                        level="L2", prescale="1", pass_through="0")
    l2chain.addTriggerTypeBit('1911')
    l2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    efchain = HLTChain( chain_name='EF_cosmic_xe10', chain_counter='1911',\
                        lower_chain_name='L2_cosmic_xe10',\
                        level="EF", prescale="1", pass_through="0")
    efchain.addTriggerTypeBit(1911)
    efchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1') 

    # Level 2 part
    include ("TrigL2CosmicMuonHypo/LArCellBuilding.py")
    if (ConfigureL2):
        from TrigL2MissingET.TrigL2MissingETConfig import T2MissingET
        from TrigMissingETHypo.TrigMissingETHypoConfig import *

        l2Fex = T2MissingET('L2METfex_cosmic')

        from AthenaCommon.SystemOfUnits import GeV
        l2Hypo = EFMissingETHypoBase('L2METhypo_cosmic')
        l2Hypo.MissingETCut=1*GeV
        l2Hypo.SumETCut=1*GeV
        l2Hypo.CutType=-2.0

        TE=triggerPythonConfig.addSequence(TE, [l2Fex, l2Hypo], "met_L2")
        l2chain.addHLTSignature(TE)
    triggerPythonConfig.addHLTChain(l2chain)

    # EF part
    if (ConfigureEF):
        from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex
        from TrigMissingETHypo.TrigMissingETHypoConfig import *
        efcaloMET = EFMissingET_Fex('EFMETfex_cosmic') 

        from AthenaCommon.SystemOfUnits import GeV
        efcaloMEThypo = EFMissingETHypoBase('EFMEThypo_cosmic')
        efcaloMEThypo.MissingETCut=1*GeV
        efcaloMEThypo.SumETCut=1*GeV
        efcaloMEThypo.CutType=-2.0
        
        TE=triggerPythonConfig.addSequence(TE,[ efcaloMET, efcaloMEThypo ], "met_EF")
        efchain.addHLTSignature(TE) 
        triggerPythonConfig.addHLTChain(efchain)

if triggerMenu.signatureIsEnabled('cosmic_METEFOnlyTest'):
    TE=CosmicL1Threshold
    chain = HLTChain(chain_name='L2_Dummy_cosmic_xe', chain_counter='1912',\
                     lower_chain_name=CosmicL1Signature, level='L2', \
                     prescale='1', pass_through='0')
    chain.addTriggerTypeBit('1912')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    efchain = HLTChain( chain_name='EF_DummyL2_cosmic_xe', chain_counter='1912',\
                        lower_chain_name='L2_Dummy_cosmic_xe',\
                        level="EF", prescale="1", pass_through="0")
    efchain.addTriggerTypeBit(1912)
    efchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1') 

    # Level 2 part
    include ("TrigL2CosmicMuonHypo/LArCellBuilding.py")
    if (ConfigureL2):
        from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
        L2_MET_Dummy_Algo = PESA__dummyAlgo("L2_MET_Dummy_Algo")
        triggerPythonConfig.addSequence(TE, [L2_MET_Dummy_Algo],'Dummy_met_L2')
        chain.addHLTSignature( ['Dummy_met_L2'] )
    triggerPythonConfig.addHLTChain(chain)

    # EF part
    if (ConfigureEF):
        from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex
        from TrigMissingETHypo.TrigMissingETHypoConfig import *
        efcaloMET = EFMissingET_Fex('EFMETfex_DummyL2_cosmic') 

        from AthenaCommon.SystemOfUnits import GeV
        efcaloMEThypo = EFMissingETHypoBase('EFMEThypo_DummyL2_cosmic')
        efcaloMEThypo.MissingETCut=1*GeV
        efcaloMEThypo.SumETCut=1*GeV
        efcaloMEThypo.CutType=-2.0

        TE=triggerPythonConfig.addSequence( "Dummy_met_L2", \
                                            [ efcaloMET, efcaloMEThypo ], "met_DummyL2_EF")
        efchain.addHLTSignature(TE) 
    triggerPythonConfig.addHLTChain(efchain)

if triggerMenu.signatureIsEnabled("cosmic_moore_mufast"):
    TE = CosmicL1Threshold
    L2chain = HLTChain(chain_name="L2_cosmic_moore_mufast", chain_counter="1919", \
                     lower_chain_name=CosmicL1Signature, level="L2", \
                     prescale="1", pass_through="0")
    L2chain.addTriggerTypeBit('1919')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_moore_mufast', chain_counter='1919',lower_chain_name='L2_cosmic_moore_mufast', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1919')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    from TrigMuonHypo.TrigMuonHypoConfig import *

    if (ConfigureL2):
        from TrigmuFast.TrigmuFastConfig import *
        sequence = [ TrigmuFastConfig('moore_900GeV') ]
        sequence += [ MufastHypoConfig('moore_900GeV','2GeV') ]
        TE=triggerPythonConfig.addSequence(CosmicL1Threshold,sequence,'L2_cosmic_moore_mufast')
    L2chain.addHLTSignature(TE)

    if (ConfigureEF):
        from TrigMoore.TrigMooreConfig import *

        ef_TrigMooreConfig_MS = TrigMooreConfig_MS('Muon_moore_mufast')
        TE=triggerPythonConfig.addSequence('L2_cosmic_moore_mufast',[ef_TrigMooreConfig_MS], 'moore_mufast_muonef')
        include("TrigMoore/jobOfragment_TrigMoore.py")
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(L2chain)
    triggerPythonConfig.addHLTChain(EFchain)
        

if triggerMenu.signatureIsEnabled('cosmic_calo'):
    include ("TrigL2CosmicMuonHypo/LArCellBuilding.py")

    TE = CosmicL1Threshold
    L2chain = HLTChain(chain_name="L2_cosmic_calo", chain_counter="1920", \
                     lower_chain_name=CosmicL1Signature, level="L2", \
                     prescale="1", pass_through="0")
    L2chain.addTriggerTypeBit('1920')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_calo', chain_counter='1920',lower_chain_name='L2_cosmic_calo', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1920')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    # Level 2 part
    if (ConfigureL2):
        from TrigT2CaloCosmic.TrigT2CaloCosmicConfig import T2CaloCosmic_Cosmic
        l2_T2CaloCosmic_Cosmic = T2CaloCosmic_Cosmic()
        l2_T2CaloCosmic_Cosmic.OutputLevel=DEBUG

        TE=triggerPythonConfig.addSequence(CosmicL1Threshold, [l2_T2CaloCosmic_Cosmic],'L2_cosmic_calo')
    L2chain.addHLTSignature(TE)

    # EF part
    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo_Calo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_Calo")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo_Calo], 'cosmic_calo_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(L2chain)
    triggerPythonConfig.addHLTChain(EFchain)


if triggerMenu.signatureIsEnabled('cosmic_jet'):
    include ("TrigL2CosmicMuonHypo/LArCellBuilding.py")

    TE = CosmicL1Threshold
    L2chain = HLTChain(chain_name="L2_cosmic_jet", chain_counter="1921", \
                     lower_chain_name=CosmicL1Signature, level="L2", \
                     prescale="1", pass_through="0")
    L2chain.addTriggerTypeBit('1921')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_jet', chain_counter='1921',lower_chain_name='L2_cosmic_jet', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1921')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    # Level 2 part
    if (ConfigureL2):
         from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet
         l2_T2CaloJet_Jet = T2CaloJet_Jet()
         l2_T2CaloJet_Jet.OutputLevel=DEBUG
         from TrigJetHypo.TrigJetHypoConfig import L2JetHypo
         l2_TrigL2JetHypo_Jet = L2JetHypo()
         l2_TrigL2JetHypo_Jet.Etcut_L2 = 5*GeV
         #TrigCaloTauHypo_tau10.AcceptAll=True

         TE=triggerPythonConfig.addSequence(TE, [l2_T2CaloJet_Jet, l2_TrigL2JetHypo_Jet],'L2_J10')
    L2chain.addHLTSignature(TE)

    # EF part
    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo_Jet = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_Jet")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo_Jet], 'cosmic_jet_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(L2chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled("cosmic_mufast_TGC"):
    TE = CosmicL1TGCThreshold
    chain = HLTChain(chain_name="L2_cosmic_muFast_TGC", chain_counter="2016", \
                     lower_chain_name=CosmicL1TGCSignature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('2016')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_mufast_TGC', chain_counter='2016',lower_chain_name='L2_cosmic_muFast_TGC', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('2016')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    from TrigMuonHypo.TrigMuonHypoConfig import *

    if (ConfigureL2):
        from TrigmuFast.TrigmuFastConfig import *

        sequence = [ TrigmuFastConfig('900GeV') ]
        sequence += [ MufastHypoConfig('900GeV','2GeV') ]

        #sequence = [ TrigmuFastConfig('tgc_900GeV') ]
        #sequence += [ MufastHypoConfig('tgc_900GeV','2GeV') ]

        TE=triggerPythonConfig.addSequence(TE,sequence, \
                                                'L2_cosmic_muFast_TGC')

    chain.addHLTSignature(TE)
    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_muFast_TGC")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'muFast_TGC_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled("cosmic_mufast_muiso_TGC"):
    include ("TrigL2CosmicMuonHypo/LArCellBuilding.py")
    TE = CosmicL1TGCThreshold
    chain = HLTChain(chain_name="L2_cosmic_muFast_muiso_TGC", chain_counter="2022", \
                     lower_chain_name=CosmicL1TGCSignature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('2022')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_mufast_muiso_TGC', chain_counter='2022',lower_chain_name='L2_cosmic_muFast_muiso_TGC', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('2022')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    from TrigMuonHypo.TrigMuonHypoConfig import *

    if (ConfigureL2):
        from TrigmuFast.TrigmuFastConfig import *

        sequence = [ TrigmuFastConfig('muiso_TGC_900GeV') ]
        sequence += [ MufastHypoConfig('muiso_TGC_900GeV','2GeV') ]

        TE=triggerPythonConfig.addSequence(TE,sequence, \
                                                'L2_cosmic_muFast_pre_muiso_TGC')
        chain.addHLTSignature(TE)

        from TrigmuIso.TrigmuIsoConfig import *
        sequence = [ muIsoConfig("muIsoConfig_TGC") ]
        sequence += [ MuisoHypoConfig("muIsoConfig_TGC") ]
	
        TE=triggerPythonConfig.addSequence(TE,sequence, \
                                                'L2_cosmic_muFast_muiso_TGC')

    chain.addHLTSignature(TE)
    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_muFast_muiso_TGC")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'muFast_muiso_TGC_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled("cosmic_moore_muon_TGC"):
    TE=CosmicL1TGCThreshold

    L2chain = HLTChain(chain_name="L2_cosmic_moore_muon_TGC", chain_counter="2013", \
                     lower_chain_name=CosmicL1TGCSignature, level="L2", \
                     prescale="1", pass_through="0")
    L2chain.addTriggerTypeBit('2013')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    efchain = HLTChain( chain_name="EF_cosmic_moore_muon_TGC", chain_counter="2013",\
                            lower_chain_name="L2_cosmic_moore_muon_TGC",\
                            level="EF", \
                            prescale='1',\
                            pass_through='0')
    efchain.addTriggerTypeBit('2013')
    efchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    
    if (ConfigureL2):
        from TrigL2CosmicMuon.TrigL2CosmicMuon_Config import TrigL2CosmicMuon_Cosmic
        from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import *

        l2_TrigL2CosmicMuon_Cosmic = TrigL2CosmicMuon_Cosmic("TrigL2CosmicMuon_Cosmic_moore_muon_TGC")
        l2_CosmicMuonHypo_Cosmic = CosmicMuonHypo_Cosmic("CosmicMuonHypo_Cosmic_moore_muon_TGC")

        TE=triggerPythonConfig.addSequence(TE,[l2_TrigL2CosmicMuon_Cosmic, \
                                               l2_CosmicMuonHypo_Cosmic], 'moore_muon_TGC')
    L2chain.addHLTSignature(TE)

    if (ConfigureEF):
        from TrigMoore.TrigMooreConfig import *

        ef_TrigMooreConfig_CS = TrigMooreConfig_CS('Muon_TGC')
        TE=triggerPythonConfig.addSequence(TE,[ef_TrigMooreConfig_CS], 'moore_muonef_TGC')
        include("TrigMoore/jobOfragment_TrigMoore.py")
    efchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(L2chain)
    triggerPythonConfig.addHLTChain(efchain)
        

if triggerMenu.signatureIsEnabled("cosmic_moore_mufast_TGC"):
    TE = CosmicL1TGCThreshold
    L2chain = HLTChain(chain_name="L2_cosmic_moore_mufast_TGC", chain_counter="2019", \
                     lower_chain_name=CosmicL1TGCSignature, level="L2", \
                     prescale="1", pass_through="0")
    L2chain.addTriggerTypeBit('2019')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_moore_mufast_TGC', chain_counter='2019',lower_chain_name='L2_cosmic_moore_mufast_TGC', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('2019')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    from TrigMuonHypo.TrigMuonHypoConfig import *

    if (ConfigureL2):
        from TrigmuFast.TrigmuFastConfig import *
        sequence = [ TrigmuFastConfig('moore_TGC_900GeV') ]
        sequence += [ MufastHypoConfig('moore_TGC_900GeV','2GeV') ]
        TE=triggerPythonConfig.addSequence(TE,sequence,'L2_cosmic_moore_mufast_TGC')
    L2chain.addHLTSignature(TE)

    if (ConfigureEF):
        from TrigMoore.TrigMooreConfig import *

        ef_TrigMooreConfig_MS = TrigMooreConfig_MS('Muon_moore_mufast_TGC')
        TE=triggerPythonConfig.addSequence(TE,[ef_TrigMooreConfig_MS], 'moore_mufast_muonef_TGC')
        include("TrigMoore/jobOfragment_TrigMoore.py")
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(L2chain)
    triggerPythonConfig.addHLTChain(EFchain)

        
if triggerMenu.signatureIsEnabled("cosmic_standalone_tileRodMu"):
    TE=CosmicL1StandaloneThreshold
    chain = HLTChain(chain_name="L2_cosmic_standalone_tileRodMu", chain_counter="1923", \
                     lower_chain_name=CosmicL1StandaloneSignature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('1923')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_standalone_tileRodMu', chain_counter='1923',lower_chain_name='L2_cosmic_standalone_tileRodMu', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1923')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigTileMuId.TrigTileRODMuAlgConfig import *
        l2_TrigTileRODMuAlg = TrigTileRODMuAlg_L2("standalone_TileRODMuAlg")
        TE=triggerPythonConfig.addSequence(TE,[l2_TrigTileRODMuAlg], 'standalonetileRodMu')
    chain.addHLTSignature(TE)

    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_standalone_tileRodMu")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'Standalone_TileRodMu_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)


if triggerMenu.signatureIsEnabled('cosmic_METmuFastTest'):
    # Level 2 part

    TE=CosmicL1CaloThreshold
    chain = HLTChain(chain_name='L2_mufast_cosmic_xe', chain_counter='1924',\
                     lower_chain_name=CosmicL1Signature, level='L2', \
                     prescale='1', pass_through='0')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    chain.addTriggerTypeBit('1924')
    triggerPythonConfig.addHLTChain(chain)


    efchain = HLTChain( chain_name='EF_mufast_cosmic_xe', chain_counter='1924',\
                        lower_chain_name='L2_mufast_cosmic_xe',\
                        level="EF", prescale="1", pass_through="0")
    efchain.addTriggerTypeBit(1924)
    efchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1') 
    triggerPythonConfig.addHLTChain(efchain)
    

    if (ConfigureL2):
        include ("TrigL2CosmicMuonHypo/LArCellBuilding.py")
        from TrigmuFast.TrigmuFastConfig import *
        sequence = [ TrigmuFastConfig('met_900GeV') ]
        #from TrigMuonHypo.TrigMuonHypoConfig import *
        #sequence += [ MufastHypoConfig('met_900GeV','2GeV') ]
        TE=triggerPythonConfig.addSequence(TE,sequence,'L2_cosmic_mufast_cosmic_xe')
    chain.addHLTSignature(TE)

    # EF part
    if (ConfigureEF):
        from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex
        from TrigMissingETHypo.TrigMissingETHypoConfig import *
        efcaloMET = EFMissingET_Fex('EFMETfex_mufastL2_cosmic') 

        from AthenaCommon.SystemOfUnits import GeV
        efcaloMEThypo = EFMissingETHypoBase('EFMEThypo_mufastL2_cosmic')
        efcaloMEThypo.MissingETCut=1*GeV
        efcaloMEThypo.SumETCut=1*GeV
        efcaloMEThypo.CutType=-2.0

        TE=triggerPythonConfig.addSequence(TE,[ efcaloMET, efcaloMEThypo ], "met_mufast_EF")
    efchain.addHLTSignature(TE) 

if triggerMenu.signatureIsEnabled("cosmic_standalone_tileLookForMu"):
    TE=CosmicL1StandaloneThreshold
    chain = HLTChain(chain_name="L2_cosmic_standalone_tileLookForMu", \
                     chain_counter="1925", \
                     lower_chain_name=CosmicL1StandaloneSignature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('1925')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_standalone_tileLookForMu', chain_counter='1925',lower_chain_name='L2_cosmic_standalone_tileLookForMu', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1925')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigTileMuId.TrigTileLookForMuAlgConfig import *
        l2_TrigTileLookForMuAlg = TrigTileLookForMuAlg_L2("standalone_TrigTileLookForMuAlg")
        TE=triggerPythonConfig.addSequence(TE,[l2_TrigTileLookForMuAlg], \
                                        'standalone_tileLookForMu')
    chain.addHLTSignature(TE)

    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_standalone_tileLookForMu")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'Standalone_TileLookForMu_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled('cosmic_dummy_TGC'):
    
    # sequences, signatures, chains

    # Level 2 part
    TE=CosmicL1TGCThreshold
    chain = HLTChain(chain_name='L2_Cosmic_Dummy_TGC', chain_counter='1926',\
                     lower_chain_name=CosmicL1TGCSignature, level='L2', \
                     prescale='1', pass_through='0')
    chain.addTriggerTypeBit('1926')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    chainEF = HLTChain(chain_name='EF_Cosmic_Dummy_TGC', chain_counter='1926',\
                       lower_chain_name='L2_Cosmic_Dummy_TGC', level='EF', \
                       prescale='1', pass_through='0')
    chainEF.addTriggerTypeBit('1926')
    chainEF.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
        L2_Cosmic_Dummy_Algo = PESA__dummyAlgo("L2_Cosmic_DummyAlgo_TGC")

        TE=triggerPythonConfig.addSequence(TE, [L2_Cosmic_Dummy_Algo], "L2_Cosmic_Dummy_TGC_Signature")
    chain.addHLTSignature( TE )
    triggerPythonConfig.addHLTChain(chain)
    
    # EF part
    if (ConfigureEF):
        from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
        EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_TGC")
        TE=triggerPythonConfig.addSequence(TE, [EF_Cosmic_Dummy_Algo], "EF_Cosmic_Dummy_TGC")
    chainEF.addHLTSignature(TE)
    triggerPythonConfig.addHLTChain(chainEF)

if triggerMenu.signatureIsEnabled('cosmic_empty_TGC'):
    # sequences, signatures, chains
    # Level 2 part
    TE=CosmicL1TGCThreshold
    chain = HLTChain(chain_name='L2_Cosmic_TGC_Empty', chain_counter='1927',\
                     lower_chain_name=CosmicL1TGCSignature, level='L2', \
                     prescale=CosmicStandaloneL2Prescale, pass_through='0')
    chain.addTriggerTypeBit('1927')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    chainEF = HLTChain(chain_name='EF_Cosmic_TGC_Empty', chain_counter='1927',\
                       lower_chain_name='L2_Cosmic_TGC_Empty', level='EF', \
                       prescale=CosmicStandaloneEFPrescale, pass_through='0')
    chainEF.addTriggerTypeBit('1927')
    chainEF.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    # EF part
    if (ConfigureEF):
        EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_TGC_DummyAlgo_mt")

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(chainEF)
        

if triggerMenu.signatureIsEnabled('cosmic_calo_dummy'):
    
    # sequences, signatures, chains

    # Level 2 part
    TE=CosmicL1CaloThreshold
    chain = HLTChain(chain_name='L2_Cosmic_Calo_Dummy', chain_counter='1928',\
                     lower_chain_name=CosmicL1CaloSignature, level='L2', \
                     prescale=CosmicCaloL2Prescale, pass_through='0')
    chain.addTriggerTypeBit('1928')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    chainEF = HLTChain(chain_name='EF_Cosmic_Calo_Dummy', chain_counter='1928',\
                       lower_chain_name='L2_Cosmic_Calo_Dummy', level='EF', \
                       prescale=CosmicCaloEFPrescale, pass_through='0')
    chainEF.addTriggerTypeBit('1928')
    chainEF.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
        L2_Cosmic_Dummy_Algo = PESA__dummyAlgo("L2_Cosmic_Calo_DummyAlgo")

        TE=triggerPythonConfig.addSequence(TE, [L2_Cosmic_Dummy_Algo], "L2_Cosmic_Calo_Dummy")
    chain.addHLTSignature(TE)
    triggerPythonConfig.addHLTChain(chain)
    
    # EF part
    if (ConfigureEF):
        from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
        EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_Calo_DummyAlgo")
        TE=triggerPythonConfig.addSequence(TE, [EF_Cosmic_Dummy_Algo], "EF_Cosmic_Calo_Dummy")
    chainEF.addHLTSignature( TE )
    triggerPythonConfig.addHLTChain(chainEF)
        
if triggerMenu.signatureIsEnabled('cosmic_calo_empty'):
    # sequences, signatures, chains
    # Level 2 part
    TE=CosmicL1CaloThreshold

    chain = HLTChain(chain_name='L2_Cosmic_Calo_Empty', chain_counter='1929',\
                     lower_chain_name=CosmicL1CaloSignature, level='L2', \
                     prescale=CosmicCaloL2Prescale, pass_through='0')
    chain.addTriggerTypeBit('1929')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    chainEF = HLTChain(chain_name='EF_Cosmic_Calo_Empty', chain_counter='1929',\
                       lower_chain_name='L2_Cosmic_Calo_Empty', level='EF', \
                       prescale=CosmicCaloEFPrescale, pass_through='0')
    chainEF.addTriggerTypeBit('1929')
    chainEF.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    
    # EF part
    if (ConfigureEF):
        EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_Calo_DummyAlgo_mt")

    triggerPythonConfig.addHLTChain(chainEF)
    triggerPythonConfig.addHLTChain(chain)
        


if triggerMenu.signatureIsEnabled("cosmic_calo_tileRodMu"):
    TE=CosmicL1CaloThreshold
    chain = HLTChain(chain_name="L2_cosmic_calo_tileRodMu", chain_counter="1930", \
                     lower_chain_name=CosmicL1CaloSignature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('1930')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_calo_tileRodMu', chain_counter='1930',lower_chain_name='L2_cosmic_calo_tileRodMu', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1930')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigTileMuId.TrigTileRODMuAlgConfig import *
        l2_TrigTileRODMuAlg = TrigTileRODMuAlg_L2("calo_TileRODMuAlg")
        TE=triggerPythonConfig.addSequence(TE,[l2_TrigTileRODMuAlg], 'calotileRodMu')
    chain.addHLTSignature(TE)

    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_calo_tileRodMu")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'Calo_TileRodMu_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)


if triggerMenu.signatureIsEnabled("cosmic_calo_tileLookForMu"):
    TE=CosmicL1CaloThreshold
    chain = HLTChain(chain_name="L2_cosmic_calo_tileLookForMu", \
                     chain_counter="1931", \
                     lower_chain_name=CosmicL1CaloSignature, level="L2", \
                     prescale="1", pass_through="0")
    chain.addTriggerTypeBit('1931')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_cosmic_calo_tileLookForMu', chain_counter='1931',lower_chain_name='L2_cosmic_calo_tileLookForMu', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1931')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigTileMuId.TrigTileLookForMuAlgConfig import *
        l2_TrigTileLookForMuAlg = TrigTileLookForMuAlg_L2("calo_TrigTileLookForMuAlg")
        TE=triggerPythonConfig.addSequence(TE,[l2_TrigTileLookForMuAlg], \
                                        'calo_tileLookForMu')
    chain.addHLTSignature(TE)

    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_calo_tileLookForMu")
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo], 'Calo_TileLookForMu_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled('empty'):
    # sequences, signatures, chains
    # Level 2 part
    TE=""
    chain = HLTChain(chain_name='L2_Empty', chain_counter='1932',\
                     lower_chain_name="", level='L2', \
                     prescale=CosmicEmptyL2Prescale, pass_through='0')
    chain.addTriggerTypeBit('1932')
    chain.addStreamTag(StreamTagName("All"), type='physics', obeyLB='yes', prescale='1')
    chainEF = HLTChain(chain_name='EF_Empty', chain_counter='1932',\
                       lower_chain_name='L2_Empty', level='EF', \
                       prescale=CosmicEmptyEFPrescale, pass_through='0')
    chainEF.addTriggerTypeBit('1932')
    chainEF.addStreamTag(StreamTagName("All"), type='physics', obeyLB='yes', prescale='1')

    # EF part
    if (ConfigureEF):
        EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_empty_DummyAlgo")

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(chainEF)



if triggerMenu.signatureIsEnabled('cosmic_trtxk_newAlgo'):
    TE=CosmicL1Threshold
    L2chain = HLTChain(chain_name='L2_cosmic_trtxk_newAlgo', chain_counter='1933', lower_chain_name=CosmicL1Signature,\
                       level='L2',prescale='1', pass_through='0')
    L2chain.addTriggerTypeBit('1933')
    L2chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    EFchain = HLTChain(chain_name='EF_cosmic_trtxk_newAlgo', chain_counter='1933',lower_chain_name='L2_cosmic_trtxk_newAlgo', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1933')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    from TrigEFIDCosmic.TrigEFIDCosmic_Config import *
    from TrigEFIDCosmic.IDHitsHypo_Config import *
    if (ConfigureL2):
        from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Cosmics_NewAlgo
        trtsegm  = TrigTRTSegFinder_Cosmics_NewAlgo("TrigTRTSegFinder_Cosmics_newAlgo")
        trtsegm.RoIhalfWidthDeltaPhi = 3.14
        trtsegm.RoIhalfWidthDeltaEta = 3.

        L2Seq = [trtsegm]
        if (doIDHitsHypo):
            IDHitsHypo_TRTXK_L2=IDHitsHypo_TRTXK_L2("IDHitsHypo_TRTXK_newAlgo_L2")
            L2Seq+=[IDHitsHypo_TRTXK_L2]
        TE=triggerPythonConfig.addSequence(TE,L2Seq, 'trtseg_newAlgo')
    L2chain.addHLTSignature(TE)

    if (ConfigureEF):
        if (doEFIDCosmics):
            TrigEFIDCosmicAlgo_TRTXK_EF=TrigEFIDCosmicAlgo_TRTXK_EF("TrigEFIDCosmicAlgo_TRTXK_newAlgo_EF")
            EFSeq=[TrigEFIDCosmicAlgo_TRTXK_EF]
            if (doIDHitsHypo):
                IDHitsHypo_TRTXK_EF=IDHitsHypo_TRTXK_EF("IDHitsHypo_TRTXK_newAlgo_EF")
                EFSeq+=[IDHitsHypo_TRTXK_EF]
            TE=triggerPythonConfig.addSequence(TE,EFSeq, 'TrigEFIDCosmic_TRTXK_newAlgo_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(L2chain)
    triggerPythonConfig.addHLTChain(EFchain)
    
if triggerMenu.signatureIsEnabled('cosmic_trtxk_TRTTrkHypo_newAlgo'):
    TE=CosmicL1Threshold
    chain = HLTChain('L2_cosmic_trtxk_TRTTrkHypo_newAlgo', '1934', CosmicL1Signature, level='L2',
                     prescale='1', pass_through='0')
    chain.addTriggerTypeBit('1934')
    chain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_trtxk_TRTTrkHypo_newAlgo', chain_counter='1934',\
                       lower_chain_name='L2_cosmic_trtxk_TRTTrkHypo_newAlgo', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1934')
    EFchain.addStreamTag(StreamTagName(TE), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Cosmics_NewAlgo
        trtsegm  = TrigTRTSegFinder_Cosmics_NewAlgo("trtsegm_TRTTrkHypo_newAlgo")
        trtsegm.RoIhalfWidthDeltaPhi = 3.14
        trtsegm.RoIhalfWidthDeltaEta = 3.
    
        from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic
        trthypoCosmics = CosmicTrtHypo_Cosmic("CosmicTrtHypo_Cosmic_newAlgo") # here we created an instance  
        
        TE=triggerPythonConfig.addSequence(CosmicL1Threshold, [trtsegm,trthypoCosmics], 'trtxk_TRTTrkHypo_newAlgo')
        chain.addHLTSignature(TE)
        #triggerPythonConfig.addSequence(CosmicL1Threshold, [trtsegm], 'trtseg_TRTTrkHypo')
        #TE=triggerPythonConfig.addSequence('trtseg_TRTTrkHypo', [trtxk], 'trtxk_TRTTrkHypo')
    chain.addHLTSignature(TE)

    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_trtxk_TRTTrkHypo_newAlgo")
            from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic
            trthypoCosmics_EF = CosmicTrtHypo_Cosmic("CosmicTrtHypo_Cosmic_newAlgo_EF") # here we created an instance  
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo,trthypoCosmics_EF], 'trtxk_TRTTrkHypo_newAlgo_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)

if triggerMenu.signatureIsEnabled('cosmic_trtxk_TRTTrkHypo_newAlgo_Pixel'):
    TE=CosmicL1Threshold
    chain = HLTChain('L2_cosmic_trtxk_TRTTrkHypo_newAlgo_Pixel', '1935', CosmicL1Signature, level='L2',
                     prescale='1', pass_through='0')
    chain.addTriggerTypeBit('1935')
    chain.addStreamTag(StreamTagName(TE+"_Pixel"), type='physics', obeyLB='yes', prescale='1')
    EFchain = HLTChain(chain_name='EF_trtxk_TRTTrkHypo_newAlgo_Pixel', chain_counter='1935',\
                       lower_chain_name='L2_cosmic_trtxk_TRTTrkHypo_newAlgo_Pixel', level='EF',prescale='1', pass_through='0')
    EFchain.addTriggerTypeBit('1935')
    EFchain.addStreamTag(StreamTagName(TE+"_Pixel"), type='physics', obeyLB='yes', prescale='1')

    if (ConfigureL2):
        from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Cosmics_NewAlgo
        trtsegm  = TrigTRTSegFinder_Cosmics_NewAlgo("trtsegm_TRTTrkHypo_newAlgo_Pixel")
        trtsegm.RoIhalfWidthDeltaPhi = 3.14
        trtsegm.RoIhalfWidthDeltaEta = 3.
    
        from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic
        trthypoCosmics = CosmicTrtHypo_Cosmic("CosmicTrtHypo_Cosmic_newAlgo_Pixel") # here we created an instance  
        
        TE=triggerPythonConfig.addSequence(CosmicL1Threshold, [trtsegm,trthypoCosmics], 'trtxk_TRTTrkHypo_newAlgo_Pixel')
        chain.addHLTSignature(TE)
        #triggerPythonConfig.addSequence(CosmicL1Threshold, [trtsegm], 'trtseg_TRTTrkHypo')
        #TE=triggerPythonConfig.addSequence('trtseg_TRTTrkHypo', [trtxk], 'trtxk_TRTTrkHypo')
    chain.addHLTSignature(TE)

    if (ConfigureEF):
            EF_Cosmic_Dummy_Algo = PESA__dummyAlgo("EF_Cosmic_DummyAlgo_trtxk_TRTTrkHypo_newAlgo_Pixel")
            from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic
            trthypoCosmics_EF = CosmicTrtHypo_Cosmic("CosmicTrtHypo_Cosmic_newAlgo_Pixel_EF") # here we created an instance  
            TE=triggerPythonConfig.addSequence(TE,[EF_Cosmic_Dummy_Algo,trthypoCosmics_EF], 'trtxk_TRTTrkHypo_newAlgo_Pixel_EF')
    EFchain.addHLTSignature(TE)

    triggerPythonConfig.addHLTChain(chain)
    triggerPythonConfig.addHLTChain(EFchain)
