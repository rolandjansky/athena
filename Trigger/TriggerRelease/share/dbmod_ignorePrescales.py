# TrigDB modifier
# Usage: athenaMT/PT -C 'include("TriggerRelease/dbmod_ignorePrescales.py")' ...
#
# Ignore prescales
#
from GaudiPython.Bindings import iProperty

# This will work for L2
if len(iProperty('TrigSteer_L2.Lvl1Converter').properties())>0:
   iProperty('TrigSteer_L2.Lvl1Converter').ignorePrescales = True
   
# and this for EF
if len(iProperty('TrigSteer_EF.Lvl1Converter').properties())>0:
   iProperty('TrigSteer_EF.Lvl1Converter').ignorePrescales = True   
   
