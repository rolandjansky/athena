# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

"""Provides Trigger related selectors:

TriggerSelector      Trigger menu from TriggerDB
TrigKeySelector      SMK
BGSKeySelector       BGK
L1TrigKeySelector    L1PSK
HLTTrigKeySelector   HLTPSK
RatesSelector        L1 rates from COOL

"""

import re,sys

from time import time
from collections import defaultdict

from PyCool import cool

from CoolRunQuery.selector.AtlRunQuerySelectorBase import Selector, RunLBBasedCondition
from CoolRunQuery.utils.AtlRunQueryUtils import coolDbConn, GetRanges


class TrigKeySelector(RunLBBasedCondition):
    def __init__(self, name):
        self.showRelease  = False
        self.showTrigKeys = False
        self.smkCutRange  = None
        self.relCutRange  = None
        self.trigkeys     = None
        self.release      = None
        super(TrigKeySelector,self).__init__(name=name,
                                             dbfolderkey='COOLONL_TRIGGER::/TRIGGER/HLT/HltConfigKeys',
                                             channelKeys = [(0,'SMK','MasterConfigurationKey'),
                                                            (0,'Release','ConfigSource')])
        self.applySelection = False

    def setSelectSMK(self,smks):
        self.trigkeys = smks
        if self.trigkeys:
            self.smkCutRange = GetRanges(self.trigkeys)
            self.applySelection = True

    def setSelectRelease(self,release):
        self.release = release
        if self.release:
            self.relCutRange = GetRanges(','.join(self.release), intRepFnc=self.relStringToInt)
            self.applySelection = True

    def setShow(self, what):
        if what == 'smk':
            super(TrigKeySelector,self).setShowOutput('SMK')
            self.showTrigKeys = True
        if what == 'release':
            super(TrigKeySelector,self).setShowOutput('Release')
            self.showRelease = True

    def relStringToInt(self, r):
        major, minor, sub, patch = ([int(x) for x in r.split('.')] + [0,0,0,0])[0:4]
        if sub<10:
            sub*=10
        return 1000000*major+10000*minor+100*sub+patch
    
    def __str__(self):
        if self.applySelection:
            if self.trigkeys and self.release:
                return 'SELOUT Checking if SMK matches "%s" and release matches "%s"' % (self.trigkeys,self.release)
            elif self.trigkeys:
                return 'SELOUT Checking if SMK matches "%s"' % self.trigkeys
            else:
                return 'SELOUT Checking if release matches "%s"' % (self.release)
        else:
            if self.showTrigKeys and self.showRelease:
                return "Retrieving SMK and release version"
            elif self.showTrigKeys:
                return "Retrieving SMK"
            else:
                return "Retrieving release version"

    def passes(self,values,key):
        if self.smkCutRange:
            smkPass = False
            try:
                val = int(values)
            except ValueError:
                self.selDataMissing = True
                smkPass = True
            else:
                for cr in self.smkCutRange:
                    if val>=cr[0] and val<=cr[1]:
                        smkPass= True
            if not smkPass:
                return False
        if self.relCutRange:
            relPass = False
            val = values[1]
            if val=='n.a.':
                self.selDataMissing = True
                relPass = True
            else:
                val = val.split(',')
                if len(val)>1:
                    val = self.relStringToInt(val[1])
                    for cr in self.relCutRange:
                        if val>=cr[0] and val<=cr[1]:
                            relPass= True
                else:
                    self.selDataMissing = True
                    relPass = True
            if not relPass:
                return False
        return True

    def prettyValue(self, value, key):
        if key=='Release' and value!="n.a." and ',' in value:
            return value.split(',')[1]
        return value

    def runAfterQuery(self,runlist):
        for k in self.ResultKey():
            if k != 'SMK':
                continue
            smks = set()
            for run in runlist:
                smk = run.result['SMK']
                if not str.isdigit(smk):
                    continue
                smks.add(int(smk))

            from CoolRunQuery.utils.AtlRunQueryTriggerUtils import getSmkNames, getRandom
            smknames = getSmkNames(smks)

            for run in runlist:
                smk = run.result['SMK']
                if str.isdigit(smk) and int(smk) in smknames:
                    info = list(smknames[int(smk)])
                else:
                    info = ["unknown",0,"no comment"]
                if info[2]=="" or info[2]=="~":
                    info[2]="no comment"
                run.stats[k] = { "info" : tuple(info),
                                 "random" : getRandom(int(smk)) if str.isdigit(smk) else (0,0,0,0)}


