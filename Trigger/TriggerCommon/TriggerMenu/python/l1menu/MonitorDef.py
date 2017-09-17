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

from TriggerMenu.l1.Lvl1MonCounters import Lvl1CtpinCounter, Lvl1CtpmonCounter

class MonitorDef:

    LOW_FREQ = 0
    HIGH_FREQ = 1


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
        monItemsHF = { 1 :[], 2: [], 3: [], 4: [], 5: [], 6: [], 7: [] }

        # definitions hardcoded at the moment

#        monItems[TBP] = [
#            "L1_MBTSA0", "L1_MBTSA1", "L1_MBTSA2", "L1_MBTSA3", "L1_MBTSA4", "L1_MBTSA5", "L1_MBTSA6", "L1_MBTSA7", "L1_MBTSA8", "L1_MBTSA10", "L1_MBTSA12", "L1_MBTSA14", 
#            "L1_MBTSC0", "L1_MBTSC1", "L1_MBTSC2", "L1_MBTSC3", "L1_MBTSC4", "L1_MBTSC5", "L1_MBTSC6", "L1_MBTSC7", "L1_MBTSC8", "L1_MBTSC10", "L1_MBTSC12", "L1_MBTSC14",
#            "L1_LUCID", "L1_LUCID_A_C_EMPTY",
#            "L1_LHCF",
#            "L1_ALFA_A7R1U",
#            "L1_ALFA_B7L1_OD","L1_ALFA_A7L1_OD","L1_ALFA_A7R1_OD","L1_ALFA_B7R1_OD",
#            "L1_BPTX0_BGRP0", "L1_BPTX1_BGRP0",
#            ]
#
#        
#        monItems[TAP|TAV] = [
#            "L1_EM7", "L1_EM12", "L1_EM8VH", "L1_EM10VH", "L1_EM15", "L1_EM15HI", "L1_EM15VH", 
#            "L1_MU6", "L1_MU10", "L1_MU11", "L1_MU15", "L1_MU20",
#            "L1_TAU20", "L1_TAU20IL", "L1_TAU20IM", "L1_TAU20IT", 
#            "L1_J20", "L1_J100", 
#            "L1_XE80", "L1_XS65", "L1_TE50",
#            "L1_MBTS_1", "L1_MBTS_2", "L1_MBTS_1_1",
#            ]
#        
#        monItems[TBP|TAP|TAV] = [
#            "L1_EM3", "L1_MU4", "L1_TAU12", "L1_J12", "L1_XE35", "L1_XS20", "L1_TE20",
#            "L1_MBTS_4_A", "L1_MBTS_4_C", 
#            "L1_HT200-J20s5.ETA49", "L1_HT1-J0.ETA49", "L1_HT190-J20s5.ETA49", "L1_JPSI-1M5-EMs", "L1_JPSI-1M5-EM6s", "L1_JPSI-1M5-EM12s",
#            "L1_MJJ-4", "L1_MJJ-3", "L1_MJJ-2", "L1_MJJ-1", "L1_MJJ-350-0", "L1_MJJ-300-0", "L1_MJJ-250-0", "L1_MJJ-200-0", 
#            "L1_RD0_FILLED", "L1_RD1_FILLED", "L1_RD2_FILLED", "L1_RD3_FILLED", 
#            ]
#
#        monItemsHF[TBP] = [
#            "L1_ALFA_ANY", 
#            "L1_ALFA_A7R1L","L1_ALFA_B7R1U","L1_ALFA_B7R1L", "L1_ALFA_B7L1U", "L1_ALFA_B7L1L","L1_ALFA_A7L1U","L1_ALFA_A7L1L",
#            ]
#
#
#
#        monItemsHF[TBP|TAP|TAV] = [
#            "L1_BCM_Wide_BGRP0",
#            "L1_BCM_AC_CA_BGRP0",
#            "L1_BCM_AC_UNPAIRED_ISO",
#            "L1_BCM_CA_UNPAIRED_ISO",
#            "L1_J12",
#            "L1_MBTS_1", "L1_MBTS_2", "L1_MBTS_1_1",
#            ]



        monItems[TBP|TAP|TAV] = [
            "L1_AFP_NSA_BGRP0", "L1_AFP_NSC_BGRP0",
            "L1_AFP_FSA_SIT_BGRP0", "L1_AFP_FSC_SIT_BGRP0",
            "L1_AFP_A_SPECTOF_BGRP0", "L1_AFP_C_SPECTOF_BGRP0",
            "L1_EM12", "L1_EM22VHI", "L1_EM24VHI", "L1_EM24VHIM",
            "L1_MU4", "L1_MU10", "L1_MU11", "L1_MU21", "L1_MU20",
            "L1_TAU12IM", "L1_TAU20", "L1_TAU40", "L1_TAU100",
            "L1_J20", "L1_J100",
            "L1_J75.31ETA49",
            "L1_4J15.0ETA25",
            "L1_XE35", "L1_XE60",
            "L1_TE50",
            "L1_MBTS_4_A", "L1_MBTS_4_C",
            "L1_LUCID",
            "L1_ZB",
            "L1_BPTX0_BGRP0", "L1_BPTX1_BGRP0",
            "L1_LAR-EM", "L1_LAR-J",
            "L1_HT150-J20s5.ETA31",
            "L1_MJJ-700",
            "L1_EM15TAU12I-J25",
            "L1_DY-BOX-2MU6",
            "L1_BPH-2M9-2MU6_BPH-2DR15-2MU6",
            "L1_MU6MU4-BO",
            "L1_BTAG-MU4J15",
            "L1_LFV-EM8I",
            "L1_SC85-CJ15",
            "L1_LFV-MU",
            "L1_HT150-J20.ETA31",
            "L1_DR-TAU20ITAU12I",
            "L1_KF-XE40",
            "L1_DR-TAU20ITAU12I-J25",
            "L1_JPSI-1M5",
            "L1_JPSI-1M5-EM7",
            "L1_JPSI-1M5-EM12",
            "L1_LATE-MU10_XE40",
            "L1_LATE-MU10_J50",
            "L1_LUCID_BGRP9",
            "L1_LUCID_BGRP11",
            ]

        monItemsHF[TBP|TAP|TAV] = [
            "L1_BCM_Wide_BGRP0",
            "L1_BCM_AC_CA_BGRP0",
            "L1_BCM_AC_UNPAIRED_ISO",
            "L1_BCM_CA_UNPAIRED_ISO",
            "L1_J12",
            "L1_MBTS_1", "L1_MBTS_2", "L1_MBTS_1_1",
            ]



        check = False
        if check:
            counts_LF_items = { TBP : set(), TAP : set(), TAV : set() }
            counts_HF_items = { TBP : set(), TAP : set(), TAV : set() }

            for k in range(1,8):

                if k & TBP:
                    counts_LF_items[TBP].update( monItems[k] )
                    counts_HF_items[TBP].update( monItemsHF[k] )

                if k & TAP:
                    counts_LF_items[TAP].update( monItems[k] )
                    counts_HF_items[TAP].update( monItemsHF[k] )

                if k & TAV:
                    counts_LF_items[TAV].update( monItems[k] )
                    counts_HF_items[TAV].update( monItemsHF[k] )

            counts_LF = dict( map(lambda (x,y) : (x,len(y)), counts_LF_items.items() ) )
            counts_HF = dict( map(lambda (x,y) : (x,len(y)), counts_HF_items.items() ) )

            lutsLF = ( max(counts_LF.values())-1) / 8 + 1
            lutsHF = ( max(counts_HF.values())-1) / 8 + 1

            if lutsLF + lutsHF > 8:
                print "WARNING: too many monitoring items are defined:"
                print "   low frequency  TBP: %i" % counts_LF[TBP]
                print "                  TAP: %i" % counts_LF[TAP]
                print "                  TAV: %i" % counts_LF[TAV]
                print "   required LUTs: %i" % lutsLF
                print "   high frequency TBP: %i" % counts_HF[TBP]
                print "                  TAP: %i" % counts_HF[TAP]
                print "                  TAV: %i" % counts_HF[TAV]
                print "   required LUTs: %i" % lutsHF
                print "   this menu requires %i monitoring LUTs while only 8 are available" % (lutsLF + lutsHF)
            


        #MonitorDef.checkForNonExistingMonItems(items, monItems)

        # for each item set the monitor flags
        for item in items:

            itemName = item.name
            for k,l in monItems.items():

                if itemName in l:
                    item.addMonitor(k, MonitorDef.LOW_FREQ)


            for k,l in monItemsHF.items():

                if itemName in l:
                    item.addMonitor(k, MonitorDef.HIGH_FREQ)






    @staticmethod
    def checkForNonExistingMonItems(items, monItems):
        # check is based on item names
        allItemNames = [item.name for item in items]

        # unify all item names that are monitored
        allMonitorItems = set()
        for i in range(1,8):
            allMonitorItems.update(monItems[i])

        # register all monitems that don't exist in here
        nonExistiginMonItems = []
        
        for monItem in allMonitorItems:
            if monItem not in allItemNames:
                nonExistiginMonItems += [monItem]

        if len(nonExistiginMonItems)>0:
            raise RuntimeError("These monitoring items are part of the menu: %s" % ','.join(nonExistiginMonItems))
