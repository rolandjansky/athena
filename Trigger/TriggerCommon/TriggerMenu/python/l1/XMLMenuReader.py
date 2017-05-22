# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Lvl1Thresholds import LVL1Threshold, LVL1TopoInput, ThresholdValue
from Lvl1MenuItems import LVL1MenuItem

from PrescaleHelper import getCutFromPrescale

from Logic import Logic
from Lvl1Condition import Lvl1InternalTrigger, ThrCondition
from CaloInfo import MinimumTOBPt
from Lvl1MonCounters import Lvl1CtpinCounter, Lvl1CtpmonCounter


from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerConfigLVL1")


def readLogic( logicElement, thresholdMap ):

    if logicElement.tag == 'InternalTrigger':
        return Lvl1InternalTrigger( logicElement.get('name') )

    if logicElement.tag == 'TriggerCondition':
        thr = thresholdMap[ logicElement.get('triggerthreshold') ]
        return ThrCondition(thr, int(logicElement.get('multi')) )
    
    L = Logic()
    if logicElement.tag == 'AND':
        L.logic = Logic.AND
    elif logicElement.tag == 'OR':
        L.logic = Logic.OR
    elif logicElement.tag == 'NOT':
        L.logic = Logic.NOT
    else:
        raise RuntimeError("ERROR: don't understand tag %s" % logicElement.tag)

    for c in logicElement.getchildren():
        L.subConditions += [ readLogic(c, thresholdMap) ]

    return L
    


