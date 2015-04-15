# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ------------------------------------------------------------
# DerivationFrameworkMaster.py
# J. Catmore (James.Catmore@cern.ch)
# ------------------------------------------------------------
# Contains all basic includes for running the derivation framework
#-------------------------------------------------------------

from AthenaCommon.AppMgr import ToolSvc
from D2PDMaker.D2PDFlags import D2PDFlags
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from DerivationFrameworkCore.DerivationFrameworkProdFlags import derivationFlags
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon import CfgMgr 
from AthenaCommon.AlgSequence import AlgSequence 
from JetRec.JetRecFlags import jetFlags 
from AthenaCommon.GlobalFlags  import globalflags

# Sequence for the AuxStoreWrappers (for slimming)
AuxStoreWrapperSequence = CfgMgr.AthSequencer("AuxStoreWrapperSequence")


# DerivationJob is COMMON TO ALL DERIVATIONS
DerivationFrameworkJob = AlgSequence()
DerivationFrameworkJob += AuxStoreWrapperSequence

# Set up stream auditor
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'DecisionSvc'):
        svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True

# Trigger decision tool
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter('ReadPool')
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt

# Centrally setting (jet for now) flags
# Data and MC flags
jetFlags.useTracks = True
# MC-related flags
if globalflags.DataSource()=='geant4':
    print "Switching on jetFlags.useTruth"
    jetFlags.useTruth = True

