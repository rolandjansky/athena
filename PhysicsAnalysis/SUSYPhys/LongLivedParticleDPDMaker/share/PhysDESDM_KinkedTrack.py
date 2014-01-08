##-----------------------------------------------------------------------------
## Name: PhysDESDM_KinkedTrack.py
##
## Author: Shimpei Yamamoto
## Email:  shimpei.yamamoto@cern.ch
##
## Description: This defines the content of the RPVLL_KinkedTrack primary physics
##              ESD-basedDPD output stream.
##
##-----------------------------------------------------------------------------

if primRPVLLDESDM.KinkedTrack_do2010Skim() :
    ###################################
    ### configuration for 2010 data ###
    ###################################

    ### configure trigger filters
    if len(primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames) == 0:
        if rec.triggerStream() == "Egamma":
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames = primRPVLLDESDM.KinkedTrack_triggerFilterFlags.EgammaTriggerNames
        elif rec.triggerStream() == "JetTauEtmiss":
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames = primRPVLLDESDM.KinkedTrack_triggerFilterFlags.JetTauEtmissTriggerNames
        elif rec.triggerStream() == "Muons":
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames = primRPVLLDESDM.KinkedTrack_triggerFilterFlags.MuonsTriggerNames
        elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames = primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.EgammaTriggerNames
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames += primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.JetTauEtmissTriggerNames
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames += primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.MuonsTriggerNames
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames = primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.EgammaTriggerNames
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames += primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.JetTauEtmissTriggerNames
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames += primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.MuonsTriggerNames
        else:
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

    # get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
    primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames = list(set(primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames))

    triggerFilterNames=[]

    from PrimaryDPDMaker.TriggerFilter import TriggerFilter
    for trigName in primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames:
        TriggerFilterName = "KinkedTrack_TriggerFilter_"+trigName
        topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
        triggerFilterNames.append( TriggerFilterName )
        pass

    # Create a combined filter by ORing together all the trigger filters
    combinedTriggerFilterName = "KinkedTrack_CombinedTriggerFilter"
    topSequence += LogicalFilterCombiner( combinedTriggerFilterName )
    triggerFilterCounter = 0
    cmdstring = ""
    for triggerFilterName in triggerFilterNames :
        if triggerFilterCounter > 0 :
            cmdstring += " or "
            pass
        cmdstring += triggerFilterName
        triggerFilterCounter += 1
        pass
    topSequence.KinkedTrack_CombinedTriggerFilter.cmdstring = cmdstring    

    offlineObjectFilterNames=[]

    ## configure a singlejetmet filter
    ## ( searching for chargino direct production associated with ISR jet )
    if primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags() :
        from LongLivedParticleDPDMaker.SingleJetMetFilter import SingleJetMetFilter
        singleJetMetFilterName = 'KinkedTrack_SingleJetMetFilter'
        topSequence += SingleJetMetFilter(singleJetMetFilterName)
        offlineObjectFilterNames.append( singleJetMetFilterName )
        topSequence.KinkedTrack_SingleJetMetFilter.jetCollectionName = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.jetCollectionName
        topSequence.KinkedTrack_SingleJetMetFilter.cutEt1Min = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEt1Min
        topSequence.KinkedTrack_SingleJetMetFilter.cutEta1Max = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEta1Max
        topSequence.KinkedTrack_SingleJetMetFilter.cutEt2Min = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEt2Min
        topSequence.KinkedTrack_SingleJetMetFilter.cutEta2Max = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEta2Max
        topSequence.KinkedTrack_SingleJetMetFilter.doSecondJetVeto = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.doSecondJetVeto
        topSequence.KinkedTrack_SingleJetMetFilter.requireMet = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.requireMet
        topSequence.KinkedTrack_SingleJetMetFilter.metCollectionNames = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.metCollectionNames
        topSequence.KinkedTrack_SingleJetMetFilter.cutMetMin = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutMetMin
        pass

    ## configure a multijetmet filter
    ## ( searching for charginos via colored production )
    #if primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags() :
        #from LongLivedParticleDPDMaker.MultiJetMetFilter import MultiJetMetFilter
        #multiJetMetFilterName = 'KinkedTrack_MultiJetMetFilter'
        #topSequence += MultiJetMetFilter(multiJetMetFilterName)
        #offlineObjectFilterNames.append( multiJetMetFilterName )
        #topSequence.KinkedTrack_MultiJetMetFilter.jetCollectionName = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.jetCollectionName
        #topSequence.KinkedTrack_MultiJetMetFilter.cutsEtMin = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.cutsEtMin
        #topSequence.KinkedTrack_MultiJetMetFilter.cutEtaMax = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.cutEtaMax
        #topSequence.KinkedTrack_MultiJetMetFilter.requireMet = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.requireMet
        #topSequence.KinkedTrack_MultiJetMetFilter.metCollectionNames = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.metCollectionNames
        #topSequence.KinkedTrack_MultiJetMetFilter.cutMetMin = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.cutMetMin
        #pass
    
    ########### combine the offline filters
    
    combinedOfflineFilterName = "KinkedTrack_CombinedOfflineFilter"
    topSequence += LogicalFilterCombiner( combinedOfflineFilterName )
    
    offlineFilterCounter = 0
    cmdstring = ""
    for offlineFilterName in offlineObjectFilterNames :
        if offlineFilterCounter > 0 :
            cmdstring += " or "
            pass
        cmdstring += offlineFilterName
        offlineFilterCounter += 1
        pass
    topSequence.KinkedTrack_CombinedOfflineFilter.cmdstring=cmdstring
    
    ########### combine the trigger and offline filters
    
    KinkedTrackCombinedFilter=LogicalFilterCombiner("KinkedTrackCombinedFilter")
    topSequence+=KinkedTrackCombinedFilter
    
    topSequence.KinkedTrackCombinedFilter.cmdstring="KinkedTrack_CombinedTriggerFilter and KinkedTrack_CombinedOfflineFilter"    

