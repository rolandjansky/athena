# TrigDB modifier
# Usage: athenaMT/PT -C 'include("TriggerRelease/dbmod_CostMonForCAF.py")' ...
#
# Enable writing of cost monitoring ntuples at CAF
#
from GaudiPython.Bindings import iProperty

isL2 = len(iProperty('TrigSteer_L2').properties())>0
isEF = len(iProperty('TrigSteer_EF').properties())>0
isHLT = len(iProperty('TrigSteer_HLT').properties())>0

if isL2:
    iProperty('TrigSteer_L2').doOperationalInfo = 1
    iProperty('TrigSteer_L2.TrigCostExecL2').writeAlways = True

if isEF:
    iProperty('TrigSteer_EF').doOperationalInfo = 1
    iProperty('TrigSteer_EF.TrigCostExecEF').writeAlways = True
    
if isHLT:
    iProperty('TrigSteer_HLT').doOperationalInfo = 1
    iProperty('TrigSteer_HLT.TrigCostExecHLT').writeAlways = True


