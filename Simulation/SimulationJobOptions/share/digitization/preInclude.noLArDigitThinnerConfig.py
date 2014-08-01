#
# SimulationJobOptions/preInclude.noLArDigitThinnerConfig.py
#
#
from LArROD.LArDigits import DefaultLArDigitThinner
LArDigitThinner = DefaultLArDigitThinner('LArDigitThinner', addToAlgSeq = False)
