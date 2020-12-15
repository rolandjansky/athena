#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryRun.py
# Project: AtlRunQuery
# Purpose: Library with the Run class
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Feb 10, 2009
# ----------------------------------------------------------------
#
from __future__ import with_statement, print_function
from CoolRunQuery.utils.AtlRunQueryTimer import timer

from CoolRunQuery.utils.AtlRunQueryUtils           import addKommaToNumber, filesize, prettyNumber, RunPeriods
from CoolRunQuery.utils.AtlRunQueryLookup          import DecodeDetectorMask
from CoolRunQuery.utils.AtlRunQueryIOV             import IOVRange,IOVTime
from CoolRunQuery.utils.AtlRunQueryLookup          import LArConfig, isDQ, OLCAlgorithms
from CoolRunQuery.output.AtlRunQueryRoot           import makeLBPlot, makeLBPlotList, makeTimePlotList, makeBSPlots, SaveGraphsToFile
from CoolRunQuery.AtlRunQueryQueryConfig           import QC
from CoolRunQuery.selector.AtlRunQuerySelectorBase import DataKey, Selector

import math
import time, calendar
import sys
import re
import datetime
import subprocess
import urllib

_fW = {'Run' : 12, 'NLB': 5, 'Time': 50, '#Events': 10, 'Stream': 10}

class DataEntry:
    def __init__(self, iov, value, reject=False):
        self.iov = iov
        self.lbrange = (self.iov.startTime.lb, self.iov.endTime.lb) # note that the end lb is not part of the iov
        self.value = value
        self.rejected = reject

    def __str__(self):
        return "[%i-%i) %s %r" % (self.lbrange[0], self.lbrange[1], "r" if self.rejected else "a", self.value)

    def __repr__(self):
        return "<DataEntry %s>" % self

    def __len__(self):
        length_L = self.lastlb-self.startlb+1
        if length_L>0x7FFFFFFF:
            length_L = 0x7FFFFFFF
        return int(length_L)

    @property
    def startlb(self):
        return self.lbrange[0]

    @property
    def endlb(self):
        return self.lbrange[1]

    @property
    def lastlb(self):
        return ((0x100000000 + self.endlb)-1) & 0xFFFFFFFF

    def contains(self, lb):
        return self.lbrange[0]<=lb and lb<self.lbrange[1]

    def intersects(self,lbrange):
        lbstart, lbend = lbrange
        return self.contains(lbstart) or self.contains(lbend-1) or (lbstart<=self.lbrange[0] and self.lbrange[1]<lbend)

    def pickled(self):
        v = self.value.pickled() if hasattr(self.value,'pickled') else self.value
        return { 'firstlb' : self.startlb, 'lastlb' : self.lastlb, 'value' : v, 'accepted' : not self.rejected }


class DataEntryList(list):
    def __init__(self, key, run):
        super(DataEntryList,self).__init__()
        self.key = key
        self.run = run

    def atLB(self,lb):
        for e in self:
            return [x for x in self if x.contains(lb)]
        return None

    def stops(self):
        return set([e.startlb for e in self]).union(set([e.lastlb+1 for e in self]))

    @property
    def lastlb(self):
        return max([e.lastlb for e in self])

    @property
    def endlb(self):
        return max([e.endlb for e in self])

    def pickled(self):
        return [e.pickled() for e in self]
            

class RunData:

    # this is a temporary thing to get the DQDefects working, I still want to change the way the selectors work
    DQKeys = []
    DQLogic = []
    DefectSelector = None

    def __init__(self,run):
        self.__run_o  = run
        self.run  = run.runNr
        self.data = {}
        self.data_current_lb = {}
        self.current_lb = 0
        self.stops = []

    def __contains__(self,k):
        if type(k)==DataKey:
            return k in self.data or k.ResultKey in self.data
        return k in self.data

    def __getitem__(self,k):
        if k not in self:
            self.data[k] = DataEntryList(key=k, run=self.run)
            return self.data[k]
        else:
            if type(k)==DataKey:
                return self.data[k] if k in self.data else self.data[k.ResultKey]
            return self.data[k]

    def __setitem__(self,k,value):
        self.data[k]=value

    def __iter__(self):
        self.stops = self._calcStops()
        for lb in self.stops: # the last stop is the LB after the last
            if lb>self.__run_o.lastlb:
                break
            self._setLB(lb)
            yield self

    def keys(self):
        return self.data.keys()

    @property
    def runNrS(self):
        return str(self.run)

    @property
    def lb(self):
        return self.current_lb

    def lbend(self,lb=None):
        if lb is None:
            lb=self.current_lb
        return self.stops[self.stops.index(lb)+1]-1

    @property
    def NrLBs(self):
        return "%i" % max([el.lastlb for el in self.data.values()])

    @property
    def result(self):
        return self.data_current_lb

    @property
    def isRejectedCurrentLB(self):
        #print ("Checking Reject for LB",self.lb,"-",self.lbend())
        need_dq_check = False
        for k,s in self.data_current_lb.items():
            if k in RunData.DQKeys:
                need_dq_check = True
                continue # exclude DQ from this, since it gets its own treatment
            if s is None:
                continue
            for e in s:
                if e.rejected:
                    return True
        if need_dq_check:
            if self.isDQRejected():
                return True
        if self.isDQDefectRejected():
            return True
        return False

    def isDQRejected(self):
        for orGroup in RunData.DQLogic:
            groupAccept = False
            for k in orGroup:
                if len(self.data_current_lb[k])==0:
                    passes = False
                else:
                    passes = not self.data_current_lb[k][0].rejected
                if passes:
                    groupAccept = True
                    break
            if not groupAccept:
                return True
        return False

    def isDQDefectRejected(self):
        if 'DQ' not in self.data_current_lb:
            return False
        return not RunData.DefectSelector(self.data_current_lb['DQ'])


    @property
    def isRejected(self):
        for s in self:
            if not s.isRejectedCurrentLB:
                return False
        return True


    def isReady(self,lb=None):
        if 'Ready for physics' not in self:
            raise RuntimeError("No ready data available")
        readydata = [x for x in self.data['Ready for physics'] if x.value=='1']
        if lb is None:
            return readydata
        if type(lb)==int:
            for x in readydata:
                if x.contains(lb):
                    return True
            return False
        if type(lb)==tuple and len(lb)==2:
            for x in readydata:
                if x.intersects(lb):
                    return True
            return False
        raise RuntimeError("Can't interpret lb to check isReady(lb=%r)" % lb)
            
        
    def getLBRanges(self,activeOnly=False):
        last_lb = self.__run_o.nr_lb
        a = [(int(data.lb),not data.isRejectedCurrentLB) for data in self if data.lb<=last_lb]
        #a = [(int(data.lb),not data.isRejectedCurrentLB) for data in self]
        if not a:
            return []
        start,state = a[0]
        ranges = []
        for x in a[:-1]:
            if x[1] == state:
                continue
            ranges += [(state, start, x[0])]
            start,state = x
        last_start,last_state = a[-1]
        if last_state==state:
            ranges += [(state, start, last_lb+1)]
        else:
            ranges += [(state, start, last_start)]
            ranges += [(last_state, last_start, last_lb+1)]
        if activeOnly:
            return [x for x in ranges if x[0]]
        else:
            return ranges

    def _setLB(self,lb):
        self.current_lb = lb
        self.data_current_lb = dict([(k,entrylist.atLB(lb)) for k,entrylist in self.data.items()])

    def _calcStops(self):
        stops = set()
        for entrylist in self.data.values():
            stops.update(entrylist.stops())
        return sorted(list(stops))

    def addResult(self, iov, k, value, reject):
        if not iov:
            iov = IOVRange(runStart=self.run, lbStart=1, runEnd=self.run+1, lbEnd=0)
        self[k].append(DataEntry(iov=iov, value=value, reject=reject))

    def astxt(self):
        s = ""
        if Run.showrunnr:
            s += "%*s %*s  " % (4, self.lb, 4, self.lbend())
        s += ("r" if self.isRejectedCurrentLB else "a")
        if Run.showtime:
            s += "%*s  " % (_fW['Time'],'')
        if Run.showduration:
            s += "            "
        for k in Run.SortedShowOrder():
            k = k.ResultKey
            w = 10
            if k in _fW:
                w = _fW[k]
            v=""
            if self.data_current_lb[k]:
                v = [x.value for x in self.data_current_lb[k]][0]
                if v is None:
                    v=""
            if k == "#Events" or k[0:4] == "STR:" or k=="TriggerMenu" or k=="TriggerRates" or k=="olc:bcidmask":
                v = ""
            s += "%*s  " % (w,v)
        return s