class BGSKeySelector(RunLBBasedCondition):
    def __init__(self, name):
        self.showBGKeys = False
        super(BGSKeySelector,self).__init__(name=name,
                                            dbfolderkey='COOLONL_TRIGGER::/TRIGGER/LVL1/BunchGroupKey',
                                            channelKeys = [(0,'BGS Key','Lvl1BunchGroupConfigurationKey')])
    def __str__(self):
        return "Retrieving Bunch group set key"

    def passes(self,values,key):
        return True

    def prettyValue(self, value, key):
        if not str.isdigit(value):
            return None
        return int(value)

    def runAfterQuery(self,runlist):
        for k in self.ResultKey():
            for run in runlist:
                run.stats[k] = { "blocks" : [], "first" : 0 }
                entries = run.data[k]
                if len(entries)==0:
                    continue
                blocks = []
                for entry in entries:
                    v = entry.value
                    if len(blocks) > 0 and blocks[-1][0]==v and blocks[-1][2]==entry.startlb:
                        blocks[-1][2] = entry.endlb
                    else:
                        blocks += [ [v, entry.startlb, entry.endlb] ]
                run.stats[k] = { "blocks" : blocks, "first" : entries[0].value }


class L1TrigKeySelector(RunLBBasedCondition):
    def __init__(self, name):
        self.showTrigKeys = False
        super(L1TrigKeySelector,self).__init__(name=name,
                                               dbfolderkey='COOLONL_TRIGGER::/TRIGGER/LVL1/Lvl1ConfigKey',
                                               channelKeys = [(0,'L1 PSK','Lvl1PrescaleConfigurationKey')])


    def __str__(self):
        return "Retrieving L1 PSK"

    def passes(self,values,key):
        return True

    def prettyValue(self, value, key):
        return int(value) if str.isdigit(value) else None

    def runAfterQuery(self,runlist):
        for k in self.ResultKey():
            for run in runlist:
                run.stats[k] = { "blocks" : [], "first" : 0 }
                entries = run.data[k]
                if len(entries)==0:
                    continue
                blocks = []
                for entry in entries:
                    v = entry.value
                    if v == "n.a.":
                        v = None
                    if len(blocks) > 0 and blocks[-1][0]==v and blocks[-1][2]==entry.startlb:
                        blocks[-1][2] = entry.endlb
                    else:
                        blocks += [ [v, entry.startlb, entry.endlb] ]
                run.stats[k] = { "blocks" : blocks, "first" : entries[0].value }
        HLTTrigKeySelector.combineHltL1Keys(runlist)



        
