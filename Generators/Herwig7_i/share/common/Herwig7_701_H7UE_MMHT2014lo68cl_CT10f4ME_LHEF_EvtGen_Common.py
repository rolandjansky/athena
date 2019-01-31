## JobOption fragment for Herwig 7.0.1

## Herwig7 config for the H7-UE-MMHT tune series with a CT10f4 NLO ME PDF with NLO events read from an LHEF file
include("Herwig7_i/Herwig7_701_H7UE_MMHT2014lo68cl_CT10f4ME_EvtGen_Common.py")

from Herwig7_i import config as hw
genSeq.Herwig7.Commands += hw.lhef_cmds(filename="events.lhe", nlo=True).splitlines()
