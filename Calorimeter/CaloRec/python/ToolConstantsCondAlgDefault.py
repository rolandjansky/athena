# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/ToolConstantsCondAlgDefault.py
# Created: Jun 2020, sss
# Purpose: Configure ToolConstantsCondAlg.


from AthenaCommon.AlgSequence import AthSequencer
from AthenaConfiguration.ComponentFactory import CompFactory


def ToolConstantsCondAlgDefault (key, DetStoreKey='', COOLFolder=''):
    """Configure a conditions algorithm to convert inline COOL data or detector store data to ToolConstants.
    KEY is also the key of the output conditions object.
    For reading from COOL inline data, COOLFOLDER gives the name
    of the COOL folder; the dta are given by KEY within the folder.
    The caller should register the folder with IOVDbSvc.
    For copying a ToolConstants object from the detector store,
    set DETSTOREKEY to the key of the object to copy."""

    if ((DetStoreKey == '' and COOLFolder == '') or
        (DetStoreKey != '' and COOLFolder != '')):
        raise RuntimeError ("ToolConstantsCondAlgCfg: For key " + key + ", exactly one of DetStoreKey or COOLFolder must be specified")

    name = 'ToolConstantsCondAlg_' + key
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    ToolConstantsCondAlg = CompFactory.ToolConstantsCondAlg # CaloRec
    alg = ToolConstantsCondAlg (name,
                                COOLFolderKey = COOLFolder,
                                ToolConstantsKey = key,
                                DetStoreKey = DetStoreKey)

    condSeq += alg
    return alg

    
