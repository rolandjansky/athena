# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file MuonCBNTFlags.py Flags to steer Ntuple Filling of Muon Reconstruction

## @namespace python::MuonCBNTFlags @copydoc MuonCBNTFlags.py

from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from AthenaCommon.Logging import logging
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec as recFlags
from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonRecExample.MuonRecUtils import logMuon,RecConfigInfo,AutoLoadContainerJobProperty,\
     AutoLoadContainer,fillJobPropertyContainer

import copy


## Add MDTs Raw Info (RDO,PRD) to Ntuple
class doMdtRaw(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Add RPCs Raw Info (RDO,PRD) to Ntuple
class doRpcRaw(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Add TGCs Raw Info (RDO,PRD) to Ntuple
class doTgcRaw(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Add CSCs Raw Info (RDO,PRD) to Ntuple
class doCscRaw(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Add Moore reconstruction Info to Ntuple
class doMoore(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

## Add Muonboy reconstruction Info to Ntuple
class doMuonboy(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


## Container with flags to steer ntuple filling
class MuonCBNT(JobPropertyContainer):
    def setDefaults(self):
        global muonRecFlags,recFlags
        from MuonRecUtils import setJobPropertyDefault as setDefault
        muonRecFlags.setDefaults()
        setDefault(self.doMdtRaw , False)
        setDefault(self.doRpcRaw , globalflags.DataSource == 'data')
        setDefault(self.doTgcRaw , False)
        setDefault(self.doCscRaw , False)
        setDefault(self.doMuonboy, False)
        setDefault(self.doMoore  , False)



   
jobproperties.add_Container(MuonCBNT)

## shortcut to access flags
muonCBNT = jobproperties.MuonCBNT

# fill properties in container and store for later use (due to lack of iterator)
fillJobPropertyContainer(muonCBNT,__name__)

muonCBNT.setDefaults()
