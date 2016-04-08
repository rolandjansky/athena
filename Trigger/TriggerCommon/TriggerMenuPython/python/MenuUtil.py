# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Lvl1 import Lvl1
from TriggerMenuPython.Lvl1Flags import Lvl1Flags
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging import logging

log = logging.getLogger('TriggerMenuPython.MenuUtil.py')

def applyLVL1Prescale(triggerPythonConfig, L1Prescales):
    Lvl1.updateItemPrescales(triggerPythonConfig, L1Prescales)
    
def applyHLTPrescale(triggerPythonConfig, HLTPrescale):
    for item, prescales in HLTPrescale.iteritems():
        # prescales is a list of 2 to 5 integers
        # [L2ps, EFps, L2pt, EFpt, L2rerun_prescale, EFrerun_prescale]
        if item not in triggerPythonConfig.allChains.keys():
            log.debug('Signature %s not registered to TriggerPythonConfig' % item)
            continue
        n = len(prescales)
        log.debug('Doing item: ' + item)
        l2chain = None
        efchain = None
        for ch in triggerPythonConfig.allChains[item]:
            if ch.level == 'L2': l2chain = ch
            elif ch.level == 'EF': efchain = ch
        if n > 0 and l2chain:
            l2chain.prescale = str(prescales[0])
            #triggerPythonConfig.allChains[item][0].prescale = str(prescales[0])
        if n > 1 and efchain:
            efchain.prescale = str(prescales[1])
            #triggerPythonConfig.allChains[item][1].prescale = str(prescales[1])
        if n > 2 and l2chain:
            l2chain.pass_through = str(prescales[2])
            #triggerPythonConfig.allChains[item][0].pass_through = str(prescales[2])
        if n > 3 and efchain:
            efchain.pass_through = str(prescales[3])
            #triggerPythonConfig.allChains[item][1].pass_through = str(prescales[3])
        if n == 5:
            if l2chain:
               l2chain.rerun_prescale = str(prescales[4])
            if efchain:
               efchain.rerun_prescale = str(prescales[4])
        if n == 6:
            if l2chain:
               l2chain.rerun_prescale = str(prescales[4])
            if efchain:
               efchain.rerun_prescale = str(prescales[5])
            #triggerPythonConfig.allChains[item][1].pass_through = str(prescales[3])


def assignTriggerGroups(triggerPythonConfig, TriggerGroups, HigherGroups):

    #PJB 16.02.2010 extended groups functionality
    for hgroup, item in (sorted(HigherGroups.iteritems())):
        for i in item:
            #see if we wish to add certain chain to the group
            if i in triggerPythonConfig.allChains.keys():
                #log.info("This is a chain " + i)
                cl = triggerPythonConfig.allChains[i]
                for ch in cl:
                    if hgroup not in ch.groups:
                        ch.addGroup(hgroup)

            #see if we wish to add chains with certain algo to the group
            elif i.startswith("ALG:"):
                algoname = i.split(":")[-1]
                #log.info("This is an algo name " + algoname)
                #look through all chains and see if they use this algo
                for cl in triggerPythonConfig.allChains.values():
                    for ch in cl:
                        for te in ch.getOutputTEs():
                            seq = str([str(s) for s in triggerPythonConfig.theSeqLists if s.output == te])
                            algolist = seq.split("->")[1]
                            #log.info("PJB algos are: " + algolist)
                            if not "/*" in algoname:
                                if algoname in algolist:
                                    if hgroup not in ch.groups:
                                        ch.addGroup(hgroup)
                            else:
                                algoclass = algoname.split("/*")[0]
                                if algoclass in algolist:
                                    if hgroup not in ch.groups:
                                        ch.addGroup(hgroup)
                                    
            #see if we wish to make a group of groups
            else:
                #log.info("This is a group name " + i)
                #allChains is a map of keys(chain names) to values (actual chains). Also double info, l2 and ef
                for cl in triggerPythonConfig.allChains.values():
                    for ch in cl:
                        if i in ch.groups:
                            if hgroup not in ch.groups:
                                ch.addGroup(hgroup)
                                

    for group, item in TriggerGroups.iteritems():
        for i in item:
            if i not in triggerPythonConfig.allChains.keys():
                log.warning('Signature %s not registered to TriggerPythonConfig' % i)
                continue
                       
            #log.info('Before assigning TG:' + str([ x.groups for x in triggerPythonConfig.allChains[i]]))
            #log.info('Doing detailed item: ' + str([ x.chain_name for x in triggerPythonConfig.allChains[i]]) )
            
            for ch in triggerPythonConfig.allChains[i]:
                if group not in ch.groups:
                    ch.addGroup(group)
            #log.info('After assigning TG:' + str([ x.groups for x in triggerPythonConfig.allChains[i]]))

def checkTriggerGroupAssignment(triggerPythonConfig):
    """ Checks menu consistency

    Checks trigger-groups assignment in Physics_ and MC_ menus.
    Excludes those chains which either have no group assigned or have a wrong group attached in Physics_
    which allows only primary, supporting and calibration triggers. 
    """
    menu_name = TriggerFlags.triggerMenuSetup()
    log.info( "Menu: " + menu_name)
    GroupItems = []
    CheckGroups=False

    if menu_name.startswith('Physics_lumi'):
        CheckGroups=True
        GroupItems = ['Primary', 'Supporting', 'Calibration', 'Monitoring']
    elif menu_name.startswith('MC_lumi'):
        CheckGroups=True
        GroupItems = ['Primary', 'Supporting', 'Calibration', 'Monitoring', 'Backup', 'Commissioning', 'Test']

    for chain in triggerPythonConfig.theL2HLTChains + triggerPythonConfig.theEFHLTChains:
        if len(chain.groups) == 0:
            log.error( "IN CHAIN: GROUP undefined for chain: " + str(chain.chain_name) )
        else:
            GroupAssigned=False
            
            for group in GroupItems:
                if group in chain.groups:
                    GroupAssigned=True
            if not GroupAssigned and CheckGroups:
                log.warning( "IN CHAIN: GROUP (primary, supporting, ... etc) undefined for chain: " + str(chain.chain_name) + ".")
                #log.warning( "IN CHAIN: GROUP undefined for chain: " + str(chain.chain_name) + ". The chain will be excluded from the menu!")
                #This part will become active soon
                #if chain in triggerPythonConfig.theL2HLTChains : triggerPythonConfig.theL2HLTChains.remove(chain)
                #if chain in triggerPythonConfig.theEFHLTChains: triggerPythonConfig.theEFHLTChains.remove(chain)

