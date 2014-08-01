# get calibration data to working dir (temporary solution)

os.system('get_files -data HIFlowWeight.root')

# turn off pp pilup noise suppression
from CaloTools.CaloNoiseFlags import jobproperties
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(0)
