#!/us/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Postinclude snippet for Reco_tf to bring in a GRL

from __future__ import print_function

from GoodRunsLists.GoodRunsListsConf import GoodRunsListSelectorTool
from GoodRunsListsUser.GoodRunsListsUserConf import GRLTriggerSelectorAlg
from AthenaCommon.AlgSequence import AthSequencer
import os
if 'GRL' in os.environ:
    print('Executing GRL code')
    ToolSvc += GoodRunsListSelectorTool()
    ToolSvc.GoodRunsListSelectorTool.GoodRunsListVec = [ os.environ['GRL']  ]

    seq = AthSequencer("AthMasterSeq")
    seq += GRLTriggerSelectorAlg("GRLAlg")
    seq.GRLAlg.GoodRunsListArray=[ os.environ['GRLNAME'] ]
