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
log = logging.getLogger(__name__)

from ..Base.MonCounters import CtpinCounter, CtpmonCounter

class MonitorDef:

    LOW_FREQ = 0
    HIGH_FREQ = 1


    # CTPIN counters
    # these are generated for all CTPIN signals except the two highest JET inputs on JET1 (see comment at start of file)
    @staticmethod
    def ctpinCounters( thresholds, connectors, ctpinConfig ):

        connectedCables = []
        for slotConnectors in ctpinConfig.values():
            for connName in slotConnectors.values():
                if connName:
                    connectedCables += [ connName ]

        counters = []
        for ctpinCableName in connectedCables:
            conn = connectors[ctpinCableName]
            for i, tl in enumerate(conn.triggerLines):
                if ctpinCableName == "JET1" and i==8:
                    break
                for mult in range(1, 2**tl.nbits):
                    counters += [ CtpinCounter(threshold=tl.name, multiplicity = mult) ]

        return counters




    # CTPMON counters
    # we only have a few for the moment
    @staticmethod
    def ctpmonCounters( thresholds, connectors ):

        counters = []

        cThr = {}
        cThr[1] = [ 'AFP_NSA', 'AFP_NSC', 'AFP_FSA', 'AFP_FSC', 'AFP_FSA_TOF_T0', 'AFP_FSC_TOF_T0',
                    'AFP_FSA_TOF_T1', 'AFP_FSC_TOF_T1', 'AFP_FSA_TOF_T2', 'AFP_FSC_TOF_T2', 'AFP_FSA_TOF_T3', 'AFP_FSC_TOF_T3',
                    'BPTX0', 'BPTX1', 
                    'LUCID_A', 'LUCID_C', 
                    'J20', 'MU3V', 'TE50', 'XE35', 'XE50',
                    'ZDC_0', 'ZDC_1', 'ZDC_2',
                    'MBTS_A', 'MBTS_C',
                    'MBTS_A0', 'MBTS_A1', 'MBTS_A2',  'MBTS_A3',  'MBTS_A4',  'MBTS_A5',  'MBTS_A6',  'MBTS_A7',
                    'MBTS_A8', 'MBTS_A9', 'MBTS_A10', 'MBTS_A11', 'MBTS_A12', 'MBTS_A13', 'MBTS_A14', 'MBTS_A15',
                    'MBTS_C0', 'MBTS_C1', 'MBTS_C2',  'MBTS_C3',  'MBTS_C4',  'MBTS_C5',  'MBTS_C6',  'MBTS_C7',
                    'MBTS_C8', 'MBTS_C9', 'MBTS_C10', 'MBTS_C11', 'MBTS_C12', 'MBTS_C13', 'MBTS_C14', 'MBTS_C15' 
                   ]

        for mult in cThr:
            for thrName in cThr[mult]:
                counters += [ CtpmonCounter(thrName,1) ]

        return counters



    @staticmethod
    def applyItemCounter( menuName, items ):
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
#        monItems[TBP] = [
#            "L1_ALFA_B7L1U", "L1_ALFA_B7L1L", "L1_ALFA_A7L1U", "L1_ALFA_A7L1L", "L1_ALFA_A7R1U", "L1_ALFA_A7R1L", "L1_ALFA_B7R1U", "L1_ALFA_B7R1L"
#            ]

        if 'HI' not in menuName:
            monItems[TBP|TAP|TAV] = [
                "L1_EM3",
                "L1_EM12", # keep for LUMI group
                "L1_EM20VH","L1_EM22VHI", "L1_EM24VHI",
                "L1_MU8F","L1_MU8VF","L1_MU14FCH", "L1_MU14FCHR",
                "L1_TAU12IM", "L1_TAU20IM", "L1_TAU40", "L1_TAU100",
                "L1_J100", "L1_J75p31ETA49", "L1_4J15p0ETA25", "L1_J30p31ETA49",
                "L1_XE35", "L1_XE50",
                #"L1_MBTS_4_A", "L1_MBTS_4_C", "L1_MBTS_1_A", "L1_MBTS_1_C", "L1_MBTS_1", "L1_MBTS_2", "L1_MBTS_1_1",
                #"L1_ALFA_ANY", "L1_ALFA_B7L1U", "L1_ALFA_B7L1L", "L1_ALFA_A7L1U", "L1_ALFA_A7L1L", "L1_ALFA_A7R1U", "L1_ALFA_A7R1L", "L1_ALFA_B7R1U", "L1_ALFA_B7R1L",
                "L1_ZDC_A", "L1_ZDC_C", "L1_ZDC_AND",
                #"L1_BPTX0_BGRP0","L1_BPTX1_BGRP0",
                "L1_CALREQ2",
                "L1_TGC_BURST",
                "L1_EM20VH_3J20", "L1_EM15VH_MU8F", "L1_EM15VHI_2TAU12IM", "L1_EM15VHI_2TAU12IM_J25_3J12", "L1_EM15VHI_2TAU12IM_XE35",
                "L1_MU8VF_2MU5VF", "L1_MU8F_TAU12IM", "L1_MU8F_TAU12IM_J25_2J12", "L1_MU8F_TAU12IM_XE35",
                "L1_MU8F_2J15_J20", "L1_MU14FCH_XE30", "L1_J40_XE50", "L1_J25p0ETA23_2J15p31ETA49", "L1_J45p0ETA21_3J15p0ETA25", "L1_MU14FCH_J40", 
                ]
        else:
            monItems[TBP|TAP|TAV] = [
                "L1_EM20VH", 
                "L1_EM12", # keep for LUMI group
                "L1_MU8F","L1_MU8VF","L1_MU14FCH", "L1_MU14FCHR",
                "L1_J100", "L1_J75p31ETA49","L1_J30p31ETA49",
                "L1_XE35", "L1_XE50",
                #"L1_MBTS_4_A", "L1_MBTS_4_C", "L1_MBTS_1_A", "L1_MBTS_1_C", "L1_MBTS_1", "L1_MBTS_2", "L1_MBTS_1_1",
                #"L1_ALFA_ANY", "L1_ALFA_B7L1U", "L1_ALFA_B7L1L", "L1_ALFA_A7L1U", "L1_ALFA_A7L1L", "L1_ALFA_A7R1U", "L1_ALFA_A7R1L", "L1_ALFA_B7R1U", "L1_ALFA_B7R1L",
                #"L1_ZDC_A", "L1_ZDC_C", "L1_ZDC_AND",
                #"L1_BPTX0_BGRP0","L1_BPTX1_BGRP0",
                "L1_CALREQ2",
                "L1_TGC_BURST",
                "L1_MU8VF_2MU5VF",
                ]

        monItemsHF[TBP|TAP|TAV] = [
            "L1_BCM_Wide_BGRP0",
            "L1_BCM_AC_CA_BGRP0",
            "L1_BCM_AC_UNPAIRED_ISO",
            "L1_BCM_CA_UNPAIRED_ISO",
            "L1_J12",
            "L1_MBTS_1", "L1_MBTS_2", "L1_MBTS_1_1",
            ]



        check = True
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
                log.error("too many monitoring items are defined:")
                log.error("   low frequency  TBP: %i",counts_LF[TBP])
                log.error("                  TAP: %i",counts_LF[TAP])
                log.error("                  TAV: %i",counts_LF[TAV])
                log.error("   required LUTs: %i",lutsLF)
                log.error("   high frequency TBP: %i",counts_HF[TBP])
                log.error("                  TAP: %i",counts_HF[TAP])
                log.error("                  TAV: %i",counts_HF[TAV])
                log.error("   required LUTs: %i",lutsHF)
                log.error("   this menu requires %i monitoring LUTs while only 8 are available", (lutsLF + lutsHF))
                raise RuntimeError("Reduce the number of monitored items") 


        MonitorDef.checkForNonExistingMonItems(items, monItems)

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
            raise RuntimeError("These monitoring items are not part of the menu: %s" % ','.join(nonExistiginMonItems))
