## JobOption fragment for Herwig 7.0.1

## Herwig7 config for the H7-UE-MMHT tune series with a CT10f4 NLO ME PDF
include("Herwig7_i/Herwig7_701_Base_Fragment.py")

## Construct command set
from Herwig7_i import config as hw
cmds = hw.energy_cmds(runArgs.ecmEnergy) + hw.base_cmds() \
    + hw.nlo_pdf_cmds("CT10f4.LHgrid", "MMHT2014lo68cl.LHpdf") \
    + hw.ue_tune_cmds("H7-UE-MMHT")
genSeq.Herwig7.Commands += cmds.splitlines()
del cmds
include("Herwig7_i/Herwig7_701_EvtGen.py")

evgenConfig.tune = "H7-UE-MMHT"
