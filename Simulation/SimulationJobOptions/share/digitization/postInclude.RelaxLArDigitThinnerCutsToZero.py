#########################################################
#
# SimulationJobOptions/postOptions.RelaxLArDigitThinnerCutsToZero.py
# Guillaume Unal
#
# Switches off LArDigitThinner cuts.
#
# This job option should be added via the postInclude
# command line argument.
#
#########################################################

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
if hasattr(job,'LArDigitThinner'):
    job.LArDigitThinner.EnergyCuts_Barrel = [ 0, 0, 0, 0 ]
    job.LArDigitThinner.EnergyCuts_EndCap = [ 0, 0, 0, 0 ]
    job.LArDigitThinner.EnergyCuts_HEC = 0
    job.LArDigitThinner.EnergyCuts_FCAL = 0