elif primRPVLLDESDM.KinkedTrack_do2011Skim() :    
    ###################################
    ### configuration for 2011 data ###
    ###################################

    ### configure trigger filters
    if len(primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames) == 0:
        if rec.triggerStream() == "Egamma":
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames = primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.EgammaTriggerNames
        elif rec.triggerStream() == "JetTauEtmiss":
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames = primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.JetTauEtmissTriggerNames
        elif rec.triggerStream() == "Muons":
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames = primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.MuonsTriggerNames
        elif rec.triggerStream() == "": # for MC the string will be empty, but we want to use all of the triggers
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames = primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.EgammaTriggerNames
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames += primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.JetTauEtmissTriggerNames
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.TriggerNames += primRPVLLDESDM.KinkedTrack_triggerFilterFlags2010.MuonsTriggerNames
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames = primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.EgammaTriggerNames
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames += primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.JetTauEtmissTriggerNames
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames += primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.MuonsTriggerNames
        else:
            primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames = ["dummy"] # back-up, to prevent empty-string failure in test jobs if no triggers used

    # get rid of potential doubles in the trigger lists, since attempting to add identical algs generates an ERROR
    primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames = list(set(primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames))

    triggerFilterNames=[]
    triggerDPhiFilterNames=[]

    from PrimaryDPDMaker.TriggerFilter import TriggerFilter
    for trigName in primRPVLLDESDM.KinkedTrack_triggerFilterFlags2011.TriggerNames:
        TriggerFilterName = "KinkedTrack_TriggerFilter_"+trigName
        topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
        if "dphi" in trigName:
            triggerDPhiFilterNames.append( TriggerFilterName )
        else:
            triggerFilterNames.append( TriggerFilterName )
        pass

    # Create a combined filter by ORing together all the trigger filters
    combinedTriggerFilterName = "KinkedTrack_CombinedTriggerFilter"
    topSequence += LogicalFilterCombiner( combinedTriggerFilterName )
    triggerFilterCounter = 0
    cmdstring = ""
    for triggerFilterName in triggerFilterNames :
        if triggerFilterCounter > 0 :
            cmdstring += " or "
            pass
        cmdstring += triggerFilterName
        triggerFilterCounter += 1
        pass
    topSequence.KinkedTrack_CombinedTriggerFilter.cmdstring = cmdstring    

    combinedDPhiTriggerFilterName = "KinkedTrack_CombinedDPhiTriggerFilter"
    topSequence += LogicalFilterCombiner( combinedDPhiTriggerFilterName )
    triggerDPhiFilterCounter = 0
    cmdstring = ""
    for triggerFilterName in triggerDPhiFilterNames :
        if triggerDPhiFilterCounter > 0 :
            cmdstring += " or "
            pass
        cmdstring += triggerFilterName
        triggerDPhiFilterCounter += 1
        pass
    topSequence.KinkedTrack_CombinedDPhiTriggerFilter.cmdstring = cmdstring    

    ## configure a multijetmet filter
    ## ( searching for charginos via colored production )
    offlineMultiJetMetFilterNames=[]

    if primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags() :
        from LongLivedParticleDPDMaker.MultiJetMetFilter import MultiJetMetFilter
        multiJetMetFilterName = 'KinkedTrack_MultiJetMetFilter'
        topSequence += MultiJetMetFilter( multiJetMetFilterName )
        offlineMultiJetMetFilterNames.append( multiJetMetFilterName )
        topSequence.KinkedTrack_MultiJetMetFilter.jetCollectionName = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.jetCollectionName
        topSequence.KinkedTrack_MultiJetMetFilter.cutsEtMin = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.cutsEtMin
        topSequence.KinkedTrack_MultiJetMetFilter.cutEtaMax = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.cutEtaMax
        topSequence.KinkedTrack_MultiJetMetFilter.requireMet = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.requireMet
        topSequence.KinkedTrack_MultiJetMetFilter.metCollectionNames = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.metCollectionNames
        topSequence.KinkedTrack_MultiJetMetFilter.cutMetMin = primRPVLLDESDM.KinkedTrack_multiJetMetFilterFlags.cutMetMin
        pass
    
    ## configure a singlejetmet filter
    ## ( searching for chargino direct production associated with ISR jet )
    offlineSingleJetMetFilterNames=[]

    if primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags() :
        from LongLivedParticleDPDMaker.SingleJetMetFilter import SingleJetMetFilter
        singleJetMetFilterName = 'KinkedTrack_SingleJetMetFilter'
        topSequence += SingleJetMetFilter(singleJetMetFilterName)
        offlineSingleJetMetFilterNames.append( singleJetMetFilterName )
        topSequence.KinkedTrack_SingleJetMetFilter.jetCollectionName = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.jetCollectionName
        topSequence.KinkedTrack_SingleJetMetFilter.cutEt1Min = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEt1Min
        topSequence.KinkedTrack_SingleJetMetFilter.cutEta1Max = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEta1Max
        topSequence.KinkedTrack_SingleJetMetFilter.cutEt2Min = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEt2Min
        topSequence.KinkedTrack_SingleJetMetFilter.cutEta2Max = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEta2Max
        topSequence.KinkedTrack_SingleJetMetFilter.doSecondJetVeto = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.doSecondJetVeto
        topSequence.KinkedTrack_SingleJetMetFilter.requireMet = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.requireMet
        topSequence.KinkedTrack_SingleJetMetFilter.metCollectionNames = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.metCollectionNames
        topSequence.KinkedTrack_SingleJetMetFilter.cutMetMin = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutMetMin
        pass

    ########### combine the offline filters
    
    combinedOfflineMultiJetMetFilterName = "KinkedTrack_CombinedOfflineMultiJetMetFilter"
    topSequence += LogicalFilterCombiner( combinedOfflineMultiJetMetFilterName )
    offlineMultiJetMetFilterCounter = 0
    cmdstring = ""
    for offlineFilterName in offlineMultiJetMetFilterNames :
        if offlineMultiJetMetFilterCounter > 0 :
            cmdstring += " or "
            pass
        cmdstring += offlineFilterName
        offlineMultiJetMetFilterCounter += 1
        pass
    topSequence.KinkedTrack_CombinedOfflineMultiJetMetFilter.cmdstring=cmdstring

    combinedOfflineSingleJetMetFilterName = "KinkedTrack_CombinedOfflineSingleJetMetFilter"
    topSequence += LogicalFilterCombiner( combinedOfflineSingleJetMetFilterName )
    offlineSingleJetMetFilterCounter = 0
    cmdstring = ""
    for offlineFilterName in offlineSingleJetMetFilterNames :
        if offlineSingleJetMetFilterCounter > 0 :
            cmdstring += " or "
            pass
        cmdstring += offlineFilterName
        offlineSingleJetMetFilterCounter += 1
        pass
    topSequence.KinkedTrack_CombinedOfflineSingleJetMetFilter.cmdstring=cmdstring
    
    ########### combine the trigger and offline filters
    
    KinkedTrackCombinedFilter=LogicalFilterCombiner("KinkedTrackCombinedFilter")
    topSequence+=KinkedTrackCombinedFilter
    
    topSequence.KinkedTrackCombinedFilter.cmdstring="( KinkedTrack_CombinedTriggerFilter and KinkedTrack_CombinedOfflineMultiJetMetFilter ) or ( KinkedTrack_CombinedDPhiTriggerFilter and KinkedTrack_CombinedOfflineSingleJetMetFilter)"
    
########### add this to the global top filter
    
if topCmdString.__len__() > 0:
    topCmdString+=" or "
    pass
topCmdString+="KinkedTrackCombinedFilter"
 
