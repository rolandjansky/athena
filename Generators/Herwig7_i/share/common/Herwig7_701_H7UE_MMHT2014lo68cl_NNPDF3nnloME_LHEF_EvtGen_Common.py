## JobOption fragment for Herwig 7.0.1

include("Herwig7_i/Herwig7_701_Base_Fragment.py")

## Herwig7 config for the H7-UE-MMHT tune series with NNPDF3.0 NNLO ME PDF with events read from a LHEF file and decays done with EvtGen

## Construct command set
from Herwig7_i import config as hw
cmds = hw.energy_cmds(runArgs.ecmEnergy) + hw.base_cmds() \
     + hw.nlo_pdf_cmds("NNPDF30_nnlo_as_0118.LHgrid", "MMHT2014lo68cl.LHpdf") \
     + hw.ue_tune_cmds("H7-UE-MMHT")
genSeq.Herwig7.Commands += cmds.splitlines()
del cmds

evgenConfig.tune = "H7-UE-MMHT"

genSeq.Herwig7.Commands += hw.lhef_cmds(filename="events.lhe", nlo=True).splitlines()

include("Herwig7_i/Herwig7_701_EvtGen.py")
