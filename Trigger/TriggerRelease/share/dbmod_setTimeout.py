# TrigDB modifier
# Usage: athenaMT/PT -C 'include("TriggerRelease/dbmod_setTimeout.py")' ...
#
# Set timeouts
#
from GaudiPython.Bindings import iProperty

l2timeout = 10*1e9   # ns
eftimeout = 180*1e9  # ns

# This will work for L2
if len(iProperty('TrigSteer_L2').properties())>0:
   iProperty('TrigSteer_L2').softEventTimeout = 0.9*l2timeout
   iProperty('TrigSteer_L2').hardEventTimeout = l2timeout
   
# and this for EF
if len(iProperty('TrigSteer_EF').properties())>0:
   iProperty('TrigSteer_EF').softEventTimeout = 0.9*eftimeout
   iProperty('TrigSteer_Ef').hardEventTimeout = eftimeout   
   