def checkStreamConsistency(triggerPythonConfig,streamConfig):
    """ Checks streamer consistency

    Checks that all chains are assigned to existing streams
    """
    menu_name = TriggerFlags.triggerMenuSetup()
    log.info( "Menu: " + menu_name)
    CheckGroups=False

    allStreams=['UPC',
                'TRTMonitoring',
                'WarmStart',      
                'ZeroBias',       
                'CosmicCalo',     
                'ZeroBiasOverlay',
                'CosmicMuons',
                'CosmicCalo',
                'IDCosmic',
                'IDMonitoring',
                'IDTracks',
                'Muon_Calibration',
                'Standby',   
                'Background',
                'MinBias',
                'MinBiasOverlay',
                'ALFA',
                'Bphysics',
                'express',
                'Tile',
                'L1Muon',
                'L1MinBias',
                'EnhancedBias',
                'LArCells',
                'LArCellsEmpty',
                'DISCARD',
                'EFCostMonitoring',
                'HLTPassthrough',
                'beamspot',
                'PixelBeam',
                'PixelNoise',
                'VdM',
                'L2CostMonitoring',
                'SCTNoise',
                'ALFACalib',
                'L1Calo',     
                'L1CaloCalib',     
                ]

    allStreams.extend(streamConfig)

    for chain in triggerPythonConfig.theL2HLTChains + triggerPythonConfig.theEFHLTChains:
        if len(chain.stream_tag) == 0:
            logger().error( "IN CHAIN: STREAMTAG undefined for chain: " + str(chain.chain_name) )
        else:
            for stream in chain.stream_tag:
#                print 'BETTA: ', chain,' Stream Tag: ',chain.stream_tag,' ',stream[0] 
#                if stream[0] in streamConfig:
                if stream[0] in allStreams:
                    continue
                else:
                    #raise Exception( 'ERROR: Chain: ' + chain.chain_name + ' has the wrong streamer ' + stream[0])
                    log.error(' Chain: ' + chain.chain_name + ' has the wrong streamer ' + stream[0])
                


def getJetWeights(triggerPythonConfig, use_fj=False):
    jw = []
    jthr, fjthr = [], []
    def thr2weights(thrs, factor=1.25):
        n = len(thrs)
        # need to skip any turned-off thresholds
        use = []
        for i in range(n):
            if thrs[i]<1023:
               use.append(1)
            else:
               use.append(0)
        # allow for thresholds being out of order
        index = order(thrs)
        # compute weights
        sum = 0
        weights = [0]*n
        for j in range(n):
            w = 0
            if use[index[j]]>0:
                jnext = j + 1
                while (jnext < n and use[index[jnext]]==0):
                    jnext = jnext + 1
                if (jnext < n):
                    w = int( (thrs[index[j]]+thrs[index[jnext]]+1)/2.0) - sum
                else:  
                    w = int(thrs[index[j]]*factor - sum)
            weights[index[j]]=w
            sum += w
        return weights
#
    def order(thrs): 
        n = len(thrs)
        thr1 = list(thrs)
        thr2 = sorted(thrs)
        index = []
        for i in range(n):
            val = thr2[i]
            pos = thr1.index(val)
            index.append(pos)
            if (thr1.count(val)>1):
                thr1[pos] = -thr1[pos]
        return index
#
    l1_thrs = triggerPythonConfig.Lvl1Thresholds()
    jet_thresholds = l1_thrs.allThresholdsOf('JET')
    fjet_thresholds = l1_thrs.allThresholdsOf('JF')
    log.debug('N jet thresholds: %d (8 expected)' % len(jet_thresholds))
    log.debug('N fjet thresholds: %d (4 expected)' % len(fjet_thresholds))
    jthr = [1023]*8
    fjthr = [1023]*4
    jet_names, fjet_names = ['---']*8, ['---']*4
    for j in jet_thresholds:
        log.debug('jet threshold %s: %d' % (j.name, j.thresholdInGeV()))
        # jthr.append(j.thresholdInGeV())
        jthr[j.mapping] = j.thresholdInGeV()
        jet_names[j.mapping] = j.name
    for j in fjet_thresholds:
        tvalues = j.thresholdValues
        priority = 0
        threshold_value = 0
        for tv in tvalues:
            if tv.priority > priority:
                threshold_value = tv.value
        log.debug('fjet threshold %s: %d' % (j.name, threshold_value))
        fjthr.append(threshold_value)
        fjthr[j.mapping] = threshold_value
        fjet_names[j.mapping] = j.name
    #
    s = ''
    for j in jet_names:
        s += '%s ' % j
    log.debug('Jet thresholds: %s' % s)
    s = ''
    for j in fjet_names:
        s += '%s ' % j
    log.debug('Fjet thresholds: %s' % s)
    if len(jthr) <= 8:
        w = thr2weights(jthr)
        jw = w
    else:
        jw = [0]*8
    w = [0]*4
    if use_fj:
        if len(fjthr) <= 4:
            w = thr2weights(fjthr)
    jw.extend(w)
    return jw

def resetAllPrescales(triggerPythonConfig):
    for sig in triggerPythonConfig.allChains.values():
        for chain in sig:
            if float(chain.prescale) > 0.:
                chain.prescale = '1'
            if float(chain.pass_through) > 0.:
                chain.pass_through = '1'
            if float(chain.rerun_prescale) > 0.:   
                chain.rerun_prescale = '1'
    for item in triggerPythonConfig.allItems.values():
        if float(item.prescale) > 0.:
            item.prescale = '1'
        
def allSignatures():
    sigs = []
    slices = (
        TriggerFlags.MuonSlice,
        TriggerFlags.EgammaSlice,
        TriggerFlags.TauSlice,
        TriggerFlags.JetSlice,
        TriggerFlags.BjetSlice,
        TriggerFlags.BphysicsSlice,
        TriggerFlags.METSlice,
        TriggerFlags.CombinedSlice,
        TriggerFlags.MinBiasSlice,
        TriggerFlags.CalibSlice,
        TriggerFlags.L1CaloSlice,
        )
    for a in slices:
        if a.signatures.statusOn: sigs += a.signatures()
    return sigs

