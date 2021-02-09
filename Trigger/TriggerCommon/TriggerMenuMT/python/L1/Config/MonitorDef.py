# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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

from AthenaCommon.Logging import logging
log = logging.getLogger('Menu.L1.Config.MonitorDef')

from ..Base.MonCounters import CtpinCounter, CtpmonCounter

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
            if dontGenerateCounter:
                continue
            for mult in range(1, 2**thr.cableinfo.bitnum):
                counters += [ CtpinCounter(thr.name,mult) ]

        return counters




    # CTPMON counters
    # we only have a few for the moment
    @staticmethod
    def ctpmonCounters( thresholds ):

        counters = []

        cThr = {}
        cThr[1] = [ 'AFP_FSA_SIT', 'AFP_FSA_TOF', 'AFP_FSC_SIT', 'AFP_FSC_TOF', 'AFP_NSA', 'AFP_NSC',
                    'BPTX0', 'BPTX1', 'LUCID_C', 'J20', 'MU4', 'TE50', 'XE35', 'XE60',
                    'MBTS_A0', 'MBTS_A1', 'MBTS_A2',  'MBTS_A3',  'MBTS_A4',  'MBTS_A5',  'MBTS_A6',  'MBTS_A7',
                    'MBTS_A8', 'MBTS_A9', 'MBTS_A10', 'MBTS_A11', 'MBTS_A12', 'MBTS_A13', 'MBTS_A14', 'MBTS_A15',
                    'MBTS_C0', 'MBTS_C1', 'MBTS_C2',  'MBTS_C3',  'MBTS_C4',  'MBTS_C5',  'MBTS_C6',  'MBTS_C7',
                    'MBTS_C8', 'MBTS_C9', 'MBTS_C10', 'MBTS_C11', 'MBTS_C12', 'MBTS_C13', 'MBTS_C14', 'MBTS_C15' ]

        for mult in cThr:
            for thrName in cThr[mult]:
                counters += [ CtpmonCounter(thrName,1) ]

        return counters



    @staticmethod
    def applyItemCounter( items ):
        """
        this functions marks the items that should be monitored by setting the corresponding monitoring flags
        e.g. to "LF:000|HF:111" for high frequency monitoring of TBP, TAP, and TAV.
        """

        TBP=1
        TAP=2
        TAV=4

        monItems   = { 1 :[], 2: [], 3: [], 4: [], 5: [], 6: [], 7: [] }
        monItemsHF = { 1 :[], 2: [], 3: [], 4: [], 5: [], 6: [], 7: [] }

        # definitions hardcoded at the moment
        monItems[TBP] = [
            "L1_ALFA_B7L1U", "L1_ALFA_B7L1L", "L1_ALFA_A7L1U", "L1_ALFA_A7L1L", "L1_ALFA_A7R1U", "L1_ALFA_A7R1L", "L1_ALFA_B7R1U", "L1_ALFA_B7R1L"
            ]


        monItems[TBP|TAP|TAV] = [
            "L1_EM3","L1_EM20VH","L1_EM22VHI",
            "L1_MU4","L1_MU10","L1_MU20",
            "L1_TAU12", "L1_TAU12IT", "L1_TAU20", "L1_TAU60",
            "L1_J12", "L1_J20", "L1_J100", "L1_J400", "L1_J20p31ETA49", "L1_J30p31ETA49",
            "L1_XE35", "L1_XE80", "L1_XS20", 
            "L1_MBTS_4_A", "L1_MBTS_4_C", "L1_MBTS_1", "L1_MBTS_2", "L1_MBTS_1_1",
            "L1_LUCID", "L1_LUCID_A_C_EMPTY", "L1_ALFA_ANY",
            "L1_ZDC_A", "L1_ZDC_C", "L1_ZDC_AND",
            "L1_BPTX0_BGRP0","L1_BPTX1_BGRP0",
            "L1_CALREQ2",
            "L1_AFP_NSC","L1_AFP_FSC",
            "L1_BPH-2M8-2MU4","L1_BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4","L1_BPH-2M9-2MU6_BPH-2DR15-2MU6",
            "L1_MU6MU4-BO","L1_2MU4-B",
            "L1_LFV-EM8I",
            "L1_TGC_BURST",
            "L1_LLP-NOMATCH",
            "L1_DR-TAU20ITAU12I",
            "L1_HT190-J15s5pETA21",
            "L1_3J15_BTAG-MU4J15",
            "L1_MJJ-900",
            "L1_J40_DPHI-J20s2XE50",
            "L1_KF-XE55",
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

            counts_LF = dict( map(lambda x : (x[0],len(x[1])), counts_LF_items.items() ) )
            counts_HF = dict( map(lambda x : (x[0],len(x[1])), counts_HF_items.items() ) )

            lutsLF = ( max(counts_LF.values())-1) / 8 + 1
            lutsHF = ( max(counts_HF.values())-1) / 8 + 1

            if lutsLF + lutsHF > 8:
                log.warning("too many monitoring items are defined:")
                log.warning("   low frequency  TBP: %i",counts_LF[TBP])
                log.warning("                  TAP: %i",counts_LF[TAP])
                log.warning("                  TAV: %i",counts_LF[TAV])
                log.warning("   required LUTs: %i",lutsLF)
                log.warning("   high frequency TBP: %i",counts_HF[TBP])
                log.warning("                  TAP: %i",counts_HF[TAP])
                log.warning("                  TAV: %i",counts_HF[TAV])
                log.warning("   required LUTs: %i",lutsHF)
                log.warning("   this menu requires %i monitoring LUTs while only 8 are available", (lutsLF + lutsHF))
            


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
