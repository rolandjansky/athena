# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# PFlowJetCommon.py
# Schedules special jet tools
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
from DerivationFrameworkJetEtMiss.JetCommon import addPFlowJets

jetalg1 = addPFlowJets("EM", 0.4)
jetalg2 = addPFlowJets("EMC", 0.4)
DerivationFrameworkJob += jetalg1
DerivationFrameworkJob += jetalg2