def overwriteStreamTags(triggerPythonConfig, streamConfig):
    if len(streamConfig) > 0:
        for (name, chains) in triggerPythonConfig.allChains.iteritems():
            try:
                for c in chains:
                    overwriteStreamTagsInChain(c, streamConfig)
            except:
                log.warning( 'Exception in len(chains) for logical name: ' \
                          +name+', stream, '+ c.stream_tag)

def overwriteStreamTagsInChain(chain, streamConfig):
    """streamConfig: map between final stream tag for this configuration
    and all stream names belonging to this stream.
    {
      'jetEtmissTau': [ 'jets', 'etmissTau' ]
      ...
    }
    """
    stream_express = []
    stream_physics = []
    stream_calib = []
    for s in chain.stream_tag:
        # print 'checking original stream for %s: %s' % (chain.chain_name,str(s))
        # stream is a tuple (stream, type, obeyLB, prescale)
        if s[1] == 'express' or s[0]=='express' or s[1] == 'reserved':
            stream_express.append(s)
        elif s[1] == 'physics': stream_physics.append(s)
        elif s[1] == 'calibration': stream_calib.append(s)
        else:
            log.warning( 'Unknown stream type %s : %s' % (s[1], str(s)))
    chain.stream_tag = []
    new_streams = []
    # print 'stream_phys: ', stream_physics
    for s in stream_physics:
        # print 'check phys stream: ', s
        tmp = []
        for (name, ss) in streamConfig.iteritems():
            if s[0] in ss:
                tmp.append(name)
        # print 'New stream for ', s, ' is ', tmp
        if s[0].lower().find('cosmic')>=0:
            tmp.append(s[0])
        if len(tmp)==0:
            if s[0] not in ['combined']:
                tmp.append(s[0])
                log.debug('Warning! No new stream for %s, using its own' % s[0])
        if len(tmp)!=0:
            for t in tmp:
                if t not in new_streams: new_streams.append(t)
    for s in new_streams:
        chain.addStreamTag(s)
        # assume type='physics',obeyLB=True and prescale=1
        # print 'Replaced to ', s
    for s in stream_express:
        chain.addStreamTag(s[0], s[1], s[2], s[3])
    for s in stream_calib:
#Enforce obeyLB=no here
        chain.addStreamTag(s[0], s[1], "no", s[3])

def findL1TT(chain, triggerPythonConfig):
    l2_chain = chain
    if chain.level == 'EF':
        l2_chain_name = chain.lower_chain_name
        l2_chain = triggerPythonConfig.getHLTChain(l2_chain_name)
    if l2_chain == None:
        return -2
    l1_item_name = l2_chain.lower_chain_name
    l1_item = triggerPythonConfig.getLvl1Item(l1_item_name)
    if l1_item==None:
        return -1
    return l1_item.trigger_type