class HLTTrigKeySelector(RunLBBasedCondition):
    def __init__(self, name):
        self.showTrigKeys = False
        if Selector.condDB() == 'OFLP200':
            super(HLTTrigKeySelector,self).__init__(name=name,
                                                    dbfolderkey='COOLONL_TRIGGER::/TRIGGER/HLT/HltConfigKeys',
                                                    channelKeys = [(0,'HLT PSK','HltPrescaleConfigurationKey')])
        else:
            super(HLTTrigKeySelector,self).__init__(name=name,
                                                    dbfolderkey='COOLONL_TRIGGER::/TRIGGER/HLT/PrescaleKey',
                                                    channelKeys = [(0,'HLT PSK','HltPrescaleKey')])

    def __str__(self):
        return "Retrieving HLT PSK"

    def passes(self,values,key):
        return True

    def prettyValue(self, value, key):
        if not str.isdigit(value):
            return None
        return int(value)

    def runAfterQuery(self,runlist):
        for k in self.ResultKey():
            for run in runlist:
                blocks = []
                run.stats[k] = {}
                for entry in run.data[k]:
                    v = entry.value
                    if v == "n.a.":
                        v = None
                    if len(blocks) > 0 and blocks[-1][0]==v and blocks[-1][2]==entry.startlb:
                        blocks[-1][2] = entry.endlb
                    else:
                        blocks += [ [v, entry.startlb, entry.endlb] ]
                run.stats[k] = { "blocks" : blocks, "first" : run.data[k][0].value }

        HLTTrigKeySelector.combineHltL1Keys(runlist)




    @classmethod
    def combineHltL1Keys(cls, runlist):
        from CoolRunQuery.utils.AtlRunQueryTriggerUtils import isTriggerRun2
        l1keysRun1 = set() # to collect run 1 keys
        hltkeysRun1 = set()
        l1keysRun2 = set() # to collect run 2 keys
        hltkeysRun2 = set()
        for run in runlist:
            if isTriggerRun2(run_number = run.runNr): # run 2
                if 'L1 PSK' in run.stats:
                    l1keysRun2.update( [x[0] for x in run.stats['L1 PSK']['blocks']] )
                if 'HLT PSK' in run.stats:
                    hltkeysRun2.update( [x[0] for x in run.stats['HLT PSK']['blocks']] )
            else: # run 1
                if 'L1 PSK' in run.stats:
                    l1keysRun1.update( [x[0] for x in run.stats['L1 PSK']['blocks']] )
                if 'HLT PSK' in run.stats:
                    hltkeysRun1.update( [x[0] for x in run.stats['HLT PSK']['blocks']] )


        from CoolRunQuery.utils.AtlRunQueryTriggerUtils import getL1PskNames, getHLTPskNames

        l1namesRun1  = getL1PskNames(l1keysRun1, isRun2 = False)
        hltnamesRun1 = getHLTPskNames(hltkeysRun1, isRun2 = False)
        l1namesRun2  = getL1PskNames(l1keysRun2, isRun2 = True)
        hltnamesRun2 = getHLTPskNames(hltkeysRun2, isRun2 = True)

        for run in runlist:
            if isTriggerRun2(run_number = run.runNr): # run 2
                l1names  = l1namesRun2
                hltnames = hltnamesRun2
            else:
                l1names  = l1namesRun1
                hltnames = hltnamesRun1
            if not ('L1 PSK' in run.stats and 'HLT PSK' in run.stats):
                continue
            ic = 0
            blocks = set()
            for l1key, l1beg, l1end in run.stats['L1 PSK' ]['blocks']:
                for hltkey, hltbeg, hltend in run.stats['HLT PSK' ]['blocks']:
                    lbmin = max(ic,min(l1beg,hltbeg))
                    lbmax = min(l1end-1,hltend-1)
                    if lbmin > lbmax or lbmin == 0 or lbmax == 0:
                        continue
                    ic = lbmax + 1
                    # accept if new
                    l1name = l1names[l1key] if l1key else ""
                    hltname = hltnames[hltkey] if hltkey else ""
                    elm = (lbmin, lbmax, l1key, hltkey, l1name, hltname)
                    blocks.update([elm])
            run.stats['PSK' ] = {'blocks' : sorted(list(blocks)) }
            


