# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerMenu.menu.Lvl1       import Lvl1
from TriggerMenu.menu.Lvl1Flags  import Lvl1Flags
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging        import logging

log = logging.getLogger('TriggerMenu.menu.MenuUtil.py')

def applyLVL1Prescale(triggerPythonConfig, L1Prescales):
    Lvl1.updateItemPrescales(triggerPythonConfig, L1Prescales)
    
def applyHLTPrescale(triggerPythonConfig, HLTPrescale):
    for item, prescales in HLTPrescale.iteritems():
        # prescales is a list of 3 integers [HLT_prescale, HLT_pass_through, rerun_prescale]
        if item not in triggerPythonConfig.allChains.keys():
            log.debug('Signature %s not registered to TriggerPythonConfig' % item)
            continue
        n = len(prescales)
        hltchain = None
        for ch in triggerPythonConfig.allChains[item]:
            if ch.level == 'HLT': hltchain = ch
        if n > 0  and hltchain:
            hltchain.prescale = str(prescales[0])
        if n > 1  and hltchain:
            hltchain.pass_through = str(prescales[1])
        if n > 2  and hltchain:
            hltchain.rerun_prescale = str(prescales[2])

        log.info('Applied HLTPS to the item '+item+': PS'+ hltchain.prescale+" PT"+hltchain.pass_through+" RerunPS"+hltchain.rerun_prescale)
        
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

#def checkStreamConsistency(triggerPythonConfig,streamConfig):
def checkStreamConsistency(triggerPythonConfig):
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
                'physics',
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

    #allStreams.extend(streamConfig)

    for chain in triggerPythonConfig.theL2HLTChains + triggerPythonConfig.theEFHLTChains:
        if len(chain.stream_tag) == 0:
            logger().error( "IN CHAIN: STREAMTAG undefined for chain: " + str(chain.chain_name) )
        else:
            for stream in chain.stream_tag:
                if stream[0] in allStreams:
                    continue
                else:
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
        TriggerFlags.JetSlice,
        TriggerFlags.METSlice,
        TriggerFlags.TauSlice,
        TriggerFlags.EgammaSlice,
        TriggerFlags.MinBiasSlice,
        TriggerFlags.CombinedSlice,
        )
    for a in slices:
        if a.signatures.statusOn: sigs += a.signatures()
    return sigs

# def overwriteStreamTags(triggerPythonConfig, streamConfig):
#     log.info('MenuUtil: OverwriteStreamTags')
#     if len(streamConfig) > 0:
#         log.info('MenuUtil: OverwriteStreamTags: len(streamConfig) >0')
#         for (name, chains) in triggerPythonConfig.allChains.iteritems():
#             try:
#                 for c in chains:
#                     log.info('MenuUtil: OverwriteStreamTags: calling overwriteStreamTagsInChain function!')
#                     overwriteStreamTagsInChain(c, streamConfig)
#             except:
#                 log.warning( 'Exception in len(chains) for logical name: ' \
#                           +name+', stream, '+ c.stream_tag)
#     log.info('MenuUtil: OverwriteStreamTags END')


#def overwriteStreamTagsInChain(chain, streamConfig): # needed ? Might just add a check that the stream name is correct...
def overwriteStreamTagsInChain(chain): # needed ? Might just add a check that the stream name is correct...
    """streamConfig: map between final stream tag for this configuration
    and all stream names belonging to this stream.
    {
      'jetEtmissTau': [ 'jets', 'etmissTau' ]
      ...
    }
    """
    log.info('MenuUtil: OverwriteStreamTagsInChain')
    stream_express = []
    stream_physics = []
    stream_calib = []
    for s in chain.stream_tag:
        log.info('MenuUtil: OverwriteStreamTagsinChain: checking original stream for %s: %s' % (chain.chain_name,str(s)))
        # stream is a tuple (stream, type, obeyLB, prescale)
        if s[1] == 'express' or s[0]=='express' or s[1] == 'reserved':
            stream_express.append(s)
        elif s[1] == 'physics': stream_physics.append(s)
        elif s[1] == 'calibration': stream_calib.append(s)
        else:
            log.warning('Unknown stream type %s : %s' % (s[1], str(s)))
    chain.stream_tag = []
    new_streams = []
    # print 'stream_phys: ', stream_physics
    for s in stream_physics:
        # print 'check phys stream: ', s
        tmp = []
        #CAT#for (name, ss) in streamConfig.iteritems():
        #CAT#    if s[0] in ss:
        #CAT#        tmp.append(name)
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

    log.info('MenuUtil: OverwriteStreamTagsinChain END')



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
        #'xe20_EFonly' : 'L1Calo',
        #'xe_debug_EFonly' : 'L1Calo',
        #'SeededStreamerMBTS'           : 'MinBias',
        }
    
    idcosmic_chains = (

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
            #if (stype=='physics' and chain.chain_name[3:len(chain.chain_name)] not in TriggerFlags.StreamingSlice.signatures() and chain.chain_name[3:len(chain.chain_name)] not in conserved_chains):
            #    stream_prescale = '0'

            if (TriggerFlags.zero_stream_prescales):
                chain.addStreamTag(stream,stype,obeyLB,prescale=stream_prescale)
                stringToPrint = chain.chain_name+" gets stream prescale "+str(stream_prescale)+" in "+stream
                log.info("Streaming: %s" % stringToPrint)
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

##########################################################################################
# OBSOLETE!!
#
def overwriteStreamTag(chain, triggerPythonConfig):
    """ Deletes old stream tag and introduces a new one
        for chains specified in a dictionary
    """
    streaming_chains={
        }
        
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
        lowname = chain.lower_chain_name
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
