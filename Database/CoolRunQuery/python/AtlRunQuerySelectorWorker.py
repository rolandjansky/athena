# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


#import the selectors
from __future__ import print_function
from CoolRunQuery.selector.AtlRunQuerySelectorDQ      import DQSelector
from CoolRunQuery.selector.AtlRunQuerySelectorTrigger import TrigKeySelector, TriggerSelector, L1TrigKeySelector, HLTTrigKeySelector, BGSKeySelector, RatesSelector # noqa: F401
from CoolRunQuery.selector.AtlRunQuerySelectorMisc    import BPMSelector, LArcondSelector, DatasetsSelector, DetectorSelector, FilenameSelector, PartitionSelector, ReadyForPhysicsSelector, DurationSelector, BFieldSelector
from CoolRunQuery.selector.AtlRunQuerySelectorEvents  import EventSelector
from CoolRunQuery.selector.AtlRunQuerySelectorStreams import StreamSelector
from CoolRunQuery.selector.AtlRunQuerySelectorLhcOlc  import LHCSelector, OLCLumiSelector, LuminositySelector, BeamspotSelector, OLCLBDataCondition, OLCFillParamsCondition # noqa: F401

from CoolRunQuery.AtlRunQueryRun                      import Run


class SelectorWorker:

    class SelDescr:
        def __init__(self,selector, priority, doesSelect, doesShow, executed):
            self.selector   = selector
            self.priority   = priority
            self.doesSelect = doesSelect
            self.doesShow   = doesShow
            self.executed   = executed


    __selectors = []
    __executedSelectorsInOrder = []
    __showSelectorsInOrder = []
    __selectorDependencies = { 'trigger':['trigkey','l1trigkey','hlttrigkey'],
                               'dataquality':['readyforphysics']
                               }

    __creationRules = {
        'dataquality'     : 'DQSelector',
        'streams'         : 'StreamSelector',
        'trigger'         : 'TriggerSelector',
        'datasets'        : 'DatasetsSelector',
        'rates'           : 'RatesSelector',
        'lhc'             : 'LHCSelector',
        'bfield'          : 'BFieldSelector',
        'events'          : 'EventSelector',
        'allevents'       : 'AllEventsSelector',
        'release'         : 'TrigKeySelector',
        'trigkey'         : 'TrigKeySelector',
        'l1trigkey'       : 'L1TrigKeySelector',
        'hlttrigkey'      : 'HLTTrigKeySelector',
        'bgskey'          : 'BGSKeySelector',
        'detmask'         : 'DetectorSelector',
        'olclumi'         : 'OLCLumiSelector',
        'olcfillparams'   : 'OLCFillParamsCondition',
        'olclbdata'       : 'OLCLBDataCondition',
        'datasets'        : 'DatasetsSelector',
        'partition'       : 'PartitionSelector',
        'readyforphysics' : 'ReadyForPhysicsSelector',
        'larcond'         : 'LArcondSelector',
        'luminosity'      : 'LuminositySelector',
        'beamspot'        : 'BeamspotSelector',
        'bpm'             : 'BPMSelector',
        'filenametag'     : 'FilenameSelector'
        }


    @classmethod
    def selectors(cls):
        for s in SelectorWorker.getOrderedSelectorList():
            if s.executed:
                continue
            needs = cls.getOrderedNeeds(s)
            for ss in needs:
                if ss.executed:
                    continue
                yield ss.selector
                cls.__executedSelectorsInOrder += [ss]
                ss.executed=True
            yield s.selector
            cls.__executedSelectorsInOrder += [s]
            s.executed=True

    @classmethod
    def executedSelectors(cls):
        for s in cls.__executedSelectorsInOrder:
            yield s.selector

    @classmethod
    def getOrderedNeeds(cls, client):
        needs = []
        selname = client.selector.name
        if selname in cls.__selectorDependencies:
            dependson = cls.__selectorDependencies[selname]
            for d in dependson:
                dep = cls.findSelectorDescriptor(d)
                if dep is None:
                    raise RuntimeError('Selector %s depends on %s which is not defined' % (selname,d))
                needs.insert(0, dep)
        return needs

    
    @classmethod
    def addSelector(cls, selector, priority):
        s = cls.findSelectorDescriptor(selector.name)
        if s:
            return s.selector
        cls.__selectors += [cls.SelDescr(selector=selector,priority=priority,doesSelect=True,doesShow=False,executed=False)]
        return cls.__selectors[-1].selector


    # pure selector search by class name and instance name
    @classmethod
    def findSelectorDescriptor(cls, selname, selcls=None):
        """ returns a SelDescr if name and class match"""
        for s in cls.__selectors:
            if s.selector.name == selname and (selcls is None or selcls==s.selector.__class__.__name__):
                return s
        return None

    @classmethod
    def getShowSelector(cls, selname, *args, **kwargs):
        selcls = SelectorWorker.__creationRules[selname]
        # see if the selector is already there
        s = cls.findSelectorDescriptor(selname, selcls)
        if s:
            s.doesShow=True
            cls.__showSelectorsInOrder += [s]
            try:
                s.selector.addShowSelector(*args, **kwargs)
            except Exception:
                pass
        else:
            s = cls.CreateSelector( selname, True, *args, **kwargs)
            cls.__showSelectorsInOrder += [s]
            cls.__selectors += [s]
        return s.selector

    @classmethod
    def CreateSelector( cls, selname, doesShow, *args, **kwargs):
        selcls = SelectorWorker.__creationRules[selname]
        thecls = eval('%s' % selcls)
        newsel = thecls(name=selname, *args, **kwargs)  # noqa: F821
        # print ("CREATING SELECTOR %s %s('%s')" % (("SHOW" if doesShow else "RETRIEVE"), selcls, selname))
        s = cls.SelDescr(selector=newsel, priority=0, doesSelect=False, doesShow=True, executed=False)
        return s

    
    @classmethod
    def getRetrieveSelector(cls, selname, selcls=None, *args, **kwargs):
        s = cls.findSelectorDescriptor(selname, selcls)
        if s:
            try:
                s.selector.addShowSelector()
            except Exception:
                pass
            return s.selector
        elif selcls is not None:
            s = cls.CreateSelector( selname, False, *args, **kwargs)
            cls.__selectors += [s]
            return s.selector
        else:
            return None
    

    @classmethod
    def getOrderedSelectorList(cls):
        # Highes priority first
        sortedSel = cls.__selectors
        sortedSel.sort(key = lambda x: x.priority, reverse=True)
        return sortedSel

    @classmethod
    def setApplySelection(cls):
        for s in cls.__selectors:
            s.selector.applySelection = s.doesSelect

    @classmethod
    def setShowOutput(cls):
        for s in cls.__showSelectorsInOrder:
            s.selector.setShowOutput()

        
    @classmethod
    def parseSelectorOptions(cls,options):

        if options.events:
            cls.addSelector( EventSelector(name = 'events', events = options.events), 280 )

        if options.larcond:
            cls.addSelector( LArcondSelector(name = 'larcond', larcond = options.larcond), 115 )

        if options.datasets:
            cls.addSelector( DatasetsSelector(name = 'datasets', datasets = options.datasets), 110 )

        if options.duration:
            cls.addSelector( DurationSelector(name = 'duration', duration = options.duration), 95 )

        if options.streams:
            cls.addSelector( StreamSelector(name = 'streams', streams = options.streams), 95 )

        if options.smklist:
            cls.addSelector( TrigKeySelector(name = 'trigkey'), 71 ).setSelectSMK(smks = options.smklist)

        if options.release:
            cls.addSelector( TrigKeySelector(name = 'trigkey'), 70 ).setSelectRelease(release = options.release)

        if options.detmaskin or options.detmaskout:
            cls.addSelector( DetectorSelector('detmask', options.detmaskin, options.detmaskout), 60 )

        if options.lhc:
            cls.addSelector( LHCSelector(name = 'lhc', lhc = options.lhc), 48 )

        if options.beamspot:
            cls.addSelector( BeamspotSelector(name = 'beamspot', beamspot = options.beamspot), 44 )

        if options.olclumi:
            cls.addSelector( OLCLumiSelector(name = 'olclumi', olclumi = options.olclumi), 43 )

        if options.bpm:
            cls.addSelector( BPMSelector(name = 'bpm', release = options.bpm), 40 )

        if options.luminosity:
            cls.addSelector( LuminositySelector(name = 'luminosity', luminosity = options.luminosity), 40 )

        if options.projecttag:
            cls.addSelector( FilenameSelector(name = 'filenametag', projecttag = options.projecttag), 40 )

        if options.readyforphysics:
            cls.addSelector( ReadyForPhysicsSelector(name = 'readyforphysics', readyforphysics = options.readyforphysics), 31 )

        if options.partition:
            cls.addSelector( PartitionSelector(name = 'partition', partition = options.partition), 300 )
        
        if options.trigger:
            cls.addSelector( TriggerSelector(name = 'trigger', trigger = options.trigger), 25 )

        if options.bfield:
            cls.addSelector( BFieldSelector(name = 'bfield', bf = options.bfield), 10 )

        if options.dqchannels:
            cls.addSelector( DQSelector(), 50 ).addSelectionChannel(options.dqchannels)




    @classmethod
    def parseShowOption(cls,options):
        if not options.show:
            return

        for s in options.show:

            if s == 'summary' or s == 'dqeff' or s == 'dqsummary' or s == 'dqplots'  or s == 'cosmics'  or s == 'heavyions':
                pass # handled outside

            elif s=='run':
                Run.showrunnr = True

            elif s=='time':
                Run.showtime = True

            elif s=='duration':
                Run.showduration = True

            elif s[0:3]=='dq ' or s=='dq':
                SelectorWorker.getShowSelector('dataquality').addShowChannel(s[3:])

            elif s[0:7] == 'streams':
                SelectorWorker.getShowSelector('streams').addShowStreamPattern(s[8:])

            elif s[0:7] == 'trigger':
                SelectorWorker.getShowSelector('trigger').addShowTriggerPattern(s[8:])

            elif s[0:8] == 'datasets':
                SelectorWorker.getShowSelector('datasets').addShowDatasetPattern(s[9:])

            elif s[0:9] == 'trigrates':
                SelectorWorker.getShowSelector('rates').addPattern(s[10:])

            elif s[0:3] == 'lhc':
                arg = ''
                if   'all' in s:
                    arg = 'all'
                elif 'min' in s:
                    arg = 'min'
                SelectorWorker.getShowSelector('lhc',addArg=arg)

            elif 'olclumi' in s:
                SelectorWorker.getShowSelector(s)

            elif s == 'smk':
                SelectorWorker.getShowSelector('trigkey').setShow('smk')

            elif s == 'trigkeys' or s == 'smk':
                SelectorWorker.getShowSelector('trigkey').setShow('smk')
                SelectorWorker.getShowSelector('l1trigkey')
                SelectorWorker.getShowSelector('hlttrigkey')
                SelectorWorker.getShowSelector('bgskey')

            elif s == 'release':
                SelectorWorker.getShowSelector('trigkey').setShow('release')

            elif s[0:10] == 'luminosity':
                SelectorWorker.getShowSelector('luminosity').addShowTag(s.split(None,1)[1])

            elif s[0:8] == 'beamspot':
                SelectorWorker.getShowSelector('beamspot',args=s[8:])

            elif s[0:3] == 'bpm':
                SelectorWorker.getShowSelector('bpm')

            elif s == 'filenametag' or s == 'projecttag':
                SelectorWorker.getShowSelector('filenametag')

            elif s in ['bfield', 'events', 'allevents', 'detmask', 'olcfillparams', 'olclbdata', 'datasets', 'partition', 'readyforphysics', 'larcond']:
                SelectorWorker.getShowSelector(s)

            else:
                raise RuntimeError("ERROR: option 'show %s' not implemented" %s)