class RatesSelector(RunLBBasedCondition):
    def __init__(self, name, trigger=[]):
        # trigger can be ["*Electron*", "L1_*"]
        # rate will be printed for all triggers matching the pattern

        super(RatesSelector,self).__init__(name=name,
                                           dbfolderkey='COOLONL_TRIGGER::/TRIGGER/LUMI/LVL1COUNTERS',
                                           channelKeys = [(-1,'TriggerRates','TriggerName')])

        self.trigger=trigger
        from CoolRunQuery.AtlRunQuerySelectorWorker import SelectorWorker
        self.trigSel = SelectorWorker.getRetrieveSelector('trigger','TriggerSelector')
        if len(trigger)>0:
            self.trigSel.addShowTriggerPattern(','.join(trigger))

    def addPattern(self,pattern):
        if pattern=="" or pattern=="L1":
            pattern="L1_EM5,L1_TAU5,L1_XE10,L1_J5,L1_MBTS_4_4,L1_MU6"
        self.trigSel.addShowTriggerPattern(pattern)
        self.trigger += pattern.split(',')


    def __str__(self):
        return "Retrieving trigger rates for %r" % ','.join(self.trigger)

    def select(self, runlist):
        start = time()
        print (self, end='')
        sys.stdout.flush()

        fL1R = coolDbConn.GetDBConn(self.schema,db=Selector.condDB()).getFolder(self.folder)

        for r in runlist:
            menu = r.result['TriggerMenu']

            namelookup = 256*['']
            channellist = []
            for tr in menu:
                if not tr.name.startswith("L1_"):
                    continue
                ch = tr.counter
                namelookup[ch] = tr.name
                channellist.append(ch)

            if len(channellist)>50:
                r.addResult(self.ResultKey()[0], "Too many L1 triggers requested (%i), maximum is 50" % len(channellist))
                continue

            if len(channellist)==0:
                r.addResult(self.ResultKey()[0], "No match")
                continue

            channellist.sort()
            ch = channellist[0]
            chanselL1 = cool.ChannelSelection(ch,ch,cool.ChannelSelection.sinceBeforeChannel)
            for ch in channellist[1:]:
                chanselL1.addChannel(ch)

            iovmin=(r.runNr << 32)+0
            iovmax=((r.runNr+1) << 32)-1

            countsholder = defaultdict(list)

            # the hlt part
            objs = fL1R.browseObjects( iovmin, iovmax, chanselL1)
            while objs.goToNext():
                obj=objs.currentRef()
                ch  = obj.channelId()
                countsholder[namelookup[ch]].append((obj.since()&0xFFFFFFFF,
                                                     obj.payloadValue('BeforePrescale'),
                                                     obj.payloadValue('AfterPrescale'),
                                                     obj.payloadValue('L1Accept')))

            r.addResult(self.ResultKey()[0], countsholder)

        duration = time() - start

        if self.applySelection:
            print (" ==> %i runs found (%.2f sec)" % (len(runlist),duration))
        else:
            print (" ==> Done (%g sec)" % duration)

        return runlist