def setL1TTStream7Bit(chain, triggerPythonConfig):
    l1tt = findL1TT(chain, triggerPythonConfig)
    StreamName = []
    unseeded_chains = {
        # the order of things matter, the streaming can be overwritten 
        'xe20_EFonly' : 'L1Calo',
        'xe_debug_EFonly' : 'L1Calo',
        'xe_debug_EFonly_FEB' : 'L1Calo',
        'xe_debug_EFonly_allCells' : 'L1Calo',
        'te_debug_EFonly' : 'L1Calo',
        'te_debug_EFonly_FEB' : 'L1Calo',
        'te_debug_EFonly_allCells' : 'L1Calo', 
        'xe20_EFonly_noMu' : 'L1Calo', 
        'te150_EFonly': 'L1Calo', 
        'te150_EFonly_noMu': 'L1Calo', 
        '_always'     : 'MinBias',
        'mbRmMbts_1'     : 'MinBias',
        'mbRmMbts_1_1'     : 'MinBias',
        'mbRmMbts_1_1_NoCut_Time'     : 'MinBias',
        'mbRmMbts_1_NoCut_Time'     : 'MinBias',
        'mbRmMbts_2'     : 'MinBias',
        'mbRmMbts_2_NoCut_Time'     : 'MinBias',
        'mbRmMbts_1_unpaired'       : 'MinBias',
        'mbRmMbts_1_NoCut_Time_unpaired'     : 'MinBias',
        'mbRndm'     : 'MinBias',
        'mbSp'     : 'MinBias',
        'mbSpTrk'     : 'MinBias',
        'mbSp_oneD'     : 'MinBias',
        'mbTrtTrk'     : 'MinBias',
        'mb_RM_calib'     : 'MinBias',
        'mb_RM_eff'     : 'MinBias',
        'mb_RM_eff_unpaired'   : 'MinBias',
        'mb_RM_eff_cosmic'     : 'MinBias',
        'xe30_unbiased_cosmic' : 'L1Calo', 
        'xe30_unbiased_allCells_cosmic' : 'L1Calo', 
        'xe30_unbiased_FEB_cosmic' : 'L1Calo',
        'xe30_unbiased' : 'L1Calo', 
        'xe30_unbiased_allCells' : 'L1Calo', 
        'xe30_unbiased_FEB' : 'L1Calo',
        'xe30_allL1' : 'DISCARD',
        'xe30_allL1_allCells' : 'DISCARD',
        'xe30_allL1_FEB' : 'DISCARD',
        'mb_allL1_calib' : 'DISCARD',
        'EFTRTOnly_FS_allL1' : 'DISCARD',
        'EFInsideOut_FS_allL1'  : 'DISCARD',
        'L2IdScan_FS_allL1' : 'DISCARD',
        'L2SiTrack_FS_allL1'  : 'DISCARD',
        'mu4_MSonly_MB1_noL2_EFFS'     : 'MuonswBeam',
        'mu4_MSonly_MB2_noL2_EFFS'     : 'MuonswBeam',
        'mu6_MSonly_MB2_noL2_EFFS'     : 'MuonswBeam',
        'mu10_MSonly_MB2_noL2_EFFS'    : 'MuonswBeam',
        'SingleBeamRNDM'        : 'RNDM', 
        'SingleBeamBPTX'        : 'BPTX', 
        'SingleBeamL1Calo'      : 'L1Calo',
        'SingleBeamL1CaloEMFilter'      : 'L1CaloEM',
        'SingleBeamL1CaloEMFilter_V2'   : 'L1CaloEM',
        'SingleBeamRPCwBeam'    : 'RPCwBeam', 
        'SingleBeamTGCwBeam'    : 'TGCwBeam',
        'SingleBeamMuonswBeam_V2': 'MuonswBeam',
        'SingleBeamMinBias'        : 'MinBias', 
        'SingleBeamMinBiasFilter'  : 'MinBias', 
        'SingleBeamCosmicMuons'    : 'CosmicMuons',
        'SeededStreamerRNDM'        : 'RNDM',
        'SeededStreamerBPTX'        : 'BPTX',
        'SeededStreamerL1Calo'      : 'L1Calo',
        'SeededStreamerL1CaloEM'      : 'L1Calo',
        'SeededStreamerEMtoL1Calo'      : 'L1Calo',
        'SeededStreamerL1CaloTau'      : 'L1Calo',
        'SeededStreamerL1CaloJet'      : 'L1Calo',
        'SeededStreamerL1CaloMET'      : 'L1Calo',
        'SeededStreamerL1CaloEMFilter_V2'   : 'L1CaloEM',
        'SeededStreamerMuonswBeam'     : 'MuonswBeam',
  ## look further down for more similar     'SeededStreamerZeroBias'       : 'ZeroBias', 
        'SeededStreamerMinBias'        : 'MinBias', 
        'SeededStreamerMBTS'           : 'MinBias',
        'SeededStreamerBCM_LUCID_ZDC'  : 'MinBias', 
        'SeededStreamerMBTS_ZDC'       : 'MinBias',
        'SeededStreamerLUCID'          : 'MinBias',
        'SeededStreamerBCM'            : 'MinBias',
        'SeededStreamerCosmicMuons'    : 'CosmicMuons',
        'SeededStreamerCosmicCalo'     : 'CosmicCalo',
        'SeededStreamerCosmicCaloEM'   : 'CosmicCaloEM',
        'SeededStreamerCosmicMinBias'  : 'CosmicMinBias',
        'SeededStreamerWarmStart'    : 'WarmStart',
        'CosmicsAllTeIDSCAN'                : 'DISCARD',
        'CosmicsAllTeSiTrack'               : 'DISCARD',
        'CosmicsAllTeTRTxK'                 : 'DISCARD',
        'CosmicsAllTeIDSCAN_TRKHypo'        : 'CosmicMuons',
        'CosmicsAllTeSiTrack_TRKHypo'       : 'CosmicMuons',
        'TilePassthrough'      : 'CosmicMuons',
        'EM3_EMPTYPassthrough' : 'CosmicCalo',
        'J5_EMPTYPassthrough'  : 'CosmicCalo',
        'RD0_EMPTYPassthrough' : 'RNDM',
        'CosmicDownwardMuonTGCROI'          : 'CosmicDownwardMuons', 
        'CosmicDownwardMuonRPCROI'          : 'CosmicDownwardMuons', 
        'TileCalib_laser' : 'Tile', 
        'TileCalib_cis'   : 'Tile',
        'Cosmic_pixelnoise' : 'PixelNoise',
        'PixelNoise' : 'PixelNoise',
        'SCTNoise' : 'SCTNoise',
        'Cosmic_rateanalysis' : 'rateanalysis',
        'LArCalib'        : 'LArCells',
        'LArCalib_V2'        : 'LArCells',
        'j7_larcalib'          : 'LArCells',
        'j60_larcalib'          : 'LArCells',
        'g3_nocut_larcalib'  : 'LArCells',
        'g3_loose_larcalib'  : 'LArCells',
        'g5_loose_larcalib'       : 'LArCells',
        'g10_loose_larcalib'       : 'LArCells',
        'j10_larcalib'       : 'LArCells',
        'j80_larcalib'       : 'LArCells',
        'FJ3_larcalib'       : 'LArCells',
        'FJ18_larcalib'      : 'LArCells',
        'FJ5_larcalib'       : 'LArCells',
        'FJ15_larcalib'      : 'LArCells',
        'trk9_Central_Tau_IDCalib' : 'IDTracks',
        'trk9_Fwd_Tau_IDCalib' : 'IDTracks',
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'   : 'IDCosmic',
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_PEB': 'IDTracks',
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField': 'IDCosmic',
        'vtxbeamspot_jet10': 'beamspot',
        'vtxbeamspot_list': 'beamspot',
        'vtxbeamspot_FSTracks': 'beamspot',
        'vtxbeamspot_activeTE_peb': 'beamspot',
        'vtxbeamspot_allTE_peb': 'beamspot',
        'vtxbeamspot_FSTracks_allL1': 'beamspot',
        'vtxbeamspot_FSTracks_SiTrk': 'beamspot',
        'vtxbeamspot_activeTE_SiTrk_peb': 'beamspot',
        'vtxbeamspot_allTE_SiTrk_peb': 'beamspot',
        'vtxbeamspot_FSTracks_SiTrk_allL1': 'beamspot',
        'g0_mbts_a_EFfullcalo'  : 'MinBias',
        'g0_mbts_c_EFfullcalo'  : 'MinBias',
        'g0_mbts_a'             : 'MinBias',
        'g0_mbts_c'             : 'MinBias',
        'g5_EFfullcalo'         : 'L1Calo',
        'e0_mbts_a_EFfullcalo'  : 'MinBias',
        'e0_mbts_c_EFfullcalo'  : 'MinBias',
        'e5_EFfullcalo'         : 'L1Calo',
        'TRTPassthrough'        : 'IDCosmic',
        'mu0_low_rpc_peb'       : 'Muon_Calibration',
        'mu0_high_rpc_peb'      : 'Muon_Calibration',
        'mu6_rpc_peb'           : 'Muon_Calibration',
        'mu0_tgc_peb'           : 'Muon_Calibration',
        'mu6_tgc_peb'           : 'Muon_Calibration',
        'AllTeMBiasCalo_EndCapA': 'MinBias',
        'AllTeMBiasCalo_EndCapC': 'MinBias',
        'L1CaloFilter'          :'L1Calo',
        'L1CaloFilter_V2'       :'L1Calo',
        'L1CaloEMFilter'        :'L1Calo',
        #'L1CaloEM'              :'L1Calo',
        'L1CaloEMFilter_V2'     :'L1Calo',
        'L1CaloTauFilter'       :'L1Calo',
        'L1CaloTauFilter_V2'    :'L1Calo',
        'L1CaloJetFilter'       :'L1Calo',
        'L1CaloJetFilter_V2'    :'L1Calo',
        'TileMon'               :'L1Calo',
        'TileMon_cosmic'        :'CosmicCalo',
        'AllTe_MEt_EFOnly_MuonEF': 'DISCARD',
        'AllTe_MEt_MuonEF'       : 'DISCARD',
        'AllTeMEt'               : 'DISCARD',
        'AllTeMEt_EFOnly'        : 'DISCARD',
        '_FSTracks_activeTE'     : 'DISCARD',
        'L2_cost_monitor'        : 'L2CostMonitoring',
        'EF_cost_monitor'        : 'EFCostMonitoring',
        'L2_cost_monitor_rejAll' : 'L2CostMonitoring',
        'EF_cost_monitor_rejAll' : 'EFCostMonitoring',
        'Egamma_V2'              : 'CosmicCalo',
        'Jet_V2'                 : 'CosmicCalo',
        'CaloTileLookForMu_V2'   : 'CosmicCalo',
        'CaloTileRODMu_V2'       : 'CosmicCalo',
        'xe15_cosmic'            : 'CosmicCalo',
        'xe15_allCells_cosmic'   : 'CosmicCalo',
        'xe15_FEB_cosmic'        : 'CosmicCalo',
        'xe20_cosmic'            : 'CosmicCalo',
        'xe20_allCells_cosmic'   : 'CosmicCalo',
        'xe20_FEB_cosmic'        : 'CosmicCalo',
        'te150_cosmic'           : 'CosmicCalo',
        'tauNoCut_cosmic'        : 'CosmicCalo',
        'g5_nocut_cosmic'        : 'CosmicCalo',
        'e5_NoCut_cosmic'        : 'CosmicCalo',
        'fj5_cosmic'             : 'CosmicCalo',
        'j30_cosmic'             : 'CosmicCalo',
        'j50_cosmic'             : 'CosmicCalo',
        'j30_jetNoCut_cosmic'    : 'CosmicCalo',
        'j50_jetNoCut_cosmic'    : 'CosmicCalo',
        'j30_jetNoCut_cosmic'    : 'CosmicCalo',
        'j50_jetNoCut_cosmic'    : 'CosmicCalo',
        'j5_empty_NoAlg'         : 'CosmicCalo',
        'j10_empty_NoAlg'        : 'CosmicCalo',
        'j30_empty_NoAlg'        : 'CosmicCalo',
        'fj10_empty_NoAlg'       : 'CosmicCalo',
        'fj30_empty_NoAlg'       : 'CosmicCalo',
        'j5_cosmic'              : 'CosmicCalo',
        'j5_empty_exotic_L1J10'  : 'CosmicCalo',
        'j5_empty_exotic_L1J30'  : 'CosmicCalo',
        'L1ItemStreamer_L1_RD1_EMPTY' : 'CosmicCalo',
        'L1ItemStreamer_L1_RD0_EMPTY' : 'MinBias',
	'L1ItemStreamer_L1_LHCF' : 'MinBias',
        'rd1_empty_NoAlg'        : 'CosmicCalo',
        'rd1_empty_noAlg'        : 'CosmicCalo',
        'rd0_empty_NoAlg'        : 'CosmicCalo',
        'zerobias_noAlg'         : 'ZeroBias',
        'zerobias_NoAlg'         : 'ZeroBias',
        'zerobias_Overlay_NoAlg'        : 'ZeroBiasOverlay',
        'zerobias_Overlay_L2te'         : 'MinBiasOverlay',
        'zerobias_Overlay_L2mbMbts'     : 'MinBiasOverlay',
        'SeededStreamerZeroBias' : 'ZeroBias', 
        'mu0_empty_NoAlg'      : 'CosmicMuons',
        'mu0_comm_empty_NoAlg' : 'CosmicMuons',
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel' : 'IDCosmic',
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_AllID' : 'IDCosmic',
        'L1TRT_NoAlg' : 'IDCosmic',
        'Standby'     : 'Standby',
        'Background'  : 'Background',
        'Background_RD0_UNPAIRED_ISO' : 'Background',
        'Background_RD0_UNPAIRED_NONISO' : 'Background',
        'Background_RD0_EMPTY'        : 'Background,',
        'L1Calocalibration'           : 'L1CaloCalib',
        'L1Calocalibration_L1BRGP7'   : 'L1CaloCalib'
        }
    
    idcosmic_chains = (
        'CosmicsAllTeIDSCANCombined',
        'CosmicsAllTeTRTxK_TRTTrkHypo',
        'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel',
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics',
        'CosmicsAllTeIDSCAN_TrkHypo', 
        'CosmicsAllTeSiTrack_TrkHypo', 
        'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo', 
        'CosmicsAllTeSiTrack_AllPhysics_TrkHypo', 
        'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo', 
        'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo',
        'CosmicsAllTeEFID',
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel',
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_AllID',
       )
    
    log.debug('CHECK: %s : ', chain.chain_name)
    exactmatch = False
    for c in unseeded_chains.keys():
        if chain.chain_name.endswith(c) and not exactmatch:
            if (chain.chain_name[3:]==c):
                exactmatch = True
            s = unseeded_chains[c]
            log.debug('Setting new streamtag of %s to %s' % \
                     (chain.chain_name, s))
            StreamName.append(s)
    log.debug('Streamname up to now: %s' % StreamName)
    StreamBit2 = "L1Calo"
    StreamBit3 = "MuonswBeam"
    StreamBit4 = "L1CaloEM"
    if TriggerFlags.triggerMenuSetup()=="Cosmic2009_v1":
        StreamBit3 = "TGCwBeam"
        StreamBit4 = "RPCwBEAM"
    if TriggerFlags.triggerMenuSetup()=="Cosmic_v2":
        StreamBit2 = "CosmicCalo"
        StreamBit4 = "CosmicCaloEM"
    if TriggerFlags.triggerMenuSetup()=="Cosmic_v3":
        StreamBit2 = "CosmicCalo"
        StreamBit4 = "CosmicCaloEM"


    if StreamName==[] and l1tt<0:
        log.debug('Could not find L1TT for chain: %s (%d)' % \
                  (chain.chain_name, l1tt))
    elif StreamName==[]:
        l1tt = l1tt & 0x7f
        if l1tt == 0x00:
            # 2010/01/28: special case: InitialBeam_v2 - no BPTX trigger type
            # but still include MinBias chains seeded from BPTX
            # Delete this case when these chains are removed
            if chain.chain_name.count('BX0') == 1 or chain.chain_name.count('BX1') == 1:
                StreamName.append('BPTX')
            else:
                StreamName.append('ERROR_TriggerType0_ERROR')
                log.warning('Failed to determine stream from TriggerType for: '+chain.chain_name)
                
        if (l1tt & 0x01 !=0):
            StreamName.append('RNDM')
        if (l1tt & 0x02 !=0):
            StreamName.append('BPTX')
        if (l1tt & 0x04 !=0):
            StreamName.append(StreamBit2)
        if (l1tt & 0x08 !=0):
            StreamName.append(StreamBit3)
        if (l1tt & 0x10 !=0):
            StreamName.append(StreamBit4)
        if (l1tt & 0x20 !=0):
            StreamName.append('MinBias')
        if (l1tt & 0x40 !=0):
            StreamName.append('CosmicMuons')

    chain.stream_tag = filter(lambda x: x[1]!='physics', chain.stream_tag)
    chain.stream_tag = filter(lambda x: x[1]!='calibration', chain.stream_tag)
    if StreamName != []:
        log.debug('Overwrite stream tag of %s to %s' % \
                  (chain.chain_name, StreamName))
        for stream in StreamName:
            stype = 'physics'
            obeyLB = 'yes'
            if stream in [ 'IDTracks', 'LArCells', 'Tile' , 'Muon_Calibration', 'PixelNoise', 'SCTNoise', 'rateanalysis', 'L2CostMonitoring', 'EFCostMonitoring', 'beamspot' ]:
                stype  = 'calibration'
                obeyLB = 'no'
                if stream=='LArCells' and chain.level == 'L2':
                    stype  = 'physics'
                    obeyLB = 'yes'
            elif stream is 'DISCARD':
                stype = 'debug'

            conserved_chains =[
                'L1CaloFilter_V2',    
                'L1CaloEMFilter_V2',  
                'L1CaloTauFilter_V2', 
                'L1CaloJetFilter_V2',
                ]

            stream_prescale = '1'
            if (stype=='physics' and chain.chain_name[3:len(chain.chain_name)] not in TriggerFlags.StreamingSlice.signatures() and chain.chain_name[3:len(chain.chain_name)] not in conserved_chains):
                stream_prescale = '0'

            if (TriggerFlags.zero_stream_prescales):
                chain.addStreamTag(stream,stype,obeyLB,prescale=stream_prescale)
                print chain.chain_name+" gets stream prescale "+str(stream_prescale)+" in "+stream
            else:
                chain.addStreamTag(stream,stype,obeyLB,prescale='1')

            if (stream_prescale == '0' and TriggerFlags.physics_pass_through): chain.pass_through='1'


    for c in idcosmic_chains:
        if chain.chain_name.endswith(c):
            log.debug('Add stream tag of %s to %s' % \
                      (chain.chain_name, 'IDCosmic'))
            tmp = map(lambda x: x[0], chain.stream_tag)
            if 'IDCosmic' not in tmp:
                chain.addStreamTag('IDCosmic',type='physics',obeyLB='yes',prescale='1')

    if len(chain.stream_tag)==0:
        s = 'No stream tag was set for chain %s after setL1TTStream7Bit, '
        s += 'so setting %s'
        n = 'undefined_by_setL1TTStream7Bit'
        log.warning(s % (chain.chain_name, n))
        log.warning('Usually because the chain was unseeded and there was no L1 TT')
        chain.addStreamTag(n, type='physics', obeyLB='yes', prescale='1')

