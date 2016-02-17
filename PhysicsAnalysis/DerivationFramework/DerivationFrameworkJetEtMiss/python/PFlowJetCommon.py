# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# PFlowJetCommon.py
# Schedules special jet tools
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *

from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm
jetalg1 = addPFlowJets("EM", 0.4)
jetalg2 = addPFlowJets("EMC", 0.4)
DerivationFrameworkJob += jetalg1
DerivationFrameworkJob += jetalg2
