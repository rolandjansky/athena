from Herwig7_i.Herwig7_iConf import Herwig7
from Herwig7_i.Herwig7ConfigFxFx import Hw7ConfigFxFx

genSeq += Herwig7()

## Provide config information
evgenConfig.generators += ["aMcAtNlo", "Herwig7"] 
evgenConfig.tune        = "MMHT2014"
evgenConfig.description = "MG5aMCatNLO/Herwig7 FxFx"
evgenConfig.keywords    = ["SM","ttbar"]
evgenConfig.contact     = ["Krishna Kulkarni (krishna.kulkarni@cern.ch), Daniel Rauch (daniel.rauch@desy.de)", "atlas-generators-herwig7@cern.ch"]

## Configure Herwig7
## These are the commands corresponding to what would go
## into the regular Herwig infile

## initialize generator configuration object
generator = Hw7ConfigFxFx(genSeq, runArgs, run_name="HerwigFxFx")

## configure generator
generator.me_pdf_commands(order="NLO", name="PDF4LHC15_nlo_mc")
generator.tune_commands()
generator.fxfx_commands(merging_scale=100, njets_max=2, lhe_filename=runArgs.inputGeneratorFile, me_pdf_order="NLO")

## specific to pp-tt+0/1/2j
generator.add_commands("""
set /Herwig/Partons/RemnantDecayer:AllowTop Yes
""")

## run generator
generator.run()
