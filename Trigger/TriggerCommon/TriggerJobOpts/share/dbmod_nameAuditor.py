# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# TrigDB modifier
# Usage: athenaMT/PT -C 'include("TriggerJobOpts/dbmod_nameAuditor.py")' ...
#
# Enable NameAuditor
#
from TrigCommon.TrigPyHelper import trigApp
from GaudiPython.Bindings import iProperty
trigApp.changeJobProperties('.*', 'AuditAlgorithms', True)
trigApp.changeJobProperties('.*', 'AuditServices', True)
trigApp.changeJobProperties('.*', 'AuditTools', True)
trigApp.changeJobProperties('.*', 'AuditInitialize', True)

iProperty('AuditorSvc').Auditors.push_back("'NameAuditor/NameAuditor'")

