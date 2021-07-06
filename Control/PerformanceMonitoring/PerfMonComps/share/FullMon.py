# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: PerfMonComps/FullMon.py
# @author: Sebastien Binet
# $Id$

######################################################
# Configure PerfMonSvc with full details
# (meant to be production friendly)
######################################################
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doFastMon = False
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doDetailedMonitoring = True

