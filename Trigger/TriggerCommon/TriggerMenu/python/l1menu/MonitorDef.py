# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
The CTP monitors three different types of signals. In the XML file
they are listed in the section <TriggerCounterList>

1) type CTPIN

Almost each input threshold is monitored with one counter per
multiplicity it can have. E.g. there are 7 counters for the EM10VH
threshold: "1EM10VH" ... "7EM10VH"

Due to limitations of the CTPIN monitoring LUT (there are only 4 LUT
per CTPIN connector and a LUT has 8 bit input) only 2 3-bit thresholds
can be monitored per LUT, so max 8 3-bit thresholds per
connector. Hence JET thresholds 8 and 9 can not be monitored.


2) type CTPMON

This allows to monitor any combination of conditions built in the same
way as L1 Items. However, since we can also monitor L1 Items directly
(see 3)), there is little need for this type of monitoring. We use it to monitor simple conditions


3) type TBP, TAP, TAV

For each of these type 64 L1Items can be monitored independently

"""

from TriggerMenu.l1.Lvl1Menu import Lvl1Menu
from TriggerMenu.l1.Lvl1MonCounters import Lvl1MonCounters, Lvl1CtpinCounter, Lvl1CtpmonCounter

class MonitorDef:

    # CTPIN counters
    # these are generated for all CTPIN except the two highest JET inputs and the direct inputs
    @staticmethod
    def ctpinCounters( thresholds ):

        counters = []

        for thr in thresholds:
            # this special check addresses the LUT size issue for the monitoring (see file header and Cabling.py)
            dontGenerateCounter = (thr.ttype=="JET" and (thr.mapping==8 or thr.mapping==9)) \
                                  or thr.ttype=="TOPO" or thr.ttype=="ALFA"
            if dontGenerateCounter: continue
            for mult in range(1, 2**thr.cableinfo.bitnum):
                counters += [ Lvl1CtpinCounter(thr.name,mult) ]

        return counters


    # CTPMON counters
    # we only have a few for the moment
    @staticmethod
    def ctpmonCounters( thresholds ):

        counters = []

        from operator import attrgetter
        emthresholds = sorted([thr for thr in thresholds if thr.ttype=='EM'], key = attrgetter('mapping'))
        
        thrNames = [thr.name for thr in thresholds]
        if "MBTS_A" in thrNames:
            counters += [ Lvl1CtpmonCounter("MBTS_A",1) ]
        if "MBTS_C" in thrNames:
            counters += [ Lvl1CtpmonCounter("MBTS_C",1) ]
        if emthresholds:
            counters += [ Lvl1CtpmonCounter(emthresholds[0].name,1) ] # lowest EM threshold

        return counters

    @staticmethod
    def applyItemCounter( items ):

        TBP=1
        TAP=2
        TAV=4

        monItems = { 1 :[], 2: [], 3: [], 4: [], 5: [], 6: [], 7: [] }

        # definitions hardcoded at the moment

        monItems[TBP] = [
            "L1_EM3",
            "L1_EM7"
            ]

        monItems[TBP|TAP] = [
            "L1_EM12",
            "L1_EM15"
            ]
        
        monItems[TAP|TAV] = [
            "L1_EM8VH",
            "L1_EM10VH"
            ]
        
        monItems[TBP|TAP|TAV] = [
            "L1_EM8I",
            "L1_EM15I",
            "L1_EM15VH",
            "L1_EM18VH"
            ]

        # for each item set the monitor flags
        for item in items:

            itemName = item.name
            for k,l in monItems.items():

                if  (k & TBP) and itemName in l:
                    item.addMonitor("TBP")

                if  (k & TAP) and itemName in l:
                    item.addMonitor("TAP")

                if  (k & TAV) and itemName in l:
                    item.addMonitor("TAV")