class TriggerSelector(RunLBBasedCondition):

    def __init__(self, name, trigger=[]):
        # trigger can be ["*Electron*", "L1_*"]
        #    will select runs that satiesfy an 'or' of all patterns in the list
        # a pattern without number are checked for existence
        # a pattern with number will be checked against the sum of the events in all matching triggers
        from CoolRunQuery.AtlRunQuerySelectorWorker import SelectorWorker
        SelectorWorker.getRetrieveSelector('trigkey','TrigKeySelector')
        SelectorWorker.getRetrieveSelector('l1trigkey','L1TrigKeySelector')
        SelectorWorker.getRetrieveSelector('hlttrigkey','HLTTrigKeySelector')

        super(TriggerSelector,self).__init__(name=name,
                                             dbfolderkey='COOLONL_TRIGGER::/TRIGGER/HLT/Menu',
                                             channelKeys = [(-1,'TriggerMenu','ChainName')])

        self.triggers = ' or '.join(trigger)
        self.applySelection = (len(trigger)>0)
        self.seltriggerpatterns = []
        self.showtriggerpatterns = []

        for s in trigger:
            s += "$"
            p = re.compile(s.split()[0].replace(',','|').replace('*','.*').replace('?','.').replace('%','.*'), re.I)
            self.seltriggerpatterns += [p]
        

    def addShowTriggerPattern(self, triggerpattern):
        if triggerpattern=="":
            triggerpattern = "*"
        self.showtriggerpatterns += triggerpattern.split(',')

    def __str__(self):
        if self.applySelection:
            return 'SELOUT Checking if the trigger name matches "%s"' % self.triggers
        else:
            return "Retrieving trigger names [%s]" % ','.join(self.showtriggerpatterns)


    def getL1Prescales(self, runl1psks):
        """
        runl1psks is a dict run -> [l1psk,...]
        """
        from CoolRunQuery.utils.AtlRunQueryTriggerUtils import getL1Prescales
        l1pscache = dict()
        for run_number, pss in runl1psks.items():
            for l1psk in pss:
                l1pscache[(run_number,l1psk)] = getL1Prescales(l1psk,run_number = run_number)
        return l1pscache

    def getHLTPrescales(self, runhltpsks):
        """
        runhltpsks is a dict run -> [hltpsk,...]
        """
        from CoolRunQuery.utils.AtlRunQueryTriggerUtils import getHLTPrescales
        l2pscache   = dict()
        efpscache   = dict()
        for run_number, pss in runhltpsks.items():
            for psk in pss:
                l2pscache[(run_number,psk)], efpscache[(run_number,psk)] = getHLTPrescales(psk, run_number = run_number)

        return l2pscache, efpscache


    def getMenuFromRun(self, smks):
        from CoolRunQuery.utils.AtlRunQueryTriggerUtils import getL1Menu, getHLTMenu

        l1menucache  = dict()
        l2menucache  = defaultdict(dict)
        efmenucache  = defaultdict(dict)

        # fill the menu and prescale caches
        for smk in smks:

            # the l1 menu
            l1items = getL1Menu(smk)
            l1menucache[smk] = l1items

            # the hlt menu
            l2chains, efchains = getHLTMenu(smk)
            d2 = l2menucache[smk] = dict([(c.counter,c) for c in l2chains])

            # connect the levels (assign lower counter)
            for chain in efchains:
                if chain.lowername=="":
                    continue
                cc = chain.counter
                if (cc in d2) and (chain.lowername == d2[cc].name):
                    chain.lower = d2[cc]
                    chain.lowercounter = cc
                    continue
                for l2ch in l2chains:
                    if chain.lowername == l2ch.name:
                        chain.lower = l2ch
                        chain.lowercounter = l2ch.counter
                        break

            for chain in l2chains:
                if chain.lowername == "":
                    continue
                for l1item in l1items:
                    if l1item and chain.lowername == l1item.name:
                        chain.lower = l1item
                        chain.lowercounter = l1item.counter
                        break

            # set display flag
            for item in l1items + l2chains + efchains:
                if item:
                    item.forshow = self.matchShowPattern(item.name)

            # set 'selected' flag
            if self.applySelection:
                for item in l1items + l2chains + efchains:
                    if item:
                        item.forselect = self.matchSelectPattern(item.name)

        return l1menucache, l2menucache, efmenucache
    

    def select(self, runlist):
        start = time()
        # some preparation: compile the show patterns
        self.compiledShowPatterns = []
        if '*' in self.showtriggerpatterns:
            self.compiledShowPatterns = ['all']
        else:
            for p in self.showtriggerpatterns:
                p += "$"
                self.compiledShowPatterns += [re.compile(p.replace('*','.*').replace('?','.').replace('%','.*'),re.I)]

        print (self, end='')
        sys.stdout.flush()
        newrunlist = []

        smks = [r.result['SMK'] for r in runlist]
        runl1psks = dict()
        runhltpsks = dict()
        for r in runlist:
            runl1psks[r.runNr]  = [l1psk for (l1psk,firstlb,lastlb) in r.stats['L1 PSK']['blocks']]
            runhltpsks[r.runNr] = [hltpsk for (hltpsk,firstlb,lastlb) in r.stats['HLT PSK']['blocks']]

        l1menucache, l2menucache, efmenucache = self.getMenuFromRun(smks)

        l1pscache = self.getL1Prescales(runl1psks)

        l2pscache, efpscache = self.getHLTPrescales(runhltpsks) # for RUN 2 only efpscache is filled, l2pscache is None

        for run in runlist: # go through old runlist and see

            smk  = run.result['SMK']
            psks = [(x[2],x[3]) for x in run.stats['PSK']['blocks']]

            l1items  = [it for it in l1menucache[smk] if it and (it.forshow or it.forselect)]
            l2chains = [ch for ch in l2menucache[smk].values() if (ch.forshow or ch.forselect) and not ch.multiseeded]
            efchains = [ch for ch in efmenucache[smk].values() if (ch.forshow or ch.forselect) and not ch.multiseeded and ch.lower and not ch.lower.multiseeded]

            value = defaultdict(list)
            
            for item in l1items:
                for l1psk,hltpsk in psks:
                    if not l1psk:
                        value[item].append(None)
                        continue
                    itemps = l1pscache[(run.runNr,l1psk)][item.counter]
                    value[item].append(itemps if itemps>=0 else -1)

            for l2chain in l2chains:
                for l1psk,hltpsk in psks:
                    if not l1psk or not hltpsk:
                        value[l2chain].append(None)
                        continue
                    chainps,chainpt = l2pscache[(smk,hltpsk)][l2chain.counter]
                    if chainps<0:
                        value[l2chain].append(-1)
                        continue
                    chainps *= l1pscache[(run.runNr,l1psk)][l2chain.lowercounter]
                    value[l2chain].append(chainps if chainps>=0 else -1)

            for efchain in efchains:
                l2chain = efchain.lower
                for l1psk,hltpsk in psks:
                    if not l1psk or not hltpsk:
                        value[efchain].append(None)
                        continue
                    chainps,chainpt = efpscache[(smk,hltpsk)][efchain.counter]
                    if chainps<0:
                        value[efchain].append(-1)
                        continue
                    l2chainps,l2chainpt = l2pscache[(smk,hltpsk)][l2chain.counter]
                    chainps *= l2chainps
                    if chainps<0:
                        value[efchain].append(-1)
                        continue
                    chainps *= l1pscache[(run.runNr,l1psk)][l2chain.lowercounter]
                    value[efchain].append(chainps if chainps>=0 else -1)

                # remove all the disabled triggers
                for tr,pslist in value.items():
                    if not any([ps is None or ps>=0 for ps in pslist]):
                        value.pop(tr)

            if self.applySelection and not self.passes(value):
                continue
            newrunlist += [run.runNr]

            run.addResult(self.ResultKey()[0], value)

        runlist = [r for r in runlist if r.runNr in newrunlist]

        duration = time() - start

        if self.applySelection:
            print (" ==> %i runs found (%.2f sec)" % (len(runlist),duration))
        else:
            print (" ==> Done (%g sec)" % duration)

        return runlist

    def matchShowPattern(self,name):
        if len(self.compiledShowPatterns)==0 or self.compiledShowPatterns[0]=='all':
            return True
        else:
            return any([p.match(name) for p in self.compiledShowPatterns])
        
        
    def matchSelectPattern(self,name):
        return any([p.match(name) for p in self.seltriggerpatterns])

    def prettyValue(self, value):
        form = []
        for v in value:
            if len(v)==2:
                form += ["%s (%s)" % (v[0],','.join(v[1]))]
            else:
                form += ["%s (%s|%s)" % v]
        return ', '.join(form)


    def passes(self,triggers):
        for chain in triggers:
            if chain.forselect:
                return True
        return False
