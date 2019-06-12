## These JobOptions are meant for development / debugging purposes only.
## Please do not use them for physics purposes.
##
## Specifically, standalone (outside of athena) Herwig7 runs can be reproduced
## inside of athena by re-using a standalone Herwig7 infile.

## Initialise Herwig7 for standalone run reproduction
from Herwig7_i.Herwig7_iConf import Herwig7
from Herwig7_i.Herwig7ConfigGeneric import Hw7ConfigGeneric
genSeq += Herwig7()
Herwig7Config = Hw7ConfigGeneric(genSeq, runArgs, "Herwig.in", "Herwig.run")

## Provide config information
evgenConfig.generators += ["Powheg", "Herwig7"] 
evgenConfig.tune        = "H7-UE-MMHT2014"
evgenConfig.description = "PowhegBox/Herwig7 LHEF"
evgenConfig.keywords    = ["SM","Z","electron"]
evgenConfig.contact     = ["Daniel Rauch (daniel.rauch@desy.de)"]

## Run event generator using the
##
##   Herwig read <infile>
##   Herwig run <runfile> -s <seed>
##
## sequence
Herwig7Config.read_run()
