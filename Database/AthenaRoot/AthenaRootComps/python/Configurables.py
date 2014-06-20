# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: Configurables.py
# @purpose: customized Configurable classes for AthenaRootComps
# @author: Sebastien Binet <binet@cern.ch>

## import the automatically generated Configurables
from AthenaRootCompsConf import TTreeEventSelector as _TTreeEventSelector

class TTreeEventSelector(_TTreeEventSelector):
    __slots__ = { '_svc_helper' : None }

    def __init__(self, name="TTreeEventSelector", **kw):
        # init base class
        kw['name'] = name
        super(TTreeEventSelector, self).__init__(**kw)
        return

    def setDefaults(cls, handle):

        ## continue, only if it is our Configurable
        if not isinstance(handle, TTreeEventSelector):
            return

        ## configure our helper service whose sole purpose is to be
        ## initialized early to populate the THistSvc w/ TTrees/TChains
        if 1:
            from AthenaCommon.Configurable import Configurable
            svc = Configurable.allConfigurables['TTreeEventSelectorHelperSvc']
            svc.InputCollections = handle.InputCollections[:]
            svc.TupleName        = handle.TupleName
            svc.ActiveBranches   = handle.ActiveBranches[:]
    
        ##
        return
    pass # class TTreeEventSelector

### ---------------------------------------------------------------------------


### ---------------------------------------------------------------------------
from OutputStreamAthenaRoot import createNtupleOutputStream

### ---------------------------------------------------------------------------
## import the automatically generated Configurables
