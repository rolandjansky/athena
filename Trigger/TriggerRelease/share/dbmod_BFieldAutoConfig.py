# TrigDB modifier
# Usage: athenaHLT.py --postcommand 'include("TriggerRelease/dbmod_BFieldAutoConfig.py")' ...
#
# Use DCS currents to configure magnetic field
#
from GaudiPython.Bindings import iProperty

db = 'CONDBR2'
if _run_number in dir() and _run_number<222222: db = 'COMP200'

# Add the DCS folder (note the double-use of quotes)
iProperty('IOVDbSvc').Folders.push_back("'<db>COOLOFL_DCS/%s</db> /EXT/DCS/MAGNETS/SENSORDATA'" % db)
iProperty('AtlasFieldSvc').UseDCS = True