class Run:
    ShowOrder = []
    _SortedShowOrder = None
    DisplayOrder = map(re.compile,['#Events','Ready','lhc:|olc:', 'TriggerMenu|SMK|Release|L1 PSK|HLT PSK|BGS|TriggerRates', 'bs:', 'STR:', 'DQ|SHIFTOFL:|LBSUMM:', 'lar:', 'Detector','.*'])    
    PromptCalibRuns = []
    NemoTasks = {}
    showrunnr = False
    showtime = False
    showduration = False
    writehtml = False
    bgcolor = "1"
    totevents = [0, 0]
    runnropen = -1
    showCAFLinks = False
    Datapath = ''
    Fieldinfo = {}
    GlobalTooltips = []
    BeamspotSource = ''
    runPeriods = RunPeriods()
    TmpBoxContent = ''

    @classmethod
    def AddToShowOrder(cls,key):
        #print ("ADDING TO SHOWORDER  %r" % key)
        if key not in cls.ShowOrder:
            cls.ShowOrder += [ key ]

    @classmethod
    def addGlobalToolTip(cls, var, content):
        cls.GlobalTooltips += ['dw_Tooltip.content_vars["%s"] = {content: \'%s\'};' % (var, content.replace("'","&#39;"))]
    
    def __init__(self, runNr=0):
        self.runNr = runNr
        self.sor = 0
        self.eor = 0
        self.lastlb = 0
        self.selDataIncomplete = False
        self.showDataIncomplete = False
        self.xvecStb = []
        self.hasStableBeamsInfo = False
        # the results of the query index by query key
        self.result = {}
        # the run statistics index by query key (filled in selector.runAfterQuery)
        self.stats = {}
        # the start times of all lumiblocks [start_LB1, start_LB2, start_LB3, ..., start_LBN]
        self.lbtimes = []
        self.tooltips = []
        self.data = RunData(run=self)
        # luminosity unil
        self.lumiunit    = 1.0 # default unit
        self.lumiunittxt = 'nb'
        self.instlumiunittxt = '30'
        self.givepeakmuinfo = True
        if self.runNr > 168206 and self.runNr <= 170482:
            self.lumiunit = 1.0e6 # unit is mbarn for heavy ions
            self.lumiunittxt = 'mb'
            self.instlumiunittxt = '24'
            self.givepeakmuinfo = False


    @property
    def runNrS(self):
        return "%i" % self.runNr

    @property
    def data_current_lb(self):
        return self.data.data_current_lb

    @property
    def NrLBs(self):
        return "%i" % self.lastlb

    @property
    def nr_lb(self):
        return self.lastlb

    def addToolTip(self, var, content):
        self.tooltips += ['dw_Tooltip.content_vars["%s\"] = {content: \'%s\'};' % (var, content.replace("'","&#39;"))]

    def getStableBeamsVector(self):
        if len(self.xvecStb)==0 and 'lhc:stablebeams' in Run.ShowOrder and 'lhc:stablebeams' in self.data:
            entries = self.data['lhc:stablebeams']
            for entry in entries:
                if entry.startlb == 0:
                    continue
                if 'true' in entry.value.lower():
                    self.xvecStb += range(entry.startlb,entry.endlb)
            self.hasStableBeamsInfo = len(self.xvecStb)>0
        return self.hasStableBeamsInfo, self.xvecStb

    @classmethod
    def SortedShowOrder(cls):
        if Run._SortedShowOrder is not None:
            if len(Run._SortedShowOrder) != len(Run.ShowOrder):
                raise RuntimeError ("Sorting not up-to-date %i %i" % (len(Run._SortedShowOrder), len(Run.ShowOrder) ))
            return Run._SortedShowOrder
        hk = []
        for order in Run.DisplayOrder:
            for x in Run.ShowOrder:
                if not order.match(x.ResultKey):
                    continue
                if x in hk:
                    continue
                hk += [x]
        Run._SortedShowOrder = hk
        if len(Run._SortedShowOrder) != len(Run.ShowOrder):
            raise RuntimeError ("Sorting not up-to-date after creation %i %i" % (len(Run._SortedShowOrder), len(Run.ShowOrder) ))
        return Run._SortedShowOrder
    
    @classmethod
    def headerkeys(cls):
        hk = []
        if Run.showrunnr:
            hk += ['Run','Links','#LB']
        if Run.showtime:
            hk += ['Start and endtime (%s)' % QC.localStr()]
        if Run.showduration:
            hk += ['Duration']
        for x in cls.SortedShowOrder():
            x = x.ResultKey
            if 'L1 PSK' in x or ('olc:' in x and ('beam1bunches' in x or 'beam2bunches' in x or 'collbunches' in x or 'beam1intensity' in x or 'beam2intensity' in x)):
                continue
            hk += [x]
        return hk


    @classmethod
    def header(cls):
        if Run.writehtml:
            # flags...
            ofllumiFlag = False
            s = '<tr>\n'
            if Run.showrunnr:
                s += '  <th>Run</th><th>Links</th><th>#LB</th>\n'
            if Run.showtime:
                s += '  <th>Start and endtime (%s)</th>\n' % QC.localStr()
            if Run.showduration:
                s += '  <th>Duration</th>\n'
            for ik, data_key in enumerate(Run.SortedShowOrder()):
                k = data_key.ResultKey
                if k[0:4] == 'STR:':
                    s += '  <th><font size="-2">%s_<BR>%s</font></th>' % tuple(k[4:].split('_',1))
                elif k[0:8] == "Detector":
                    s += '  <th>%s</th>' % k
                elif "SolCurrent" in k:
                    s += '  <th>Solenoid<br>current&nbsp;(A)</th>'
                elif "TorCurrent" in k:
                    s += '  <th>Toroid<br>current&nbsp;(A)</th>'
                elif 'DQ' == k:
                    s += '  '
                    matching_names = dict(Run.Fieldinfo['DQ']['DefMatch'])
                    first = True
                    for channelname in Run.Fieldinfo['DQ']['DefChannels']:
                        tip = "defect_match_%s" % channelname
                        info = "%s # %s" % (channelname if channelname!="" else "*",data_key._second_internal_key if data_key._second_internal_key!="" else "HEAD")
                        s += '<th style="align:center; white-space:nowrap; padding-right: 10px; padding-left: 10px; ">'
                        s += 'Data Quality<br><span class="showTip %s tooltip" style="font-weight:normal; font-size:80%%">(%s)</span>' % (tip, info)
                        if first:
                            s += '<div style="font-size: xx-small; cursor: pointer;" onclick="toggle_dq(this)">[show tolerable]</div>'
                            first = False
                        s += '</th>'
                        
                        a = sorted(matching_names[channelname])
                        #horizontal
                        #slices = [slice(x,len(a)+3,4) for x in range(4)]
                        #zipped = zip(*map((a+4*['']).__getitem__,slices))
                        #vertical
                        n = 4*[len(a)/4]
                        for x in range(len(a)-4*(len(a)/4)):
                            n[x]+=1

                        cx = 0
                        slices = []
                        for x in n:
                            slices += [slice(cx,cx+x)]
                            cx+=x
                        m = map(a.__getitem__,slices)
                        for x in range(1,len(m)):
                            m[x]+=['']
                        zipped = zip(*m)
                        
                        tts = ''
                        for z in zipped:
                            tts += '<tr>%s</tr>' % ''.join(["<td>%s</td>"% x for x in z])
                        content = '<table style="width:500px; font-size:80%%;">%s</table>' % tts
                        Run.addGlobalToolTip(tip,content)
                    
                elif isDQ(k):
                    foldertag,flag = k.split(':')

                    if flag in Run.Fieldinfo:
                        tipname = 'dqvfdesc_%s' % flag
                        commentbox = '%s' % (Run.Fieldinfo[flag])
                        Run.addGlobalToolTip(tipname, commentbox)
                        s += '<th class="showTip %s tooltip" style="cursor:pointer">' % tipname
                    else:
                        s += '  <th>'

                    s += '<font size="-1">%s</font><BR><font style="font-weight:normal;font-size:70%%">' % flag
                    if '#' in foldertag:
                        s += '(%s #<br>%s)' % tuple(foldertag.split('#',1))
                    else:
                        s += '(%s)' % (foldertag)
                    s += '</font></th>'
                    
                elif k == "L1 PSK":
                    continue
                elif k == "HLT PSK":
                    s += '  <th>Prescale keys</th>' 
                elif k == "Release":
                    s += '  <th>HLT Release</th>'
                elif k == "Datasets":
                    s += '  <th>Tier-0 Datasets</th>'
                elif k == '#Events (incl. calib.)':
                    s += '  <th>#Events<br><font size="-2">(incl.&nbsp;calib.)</font></th>'
                elif k == '#Events (streamed)'    :
                    s += '  <th>#Events<br><font size="-2">(streamed)</font></th>'
                elif 'lar:' in k:
                    if   'runtype'        in k:
                        s += '  <th><font size="-2">Run type</font></th>' 
                    elif 'nsamples'       in k:
                        s += '  <th><font size="-2">#Samples</font></th>' 
                    elif 'firstsample'    in k:
                        s += '  <th><font size="-2">1st sample</font></th>' 
                    elif 'format'         in k:
                        s += '  <th><font size="-2">Format</font></th>' 
                    else:
                        print ('ERROR: unknown LAr option "%s"' % k)
                        sys.exit(1)
                elif 'lhc:' in k:
                    if   'fillnumber'     in k:
                        s += '  <th>  LHC Fill</th>'
                    elif 'stablebeams'    in k:
                        s += '  <th>Stable beams</th>'
                        if 'lhc:beammode' in Run.ShowOrder:
                            s += '  <th>Beam mode</th>' 
                        if 'lhc:beamenergy' in Run.ShowOrder:
                            s += '  <th>Beam&nbsp;energy and&nbsp;intensities</th>'
                            
                    elif 'beamenergy'     in k:
                        continue # included in 'fillbeams' summary
                    elif 'nbunch1'        in k:
                        s += '  <th>#Bunches B1 <br><font size="-2"><b>(NOT YET RELIABLE)</b></font></th>' 
                    elif 'nbunch2'        in k:
                        s += '  <th>#Bunches B2 <br><font size="-2"><b>(NOT YET RELIABLE)</b></font></th>' 
                    elif 'nbunchcoll'     in k:
                        s += '  <th>#Bunches<br>colliding <br><font size="-2"><b>(NOT YET RELIABLE)</b></font></th>' 
                    elif 'beamtype1'      in k:
                        s += '  <th>Beam type&nbsp;B1</th>' 
                    elif 'beamtype2'      in k:
                        s += '  <th>Beam type&nbsp;B2</th>' 
                    elif 'machinemode'    in k:
                        s += '  <th>LHC mode</th>' 
                    elif 'beammode'       in k:
                        continue
                    else:
                        print ('ERROR: unknown LHC option "%s"' % k)
                        sys.exit(1)
                elif 'ofllumi:' in k:
                    if not ofllumiFlag:
                        s += '  <th>Offline Luminosity<br><font style="font-weight:normal">(%s)</font></font></th>' % k.split(':')[-1]
                        ofllumiFlag = True
                elif 'bs:' in k:
                    kt = k.replace('bs:','')
                    if 'bs:Pos' in k or 'bs:Sig' in k:
                        s += '  <th><font size="-2">%s<br><font style="font-weight:normal">(mm)</font></font></th>' % kt
                    elif 'bs:Tilt' in k:
                        s += '  <th><font size="-2">%s<br><font style="font-weight:normal">(rad)</font></font></th>' % kt
                    else:
                        s += '  <th><font size="-2">%s</font></th>' % kt
                elif 'BPM' in k:
                    s += '  <th>Beam&nbsp;Position Monitors&nbsp;(BPM)</th>' 
                elif 'olc:' in k:
                    if 'olc:lumi' in k:
                        tp1, tp2, chan = k.split(':')
                        try:
                            chan = float(chan)
                        except ValueError:
                            chan = -1
                        if chan in OLCAlgorithms:
                            chanstr = OLCAlgorithms[chan]
                        else:
                            chanstr = 'UNKNOWN'
                        s += '  <th>Online&nbsp;del.&nbsp;Luminosity&nbsp;<font size="-2"><br><font style="font-weight:normal">[%s]</font></font></th>' % chanstr
                    elif 'beam1bunches' in k:
                        continue # not used anymore
                    elif 'beam2bunches' in k:
                        continue # not used anymore
                    elif 'collbunches'  in k:
                        continue # not used anymore
                    elif 'bcidmask' in k:
                        s += '  <th>Bunch&nbsp;structure</th>'
                    elif 'beam1intensity' in k:
                        continue # included in 'fillbeams' summary
                    elif 'beam2intensity' in k:
                        continue # included in 'fillbeams' summary
                    else:
                        s += '  <th>%s</th>' % k

                elif 'BGS Key' in k:
                    s += '  <th>Bunch group key</th>'
                        
                else:
                    s += '  <th>%s</th>' % k
                s += '\n'
            s += '</tr>'

            # generate a second header line
            secondheader = ''
            patterns = [ ('lar:',          'LAr configuration'),
                         ('lhc:|olc:',     'LHC and online luminosity information' if any(['olc:' in x for x in Run.headerkeys()]) else 'LHC information' ),
                         ('bs:',           'Beam spot parameters (%s)' % Run.BeamspotSource),
                         ('STR:',          'Data stream statistics'),
                         ('TriggerMenu|SMK|Release|L1 PSK|HLT PSK|BGS|TriggerRates', 'Trigger information'),
                         ('SHIFTOFL:',     'Data quality (SHIFTOFL)'),
                         ('LBSUMM:',       'Data quality (LBSUMM)')
                         ]

            order = []
            for (p,hdesc) in patterns:
                matchedpositions = [idx for (idx,head) in enumerate(Run.headerkeys()) if re.match(p,head)]
                #print ("pattern",p,matchedpositions)
                if matchedpositions:
                    order += [(min(matchedpositions),max(matchedpositions),hdesc)]
            order.sort()

            mergeempty = True
            if len(order)>0:
                current=0
                for th in order:
                    if mergeempty:
                        if th[0]>current:
                            secondheader += '<th colspan="%s"></th>'  % (th[0]-current)
                    else:
                        for x in range(th[0]-current):
                            secondheader += '<th></th>'
                    secondheader += '<th colspan="%s">%s</th>' % (th[1]-th[0]+1,th[2])
                    current=th[1]+1
                if len(Run.headerkeys())>current:
                    if mergeempty:
                        secondheader += '<th colspan="%s"></th>' % (len(Run.headerkeys())-current)
                    else:
                        for x in range(len(Run.headerkeys())-current):
                            secondheader += '<th></th>'
                secondheader = "<tr>%s</tr>" % secondheader
            

            s = '<thead>' + secondheader + s + '</thead>\n'

            # Global (run independent) tooltips

            # OLC
            if any([k for k in Run.ShowOrder if "olc:lumi" in k.ResultKey]):
                boxcontent = '<font color="#AA0000">Click&nbsp;to&nbsp;enlarge&nbsp;figure and&nbsp;to&nbsp;obtain&nbsp;online&nbsp;integrated&nbsp;luminosity&nbsp;versus&nbsp;LB</font>'
                Run.addGlobalToolTip("OLCLumi", boxcontent)

            # OFLLumi
            if any([k for k in Run.ShowOrder if "ofllumi" in k.ResultKey]):
                boxcontent = '<font color="#AA0000">Click to obtain offline integrated luminosity versus LB</font>'
                Run.addGlobalToolTip("OFLLumi", boxcontent)

            # OFLBS - beamspot
            if any([k for k in Run.ShowOrder if "bs:" in k.ResultKey]):
                boxcontent = '<font color="#AA0000">Click to obtain %s beamspot versus LB</font>' % (Run.BeamspotSource.split()[0])
                Run.addGlobalToolTip("OFLBS", boxcontent)

            # BPM info
            if any([k for k in Run.ShowOrder if "BPM" == k.ResultKey]):
                boxcontent = '<font color="#AA0000">Click to enlarge figure</font>'
                Run.addGlobalToolTip("BPM", boxcontent)

            # LHC Summary
            if any([k for k in Run.ShowOrder if "lhc:fillnumber" in k.ResultKey]):
                boxcontent = '<font color="#AA0000">Click to enlarge figure</font>'
                Run.addGlobalToolTip("LHCSummary", boxcontent)

        else:
            s = ''
            for lll in [1,2]:
                if Run.showrunnr:
                    if lll==0:
                        s += '%-*s  %*s  ' % (_fW['Run'],'Run',_fW['NLB'],'#LB')
                    if lll==1:
                        s += '%-*s  %*s  ' % (_fW['Run'],' ',_fW['NLB'],' ')
                s+=" " # accept/reject flag
                if Run.showtime:
                    hstr = 'Start and endtime (%s)' % QC.localStr()
                    if lll==0:
                        s += '%*s  ' %(_fW['Time'],hstr)
                    if lll==1:
                        s += '%*s  ' %(_fW['Time'],' ')
                if Run.showduration:
                    if lll==0:
                        s += '%*s  ' % (10,'Duration')
                    if lll==1:
                        s += '%*s  ' % (10,' ')
                for k in Run.SortedShowOrder():
                    if k.Type == DataKey.STREAM:
                        w = max(len(k.Header)-k.Header.find('_'),_fW['Stream'])
                        if lll==0:
                            s += '%*s  ' % (w,k.Header[4:k.Header.find('_')])
                        if lll==1:
                            s += '%*s  ' % (w,k.Header[k.Header.find('_')+1:])
                    else:
                        w = 10
                        if k in _fW:
                            w = _fW[k.ResultKey]
                        if lll==0:
                            s += '%*s  ' % (w,k.Header)
                        if lll==1:
                            s += '%*s  ' % (w,' ')
                if lll==0:
                    s+='\n'
        return s

    def addResult(self, resDictKey, value, iov=None, reject=False):
        if resDictKey=='DQ' and value=='n.a.':
            return
        #print ("Run.addResult:",resDictKey, value, iov,"reject=",reject)
        if resDictKey not in self.result:
            self.result[resDictKey] = value
        if iov:
            if iov.startTime.lb>self.lastlb:
                return # sometimes there are IOVs completely outside the run e.g. run 165821 DQ SHIFTOFL
            iov.endTime = min(iov.endTime, IOVTime(self.runNr,self.lastlb+1) ) 
        self.data.addResult(iov, resDictKey, value, reject)

    def runlinks(self):
        s = ''
        # DS
        if time.gmtime(self.sor/1.E9).tm_year >= 2010:
            s  += '    <a href="http://atlas.web.cern.ch/Atlas/GROUPS/DATAPREPARATION/DataSummary/%i/run.py?run=%i" target="_blank" title="Data summary information for run %i"><font size="-3">DS</font></a>,&nbsp;\n' % (time.gmtime(self.sor/1.E9).tm_year, self.runNr, self.runNr)
        # RS
        s += '    <a href="http://atlas-service-db-runlist.web.cern.ch/atlas-service-db-runlist/php/runDetails.php?run=%i" target="_blank" title="Run summary information for run %i"><font size="-3">RS</font></a>,&nbsp;\n' % (self.runNr, self.runNr)
        # BS
        s += '    <a href="https://atlas-beamspot.cern.ch/webapp/jobs/?r=%i" target="_blank" title="Beam spot fit information for run %i"><font size="-3">BS</font></a>,&nbsp;\n' % (self.runNr, self.runNr)        
        # AMI
        s += '    <a href="https://ami.in2p3.fr/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.Command?Converter=/AMIXmlToAMIProdHtml.xsl&Command=FormBrowseDatasetPerRun+-runNumber=%i" target="_blank" title="AMI reference for run %i"><font size="-3">AMI</font></a>,&nbsp;\n' % (self.runNr, self.runNr)
        # DQ
        s += '    <a href="https://atlasdqm.web.cern.ch/atlasdqm/DQBrowser/makeMatrix.php?selection=All&run=%i&runup=&dbinstance=COMP200_SHIFTOFL&tag=HEAD" target="_blank" title="Browse detailed data quality entries for run %i (uses SHIFTOFL folder in COMP200 conditions database)"><font size="-3">DQ</font></a>,<br>\n' % (self.runNr, self.runNr)
        # ELOG
        s += '    <a href="https://atlasop.cern.ch/elog/ATLAS/ATLAS/?mode=full&reverse=0&reverse=1&npp=20&ma=1&ya=2008&last=3064&subtext=%i&sall=0" target="_blank" title="Search for ELOG entries on run %i"><font size="-3">ELOG</font></a>,&nbsp;\n' % (self.runNr, self.runNr)
        # DCS reference, format: http://atlas-dcs.cern.ch/navigation.php?datestring=2009-07-26-10-12
        tbeg = time.strftime("%Y-%m-%d-%H-%M-%S", time.gmtime(self.sor/1.E9))
        t    = time.gmtime(self.eor/1.E9)
        tend = "%4i-%02i-%02i-%02i-%02i" % (t[0], t[1], t[2], t[3], t[4])
        s += '    <a href="http://atlas-dcs.cern.ch/navigation.php?date2=%s" target="_blank" title="DCS status at begin of run %i"><font size="-3">DCS:SoR</font></a>/' % (tbeg, self.runNr)
        s += '<a href="http://atlas-dcs.cern.ch/navigation.php?date2=%s" target="_blank" title="DCS status at end of run %i"><font size="-3">EoR</font></a>,&nbsp;\n  ' % (tend, self.runNr)
        #s += '    <a href="https://atlas-dcs.cern.ch/navigation.php?date2=2008-09-29-19-01-42">DCS:SoR</font></a>/' % (tbeg, self.runNr)
        #s += '<a href="https://atlas-dcs.cern.ch/navigation.php?date2=2008-09-30-08-17"><font size="-3">EoR</font></a>,&nbsp;\n  ' % (tend, self.runNr)
        # OKS
        # protect against missing OKS information
        if self.data['oks']:
             s += '    <a href="http://cern.ch/atlas-project-tdaq-cc/cgi/getdata.sh?tdaq-05-05-00&oracle://atlas_oks/r&atlas_oks_archive&%i&%i&ATLAS" target="_blank" title="Download online configuration for run %i"><font size="-3">OKS</font></a>\n' % ( self.data['oks'][0].value + (self.runNr,) )
        # lumi summary
        # typical address: https://atlas.web.cern.ch/Atlas/GROUPS/DATAPREPARATION/RunSummary/run142308_summary.html
        # OLD: fname = 'https://atlas.web.cern.ch/Atlas/GROUPS/DATAPREPARATION/RunSummary/run%i_summary.html' % (self.runNr)
        fname = 'http://atlas.web.cern.ch/Atlas/GROUPS/DATAPREPARATION/DataSummary/runsum.py?run=%i\n' % (self.runNr)
        fwget = urllib.urlopen(fname)

        wincontent = ''
        for line in fwget:
            if '</head>' in line and '<base href' not in wincontent:
                wincontent += '<base href="http://atlas.web.cern.ch/Atlas/GROUPS/DATAPREPARATION/DataSummary/2010/"></base>'
            wincontent += line
        wincontent = wincontent.replace('href="css/atlas-datasummary.css"', 'href="http://atlas.web.cern.ch/Atlas/GROUPS/DATAPREPARATION/DataSummary/css/atlas-datasummary.css"')

        # remove temporarily second plot with integration
        wincontent = wincontent.replace( '<td>\n<a href="rundata/run%i/run%i_ilumr.png"><img src="rundata/run%i/run%i_ilumr.png" alt="InstLumi" style="width: 200px; "></a></td>' % (self.runNr,self.runNr,self.runNr,self.runNr),'')
        wincontent = wincontent.replace('"','&quot;')
        wincontent = wincontent.replace('./RunSummary/figs','https://atlas.web.cern.ch/Atlas/GROUPS/DATAPREPARATION/RunSummary/figs')
        wincontent = wincontent.replace('<a href','<a target=&quot;_blank&quot; href')
        wincontent = wincontent.replace('width: 200px','width: 350px')
        wincontent = wincontent.replace('width: 250px','width: 350px')


        openwincmd = 'javascript:openLumiWindow('
        openwincmd += "'Print','<!DOCTYPE html PUBLIC &quot;-//W3C//DTD HTML 4.01 Transitional//EN&quot;><html xmlns:&quot;my&quot;><head><title>Luminosity information for run %i</title></head><body style=&quot;background-color:#ffffff&quot;>" % self.runNr
        openwincmd += wincontent        
        openwincmd += '</body></html>'
        openwincmd += "')"

        return s


    def timestr(self,format='html',closed=True):
        """ sor and eor is always in UTC, so sor=0 means the run started 1.1.1970 at 00:00:00"""
        if QC.localtime:
            begin = time.localtime(self.sor/1.E9)
            end   = time.localtime(self.eor/1.E9)
        else:
            begin = time.gmtime(self.sor/1.E9)
            end   = time.gmtime(self.eor/1.E9)

        # begin string
        beginstr = time.strftime("%a %b %d %Y %X",begin)

        # end format
        endformat = "%X"
        if end[0]>begin[0]:
            endformat = "%a %b %d, %Y %X"
        elif end[1]>begin[1] or end[2]>begin[2]:
            endformat = "%a %b %d, %X"

        # end string
        if closed:
            endstr = time.strftime(endformat,end)
        else:
            endstr = '<font color=#BB0000>ongoing</font>' if format=='html' else 'ongoing'

        # output
        if format=='html':
            return '%s&nbsp;&minus;&nbsp;%s' % (beginstr,endstr)
        elif format=='seconds': #mainly for dictionary
            return '%12.3f, %12.3f' % (self.sor/1.E9, self.eor/1.E9)
        elif format=='secondsNum': #mainly for dictionary
            return self.sor/1.E9, self.eor/1.E9
        else:
            return '%s - %s' % (beginstr,endstr)

    def durationstr(self):
        dt = time.gmtime((self.eor-self.sor)/1.E9)[2:6]
        if dt[0]>1:
            return "%id %ih %im %is" % ((dt[0]-1,) + dt[1:])
        else:
            return "%ih %im %is" % dt[1:]

    @classmethod
    def prettyChain(cls,tr,ps):
        i = len(ps)-1
        while i>0:
            if ps[i]==ps[i-1]:
                ps[i]=' '
            elif ps[i]<0:
                ps[i]='x'
            else:
                ps[i]=str(ps[i])
            i-=1
        if ps[0] is None:
            ps[0]='n.a.'
        elif ps[0]<0:
            ps[0]='x'
        else:
            ps[0]=str(ps[0])
        
        pss = '-'.join(ps)
        s = "%s (%s)" % (tr.name,pss)
        return s

    def splitTriggerChains(self, chainlist):
        res = { 'L1': [], 'L2': [], 'EF': [] }
        for tr,pslist in chainlist.items():
            if not tr.forshow:
                continue
            k = tr.name[0:2]
            res[k] += [Run.prettyChain(tr,pslist)]
        res['L1'].sort()
        res['L2'].sort()
        res['EF'].sort()
        l1chains = '<br> '.join(res['L1'])
        l2chains = '<br> '.join(res['L2'])
        efchains = '<br> '.join(res['EF'])
        ret = ()
        if l1chains:
            ret += (l1chains.replace('.0',''), )
        if l2chains:
            ret += (l2chains.replace('.0',''), )
        if efchains:
            ret += (efchains.replace('.0',''), )
        return ret

    def __str__(self):
        if Run.writehtml:
            s = ashtml(self)
            return s
        else:
            s = self.astxt()
            for lbr in self.data:
                s+= "\n" + lbr.astxt()
            return s

    def __cmp__(self,other):
        if isinstance(other,Run):
            return self.runNr - other.runNr
        return self.runNr - other


    def astxt(self):
        s = ""
        if Run.showrunnr:
            s += "run %*s  %*s  " % (_fW['Run']-4,self.runNrS, _fW['NLB'], self.NrLBs)
        s += " "

        if Run.showtime:
            s += "%*s  " % (_fW['Time'],self.timestr('txt'))

        if Run.showduration:
            dt = time.gmtime((self.eor-self.sor)/1.E9)[2:6]
            if dt[0]>1:
                s += "%id %ih %im %is " % ((dt[0]-1,) + dt[1:])
            else:
                s += "%2ih %2im %2is " % dt[1:]

        for k in Run.SortedShowOrder():
            w = 10
            if k in _fW:
                w = _fW[k]
            v = self.result[k.ResultKey]
            ostr=v
            if k == "#Events":
                ostr = addKommaToNumber(v)
            elif k.Type == DataKey.STREAM:
                w=max(len(k.Header)-k.Header.find('_'),_fW['Stream'])
                if isinstance(ostr,tuple):
                    ostr = ostr[0]
            elif k=="TriggerMenu":
                pv = [Run.prettyChain(tr,pslist) for (tr,pslist) in v.items() if tr.forshow]
                ostr = ", ".join(pv)
            elif k=="TriggerRates":
                ostr = "NOT YET"
            elif k=="olc:bcidmask":
                ostr = "NOT YET"

            s += "%*s  " % (w,ostr)

        return s



