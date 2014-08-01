##############################################################################
#
# SimulationJobOptions/preInclude.CaloNoiseOffDigitConfig.py
# For use with Sim_trf.py and Digi_trf.py
# This runs digitization without noise in the LAr and Tile subdetectors.
# Requested for use with FastCaloSim.
#
#############################################################################

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doCaloNoise  = False
