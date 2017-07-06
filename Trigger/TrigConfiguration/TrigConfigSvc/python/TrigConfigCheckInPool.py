# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT, cppyy
import AthenaROOTAccess.transientTree

from AthenaCommon.Logging import logging
mlog = logging.getLogger( 'TrigConfigCheckInPool.py' ) ## get the logger
mlog.level=1

MetaDataTree = ROOT.AthenaROOTAccess.TChainROOTAccess('MetaData')

def _iter(self) :
    sequential = self.begin()
    end        = self.end()
    while sequential != end :
        yield sequential.__deref__()
        sequential.__preinc__()
    raise StopIteration

_plcClass = cppyy.makeClass("IOVPayloadContainer")
_plcClass.iter  = _iter

ROOT.IOVTime.__str__ = lambda x: "%i/%i" % (x.run(),x.event())
ROOT.IOVRange.__str__ = lambda x: "%s - %s" % (x.start(),x.stop())


def checkPoolFileForRunLevel(poolfilename):
    folderName = {
        "L1M"  : "_TRIGGER_LVL1_Menu",
        "HLTM" : "_TRIGGER_HLT_Menu",
        "L1K"  : "_TRIGGER_LVL1_Lvl1ConfigKey",
        "HLTK" : "_TRIGGER_HLT_HltConfigKeys",
        "L1PS" : "_TRIGGER_LVL1_Prescales"
        }
    MetaDataTree.Add(poolfilename+'/MetaData')
    mdt = AthenaROOTAccess.transientTree.makeTree(MetaDataTree,
                                                  persTreeName = 'MetaData',
                                                  dhTreeName = 'MetaDataHdrDataHeader')
    l1keys = []
    hltkeys = []

    if hasattr(mdt,folderName["L1K"]):
        br = mdt.GetBranch(folderName["L1K"])
        for i in xrange(mdt.GetEntries()):
            br.GetEntry(i)
            metaData = getattr(mdt, br.GetName())
            plc = metaData.payloadContainer()
            for payload in plc.iter():
                for i in xrange(payload.size()):
                    chanNum = int(payload.chanNum(i))
                    iovr = payload.iovRange(chanNum)
                    l1keys += [ payload.attributeList(chanNum)["Lvl1PrescaleConfigurationKey"].data("unsigned int")() ]

    if hasattr(mdt,folderName["HLTK"]):
        br = mdt.GetBranch(folderName["HLTK"])
        for i in xrange(mdt.GetEntries()):
            br.GetEntry(i)
            metaData = getattr(mdt, br.GetName())
            plc = metaData.payloadContainer()
            for payload in plc.iter():
                for i in xrange(payload.size()):
                    chanNum = int(payload.chanNum(i))
                    iovr = payload.iovRange(chanNum)
                    hltkeys += [ payload.attributeList(chanNum)["HltPrescaleConfigurationKey"].data("unsigned int")() ]

    mlog.info("File contained these LVL1 prescale keys: %r" % l1keys)
    mlog.info("File contained these HLT prescale keys: %r" % hltkeys)

    hasL1 = len(l1keys)>0 and not 0 in l1keys 
    hasHLT = len(hltkeys)>0 and not 0 in hltkeys 

    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if hasL1 or hasHLT:
        if hasL1 and hasHLT:
            TriggerFlags.dataTakingConditions='FullTrigger'
        elif hasL1:
            TriggerFlags.dataTakingConditions='Lvl1Only'
        else:
            TriggerFlags.dataTakingConditions='HltOnly'
        mlog.info("Set TriggerFlags.dataTakingConditions to '%s'" % TriggerFlags.dataTakingConditions())



    