def overwriteStreamTag(chain, triggerPythonConfig):
    """ Deletes old stream tag and introduces a new one
        for chains specified in a dictionary
    """

    streaming_chains={
        'SeededStreamerWarmStart'  : 'WarmStart',      
        'SeededStreamerZeroBias'   : 'ZeroBias',       
        'SeededStreamerCosmicCalo' : 'CosmicCalo',     
        'zerobias_NoAlg'           : 'ZeroBias',       
        'zerobias_Overlay_NoAlg'   : 'ZeroBiasOverlay',
        'rd1_empty_NoAlg'          : 'CosmicCalo',
        'zerobias_noAlg'           : 'ZeroBias',
        'rd1_empty_noAlg'          : 'CosmicCalo',
        'rd0_empty_noAlg'          : 'CosmicCalo',
        'mu4_MSonly_cosmic'        : 'CosmicMuons',
        'mu4T_MSonly_cosmic'        : 'CosmicMuons',
        'mu4_comm_cosmic'          : 'CosmicMuons',
        'mu4_comm_MSonly_cosmic'   : 'CosmicMuons',        
        'mu4_tile_cosmic'          : 'CosmicMuons',
        'mu4_trod_cosmic'          : 'CosmicMuons',
        'mu6_cosmic'               : 'CosmicMuons',
        'mu4_cosmic'               : 'CosmicMuons',
        'mu6T_cosmic'               : 'CosmicMuons',
        'mu4T_cosmic'               : 'CosmicMuons',
        'j7_cosmic'                : 'CosmicCalo',
        'j10_cosmic'               : 'CosmicCalo',
        'j30_cosmic'               : 'CosmicCalo',
        'j50_cosmic'               : 'CosmicCalo',
        'j30_jetNoCut_cosmic'      : 'CosmicCalo',
        'j50_jetNoCut_cosmic'      : 'CosmicCalo',
        'L2_j25_jetNoCut_cosmic'      : 'CosmicCalo',
        'L2_j45_jetNoCut_cosmic'      : 'CosmicCalo',
        'j30_jetNoCut_cosmic'      : 'CosmicCalo',
        'j50_jetNoCut_cosmic'      : 'CosmicCalo',
        'j5_empty_NoAlg'           : 'CosmicCalo',
        'j10_empty_NoAlg'           : 'CosmicCalo',
        'fj10_empty_NoAlg'         : 'CosmicCalo',
        'j60_cosmic'               : 'CosmicCalo',
        'j80_cosmic'               : 'CosmicCalo',
        'tauNoCut_cosmic'          : 'CosmicCalo',
        'j30_LArNoiseBurst_empty'  : 'CosmicCalo',
        'j30_c4uchad_empty_LArNoiseBurst'  : 'CosmicCalo',
        'j30_u0uchad_empty_LArNoiseBurst'  : 'CosmicCalo',
        'j55_LArNoiseBurst_empty'  : 'CosmicCalo',
        'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo' : 'IDCosmic',
        'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo' : 'IDCosmic',
        'CosmicsAllTeSiTrack_AllPhysics_TrkHypo' : 'IDCosmic',
        'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo' : 'IDCosmic',
        'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel' : 'IDCosmic',
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics' : 'IDCosmic',
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField' : 'IDCosmic',
        'CosmicsAllTeEFID' : 'IDCosmic',
        'mu10_MSonly_cosmic ' : 'CosmicMuons',
        'em3_empty_noAlg'     : 'CosmicCalo',
        'g5_nocut_cosmic'     : 'CosmicCalo',
        'g5_NoCut_cosmic'     : 'CosmicCalo',
        'g10_NoCut_cosmic'    : 'CosmicCalo',
        'j5_empty_noAlg'      : 'CosmicCalo',
        'j10_empty_NoAlg'     : 'CosmicCalo',
        'j30_empty_NoAlg'     : 'CosmicCalo',
        'j30_cosmic'          : 'CosmicCalo',
        'j50_cosmic'          : 'CosmicCalo',
        'fj10_empty_noAlg'    : 'CosmicCalo',
        'tau5_empty_noAlg'    : 'CosmicCalo',
        'tau5_empty_NoAlg'    : 'CosmicCalo',
        'mu0_empty_NoAlg'      : 'CosmicMuons',
        'mu0_comm_empty_NoAlg' : 'CosmicMuons',
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel' : 'IDCosmic',
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_AllID' : 'IDCosmic',
        'L1Calo_NoAlg'        : 'L1Calo',
        'L1Muon_NoAlg'        : 'L1Muon',
        'L1MinBias_NoAlg'     : 'L1MinBias',
        'L1TRT_NoAlg' : 'IDCosmic',
        'Standby'     : 'Standby',   
        'Background'  : 'Background',
        'Background_RD0_UNPAIRED_ISO' : 'Background',
        'Background_RD0_UNPAIRED_NONISO' : 'Background',
        'Background_RD0_EMPTY'        : 'Background',
        'SeededStreamerExpressPhysicsHI': 'MinBias',
	'SeededStreamerExpressStandbyHI': 'Standby',
        "SeededStreamerRNDM" : 'MinBias',
        "SeededStreamerL1Calo"  : 'MinBias',
        "SeededStreamerMinBias"  : 'MinBias',
        "SeededStreamerMBTS"  : 'MinBias',
        "SeededStreamerBCM_LUCID_ZDC"  : 'MinBias',
        "SeededStreamerMuonswBeam"  : 'MinBias',
        "mbZdc_a_c_overlay_NoAlg" : 'MinBiasOverlay',
        "L1MBTS_1_1_overlay_NoAlg" : 'MinBiasOverlay',
	"mbMbts_1_1_overlay" : 'MinBiasOverlay',
        "SeededStreamerL1CaloXS"      : 'L1Calo',
        "xe30_allL1" : 'DISCARD',
        "xe30_allL1_FEB" : 'DISCARD',
        "xe30_allL1_allCells" : 'DISCARD',
        "xe30_allL1_noMu" : 'DISCARD',
        #"xe_NoCut_allL1" : 'DISCARD',
        "mu4_empty_NoAlg" : 'CosmicMuons',
        "mu11_empty_NoAlg" : 'CosmicMuons',
        "mu4_L1MU11_cosmic" : 'CosmicMuons',
        "mu4_L1MU11_MSonly_cosmic" : 'CosmicMuons',
        "allL1_NoAlg" : 'HLTPassthrough',
        "L1J5_empty_NoAlg" : 'CosmicCalo',
        "L1FJ0_empty_NoAlg" : 'CosmicCalo',
        "L1FJ1_empty_NoAlg" : 'CosmicCalo',
        "L1J10_empty_NoAlg" : 'CosmicCalo',
        "L1J30_empty_NoAlg" : 'CosmicCalo',
        "L1FJ10_empty_NoAlg" : 'CosmicCalo',
        "L1FJ30_empty_NoAlg" : 'CosmicCalo',
        'ALFA_Phys' : 'ALFA',
        'ALFA_Phys_Any' : 'ALFA',
        "L1RD0_firstempty_NoAlg" : 'JetTauEtmiss',
        #'PreS_L1J10_firstempty' : 'JetTauEtmiss',
        #'PreS_L1J30_firstempty' : 'JetTauEtmiss',
        }

    # menu-dependent part now
    menu_name = TriggerFlags.triggerMenuSetup()
    if not '_HI_' in menu_name:
        streaming_chains["PreS_L1J10_firstempty"]="JetTauEtmiss"
        streaming_chains["PreS_L1J30_firstempty"]="JetTauEtmiss"

    if '_HI_' in menu_name:
        streaming_chains["PreS_L1J10_firstempty"]="MinBias"
        streaming_chains["PreS_L1J30_firstempty"]="MinBias"

        
    for c in streaming_chains.keys():
        if chain.chain_name.endswith(c):
            for i in range(len(chain.stream_tag)):
                log.debug('Remove original stream tag %s for %s' % \
                         (chain.stream_tag[i], chain.chain_name))
                del chain.stream_tag[i]
                
            log.debug('Add new stream tag of %s to %s' % \
                     (chain.chain_name, streaming_chains[c]))
            #tmp = map(lambda x: x[0], chain.stream_tag)
            #if streaming_chains[c] not in tmp:
            chain.addStreamTag(streaming_chains[c],type='physics',obeyLB='yes',prescale='1')

    bphysics_streaming_chains=[]
    bphysics_and_muon_streaming_chains=[]
    if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
        bphysics_streaming_chains+=[
            '2mu4T_Bmumux',           
            '2mu4T_Bmumux_BarrelOnly',
            '2mu4T_Bmumux_Barrel',
            '2mu4T_Jpsimumu',
            '2mu4T_Jpsimumu_BarrelOnly',
            '2mu4T_Jpsimumu_Barrel',
            '2mu4T_Upsimumu',
            '2mu4T_Upsimumu_BarrelOnly',
            '2mu4T_Upsimumu_Barrel',
            'mu4Tmu6_Bmumux',
            'mu4Tmu6_Bmumux_Barrel',
            'mu4Tmu6_Jpsimumu',
            'mu4Tmu6_Jpsimumu_Barrel',
            'mu4Tmu6_Upsimumu',
            'mu4Tmu6_Upsimumu_Barrel',
            '3mu4T', '3mu4T_DiMu', '3mu4T_Jpsi', '3mu4T_Upsi', 
            '2mu6i_DiMu_DY',
            '2mu6_DiMu_DY20',
            '2mu6_DiMu_DY25',
            '2mu4T_DiMu_e5_tight1',
            '2mu4T_2e5_tight1',
            '2mu4T_Bmumux_v2',
            'mu4Tmu6_Bmumux_v2',
            '2mu4T_Bmumux',
            'mu4Tmu6_Bmumux',

            '2mu4T_Bmumux_L2StarB',
            '2mu4T_Bmumux_v2_L2StarB',
            'mu4Tmu6_Bmumux_v2_L2StarB',
            '2mu4T_Jpsimumu_L2StarB',
            '2mu4T_Jpsimumu_BarrelOnly_L2StarB',
            '2mu4T_Jpsimumu_Barrel_L2StarB',
            'mu4Tmu6_Jpsimumu_L2StarB',
            'mu4Tmu6_Jpsimumu_Barrel_L2StarB',
            '2mu4T_Upsimumu_L2StarB',
            '2mu4T_Bmumux_Barrel_v2_L2StarB',
            '2mu4T_Bmumux_BarrelOnly_v2_L2StarB',
            'mu4Tmu6_Bmumux_Barrel_v2_L2StarB',

            '2mu4T_Upsimumu_Barrel_L2StarB',
            '2mu4T_Upsimumu_BarrelOnly_L2StarB', 
            'mu4Tmu6_Upsimumu_L2StarB',
            'mu4Tmu6_Upsimumu_Barrel_L2StarB',
            ]     
        bphysics_and_muon_streaming_chains+=[
            '2mu6_Bmumu',
            '2mu6_Bmumux',
            '2mu6_Jpsimumu',
            '2mu6_Upsimumu',
            '2mu4T_DiMu_noVtx_noOS',
            'mu4Tmu6_DiMu_noVtx_noOS',
            '2mu6_DiMu_noVtx_noOS',
            '2mu4T_Jpsimumu_IDTrkNoCut',
            'mu4Tmu6_Jpsimumu_IDTrkNoCut',
            '2mu4T_DiMu',
            'mu4Tmu6_DiMu',
            '2mu6_DiMu',
            '2mu4T_DiMu_L2StarB',
            '2mu4T_DiMu_L2StarC',
            '2mu4T_DiMu_l2muonSA',
            'mu6_Trk_Jpsi_loose',
            'mu6_Jpsimumu_tight',
            'mu10_Jpsimumu',
            'mu10_Upsimumu_tight_FS',

            '2mu6_Jpsimumu_L2StarB',
            '2mu4T_DiMu_noVtx_noOS_L2StarB', 
            '2mu6_DiMu_noVtx_noOS_L2StarB', 
            "mu6_Trk_Jpsi_loose_L2StarB",

            '2mu6_Bmumux_v2',
            '2mu6_Bmumu_L2StarB',
            '2mu6_Bmumux_v2_L2StarB',
            '2mu6_DiMu_L2StarB',
            'mu6_Jpsimumu_tight_L2StarB',
            '2mu6_Upsimumu_L2StarB',
            ]

    for c in bphysics_streaming_chains:
        if (chain.chain_name.endswith(c)):
            for i in range(len(chain.stream_tag)):
                if not 'mu18_tight_L2_2mu4T_Jpsimumu' in chain.chain_name:
                    log.debug('Remove original stream tag %s for %s' % \
                             (chain.stream_tag[i], chain.chain_name))
                    del chain.stream_tag[i]
                
                log.debug('Add new stream tag of %s to Bphysics' % chain.chain_name)
                chain.addStreamTag('Bphysics',type='physics',obeyLB='yes',prescale='1')

    for c in bphysics_and_muon_streaming_chains:
        if (chain.chain_name.endswith(c)):
            log.debug('Add stream tag of %s to Bphysics' % chain.chain_name)
            chain.addStreamTag('Bphysics',type='physics',obeyLB='yes',prescale='1')
                

