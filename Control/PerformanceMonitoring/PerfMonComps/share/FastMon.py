# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: PerfMonComps/FastMon.py
# @author: Sebastien Binet
# $Id: FastMon.py,v 1.2 2007-12-03 19:07:38 binet Exp $

######################################################
# Configure PerfMonSvc in fast and lightweight manner
# (meant to be production friendly)
######################################################
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doFastMon = True
jobproperties.PerfMonFlags.doMonitoring = False
jobproperties.PerfMonFlags.doDetailedMonitoring = False

