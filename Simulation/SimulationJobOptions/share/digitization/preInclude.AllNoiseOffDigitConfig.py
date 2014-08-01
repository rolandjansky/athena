##############################################################################
#
# SimulationJobOptions/preInclude.AllNoiseOffDigitConfig.py
# For use with Sim_trf.py and Digi_trf.py
# This runs digitization without noise in all subdetectors.
# (equivalent to the Digi_try.py command line option doAllNoise=False)
#
#############################################################################

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doInDetNoise = False
digitizationFlags.doCaloNoise  = False
digitizationFlags.doMuonNoise  = False