##########################################################################################
#
# remap L1 thesholds
#
def remapL1Thresholds(tpc, thrMap):
    """ Iterate over HLT sequences and replace old L1 thresholds
        inputs to this function: 1) TriggerPythonConfig
                                 2) dictionary old->new thresholds
    """

    log.debug('Running remapL1Thresholds...')
    
    # Iterate over all input TEs for all sequences
    for seq in tpc.theSeqLists:
        newInput = []
        hasChanged = False
        for inputTE in seq.input:
            
            # Check if inputTE name is on list of old thresholds
            if inputTE in thrMap:
                hasChanged = True
                newInput.append(thrMap[inputTE])
            else:
                newInput.append(inputTE)

        # Replace input TEs with new list
        if hasChanged:
            log.debug('Sequence '+seq.output+': replaced input '+str(seq.input)+' with '+str(newInput))
            seq.input = newInput
            
##########################################################################################
#
# remap L1 items: inputs=(TriggerPythonConfig, dictionary old->new)
#  - Iterate over HLT chains and replace old L1 items
#
def remapL1Items(tpc, itemMap):
    """ Iterate over HLT sequences and replace old L1 items
        inputs to this function: 1) TriggerPythonConfig
                                 2) dictionary old->new items
    """
    log.debug('Running remapL1Items...')
    
    # Iterate over all L2 chains
    for chain in tpc.theL2HLTChains:
        lowname = lowname
        if lowname in itemMap:
            chain.lower_chain_name = itemMap[lowname]
            log.debug('"The" chain '+chain.chain_name+': replaced input '+lowname+' with '+itemMap[lowname])
            
    # Iterate over all HLT chains
    for logic, chains in tpc.allChains.iteritems():
        for chain in chains:
            lowname = chain.lower_chain_name
            if chain.isL2() and lowname in itemMap:
                if 'L1ItemStreamer' in chain.chain_name:
                    log.debug('Skipping input remap for '+chain.chain_name)
                    continue
                chain.lower_chain_name = itemMap[lowname]
                log.debug('"All" chain '+chain.chain_name+': replaced input '+lowname+' with '+itemMap[lowname])
