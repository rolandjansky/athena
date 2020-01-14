## Initialise Herwig7 for LHEF showering
include("Herwig7_i/Herwig7_LHEF.py")

## Provide config information
evgenConfig.generators += ["aMcAtNlo", "Herwig7"]
evgenConfig.tune        = "H7.1-Default"
evgenConfig.description = "MG5aMCatNLO/Herwig7 LHEF"
evgenConfig.keywords    = ["SM","Z","electron"]
evgenConfig.contact     = ["Daniel Rauch (daniel.rauch@desy.de)"]

## Configure Herwig7
Herwig7Config.me_pdf_commands(order="NLO", name="CT10")
Herwig7Config.tune_commands()
Herwig7Config.lhef_mg5amc_commands(lhe_filename=runArgs.inputGeneratorFile, me_pdf_order="NLO")

## run generator
Herwig7Config.run()