def readMenuFromXML(l1menu, filename):

    log.info("Reading L1 menu from %s" % filename)
    l1menu.l1menuFromXML = True

    from XMLReader import L1MenuXMLReader
    reader = L1MenuXMLReader(filename)


    if 'ctpVersion' in reader.LVL1Config:
        from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
        Lvl1Flags.CTPVersion = int ( reader.LVL1Config['ctpVersion'] )


    l1menu.menuName = reader.LVL1Config['name']
    l1menu.items.menuName = reader.LVL1Config.TriggerMenu['name']
    l1menu.items.pssName = reader.LVL1Config.PrescaleSet['name']

    prioMap = {}
    if hasattr(reader.LVL1Config,"PrioritySet"): # new files have no more PrioritySet
        for prio in reader.LVL1Config.PrioritySet.Prioritys:
            prioMap[int(prio['ctpid'])] = 0 if prio.strippedText()=="HIGH" else 1
            


    # Prescales
    psMap = {}
    for ps in reader.LVL1Config.PrescaleSet.Prescales:
        if 'cut' in ps:
            psMap[int(ps['ctpid'])] = int(ps['cut'], 16)  # new style
        else:
            psMap[int(ps['ctpid'])] = getCutFromPrescale(int(ps.strippedText())) # old style


    # Thresholds from the TriggerThresholds section
    for x in reader.getL1Thresholds():
        seed = seed_type = ''
        seed_multi = bcdelay = 0

        if x['type']=='ZB':
            seed       = x['seed']
            seed_multi = x['seed_multi']
            bcdelay    = x['bcdelay']
            
            seed_type  = [t for t in reader.getL1Thresholds() if t['name']==seed][0]['type']
            
        if x['type']=='TOPO' or x['type']=='ALFA':
            ca = x.Cable
            si = ca.Signal
            firstbit = int(si['range_begin'])
            lastbit = int(si['range_end'])

            thr = LVL1TopoInput( thresholdName = x['name'], ttype = x['type'], mapping = int(x['mapping']), connector = ca['connector'],
                                 firstbit = firstbit, numberOfBits = lastbit - firstbit + 1, clock = int(si['clock']))
        else:
            thr = LVL1Threshold( name=x['name'], ttype=x['type'], mapping = x['mapping'], active = x['active'],
                                 seed_type = seed_type, seed = seed, seed_multi = seed_multi, bcdelay = bcdelay)


        ca = x.Cable
        thr.setCableInput()
        #print "x[bitnum]", x['bitnum']
        #print "type x[bitnum]", type(x['bitnum'])
        #print "x['type']", x['type']


        # overwrite cable info with data from xml file
        si = ca.Signal
        thr.cableinfo.bitnum      = int(x['bitnum'])
        thr.cableinfo.name        = ca['name']
        thr.cableinfo.slot        = ca['input'] if 'input' in ca else ca['ctpin']
        thr.cableinfo.connector   = ca['connector']
        thr.cableinfo.range_begin = int( si['range_begin'] )
        thr.cableinfo.range_end   = int( si['range_end'] )

        if hasattr(x,'TriggerThresholdValues'):
            for xV in x.TriggerThresholdValues:
                try:
                    value = int(xV['thresholdval'])
                except:
                    value = float(xV['thresholdval'])

                thrVal = ThresholdValue(thrtype = xV['type'], value = value,
                                        etamin = int(xV['etamin']), etamax = int(xV['etamax']), phimin = int(xV['phimin']), phimax = int(xV['phimax']),
                                        window = int(xV['window']), priority = int(xV['priority']), name = xV['name'])

                if xV['type']=='EM' or xV['type']=='TAU':
                    em_isolation = int(xV['em_isolation'])
                    had_isolation = int(xV['had_isolation'])
                    had_veto = int(xV['had_veto'])
                    isobits = xV['isobits'] if 'isobits' in xV else '00000'
                    use_relIso = xV['use_relIso'] if 'use_relIso' in xV else True
                    thrVal.setIsolation(em_isolation, had_isolation, had_veto, isobits, use_relIso)
                
                thr.thresholdValues.append(thrVal)

        l1menu.thresholds.thresholds += [ thr ]

    # for faster lookup, needed when building the items
    thresholdMap = {}
    for thr in l1menu.thresholds:
        thresholdMap[thr.name] = thr

    # Items
    for itemelem in reader.getL1Items():
        ctpid            = int(itemelem['ctpid'])
        partition        = int(itemelem['partition'])
        complex_deadtime = int(itemelem['complex_deadtime'] if 'complex_deadtime' in itemelem else prioMap[ctpid])
        psCut            = psMap[ctpid]
        triggerType      = int( itemelem['trigger_type'], 2 )

        item = LVL1MenuItem( itemelem['name'], ctpid = ctpid, complex_deadtime = complex_deadtime, psCut = psCut).setTriggerType( triggerType )
        item.partition = partition
        
        # read the thresholds for each item
        if hasattr(itemelem,'AND'):
            topLevelLogic = readLogic( itemelem.AND.element, thresholdMap )
        else:
            # L1_BGRP0 has no AND around, it is a single InternalTrigger
            topLevelLogic = readLogic( itemelem.InternalTrigger.element, thresholdMap )
        item.setLogic(topLevelLogic)

        if 'monitor' in itemelem and itemelem['monitor'].startswith('LF'):
            lf,hf = [ int(m[-3:],2) for m in itemelem['monitor'].split('|') ]
            from TriggerMenu.l1menu.MonitorDef import MonitorDef
            item.addMonitor(lf, MonitorDef.LOW_FREQ)
            item.addMonitor(hf, MonitorDef.HIGH_FREQ)

        l1menu.addItem( item )



    # Bunch group set
    bgs = reader.LVL1Config.BunchGroupSet
    l1menu.CTPInfo.setBunchGroupSetName(bgs['name'])
    lastBGNumber = max([int(bg['internalNumber']) for bg in reader.LVL1Config.BunchGroupSet.BunchGroups])
    l1menu.CTPInfo.bunchGroupSet.resize(1+lastBGNumber)
    for bg in reader.LVL1Config.BunchGroupSet.BunchGroups:
        bunches = []
        if hasattr(bg,'Bunch'):
            for b in bg.Bunchs:
                bunches.append( int(b['bunchNumber']) )
        l1menu.CTPInfo.addBunchGroup( bg['name'], int(bg['internalNumber']), bunches )


    # Random
    rndm = reader.LVL1Config.Random
    if 'name' in rndm:
        # old style
        l1menu.CTPInfo.random.name  = rndm['name']
        l1menu.CTPInfo.random.rate1 = int( rndm['rate1'] )
        l1menu.CTPInfo.random.rate2 = int( rndm['rate2'] )
        l1menu.CTPInfo.random.seed1 = int( rndm['seed1'] )
        l1menu.CTPInfo.random.seed2 = int( rndm['seed2'] )
    else:
        # new style
        for i in range(4):
            l1menu.CTPInfo.random.names[i] = rndm['name%i' % i]
            l1menu.CTPInfo.random.cuts[i]  = int( rndm['cut%i' % i] )


    # Deadtime
    # has been removed

    # CaloInfo
    ci = reader.LVL1Config.CaloInfo
    l1menu.CaloInfo.name = ci['name']
    try:
        l1menu.CaloInfo.setGlobalEmScale(float(ci['global_em_scale']))
        l1menu.CaloInfo.setGlobalJetScale(float(ci['global_jet_scale']))
    except:
        pass # will resolve itself once we have the em and jet scales in all XML

    if hasattr(reader.LVL1Config.CaloInfo,"JetWeights"): # new CaloInfo have no more JetWeights
        # jet weights
        jw = [x[1] for x in sorted([ ( int(jw['num']),int(jw.strippedText())) for jw in reader.LVL1Config.CaloInfo.JetWeights])]
        l1menu.CaloInfo.setJetWeights(jw)



    for iso in reader.LVL1Config.CaloInfo.Isolations:
        isoGroupName = iso['thrtype']
        for p in iso.Parametrizations:
            l1menu.CaloInfo.isolation[isoGroupName].addIsolation( isobit=p['isobit'],
                                                                  offset=p['offset'],
                                                                  slope=p['slope'],
                                                                  mincut=p['mincut'],
                                                                  upperlimit=p['upperlimit'],
                                                                  etamin=p['etamin'],
                                                                  etamax=p['etamax'],
                                                                  priority=p['priority'])


    for mintobpt in reader.LVL1Config.CaloInfo.MinimumTOBPts:
        l1menu.CaloInfo.minTOBPt += [ MinimumTOBPt( thrtype = mintobpt['thrtype'],
                                                    ptmin = mintobpt['ptmin'],
                                                    window = mintobpt['window'] if 'window' in mintobpt else 0,
                                                    etamin  = mintobpt['etamin'],
                                                    etamax = mintobpt['etamax'],
                                                    priority = mintobpt['priority']) ]


    # Trigger counters for monitoring
    for triggercounter in reader.LVL1Config.TriggerCounterList.TriggerCounters:
        counterType = triggercounter['type']

        triggerCondition = triggercounter.TriggerCondition

        if counterType=='CTPIN':
            l1menu.counters.addCounter( Lvl1CtpinCounter(  triggerCondition['triggerthreshold'], triggerCondition['multi']) )
        elif counterType=='CTPMON':
            l1menu.counters.addCounter( Lvl1CtpmonCounter(  triggerCondition['triggerthreshold'], triggerCondition['multi']) )
        else:
            raise RuntimeError("Monitoring counter of unknown type '%s'" % counterType)

