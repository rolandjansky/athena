################################################################################
###
###		jobConfig for jobTransforms (mainly testet with the csc_simul_trf.py).
###		This runs the G4 simulation of the ATLAS detector
###		without noise in the calorimeters.
###		As a post-option a jobOption-file is included. This jobOption will 
###		call the G4UserActions/FastIDKiller to kill all particles leaving the 
###		inner detector except muons.
###
###############################################################################

from SimuJobTransforms.AtlasG4Config import atlasG4Config
atlasG4Config.postOptions="SimuJobTransforms/postOptions.FastIDKiller.py"
