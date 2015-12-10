# TrigDB modifier
# Usage: athenaMT/PT -C 'include("TriggerRelease/dbmod_nameAuditor.py")' ...
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