def ashtml(run):
    import CoolRunQuery.html.AtlRunQueryHtmlUtils as HU

    with timer("print runnur, runtime, etc"):
        s = "<!-- Run %s -->\n" % run.runNrS

        # disctionary with results
        if Run.bgcolor == "1":
            Run.bgcolor = "2"
        else:
            Run.bgcolor = "1"
        color = Run.bgcolor

        runrowclass = "out"
        if run.showDataIncomplete:
            runrowclass="showmiss"
        if run.selDataIncomplete:
            runrowclass="selmiss"
        runrowclass += color   

        s += '<tr class="out%s">\n' % (color)

        if Run.showrunnr: # show columns Run, Links, LB
            # run number
            if run.runNr == Run.runnropen: # is the run still ongoing?
                s += '  <td style="text-align:center" class="%s"><font color="#BB0000">%s<br><font size="-2">(ongoing)</font></font>' % (runrowclass, run.runNrS)
            else:
                s += '<td style="text-align:center" class="%s">%s' % (runrowclass, run.runNrS)        
                p = Run.runPeriods.findPeriod( run.runNr )
                if p:
                    # split in the middle
                    idx=-1
                    for i in range(p.count(',')//2+1):
                        idx = p.find(',',idx+1)
                    s += '<br><font color="#488AC7"><font size=-2><nobr>Period: %s<br>%s</nobr></font></font>' % (p[:idx],p[idx+1:])

            # open for prompt calibration?
            if run.runNr in Run.PromptCalibRuns:
                s += '<br><font color="#008800"><font size="-2">(in&nbsp;calib&nbsp;loop)</font></font>'
            s += '</td>'

            # links to other information
            s += '\n  <td>\n%s</td>' % run.runlinks()

            # luminosity block and average lb duration
            lbcontent = '%s' % run.NrLBs
            try:
                vf = float(run.NrLBs)
                if vf > 0:
                    lbcontent = '%s<br><font size="-2">(%i&nbsp;s)</font>' % (run.NrLBs, (run.eor-run.sor)/(1.0E9*vf) )
            except ValueError:
                pass

            window  = HU.OpenWindow( HU.CreateLBTable(run), extracss=['html/atlas-runquery-lb.css'] )

            # put into html page
            tip = 'showTip LBStart_%i' % run.runNr
            s += '\n  <!-- LB Content -->\n'
            s += '  <td style="text-align: right"><div class="%s tooltip" style="display:inline;cursor:pointer">%s%s</a></div></td>\n' % (tip, window, lbcontent)


        if Run.showtime:
            s += '  <td>%s</td>' % run.timestr('html',run.runNr != Run.runnropen) # is the run still ongoing?


        if Run.showduration:
            if run.runNr == Run.runnropen:
                s += '  <td><font color="#BB0000">%s<br><font size="-2">(ongoing)</font></font></td>' % run.durationstr()
            else:
                s += "  <td>%s</td>" % run.durationstr()

        # find maximum output rate in case of streams
        sumstrnev = 0
        for k in Run.SortedShowOrder():
            if k.Type == DataKey.STREAM and k.ResultKey in run.stats and 'calibration' not in k.ResultKey:
                sumstrnev += run.result[k.ResultKey][0]
        if sumstrnev == 0:
            sumstrnev = -1

        # flags...
        ofllumiFlag = False


    # the loop
    with timer("print keys"):
        for data_key in Run.SortedShowOrder():
            k = data_key.ResultKey
            v = run.result[k]

            with timer("keybuild %s" % k):
                if any(['olc:beam2intensity' in k,
                        'olc:beam1intensity' in k,
                        'lhc:beamenergy' in k,
                        'beam1bunches' in k,
                        'beam2bunches' in k,
                        'collbunches' in k,
                        'L1 PSK' == k,
                        ]):
                    continue


                
                s += '\n  '

                # -----------------------------------------------------------------------------------------------------------------------

                if k == "#Events":
                    # compute approximative output rate in Hz
                    durInSec = (run.eor-run.sor)/1.0E9                
                    rate = 'n.a. Hz'
                    try:
                        if durInSec > 0:
                            rate = '%.1f Hz' % (float(v)/durInSec)
                    except ValueError: # if n.a.
                        pass
                    s += '  <td style="text-align: right">%s<BR><font size="-2">(%s)</font></td>' % (addKommaToNumber(v),rate)
                    continue

                # -----------------------------------------------------------------------------------------------------------------------

                if k == "#L1A" or k == '#L2A' or k == '#EFA' or k == '#Events (streamed)' or k == '#Events (incl. calib.)':
                    s += '  <td style="text-align: right">%s</td>' % addKommaToNumber(v)
                    continue

                # -----------------------------------------------------------------------------------------------------------------------

                if k == 'DQ': # DEFECT
                    intolerable = Run.Fieldinfo['DQ']['IntolerableDefects']

                    for channelname in Run.Fieldinfo['DQ']['DefChannels']:
                        if channelname == '':
                            matching_dqentries = run.stats[k]["defects"]
                        elif channelname == 'DET':
                            matching_dqentries = [dqentry for dqentry in run.stats[k]["defects"] if '_' not in dqentry[0]]
                        else:
                            from re import compile
                            pattern = compile(channelname)
                            matching_dqentries = [dqentry for dqentry in run.stats[k]["defects"] if pattern.match(dqentry[0])]


                        # reduce to LB ranges inside READY
                        matching_dqentries_ready = []
                        for (defect, listLBranges) in matching_dqentries: # dqentry is of format ('SCT_GLOBAL_STANDBY', [(1L, 49L), (119L, 123L)])
                            readyLBRanges = [lbrange for lbrange in listLBranges if run.data.isReady(lbrange)]
                            if len(readyLBRanges)>0:
                                matching_dqentries_ready += [ (defect, readyLBRanges) ]


                        if len(matching_dqentries_ready)==0:
                            dq_info = "<table class='dqdefects' style='border-color: limegreen; height=100%'><tr><td>&nbsp;</td></tr></table>\n"
                        else:
                            dq_info = "<table class='dqdefects'>\n"
                            for defect,listLBranges in matching_dqentries_ready:
                                tip = 'showTip dqdefect_%i_%s tooltip' % (run.runNr, defect)
                                if defect in intolerable:
                                    dq_info += "<tr class='%s'><td class='intolerable'>%s</td><td class='lb'>" % (tip, defect)
                                else:
                                    dq_info += "<tr class='%s tolerable' style='visibility:collapse;'><td>%s</td><td class='lb'>" % (tip, defect)
                                dq_info += ", ".join([("%g&minus;%g" % (x[0],x[1]-1) if x[1]-x[0]!=1 else "%g" % x[0]) for x in listLBranges])
                                dq_info += "</td></tr>\n"
                            dq_info += '<tr height="100%%"><td height="100%%"></td></tr>\n'
                            dq_info += "</table>\n"
                        s += '<td class="def">%s</td>' % dq_info

                    continue

                # -----------------------------------------------------------------------------------------------------------------------

                if isDQ(k): # old, traffic-light DQ
                    extraDQInfo = ''
                    tip = ''
                    dqmax = run.stats[k]["max"]
                    n     = run.stats[k]["counts"]

                    foundComment = False
                    for (dq,comment), start, end in run.stats[k]["blocks"]:
                        dqcss = dq
                        if dq == 'n.a.':
                            dqcss = 'NA'
                        elif comment:
                            foundComment = True

                        if n[dq]>0 and dq != dqmax:
                            if start == end-1:  # only one LB
                                extraDQInfo += '<tr><td class="td%s" style="min-width: 45px; text-align: left; border-width: 0px; padding:1px;">&nbsp;LB&nbsp;%g:</td><td class="td%s" style="text-align: left; border-width: 0px; padding: 1px; ">%s&nbsp;</td></tr>' % (dqcss,start,dqcss,dq)                  
                            else:               # range of LBs
                                extraDQInfo += '<tr><td class="td%s" style="min-width: 45px; text-align: left; border-width: 0px; padding:1px;">&nbsp;LB&nbsp;%g&minus;%g:</td><td class="td%s" style="text-align: left; border-width: 0px; padding: 1px; ">%s&nbsp;</td></tr>' % (dqcss,start,end-1,dqcss,dq)

                    if foundComment:
                        tip = 'showTip dqcomm_%s_%i' % (k, run.runNr)
                    if extraDQInfo:
                        extraDQInfo = '<br><img vspace=2 height="1" width="1" src="wdot.jpg"><br><table style="width: 100%; border: 1px solid; border-width: 1px; border-spacing: 0px; border-color: #eeeeee; border-collapse: separate; padding: 0px; font-size: 65%"><tr>' + extraDQInfo + '</table>'

                    if tip:
                        s += '<td class="%s td tooltip td%s">%s%s</td>' % (tip,dqmax,dqmax,extraDQInfo)
                    else:
                        s += '<td class="td td%s">%s%s</td>' % (dqmax,dqmax,extraDQInfo)

                    continue

                # -----------------------------------------------------------------------------------------------------------------------

                if "detector systems" in k.lower():
                    if v!='n.a.':
                        mask = '0x'+v if Selector.condDB()=='CONDBR2' else v # run2 the format is a hex string, in run 1 it is an int (represented as string)
                        v = DecodeDetectorMask( mask, Selector.condDB()=='CONDBR2',True)
                    s += '  <td style="min-width:%ipx"><font size="-2">%s<hr color="#aaaaaa" size=1>[<a href="http://sroe.home.cern.ch/sroe/cgi-bin/avgmodule.py?run=%i" target=_blank>SCT HV setting</a>]</font></td>' % (1.1*len(v),v,run.runNr)
                    continue

                # -----------------------------------------------------------------------------------------------------------------------

                if 'olc:lumi' in k:

                    # find range with stable beams (requires LHC information to be available)
                    hasStableBeamsInfo, xvecStb = run.getStableBeamsVector()

                    # is pileup information available?
                    printMu = 'olc:bcidmask' in Run.SortedShowOrder() and run.runNr>=151260 and run.givepeakmuinfo

                    # insert events per LB
                    xvec       = []
                    yvec       = []
                    yvecInt    = []
                    for entry in run.data[k]:
                        assert entry.startlb != 0, 'entry should not start at LB=0'
                        val = entry.value if (entry.value!='n.a.' and entry.value>0) else 0
                        lastlb = min(entry.lastlb,run.nr_lb)
                        xvec += range(entry.startlb,lastlb+1)
                        nlb = lastlb - entry.startlb + 1
                        yvec    += nlb*[val]
                        yvecInt += nlb*[val]

                    # correct for time span
                    intlumi, intlumiStb = (0, 0)

                    # for lifetime
                    lifetime   = 0
                    lb1        = 0
                    lb2        = 0
                    t1         = 0
                    t2         = 0
                    y1         = 0
                    y2         = 0
                    if len(xvecStb) > 15:
                        lb1 = xvecStb[3]
                        lb2 = xvecStb[-1-5]

                    peaklumi = -1
                    for idx,(lbtime,lbendtime) in enumerate(run.lbtimes):
                        lb=idx+1                    
                        dt           = (float(lbendtime)-float(lbtime))/1.E9
                        yvecInt[idx] *= dt/1000.0*run.lumiunit   # unit of yvec was: ub-1 s-1 -> nb-1
                        yvec[idx]    *= 1.0*run.lumiunit         # unit of yvec was: ub-1 s-1, transform to: 10^30 cm-2s-1
                        intlumi      += yvecInt[idx]

                        if yvec[idx] > peaklumi:
                            peaklumi = yvec[idx]
                        if lb in xvecStb:
                            intlumiStb += yvecInt[idx]
                            if   lb == lb1:
                                t1 = lbtime/1.E9
                                y1 = yvec[idx]
                            elif lb == lb2:
                                t2 = lbtime/1.E9
                                y2 = yvec[idx]

                    if y1 > 0 and y2 > 0:
                        lifetime = (t2 - t1)/(3600*math.log(y1/y2))  # unit: hours

                    # line to be printed in column
                    if hasStableBeamsInfo:
                        fullprint_ = 'All&nbsp;LBs:&nbsp;%0.4g<br>Stable&nbsp;B:&nbsp;%0.4g' % (intlumi,intlumiStb)
                        histoText  = 'Integrated luminosity: %.4g %s-1 (all LBs) and %.4g %s-1 (stable beams)' % (intlumi,run.lumiunittxt,intlumiStb,run.lumiunittxt)
                    else:
                        fullprint_ = 'All&nbsp;LBs:&nbsp;%0.4g' % (intlumi)
                        histoText  = 'Integrated luminosity: %.4g %s-1 (all LBs)' % (intlumi,run.lumiunittxt)

                    # find out which channel was used
                    tp1, tp2, chan = k.split(':')
                    try:
                        chan = int(chan)
                        if chan in OLCAlgorithms:
                            chanstr = OLCAlgorithms[chan]
                        else:
                            chanstr = 'UNKNOWN'            
                    except ValueError:
                        chanstr = chan

                    path = makeLBPlot( xvec, xvecStb, yvec, 'Luminosity block number', 'Online luminosity (10^{%s}cm^{-2}s^{-1})' % run.instlumiunittxt, '',
                                       'olclumi_vs_lb_run_%i' % (run.runNr),
                                       'Online lumi [%s] per LB for run %i' % (chanstr, run.runNr),
                                       Run.Datapath, histoText )

                    # create window and tooltip
                    wincontent  = '<table class=&quot;outer&quot; style=&quot;padding: 5px&quot;><tr><td>'
                    wincontent += '<strong><b>Online luminosity [algorithm: %s] per LB for run %i:</b></strong><br><font size=&quot;-1&quot; color=&quot;#888888&quot;>(Begin/end of run: %s)</font>' % (chanstr, run.runNr, run.timestr('html', run.runNr != Run.runnropen))
                    wincontent += '<hr color=&quot;black&quot; size=1>'
                    wincontent += '<table style=&quot;padding: 0px&quot;><tr><td>'
                    wincontent += '<img src=&quot;%s&quot; align=&quot;left&quot;>' % path
                    wincontent += '</td></tr></table>'
                    wincontent += '<hr color=&quot;black&quot; size=1>'
                    wincontent += '<table class=&quot;LB&quot;>'
                    wincontent += '<tr><th>LB</th><th>Start time<br> (%s)</th><th>Duration<br>(sec)</th><th>Inst. luminosity<br>(1e%s&nbsp;cm<sup>&minus;2</sup>s<sup>&minus;1</sup>)</th>' % (QC.tzdesc(), run.instlumiunittxt)
                    wincontent += '<th>Int. luminosity<br>(%s<sup>&minus;1</sup>)</th><th>Cumul. luminosity<br>(%s<sup>&minus;1</sup>)</th>' % (run.lumiunittxt, run.lumiunittxt)
                    if printMu:
                        wincontent += '<th>&nbsp;&nbsp;&lt;&mu;&gt;&nbsp;&nbsp;</th>'
                    if hasStableBeamsInfo:
                        wincontent += '<th>Stable beams</th>'
                    wincontent += '</tr><tr style=&quot;background:%s&quot;>' % '#eeeeee'
                    intlumi = 0
                    mumax = -1
                    minBiasXsec = 80.0 if Selector.isRun2() else 71.5 # minbias cross-section for 8TeV (71.5) and 13TeV (80.0)

                    is5TeVRun = (run.sor/1.E9) > time.mktime((2017,11,11,0,0,0,0,0,0)) and (run.sor/1.E9) < time.mktime((2017,11,21,8,0,0,0,0,0))
                    if is5TeVRun:
                        minBiasXsec = 66.25 

                    is5TeVHIRun = (run.sor/1.E9) > time.mktime((2018,11,7,0,0,0,0,0,0)) and (run.sor/1.E9) < time.mktime((2018,12,31,0,0,0,0,0,0))
                    if is5TeVHIRun:
                        minBiasXsec = 7660
                    
                    for idx,(lbtime,lbendtime) in enumerate(run.lbtimes):
                        lb=idx+1
                        isStableBeams = (lb in xvecStb)
                        intlumi += yvecInt[idx]
                        stbBeams = 'T' if isStableBeams else '&minus;'
                        timetuple = time.localtime(lbtime/1.E9) if QC.localtime else time.gmtime(lbtime/1.E9)
                        wincontent += '<td>%s</td><td>%s</td><td>%.2f</td><td>%.4g</td><td>%.4g</td><td>%.4g</td>' % (lb, time.strftime("%X",timetuple), (float(lbendtime)-float(lbtime))/1.E9, yvec[idx], yvecInt[idx], intlumi)
                        # pileup <mu> information?
                        if printMu:
                            nb = 0
                            for n,v,lbstart,lbend in run.stats['olc:bcidmask']['blocks']:
                                if lb >= lbstart and lb <= lbend:
                                    if v:
                                        nb = len(v[2])
                                    break
                            # compute expected number of interactions
                            lumi_mb = yvec[idx]*1000.0
                            mu = 0
                            if nb:
                                mu = lumi_mb * minBiasXsec / 11245.511 / nb
                            wincontent += '<td>%.3g</td>' % mu
                            if isStableBeams and mu > mumax:
                                mumax = mu
                        if hasStableBeamsInfo:
                            wincontent += '<td>%s</td>' % stbBeams
                        if idx%2!=0:
                            col = '#eeeeee'
                        else:
                            col = '#ffffff'
                        wincontent += '</tr><tr style=&quot;background:%s&quot;>' % col

                    printMuInfoToFile = True
                    if printMuInfoToFile:
                        mutextfilename = Run.Datapath + '/mu_vs_run_output.txt' 
                        muout          = open( mutextfilename, 'a' )
                        muout.write( '%10i   %f\n' % (run.runNr, mumax) )
                        muout.close()

                    if mumax < 0:
                        s_mumax = "n.a."
                    elif mumax < 0.1:
                        s_mumax = "%0.3e" % mumax
                    else:
                        s_mumax = "%0.3f" % mumax

                    if run.runNr == Run.runnropen: # run still ongoing
                        wincontent += '<tr><td style=&quot;text-align:left&quot;colspan=&quot;3&quot;><i>&nbsp;&nbsp;Run still ongoing ...</i></td></tr>'
                    wincontent += '</tr></table>'

                    wincontent += """<hr color=&quot;red&quot; size=1><font color=&quot;#777777&quot;><font size=&quot;-1&quot;><i><font size=&quot;-2&quot;>Created by AtlRunQuery on: %s</font></i></font></td></tr></table><script type=&quot;text/javascript&quot;></script></body></html>""" % str(datetime.datetime.now())
                    wincontent += '</td></tr></table>'

                    openwincmd = '<a href="javascript:openLargeWindow('
                    openwincmd += "'Print','<!DOCTYPE html PUBLIC &quot;-//W3C//DTD HTML 4.01 Transitional//EN&quot;><html xmlns:&quot;my&quot;><head><title>Run query result for online luminosity</title><LINK href=&quot;html/atlas-runquery-lb.css&quot; rel=&quot;stylesheet&quot; type=&quot;text/css&quot;></head><body><table style=&quot;font-family: sans-serif; font-size: 85%%&quot;>" 
                    openwincmd += wincontent        
                    openwincmd += "')\">"

                    # line to be printed in column
                    fullprint = '<table class="bcidtable">'
                    fullprint += '<tr><td colspan="2"><img src="%s" align="left" width="90px"></td></tr>' % path
                    if hasStableBeamsInfo:
                        fullprint += '<tr><td style="text-align:left">Entire&nbsp;run:</td><td style="text-align:left">&nbsp;%0.4g&nbsp;%s<sup>&minus;1</sup></td></tr><tr><td style="text-align:left">Stable&nbsp;beams:</td><td style="text-align:left">&nbsp;%0.4g&nbsp;%s<sup>&minus;1</sup></td></tr><tr><td style="text-align:left">Peak&nbsp;lumi:</td><td style="text-align:left">&nbsp;%.2g&nbsp;e%s&nbsp;cm<sup>&minus;2</sup>s<sup>&minus;1</sup></td></tr><tr><td style="text-align:left">Peak&nbsp;&lt;&mu;&gt;:</td><td style="text-align:left">&nbsp;%s</td></tr>' % (intlumi, run.lumiunittxt, intlumiStb, run.lumiunittxt, peaklumi, run.instlumiunittxt, s_mumax)
                        if lifetime > 0:
                            fullprint += '<tr><td style="text-align:left">Approx.&nbsp;lifetime:</td><td style="text-align:left">&nbsp;%0.2g&nbsp;h</td></tr>' % (lifetime)
                    else:
                        fullprint += '<td style="text-align:left">All&nbsp;LBs:</td><td style="text-align:left">&nbsp;%0.4g</td></tr>' % (intlumi)
                    fullprint += '</table>'

                    # put into html page
                    s += '<td class="showTip OLCLumi stream" style="text-decoration:none; text-align: right">%s%s</a></td>' % (openwincmd, fullprint)

                    continue

                # -----------------------------------------------------------------------------------------------------------------------

                if 'olc:bcidmask' in k:

                    firstGoodRun = 151260
                    if run.runNr < firstGoodRun: # OLC information not properly filled
                        s += '<td style="text-align:center;color:#AA0000;font-size:60%%;">BCID information only available for run numbers larger than %i</font></td>' % firstGoodRun
                        continue

                    # line to be printed in column
                    fullprint = '<font size=-2><i>Run without stable beams (or, in few cases, missing bunch information in COOL). Click here to obtain bunch crossing information</i>.</font>'

                    wincontent  = '<table class=&quot;outer&quot; style=&quot;padding: 5px&quot;><tr><td>'
                    wincontent += '<strong><b>Identifiers (BCID) for paired and unpaired bunches for run %i:</b></strong><br><font size=&quot;-1&quot; color=&quot;#888888&quot;>(Begin/end of run: %s)</font>' % (run.runNr, run.timestr('html', run.runNr != Run.runnropen))
                    wincontent += '<hr color=&quot;black&quot; size=1>'
                    wincontent += '<table class=&quot;bcidtable&quot; align=&quot;center&quot;><tr class=&quot;tr0&quot;><td style=&quot;text-align:left;font-weight:bold&quot; colspan=&quot;3&quot;>LB&nbsp;range</td><td style=&quot;text-align:right;font-weight:bold&quot;>&nbsp;&nbsp;&nbsp;&nbsp;Stable<br>beams</td><td style=&quot;text-align:right;font-weight:bold&quot;>%s</td><td style=&quot;text-align:right;font-weight:bold&quot;>%s</td><td style=&quot;text-align:right;font-weight:bold&quot;>%s</td></tr>' % ('&nbsp;&nbsp;Crossing&nbsp;(Paired)', '<nobr>&nbsp;&nbsp;Unpaired&nbsp;Beam-1&nbsp;</nobr>', '<nobr>&nbsp;&nbsp;Unpaired&nbsp;Beam-2</nobr>')
                    ic = 0

                    # retrieve stable beam information
                    hasStableBeamsInfo, xvecStb = run.getStableBeamsVector()

                    first = True
                    for n,v,lbstart,lbend in run.stats[k]['blocks']:
                        ic += 1
                        cls = 'tr1' if (ic%2==1) else 'tr2'

                        # is this block during stable beams ?
                        isInStableBeamPeriod = False
                        stableBeamWord       = '&minus;'
                        if hasStableBeamsInfo:
                            if lbstart <= xvecStb[-1] and lbend >= xvecStb[0]:
                                isInStableBeamPeriod = True
                                stableBeamWord = '<b>T</b>'

                        wincontent += '<tr class=&quot;%s&quot;><td class=&quot;td1&quot; style=&quot;text-align:right&quot;><b>%s</b></td><td style=&quot;text-align:right&quot;><b>&minus;</b></td><td style=&quot;text-align:right&quot;><b>%s:</b>&nbsp;&nbsp;&nbsp;</td><td>%s&nbsp;&nbsp;&nbsp;&nbsp;</td>' % (cls,str(lbstart),str(lbend-1),stableBeamWord)
                        if v:
                            beam1, beam2, beam12 = v
                            unpaired1 = [b for b in beam1 if b not in beam12]
                            unpaired2 = [b for b in beam2 if b not in beam12]

                            noBunches = not unpaired1 and not unpaired2 and not beam12  # happens at least in run 152508
                            if not unpaired1:
                                unpaired1 = ['&minus;']
                            if not unpaired2:
                                unpaired2 = ['&minus;']
                            if not beam12:
                                beam12    = ['&minus;']

                            # analyse bunch train structure
                            if isInStableBeamPeriod and first and not noBunches:
                                first = False # don't do that again

                                maxBunchDistanceInTrain = 6                            
                                if int(run.runNr) > 168665:
                                    maxBunchDistanceInTrain = 25 # large (20) bunch distance for Heavy Ion collisions
                                trains = []
                                bp = -1
                                for b in beam12:
                                    if bp > 0 and b - bp <= maxBunchDistanceInTrain:
                                        if trains[-1][-1] != bp:
                                            trains.append([bp])
                                        trains[-1].append(b)
                                    elif len(beam12) > 1 and bp <= 0 and beam12[0] - b <= maxBunchDistanceInTrain:
                                        trains.append([b])
                                    bp = b

                                fullprint  = '<table class = "triggerlinktable">'
                                fullprint += '<tr><td style="text-align:left"><nobr>No. of coll. bunches:&nbsp;&nbsp;</nobr></td><td> <b>%i</b></td></tr>' % len(beam12)
                                fullprint += '<tr><td style="text-align:left"><nobr>No. of bunch trains:</nobr></td><td><b> %i</b></td></tr>' % len(trains)
                                if trains:
                                    Run.TmpBoxContent  = '<strong><b><nobr>Bunch train configuration for run <font color="#AA0000">%i</font></nobr></b></strong>' % run.runNr
                                    Run.TmpBoxContent += '<hr size=1>' 

                                    # it may be that the BCID=1 is a single pilot bunch only without a train
                                    # in that case, don't use it to compute bunch distance in train, but print comment
                                    icoor = 0 
                                    if len(trains) > 1:
                                        icoor = 1
                                    if len(trains[icoor]) > 1:
                                        bunchDist = trains[icoor][1] - trains[icoor][0]
                                        fullprint += '<tr><td style="text-align:left"><nobr>Bunch dist. in trains:</nobr></td><td><nobr><b> %i</b> (%i ns)</nobr></td></tr>' % (bunchDist, bunchDist*25)
                                    # print the comment in case of single pilot bunch
                                    if len(trains) > 0:
                                        if len(trains[0]) == 1:
                                            fullprint += '<tr><td style="text-align:left;color:#4477EE" colspan=2><i>The first "bunch train" is trivial and consists of only the colliding pilot bunch</td></tr>'
                                    fullprint += '</table><hr size=1>'                            
                                    Run.TmpBoxContent += '<table><tr><td>Train</td><td><nobr>No. of bunches</nobr></td><td style="text-align:right">&nbsp;&nbsp;&nbsp;BCID range</td></tr>'
                                    for it, train in enumerate(trains):
                                        Run.TmpBoxContent += '<tr><td>&nbsp;&nbsp;%i:&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%i</td><td style="text-align:right"><nobr>&nbsp;&nbsp;%i &minus; %i</nobr></td></tr>' % (it+1, len(train), train[0], train[-1])
                                    Run.TmpBoxContent += '</table><hr size=1>'
                                else:
                                    Run.TmpBoxContent = '<nobr>No bunch trains in this run</nobr> <hr size=1>'
                                    fullprint += '</table><hr size=1>' 
                                Run.TmpBoxContent += '<p><font color="#AA0000"><b><nobr>Click to obtain full list of BCIDs</nobr></b></font>'
                                fullprint += '<span style="font-size: xx-small; color: #555555">[ <i>Mouse over for train configuration. Click for full list of BCIDs</i> ]</span>'

                            wincontent += '<td class=&quot;td1&quot; style=&quot;text-align:right&quot;>%s</td><td style=&quot;text-align:right; border-left:1px #C0C0D0 solid;border-right:1px #C0C0D0 solid&quot;>%s</td><td style=&quot;text-align:right&quot;>%s</td>' % (', '.join(map(str,beam12)), ', '.join(map(str,unpaired1)), ', '.join(map(str,unpaired2)) )
                        else:
                            wincontent += '<td class=&quot;td1&quot; style=&quot;text-align:right&quot;>%s</td><td style=&quot;text-align:right&quot;>%s</td><td style=&quot;text-align:right&quot;>%s</td>' % ('&minus;','&minus;','&minus;')

                        wincontent += '</tr>'


                    wincontent += '</table></td>'
                    wincontent += '</td></tr></table>'


                    openwincmd = HU.OpenWindow(wincontent, title="Run query result for online luminosity", extracss=None, size="large")
                    #openwincmd = '<a href="javascript:openLargeWindow('
                    #openwincmd += "'Print','<!DOCTYPE html PUBLIC &quot;-//W3C//DTD HTML 4.01 Transitional//EN&quot;><html xmlns:&quot;my&quot;><head><title>Run query result for online luminosity</title><LINK href=&quot;html/atlas-runquery-results.css&quot; rel=&quot;stylesheet&quot; type=&quot;text/css&quot;></head><body><table style=&quot;font-family: sans-serif; font-size: 85%%&quot;>" 
                    #openwincmd += wincontent        
                    #openwincmd += "')\">"

                    s += '<td class="showTip OLCBCID_%i stream" style="text-decoration:none; text-align: left">%s%s</a></td>' % (run.runNr, openwincmd, fullprint)

                    continue

                # -----------------------------------------------------------------------------------------------------------------------

                if k.startswith('bs:Pos') or k.startswith('bs:Sig') or k.startswith('bs:Tilt'):

                    # reference for average
                    refkey  = 'bs:Sig-X' # must exist !
                    statkey = 'bs:Status' # must exist !

                    onloffltype = 'Online' if (run.stats['Beamspot']=='online') else 'Offline'

                    # given is beamspot position or width or tilt
                    xvec       = []
                    yvec       = []
                    yvecSt     = []
                    bsidx      = []
                    isOK       = False
                    vave       = 0
                    eave       = 0
                    iave       = float(0)
                    idx        = 0

                    for entry in run.data[k]:
                        if entry.startlb==0 and entry.lastlb==0:
                            continue

                        val,valerr = entry.value
                        ref  = run.data[refkey].atLB(entry.startlb)[0].value[0]
                        stat = run.data[statkey].atLB(entry.startlb)[0].value

                        lastlb = min(entry.lastlb,run.lastlb)
                        lbs = range(entry.startlb,lastlb+1)
                        nlb = len(lbs)

                        xvec    += [nlb]
                        yvec    += [(val,valerr)]# * nlb
                        yvecSt  += [stat]# * nlb
                        bsidx   += [idx] * nlb
                        idx     += 1

                        # compute average if BS has been determined
                        if ref < 10:
                            vave += nlb * val
                            eave += nlb * val**2
                            iave += nlb
                            isOK  = True

                    if not isOK:
                        s += '<td>n.a.</td>'
                        continue

                    if iave>0:
                        vave /= float(iave)
                        rad = eave/iave - vave*vave
                        if rad>=0:
                            eave = math.sqrt(rad)
                        else:
                            eave = 0
                    else:
                        vave = -1

                    ymin = vave - 3*eave
                    ymax = vave + 3*eave

                    if ymin >= ymax:
                        ymin = ymin*0.9
                        ymax = ymin*1.1
                        if ymin >= ymax:
                            ymin = -0.1
                            ymax = 0.1

                    if ymin==ymax:
                        delta = 0.1*abs(ymax)
                    else:
                        delta = 0.1*(ymax-ymin)
                    ymin -= delta
                    ymax += delta


                    bstype = 'pos'
                    if   'Sig'  in k:
                        bstype = 'width'
                    elif 'Tilt' in k:
                        bstype = 'tilt'
                    bstype += k[k.find('-'):] 

                    histoText = 'Average beam spot %s: (%.4g +- %.1g) mm' % (bstype, vave, eave)

                    path = makeBSPlots( xvec=xvec, yvec=yvec,
                                        xtitle = 'Luminosity block number', ytitle = '%s beamspot %s  (mm)' % (onloffltype, bstype),
                                        histname = '%s_vs_lb_run_%i' % (k.replace(':','_').replace('-','_'), run.runNr),
                                        histtitle = '%s beamspot %s per LB for run %i' % (onloffltype, bstype, run.runNr),
                                        datapath = Run.Datapath, ymin = ymin, ymax = ymax, printText = histoText )


                    # create window and tooltip
                    wincontent = '<table class=&quot;outer&quot; style=&quot;padding: 5px&quot;><tr><td>'
                    wincontent += '<strong><b>%s beamspot %s per LB for run %i:</b></strong><br><font size=&quot;-1&quot;><font color=&quot;#888888&quot;>(Begin/end of run: %s)</font></font><hr color=&quot;black&quot; size=1>' % (onloffltype, bstype, run.runNr, run.timestr('html', run.runNr != Run.runnropen))
                    wincontent += '<table style=&quot;padding: 0px&quot;><tr><td>'
                    wincontent += '<img src=&quot;%s&quot; align=&quot;left&quot;>' % (path)
                    wincontent += '</td></tr></table>'
                    wincontent += '<hr color=&quot;black&quot; size=1>'

                    wincontent += '<table style=&quot;width: auto; border: 0px solid; border-width: margin: 0 0 0 0; border-spacing: 0px; border-collapse: separate; padding: 0px; font-size: 90%; vertical-align:top; &quot;>'
                    wincontent += '<tr><th style=&quot;text-align:right&quot;>LB</th><th style=&quot;text-align:right&quot;>&nbsp;&nbsp;&nbsp;Start time<br> (%s)</th><th style=&quot;text-align:right&quot;>&nbsp;&nbsp;&nbsp;Duration<br>(sec)</th><th style=&quot;text-align:right&quot;>&nbsp;&nbsp;&nbsp;Beamspot %s<br>(mm)</th><th style=&quot;text-align:right&quot;>&nbsp;&nbsp;&nbsp;Fit&nbsp;status</th>' % (QC.tzdesc(),bstype)
                    wincontent += '</tr><tr style=&quot;background:%s&quot;>' % '#eeeeee'
                    for idx,(lbtime,lbendtime) in enumerate(run.lbtimes):
                        lb=idx+1
                        timetuple = time.localtime(lbtime/1.E9) if QC.localtime else time.gmtime(lbtime/1.E9)
                        wincontent += '<td style=&quot;text-align:right&quot;>&nbsp;&nbsp;%s</td>' % lb
                        wincontent += '<td style=&quot;text-align:right&quot;>&nbsp;%s</td>' % time.strftime("%X",timetuple)
                        wincontent += '<td style=&quot;text-align:right&quot;>%.2f</td>' % ((float(lbendtime)-float(lbtime))/1.E9)
                        wincontent += '<td style=&quot;text-align:right&quot;>%.4g &plusmn; %.2g</td>' % yvec[bsidx[idx]]
                        wincontent += '<td style=&quot;text-align:right&quot;>%i</td>' % yvecSt[bsidx[idx]]

                        if idx%2!=0:
                            col = '#eeeeee'
                        else:
                            col = '#ffffff'
                        wincontent += '</tr><tr style=&quot;background:%s&quot;>' % col
                    if run.runNr == Run.runnropen: # run still ongoing
                        wincontent += '<tr><td style=&quot;text-align:left&quot;colspan=&quot;3&quot;><i>&nbsp;&nbsp;Run still ongoing ...</i></td></tr>'
                    wincontent += '</tr></table>'

                    wincontent += """<hr color=&quot;red&quot; size=1><font color=&quot;#777777&quot;><font size=&quot;-1&quot;><i><font size=&quot;-2&quot;>Created by AtlRunQuery on: %s</font></i></font></td></tr></table><script type=&quot;text/javascript&quot;></script></body></html>""" % str(datetime.datetime.now())
                    wincontent += '</td></tr></table>'

                    openwincmd = '<a href="javascript:openWindow('
                    openwincmd += "'Print','<!DOCTYPE html PUBLIC &quot;-//W3C//DTD HTML 4.01 Transitional//EN&quot;>"
                    openwincmd += "<html><head><title>Run query result for the %s beamspot</title><LINK href=&quot;html/atlas-runquery-lb.css&quot; rel=&quot;stylesheet&quot; type=&quot;text/css&quot;></head>" % (onloffltype.lower())
                    openwincmd += "<body><table style=&quot;font-family: sans-serif; font-size: 85%&quot;>"
                    openwincmd += wincontent
                    openwincmd += "</table></body></html>"
                    openwincmd += "')\">"

                    # plot
                    fullprint  = '<table class="triggerlinktable">'
                    fullprint += '<tr><td colspan="2"><img src="%s" align="left" width="90px"></td></tr>' % path
                    fullprint += '<tr><td colspan="2"></td></tr>' 
                    if isOK:
                        fullprint += '<tr><td style="text-align:left">Average&nbsp;%s:</td></tr><tr><td style="text-align:left"><nobr>&nbsp;%0.3g&nbsp;&plusmn;&nbsp;%.2g&nbsp;(RMS)</nobr></td></tr>' % (bstype, vave, eave)
                    fullprint += '</table>'

                    # put into html page
                    s += '<td class="showTip OFLBS stream" style="text-decoration:none; text-align: right">%s%s</a></td>' % (openwincmd, fullprint)  
                    continue

                if "STR:" == k[0:4]:
                    streamName = k[4:]
                    if 'n.a.' in v:
                        s += '<td class="tdna">n.a.</td>'
                        continue

                    # compute approximative output rate in Hz
                    durInSec = (run.eor-run.sor)/1.0E9                
                    rate = 'n.a.&nbsp;Hz'
                    try:
                        if durInSec > 0:
                            rate = '%.3f&nbsp;Hz' % (v[0]/durInSec)
                    except ValueError:
                        pass
                    strevents = v[0]
                    strsize = v[1]
                    # add relative fraction (only if not calibrtion string)
                    fracstr = ''
                    if 'calibration' not in k:
                        fracstr = '%.1f%%,' % (float(strevents)/sumstrnev*100)                


                    ev = {}
                    xvec    = []
                    xvecStb = []
                    yvecN   = []
                    yvecR   = []
                    nevtot = 0

                    # statistics
                    maxrate    = -1
                    averate    = 0
                    nc         = 0

                    # insert events per LB
                    lbrecinfo = run.stats[k]['LBRecInfo']

                    isPhysicsStream = lbrecinfo is not None and len(lbrecinfo)>0
                    if isPhysicsStream: # only for physics stream we have LB wise information
                        ev = dict(lbrecinfo)
                        for lb in ev:
                            ev[lb]=0
                        for lb,nev in lbrecinfo:
                            ev[lb] += nev
                        for idx,(lbtime,lbendtime) in enumerate(run.lbtimes):
                            lb=idx+1                    
                            nev = 0
                            if lb in ev:
                                nev = ev[lb]
                            xvec.append(lb)
                            yvecN.append(nev)                        
                            yvecR.append(0)
                            nevtot += nev
                            try:
                                if float(lbendtime)-float(lbtime) > 0:
                                    yvecR[-1] = nev/((float(lbendtime)-float(lbtime))/1.E9)
                            except ValueError:
                                pass

                        lbs = ev.keys()
                        lbs.sort()

                        # find range with stable beams (requires LHC information to be available)
                        hasStableBeamsInfo, xvecStb = run.getStableBeamsVector()

                        # prepare pop-up window
                        wincontent  = '<table class=&quot;outer&quot; style=&quot;padding: 5px&quot;>'
                        wincontent += '<tr><td><b>Number of events and rates per LB for stream %s run %i:</b><br><font size=&quot;-1&quot;><font color=&quot;#888888&quot;>(Begin/end of run: %s)</font></font><hr color=&quot;black&quot; size=1>' % (streamName, run.runNr, run.timestr('html', run.runNr != Run.runnropen))

                        # create plot
                        pathN = makeLBPlot( xvec, xvecStb, yvecN,  'Luminosity block number', 'Number of events / LB', '',
                                            'nev_vs_lb_str_%s_run_%i' % (streamName, run.runNr),
                                            'N events per LB in stream "%s", run %i' % (streamName, run.runNr),
                                            Run.Datapath, 'Total number of events: %i' % nevtot )
                        pathR = makeLBPlot( xvec, xvecStb, yvecR, 'Luminosity block numer', 'Event rate [Hz] / LB', '',
                                            'rate_vs_lb_str_%s_run_%i' % (streamName, run.runNr),
                                            'Event rate per LB in stream "%s", run %i' % (streamName, run.runNr),
                                            Run.Datapath )
                        wincontent += '<table style="padding: 0px">\n<tr><td>'
                        wincontent += '<img src="%s" width="350">' % pathN.split('/')[-1]
                        wincontent += '</td><td>'
                        wincontent += '<img src="%s" width="350">' % pathR.split('/')[-1]
                        wincontent += '</td></tr></table>\n'
                        wincontent += '<hr color="black" size=1>\n'
                        wincontent += '<table class="lb">\n'
                        wincontent += '<tr><th>LB</th><th>Start time (%s)</th><th>Duration (sec)</th><th>Nevents</th><th>Cumul. Nevents</th><th>Rate [Hz]</th>'  % QC.tzdesc()
                        if hasStableBeamsInfo:
                            wincontent += '<th>Stable beams</th>'
                        wincontent += '</tr>\n<tr style="background:%s">' % '#eeeeee'
                        sumnev     = 0
                        for idx,(lbtime,lbendtime) in enumerate(run.lbtimes):
                            lb=idx+1                    
                            nev = 0
                            if lb in ev:
                                nev = ev[lb]
                            sumnev += nev
                            stbBeams = '&minus;'                                
                            if yvecR[idx] > 0:
                                averate += yvecR[idx]
                                if yvecR[idx] > maxrate:
                                    maxrate = yvecR[idx]
                                nc += 1
                            if lb in xvecStb:
                                stbBeams = 'T'

                            timetuple = time.localtime(lbtime/1.E9) if QC.localtime else time.gmtime(lbtime/1.E9)
                            wincontent += '<td>%s</td><td>%s</td><td>%.2f</td><td>%i</td><td>%i</td><td>%.3g</td>' % (lb, time.strftime("%X",timetuple), (float(lbendtime)-float(lbtime))/1.E9, int(yvecN[idx]), sumnev, yvecR[idx])
                            if hasStableBeamsInfo:
                                wincontent += '<td>%s</td>' % stbBeams
                            if idx%2!=0:
                                col = '#eeeeee'
                            else:
                                col = '#ffffff'
                            wincontent += '</tr>\n<tr style="background:%s">' % col

                        # compute max and averages
                        if nc>0:
                            averate    /= float(nc)

                        if run.runNr == Run.runnropen: # run still ongoing
                            wincontent += '<tr><td style="text-align:left"colspan="3"><i>&nbsp;&nbsp;Run still ongoing ...</i></td></tr>\n'
                        wincontent += '</tr></table>\n'
                        wincontent += '<hr color="red" size=1><font color="#777777"><font size="-1"><i><font size="-2">Created by AtlRunQuery on: %s</font></i></font></td></tr></table><script type="text/javascript"></script></body></html>' % str(datetime.datetime.now())
                        wincontent += '</td></tr></table>'
                    else:
                        wincontent = ""

                    # the output
                    tooltipkey = "STROV_%i_%s" % (run.runNr, streamName)

                    physStrOnlyInfo = ""
                    if isPhysicsStream:
                        maxrate = 'max:&nbsp;%.2f&nbsp;Hz' % (maxrate)
                        averate = 'ave:&nbsp;%.2f&nbsp;Hz' % (averate)
                        physStrOnlyInfo = "%s,<br>%s,<br>%s,&nbsp;" % (maxrate, averate, fracstr)
                    debugStrLink = ""
                    if 'debug_' in k:
                        debugStrLink = '&nbsp;<a href="https://trigmon.web.cern.ch/trigmon/ResultsAnalysis/RESULTS/%i/00%i_%s" target=_blank><font color="#993333">[MON]</font></a>' % (time.gmtime(run.sor/1.E9)[0], run.runNr, k[4:].strip())
                    evtsize = ',<br>%s/evt' % filesize(float(strsize)/float(strevents)) if (strevents>0) else ""
                    tdcontent = '%s <BR><font size="-2">(%s%s/run%s%s)' % (addKommaToNumber(strevents), physStrOnlyInfo, filesize(strsize), evtsize, debugStrLink)

                    # wrap content in div with tooltip
                    tdcontent = '<div class="showTip %s stream" style="display:inline;cursor:pointer">%s</div>' % (tooltipkey, tdcontent)

                    if isPhysicsStream:
                        # create popup page and wrap content into href
                        filename = HU.CreatePopupHtmlPage( 'streamov_%i_%s' % (run.runNr, streamName),  HU.WrapIntoHTML(wincontent, title="%s Stream Content" % streamName) )
                        tdcontent = '<a class="externalWindow" style="text-decoration:none" href="%s">%s</a>' % (filename, tdcontent)

                    s += '<td style="text-decoration:none">%s</td>' % (tdcontent)
                          
                    continue



                if k=="TriggerMenu":
                    chains = run.splitTriggerChains(v)
                    if len(chains)==0:
                        s += '<td></td>' 
                    elif len(chains)==1:
                        s += '<td style="min-width:300px"><font size="-2">%s</font></td>' % ( chains )
                    elif len(chains)==2:
                        s += '<td style="min-width:300px"><font size="-2">%s<hr color="gray" size=1>%s</font></td>' % ( chains )
                    else:
                        s += '<td style="min-width:300px"><font size="-2">%s<hr color="gray" size=1>%s<hr color="gray" size=1>%s</font></td>' % ( chains )
                    continue

                if k=="TriggerRates":
                    if type(v)==str:
                        s += '<td>%s</td>'%v
                    else:
                        tablecontent = ""
                        triggergroups,imgpaths,openwincmds = HU.createRatePopupWindow(v,run)
                        for idx,(trgroup,imgpath,openwincmd) in enumerate(zip(triggergroups,imgpaths,openwincmds)):
                            tooltipkey = "TrRate_Set%i_%i" % (idx,run.runNr)
                            contentpage = "%s/popupContent_trRates_%i_%i.html" % (Run.Datapath, run.runNr, idx)

                            if idx!=0:
                                tablecontent += '      <tr><td colspan="2"><hr color="gray"></td></tr>'
                            imgcontent = '<img src="%s" align="center" width="90px"><br>' % imgpath
                            imagerow = '<div class="showTip %s ratespopup" id="%s" style="display:inline;cursor:pointer;font-size:80%%;">%s</div>' % (tooltipkey, contentpage, imgcontent)
                            tablecontent += '      <tr><td  colspan="2" >%s</td></tr>' % imagerow
                            for tr,avr in trgroup:
                                tablecontent += '      <tr><td>%s</td><td>: %3.1f Hz</td></tr>' % (tr,avr)
                        s+="<td><table class='ratestable'>\n%s\n  </table></td>" % tablecontent
                    continue

                if "Datasets" == k:
                    # separate datasets into stream types
                    streamtypes = ['physics_','express_','calibration_','debug_', 'debugrec_']
                    steptypes   = ['.daq.','.recon.','.merge.','other','Reproc' ]
                    datatypes   = ['.RAW','.DRAW','.ESD.','.DESD','.AOD.','.TAG.','.TAG_COMM.','.HIST.','.NTUP','other']
                    vetotypes   = ['_TMP']
                    shownlist   = []

                    s += '<td>'
                    s += '<table class="datasettable">'
                    hrline = ''
                    allcafdsnames = ''
                    first = True
                    for stype in streamtypes:
                        for step in steptypes:
                            for dtype in datatypes:
                                selectedds = {}
                                for ds in v:
                                    if (stype in ds[0] and step in ds[0] and dtype in ds[0]) or (stype in ds[0] and stype == 'debug_' and step == 'other' and (dtype in ds[0] or dtype == 'other')) and not ds[0] in shownlist:
                                        vetoed = False
                                        for veto in vetotypes:
                                            if veto in ds[0]:
                                                vetoed = True
                                                break
                                        if not vetoed:
                                            # number of files, size, events
                                            st  = ['&nbsp;%s&nbsp;files' % (prettyNumber(ds[4]))]
                                            st += ['&nbsp;%s'         % (filesize(ds[5]))]
                                            if ds[6] == 0:
                                                st += ['&nbsp;unkn.&nbsp;evts&nbsp;']
                                            else:
                                                st += ['&nbsp;%s&nbsp;evts&nbsp;'  % (prettyNumber(ds[6]))]
                                            selectedds[ds[0]] = st
                                            # replication status
                                            if 'replicate:done' in ds[2]:
                                                selectedds[ds[0]] += [True]
                                            else:
                                                selectedds[ds[0]] += [False]
                                            if ds[3] == 'DONE':
                                                selectedds[ds[0]] += [True]
                                            else:
                                                selectedds[ds[0]] += [False]
                                            selectedds[ds[0]] += [ds[7]] # add dataset creation time
                                            shownlist.append(ds[0])                                    

                                if not selectedds:
                                    continue
                                dsnames = selectedds.keys()
                                dsnames.sort()
                                if not first:
                                    s += hrline
                                first = False
                                s += '<tr><td colspan="1"><font color="#666666"><i>[&nbsp;Stream&nbsp;type&nbsp;+&nbsp;processing&nbsp;step&nbsp;+&nbsp;data&nbsp;type:&nbsp;<b>%s</b>&nbsp;+&nbsp;<b>%s</b>&nbsp;+&nbsp;<b>%s</b>&nbsp;]</i> </font></td></tr>' % (stype, step, dtype)
                                for dsname in dsnames:
                                    # build AMI link to dataset
                                    dsnspl = dsname.split('.')
                                    lk = "https://ami.in2p3.fr/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.Command?Converter=/AMIXmlToAMIProdHtml.xsl&Command=FormBrowseDatasetPerParameter+-runNumber%3D"
                                    project = dsnspl[0]
                                    runNum  = dsnspl[1]
                                    stream  = dsnspl[2]
                                    format  = dsnspl[4]                            
                                    s += '<tr><td>'
                                    s += '<a href="https://ami.in2p3.fr/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.Command?Converter=/AMIXmlToAMIProdHtml.xsl&Command=FormBrowseDatasetPerParameter+-runNumber%3D'
                                    s += runNum + "+-dataType=" + format + "+-projectName=" + project + "+-streamName=" + stream
                                    s += '" target="_blank" title="AMI dataset information">'
                                    s += dsname + '</td>'
                                    res = selectedds[dsname]
                                    s += '<td style="text-align:right"><font color="#005500">&nbsp;[</font></td>' 
                                    for st in res[0:3]:
                                        s += '<td style="text-align:right"><font color="#005500">%s</font></td>' % st
                                    s += '<td style="text-align:right"><font color="#005500">]</font></td>'
                                    if res[3]:
                                        if Run.showCAFLinks:
                                            s += '<td><a href="javascript:openPageWindow('
                                            s += "'data/%s.html','%s')" % (dsname,dsname)
                                            s += '"title="Check which files of this dataset are available on ATLCAL disk pool">' 
                                            s += '<font color="#BB0517">&nbsp;[&nbsp;on&nbsp;CAF&nbsp;]</font></a></td>'

                                            # get age of file in hours
                                            deltat = (datetime.datetime.utcnow() - res[5])
                                            deltat = deltat.days*24.*3600. + deltat.seconds
                                            allcafdsnames += dsname + '@' + '%g' % deltat + ','                                    
                                        else:
                                            s +='<td><font color="#BB0517">&nbsp;[&nbsp;on&nbsp;CAF&nbsp;]</font></td>'
                                    else:
                                        s += '<td></td>'
                                    if res[4]:
                                        lk = '<a href="https://ami.in2p3.fr/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.Command?Command=FormGetDQ2InfoPerDataset+-logicalDatasetName%3D%22' + dsname + '%22&Converter=%2FAMIXmlToProdFrameHtml.xsl" target="_blank" title="DQ2 information (via AMI) for dataset: ' + dsname + '">'
                                        s += '<td>%s<font color="#153EBB">&nbsp;[&nbsp;in&nbsp;DDM&nbsp;]</font></a></td>' % lk
                                    else:
                                        s += '<td></td>'

                                    s += '</tr>'
                                if dtype != datatypes[-1]:
                                    hrline = '<tr><td colspan="8"><hr style="width:100%; background-color: #BBBBBB; height:1px; margin-left:0; border:0"/></td>'
                                else:
                                    hrline = ''

                            if step != steptypes[-1]:
                                hrline = '<tr><td colspan="8"><hr style="width:100%; background-color: #666666; height:2px; margin-left:0; border:0"/></td>'
                        if stype != streamtypes[-1]:
                            hrline = '<tr><td colspan="8"><hr style="width:100%; background-color: #6D7B8D; height:3px; margin-left:0; border:0"/></td>'
                    # sanity check
                    s_ = ''
                    for ds in v:
                        if 'TMP.' not in ds[0] and ds[0] not in shownlist:
                            s_ += '<tr><td colspan=8>' + ds[0] + '</td></tr>'
                    if s_:
                        s += '<tr><td colspan="8"><hr style="width:100%; background-color: #EE0000; height:3px; margin-left:0; border:0"/></td>'
                        s += '<tr><td colspan=8><font color="#EE0000"><i>Remaining, unassigned datasets:</i></font></td></tr>'
                        s += s_

                    # close table
                    s += '</table>\n'
                    s += '</td>'

                    # call subprocesses
                    if allcafdsnames:
                        subprocess.Popen('python subproc.py caf %s' % (allcafdsnames[:-1]), shell=True)

                    continue

                if 'HLT PSK' == k:
                    # add another column with links to trigger menu for all SMK and PSKs combinations
                    linklist = run.stats['PSK']['blocks']

                    if linklist:
                        if len(linklist)<=15:
                            ncol = 1
                        elif len(linklist)<=30:
                            ncol = 2
                        else:
                            ncol = 3
                        if len(linklist)<=15:
                            nrow = len(linklist)
                        elif len(linklist)<=45:
                            nrow = 15
                        else:
                            nrow = (len(linklist)+2)/3
                        
                        s += '<td align="center"><table class="triggerlinktable" align="center">'
                        s += '<tr>'
                        for x in range(ncol):
                            s += '<td style="text-align:center;font-weight:bold" colspan="3">LB&nbsp;range</td><td style="text-align:center;font-weight:bold">L1&nbsp;&nbsp;&nbsp;HLT</td>'
                        s += '</tr>'

                        for row in range(nrow):
                            s += '<tr>'
                            for col in range(ncol):
                                entry = row + col*nrow
                                if entry>=len(linklist):
                                    continue
                                link = linklist[entry]
                                s += '<td style="padding-left: 0.8em">%s</td><td>&minus;</td><td>%s:&nbsp;</td>' % (str(link[0]),str(link[1]))
                                lll = '<a href="https://atlas-trigconf.cern.ch/run/smkey/%s/l1key/%s/hltkey/%s/" target="_blank" title="L1 \'%s\', HLT \'%s\'. Obtain full trigger menu corresponding to SMK=%s and prescale keys L1=%s, HLT=%s, in independent window">' % (run.result['SMK'],link[2],link[3],link[4],link[5],run.result['SMK'],link[2],link[3])
                                s += '<td style="border-right: 1px solid gray; padding-right: 0.8em">%s%s&nbsp;|&nbsp;%s</a></td>' % (lll, link[2],link[3])
                            s += '</tr>'
                        s += '</table>'
                        s += '<hr style="width:100%; background-color: #AAAABB; height:1px; margin-left:0; border:0"/>'
                        s += '<a href="https://atlas-trigconf.cern.ch/psevo/%i/" target="_blank" title="Obtain prescale evolution for all L1/L2/EF chains of this run in independent window"><font size="-2"><i>Prescale&nbsp;evolution...</i></font></a>' % run.runNr
                        s+='</td>'
                    else:
                        s += '<td style="text-align:center">n.a.</td>'
                    continue

                if 'BGS Key' == k:
                    if v!='n.a.':
                        if len(run.stats['BGS Key' ]['blocks'])>0:
                            s += '<td align="center"><table class="triggerlinktable" align="center"><tr><td style="text-align:left;font-weight:bold" colspan="3">LB&nbsp;range</td><td style="text-align:right;font-weight:bold">BGS</td></tr>'
                            for bgskey, lbbeg, lbend in run.stats['BGS Key' ]['blocks']:
                                s += '<tr><td style="text-align:right">%i</td><td style="text-align:right">&minus;</td><td style="text-align:right">%i:&nbsp;</td>' % (lbbeg, lbend-1)
                                lll = '&nbsp;<a href="https://atlas-trigconf.cern.ch/bunchgroups?key=%s" target="_blank" title="Obtain bunch group description for BGS Key=%s in independent window">' % (bgskey,bgskey)
                                s += '<td>%s%s</a></td></tr>' % (lll, bgskey)
                            s += '</table>'
                            s+='</td>'
                        else:
                            s += '<td style="text-align:center">n.a.</td>'
                    else:
                        s += '<td class="tdna">n.a.</td>'
                    continue

                if "SMK" == k:
                    if v=='n.a.':
                        s += '<td class="tdna">n.a.</td>'
                    else:
                        namecomment = '<div style="font-size:80%%;color:#488AC7"><b>%s</b> (v.%i)</div><div style="width:200px;font-size:60%%;color:#777777;text-align:left">%s</div>' % run.stats['SMK']['info']
                        s += '<td style="text-align: center;">%s<br>%s</td>' % (v,namecomment)
                    continue

                if 'lar:runtype' in k or 'lar:format' in k:
                    info = LArConfig(k.replace('lar:',''))
                    try:
                        s += '<td style="text-align: center">%s<br><font size="-2">(%s)</font></td>' % (v,info[int(v)])
                    except ValueError:
                        if v=='n.a.':
                            s += '<td class="tdna">n.a.</td>'
                        else:
                            s += '<td style="text-align: right">%s</td>' % v
                    continue

                if 'BPM' == k:

                    # compute average and RMS BPMs            
                    fullprint = '<table class="triggerlinktable" align="center">'
                    keys      = v.keys()
                    keys.sort()

                    # time results
                    xvec    = []
                    yvec    = []
                    toffset = int(run.sor/1.E9)
                    deltat  = int(run.eor/1.E9) - int(run.sor/1.E9)

                    # LB axis
                    xveclb = []
                    for idx,(lbtime,lbendtime) in enumerate(run.lbtimes):
                        lb=idx+1
                        xveclb.append( [lbtime/1.E9-toffset, (float(lbendtime)-float(lbtime))/1.E9] )

                    ymin    = +1e30
                    ymax    = -1e30
                    legend  = []
                    for ik,key in enumerate(keys):
                        y  = 0
                        y2 = 0                
                        xvec.append([])
                        yvec.append([])
                        for (val,dte) in v[key]:
                            y  += val
                            y2 += val*val

                            ts   = time.strptime( dte[:dte.rfind(':')] + '/UTC','%d-%m-%Y %H:%M:%S/%Z' )
                            tsec = int(calendar.timegm(ts))                                        
                            xvec[-1].append(tsec - toffset) # time defined with respect to offset
                            yvec[-1].append(val)

                        ic = len(v[key])
                        if ic > 0:
                            y /= float(ic)
                            arg = y2/float(ic) - y*y
                            if arg > 0:
                                y2 = math.sqrt(arg)
                            else:
                                y2 = 0
                        ymin = min(ymin,y - 1.5*y2)
                        ymax = max(ymax,y + 1.5*y2)                

                        # fill table
                        vname = 'unknown'
                        if   'B1' in key and 'hori' in key:
                            vname = 'B1&nbsp;horiz.&nbsp;IP'
                            lname = 'B1 horizont IP pos.'
                        elif 'B1' in key and 'vert' in key:
                            vname = 'B1&nbsp;vert.&nbsp;IP'
                            lname = 'B1 vertical IP pos.'
                        elif 'B2' in key and 'hori' in key:
                            vname = 'B2&nbsp;horiz.&nbsp;IP'
                            lname = 'B2 horizontal IP pos.'
                        elif 'B2' in key and 'vert' in key:
                            vname = 'B2&nbsp;vert.&nbsp;IP'
                            lname = 'B2 vertical IP pos.'
                        legend.append( lname )
                        fullprint += '<tr><td style="text-align:left">%s:&nbsp;</td><td>(</td><td style="text-align:right">&nbsp;%s</td><td style="text-align:left">&nbsp;&#177;&nbsp;</td><td style="text-align:left">%.4g</td><td>)&nbsp;um</td><td>&nbsp;&nbsp;&nbsp;</td>' % (vname, ('%.4g' % y).replace('-','&minus;'), y2)

                        if ik == 0:
                            fullprint += '<td rowspan="4">REPLACEME</td>'
                        fullprint += '</tr>'

                    # write to file (as text and TGraphs)
                    bpmrootfilename = Run.Datapath + '/bpm_output_run%i.root' % run.runNr
                    SaveGraphsToFile( bpmrootfilename, keys, xvec, yvec, "TimeOffset", toffset )

                    bpmtextfilename = Run.Datapath + '/bpm_output_run%i.txt' % run.runNr
                    bpmout          = open( bpmtextfilename, 'w' )

                    bpmout.write( '# BPM Output for run %i\n' % run.runNr )
                    bpmout.write( '# Start - end of run: %s [UTC]\n' % run.timestr('txt') )
                    bpmout.write( '# Time offset applied below in seconds: %i\n' % int(toffset) )
                    bpmout.write( '#\n' )
                    bpmout.write( '# ---------------------------------------------------------------------------------\n' )
                    for ik,key in enumerate(keys):
                        bpmout.write( '# Output for variable: "%s"\n' % key )
                        bpmout.write( '# Format: index / timestamp (seconds) wrt. begin of run / BPM position in microns\n' )
                        bpmout.write( '#\n' )
                        for i in range(len(xvec[ik])):
                            bpmout.write( "%4i  %9.2f  %14.6f\n" % (i, xvec[ik][i], yvec[ik][i]) )
                        bpmout.write( '#\n' )
                        bpmout.write( '# ---------------------------------------------------------------------------------\n' )
                    bpmout.close()

                    fullprint += '<tr><td colspan=8><hr style="width:100%; background-color:#999999; height:1px; margin-left:0; border:0"/></td></tr>'
                    fullprint += '<tr><td colspan=8 style="text-align:left"><a href="%s" target="_blank" title="Tabulated BPM information versus time for run %i"><i>Click here for tabulated BPM values versus time...</i></a></td></tr>' % (bpmtextfilename, run.runNr)
                    fullprint += '<tr><td colspan=8 style="text-align:left"><a href="%s" target="_blank" title="BPM information versus time for run %i as four TGraph objects in ROOT file"><i>Right-click to download ROOT file with TGraphs...</i></a></td></tr>' % (bpmrootfilename, run.runNr)
                    fullprint += '</table>' 

                    # make plot
                    path = makeTimePlotList( xvec, xveclb, yvec, toffset, deltat, ymin, ymax,
                                             'Time of day (UTC)', 'Beam position (microns)', legend,
                                             'bp_vs_time_run_%i' % (run.runNr),
                                             'Beam position versus time of day for run %i' % (run.runNr),
                                             Run.Datapath, '' )

                    replacetxt = '<img src="%s" align="left" width="70px">' % path
                    fullprint = fullprint.replace('REPLACEME',replacetxt)

                    # create window and tooltip
                    wincontent = '<table class=&quot;outer&quot; style=&quot;padding: 5px&quot;><tr><td>'
                    wincontent += '<strong><b>LHC beam positions at IP verssu time for run %i:</b></strong><br><font size=&quot;-1&quot;><font color=&quot;#888888&quot;>(Begin/end of run: %s)</font></font><hr color=&quot;black&quot; size=1>' % (run.runNr, run.timestr('html', run.runNr != Run.runnropen))
                    wincontent += '<table style=&quot;padding: 0px&quot;><tr><td>'
                    wincontent += '<img src=&quot;%s&quot; align=&quot;left&quot;></td>' % path
                    wincontent += '</td></tr></table>'
                    wincontent += """<hr color=&quot;red&quot; size=1><font color=&quot;#777777&quot;><font size=&quot;-1&quot;><i><font size=&quot;-2&quot;>Created by AtlRunQuery on: %s</font></i></font></td></tr></table><script type=&quot;text/javascript&quot;></script></body></html>""" % str(datetime.datetime.now())
                    wincontent += '</td></tr></table>'


                    openwincmd = HU.OpenWindow(wincontent,title="Summary of LHC information")

                    # put into html page
                    s += '<td style="text-decoration:none">%s<div class="showTip BPM stream" style="display:inline;cursor:pointer">%s</div></a></td>' % (tooltipkey, fullprint)
                    continue

                if 'ofllumi:' in k:

                    if ofllumiFlag:
                        continue
                    ofllumiFlag = True

                    # find range with stable beams (requires LHC information to be available)
                    hasStableBeamsInfo, xvecStb = run.getStableBeamsVector()

                    # find all channels that should be printed
                    kref  = []
                    chans = []
                    tags  = []
                    for kp in Run.ShowOrder:
                        if 'ofllumi:' in kp.ResultKey:
                            name, chanstr, tagstr = kp.ResultKey.split(':')
                            kref.append(kp)
                            chans.append(chanstr)
                            tags.append(tagstr)

                    # get the channel names
                    chanstr = []
                    for ch in chans:
                        ich = int(ch)
                        if ich in OLCAlgorithms:
                            chanstr.append(OLCAlgorithms[ich])
                        else:
                            chanstr.append('Unknown')

                    # insert events per LB
                    xvec       = []
                    yvec       = []
                    peaklumi   = []
                    intlumi    = []
                    intlumiStb = []
                    for ikp,kp in enumerate(kref):
                        yvec.append([])
                        peaklumi.append(-1)
                        intlumi.append(0)
                        intlumiStb.append(0)
                        for entry in run.data[kp]:
                            if entry.startlb == 0:
                                continue
                            val = entry.value if entry.value!='n.a.' and entry.value>0 else 0
                            lastlb = min(entry.lastlb,run.lastlb)
                            lbs = range(entry.startlb,lastlb+1)
                            if ikp == 0:
                                xvec += lbs
                            yvec[-1] += len(lbs)*[float(val)]


                    for idx,(lbtime,lbendtime) in enumerate(run.lbtimes):
                        lb=idx+1                    
                        dt = (float(lbendtime)-float(lbtime))/1.E9
                        for ich in range(len(chans)):
                            tmp = yvec[ich]
                            intlumi[ich]   += tmp[idx]*dt/1000.0  # unit of yvec was: ub-1 s-1 -> nb-1
                            if lb in xvecStb:
                                intlumiStb[ich] += yvec[ich][idx]*dt/1000.0

                            yvec[ich][idx] *= 1.0                # unit of yvec was: ub-1 s-1, transform to: 10^30 cm-2s-1
                            if yvec[ich][idx] > peaklumi[ich]:
                                peaklumi[ich] = yvec[ich][idx]

                    # line to be printed in column
                    fullprint_ = ''
                    if hasStableBeamsInfo:
                        for ich,ch in enumerate(chans):
                            fullprint_ += 'Integrated&nbsp;(channel:&nbsp;%s):&nbsp;%0.4g<br>Stable&nbsp;B:&nbsp;%0.4g<br>' % (ch,intlumi[ich],intlumiStb[ich])                                
                        histoText  = 'Integr. lumi (channel: %i): %.4g nb-1 (all LBs) and %.4g nb-1 (stable beams)' % (0, intlumi[0],intlumiStb[0])
                    else:
                        for ich,ch in enumerate(chans):
                            fullprint_ += 'Integrated&nbsp;(channel:&nbsp;%s):&nbsp;%0.4g<br>' % (ch,intlumi[ich])
                        histoText  = 'Integr. lumi (channel: %i): %.4g nb-1 (all LBs)' % (0, intlumi[0])

                    # make plot
                    path = makeLBPlotList( xvec, xvecStb, yvec,
                                           'Luminosity block number', 'Offline luminosity (10^{30}cm^{-2}s^{-1})', chanstr,
                                           'ofllumi%s_vs_lb_run_%i' % (chans[0],run.runNr),
                                           'Offline lumi per LB for run %i' % (run.runNr),
                                           Run.Datapath, histoText )

                    # create window and tooltip
                    wincontent  = '<table class=&quot;outer&quot; style=&quot;padding: 5px&quot;><tr><td>'
                    wincontent += '<strong><b>Offline luminosity per LB for run %i:</b></strong><br><font size=&quot;-1&quot; color=&quot;#888888&quot;>(Begin/end of run: %s)</font>' % (run.runNr, run.timestr('html', run.runNr != Run.runnropen))
                    wincontent += '<hr color=&quot;black&quot; size=1>'
                    wincontent += '<table style=&quot;padding: 0px&quot;><tr><td>'
                    wincontent += '<img src=&quot;%s&quot; align=&quot;left&quot;>' % path
                    wincontent += '</td></tr></table>'
                    wincontent += '<hr color=&quot;black&quot; size=1>'
                    wincontent += '<table class=&quot;LB&quot;>'
                    wincontent += '<tr><th>LB</th><th>Start time<br> (%s)</th><th>Duration<br>(sec)</th>' % QC.tzdesc()
                    for ch in chanstr:
                        wincontent += '<th>Inst. luminosity<br>(1e%s&nbsp;cm<sup>&minus;2</sup>s<sup>&minus;1</sup>)<br>[ %s ]</th>' % (run.instlumiunittxt,ch)
                    wincontent += '<th>Stable beams</th>'
                    wincontent += '</tr><tr style=&quot;background:%s&quot;>' % '#eeeeee'
                    for idx,(lbtime,lbendtime) in enumerate(run.lbtimes):
                        lb=idx+1                    
                        stbBeams = '&minus;'
                        if lb in xvecStb:
                            stbBeams = 'T'
                        timetuple = time.localtime(lbtime/1.E9) if QC.localtime else time.gmtime(lbtime/1.E9)
                        wincontent += '<td>%i</td><td>%s</td><td>%.2f</td>' % (lb, time.strftime("%X",timetuple), (float(lbendtime)-float(lbtime))/1.E9)
                        for ich in range(len(chans)):
                            wincontent += '<td>%.4g</td>' % (yvec[ich][idx])
                        wincontent += '<td>%s</td>' % stbBeams
                        if idx%2!=0:
                            col = '#eeeeee'
                        else:
                            col = '#ffffff'
                        wincontent += '</tr><tr style=&quot;background:%s&quot;>' % col
                    if run.runNr == Run.runnropen: # run still ongoing
                        wincontent += '<tr><td style=&quot;text-align:left&quot;colspan=&quot;3&quot;><i>&nbsp;&nbsp;Run still ongoing ...</i></td></tr>'
                    wincontent += '</tr></table>'
                    wincontent += """<hr color=&quot;red&quot; size=1><font color=&quot;#777777&quot;><font size=&quot;-1&quot;><i><font size=&quot;-2&quot;>Created by AtlRunQuery on: %s</font></i></font></td></tr></table><script type=&quot;text/javascript&quot;></script></body></html>""" % str(datetime.datetime.now())
                    wincontent += '</td></tr></table>'

                    openwincmd = HU.OpenWindow(wincontent,title="Run query result for online luminosity")

                    # line to be printed in column
                    fullprint  = '<table class="bcidtable">'
                    fullprint += '<tr><td colspan="2"><img src="%s" align="left" width="90px"></td></tr>' % path
                    if hasStableBeamsInfo:
                        for ich in range(len(chans)):
                            fullprint += '<tr><td style="text-align:left">Entire&nbsp;run:</td><td style="text-align:left">&nbsp;%0.4g&nbsp;%s<sup>&minus;1</sup></td></tr><tr><td style="text-align:left">Stable&nbsp;beams:</td><td style="text-align:left">&nbsp;%0.4g&nbsp;%s<sup>&minus;1</sup></td></tr><tr><td style="text-align:left">Peak&nbsp;lumi:</td><td style="text-align:left">&nbsp;%0.3g&nbsp;e%s&nbsp;cm<sup>&minus;2</sup>s<sup>&minus;1</sup></td></tr>' % (intlumi[ich], run.lumiunittxt, intlumiStb[ich], run.lumiunittxt, peaklumi[ich], run.instlumiunittxt)
                    else:
                        for ich in range(len(chans)):
                            fullprint += '<td style="text-align:left">All&nbsp;LBs:</td><td style="text-align:left">&nbsp;%0.4g</td></tr>' % (intlumi[ich])
                    fullprint += '</table>'

                    # put into html page
                    s += '<td class="showTip OFLLumi stream" style="text-decoration:none; text-align: right">%s%s</a></td>' % (openwincmd, fullprint)  
                    continue

                if any( [ ('lhc:' in k and 'beamenergy' not in k),
                          ('TorCurrent' in k),
                          ('SolCurrent' in k),
                          ('bs:' in k),
                          ('Ready for physics' in k),
                          ] ):
                    res  = []
                    keys = []

                    # search unique ranges            
                    for entry in run.data[k]:
                        if entry.startlb == 0:
                            continue
                        val = entry.value

                        try:
                            val = float(val)
                            if 'bs:' not in k and 'ofllumi:' not in k and 'beam1intensity' not in k and 'beam2intensity' not in k:
                                val = round(val) # no floating points, unless it is 'bs'
                        except ValueError:
                            pass

                        # special treatment for E-beam = 7864
                        if 'beamenergy' in k and val>=7864:
                            val = 'ASYNC'

                        if not keys or res[-1] != val:
                            if keys:
                                keys[-1][1] = entry.startlb
                            res.append ( val )
                            keys.append( [entry.startlb, entry.endlb] )
                        elif res[-1] == val:
                            keys[-1][1] = entry.endlb


                    if len(keys) > 1:
                        s += '<td><table class="triggerlinktable" align="center">'
                        for i, c in enumerate(keys):
                            if int(c[0]) != int(c[1])-1:
                                s += '<tr><td>LB&nbsp;</td><td style="text-align:right">%i</td><td style="text-align:right">&minus;</td><td style="text-align:right">%i:&nbsp;</td>' % (int(c[0]),int(c[1])-1)
                            else: 
                                s += '<tr><td>LB&nbsp;</td><td style="text-align:right">%i</td><td style="text-align:right"></td><td style="text-align:right">:&nbsp;</td>' % (int(c[0]))
                            try:
                                val = float(res[i])
                                if 'bs:' in k or 'beam1intensity' in k or 'beam2intensity' in k:
                                    vt = '%.4g' % abs(val)
                                    if val < 0:
                                        vt = '&minus;' + vt
                                    if 'e-' in vt:
                                        vt = vt.replace('e-','e&minus;')
                                    s += '<td style="text-align:right">%s</td></tr>' % vt
                                elif 'ofllumi:' in k:
                                    s += '<td style="text-align:right">%g</td></tr>' % val
                                else:
                                    s += '<td style="text-align:right">%i</td></tr>' % round(val)
                            except ValueError:
                                s += '<td style="text-align:left">%s</td></tr>' % res[i].strip()
                        s += '</table></td>'            

                    else:
                        try:
                            if 'bs:' in k:
                                vt = '%.4g' % abs(float(v))
                                if float(v) < 0:
                                    v = '&minus;' + vt
                                else:
                                    v = vt
                                if 'e-' in v:
                                    v = v.replace('e-','e&minus;')
                            else:
                                v = round(float(v))
                                if 'beamenergy' in k and v >= 7864:
                                    v = 'ASYNC'
                                else:
                                    v = '%i' % v                    
                        except ValueError:
                            pass
                        s += '<td style="text-align:center;font-size: 85%%">%s</td>' % v


                    if ('lhc:stablebeams' in k and 'lhc:beamenergy' in Run.ShowOrder and 'lhc:beammode' not in Run.ShowOrder) or 'lhc:beammode' in k:
                        imgpath, xvec, yvec, ymax = HU.makeSummaryPlotForLHC(run)
                        wincontent = HU.makeSummaryPageForLHC(run, yvec, imgpath)
                        openwincmd = HU.OpenWindow(wincontent, 'Summary of LHC information')
                        fullprint  = '<table class="bcidtable">'
                        fullprint += '<tr><td><img src="%s" align="left" width="90px"></td></tr>' % imgpath
                        fullprint += '<tr><td style="text-align:left">Maximum&nbsp;intensities:<br>Beam&nbsp;1:&nbsp;%0.3g&nbsp;e11&nbsp;protons<br>Beam&nbsp;2:&nbsp;%0.3g&nbsp;e11&nbsp;protons</td></tr>' % (ymax[0], ymax[1])
                        fullprint += '<tr><td style="text-align:left">Maximum&nbsp;beam&nbsp;energy:<br>%i&nbsp;GeV</td></tr>' % int(ymax[2])
                        fullprint += '</table>'
                        fullprint += '<span style="font-size: xx-small; color: #555555">[ <i>Numbers given for stable beam period (if applies)</i> ]</span>'
                        # put into html page
                        s += '<td class="showTip LHCSummary stream" style="text-decoration:none; text-align: left">%s %s </a></td>' % (openwincmd, fullprint)  

                    continue

                # all others
                if v=='n.a.':
                    s += '<td class="tdna">n.a.</td>'
                else:
                    s += '<td style="text-align: right">%s</td>' % v


    s += "</tr>\n\n"

    ###
    ###  TOOLTIPS
    ###

    # here we start filling the tooltips. there are quite a lot and we should be a bit smarter about them.
    # For instance, there is no need to define the whole table for each single box

    with timer("Tooltips"):
        createToolTips(run)

    return s



def createToolTips(run):
    import CoolRunQuery.html.AtlRunQueryHtmlUtils as HU

    HU.CreateLBTooltip(run)

    # DQ DEFECTS
    if 'DQ' in Run.ShowOrder:
        HU.CreateDQTooltip(run)

    # stream overlaps
    for k in [k for k in Run.ShowOrder if k.Type==DataKey.STREAM and k.ResultKey in run.stats]:
        HU.CreateStreamOverlapTooltip(run, k)

    # DQ (old)
    for data_key in Run.ShowOrder:
        k = data_key.ResultKey
        if not isDQ(k):
            continue
        if k not in run.stats:
            continue
        commentbox = ''
        for (dq,comment), start, end in run.stats[k]["blocks"]:
            if dq != 'n.a.':
                if not comment:
                    comment = 'no comment'
                col = "#000000"
                if   dq == 'R':
                    col = "#AA0000"
                elif dq == 'G':
                    col = "#006600"
                elif dq == 'Y':
                    col = "#716309"
                if start == end-1:
                    commentbox += '<tr style="color:%s"><td style="vertical-align:top;">LB&nbsp;</td><td style="text-align:right;vertical-align:top;">%s</td><td style="text-align:right;vertical-align:top;"></td><td style="text-align:right;vertical-align:top;">&nbsp;(%s)&nbsp;:&nbsp;</td><td style="text-align:left;vertical-align:top;"><i><strong><b>"%s"</b></strong></i></td></tr>' % (col,start,dq,comment)
                else:
                    commentbox += '<tr style="color:%s"><td style="vertical-align:top;">LB&nbsp;</td><td style="text-align:right;vertical-align:top;">%s</td><td style="text-align:right;vertical-align:top;">&minus;</td><td style="text-align:right;vertical-align:top;">%s&nbsp;(%s)&nbsp;:&nbsp;</td><td style="text-align:left;vertical-align:top;"><i><strong><b>"%s"</b></strong></i></td></tr>' % (col,start,end-1,dq,comment)
            else:
                if start == end-1:
                    commentbox += '<tr style="color:#222222"><td style="vertical-align:top;">LB&nbsp;</td><td style="text-align:right;vertical-align:top;">%s</td><td style="text-align:right;vertical-align:top;"></td><td style="text-align:right;vertical-align:top;">&nbsp;(n.a.)&nbsp;</td><td style="text-align:left;vertical-align:top;"></td></tr>' % (start)
                else:
                    commentbox += '<tr style="color:#222222"><td style="vertical-align:top;">LB&nbsp;</td><td style="text-align:right;vertical-align:top;">%s</td><td style="text-align:right;vertical-align:top;">&minus;</td><td style="text-align:right;vertical-align:top;">%s&nbsp;(n.a.)&nbsp;</td><td style="text-align:left;vertical-align:top;"></td></tr>' % (start,end-1)
        if commentbox:
            sp = k.split(':')
            commentbox = '<strong><b>Comments&nbsp;for&nbsp;%s&nbsp;DQ&nbsp;channel&nbsp;"%s"&nbsp;in&nbsp;run&nbsp;%s:</b></strong>' % (sp[0],sp[1],run.runNr) + '<table>' + commentbox
            commentbox += '</table>'
            run.addToolTip( "dqcomm_%s_%i" % (k, run.runNr), commentbox)


    # trigger keys
    lbtrigtypes = { 'L1 PSK': ['L1&nbsp;PSK', 'L1K'], 'HLT PSK' : ['HLT&nbsp;PSK','HLTK'] }
    for lbtrigtype, c in lbtrigtypes.iteritems():
        if lbtrigtype in Run.ShowOrder and 'blocks' in run.stats[lbtrigtype] and len(run.stats[lbtrigtype]['blocks'])>1:
            boxcontent = '<strong><b>%s&nbsp;evolved&nbsp;during&nbsp;run:</b></strong><br>' % c[0]
            boxcontent += '<table style="width: auto; border: 0px solid; border-width: margin: 0 0 0 0; 0px; border-spacing: 0px; border-collapse: separate; padding: 0px; font-size: 100%">'
            for item, start, stop in run.stats[lbtrigtype]['blocks']:
                boxcontent += '<tr><td>LB&nbsp;</td><td style="text-align:right">%s</td><td style="text-align:right">&minus;</td><td style="text-align:right">%s</td><td style="text-align:right">:&nbsp;%s</td></tr>' % (start,stop-1,item)
            boxcontent += '</table>'
            run.addToolTip("%s_%i" % (c[1], run.runNr), boxcontent)


    # OLC BCID
    if any([k for k in Run.ShowOrder if "olc:bcidmask" in k.ResultKey]):
        boxcontent = Run.TmpBoxContent
        if boxcontent:
            Run.addGlobalToolTip("OLCBCID_%i" % run.runNr, boxcontent)

