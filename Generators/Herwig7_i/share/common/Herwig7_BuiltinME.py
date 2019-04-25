# initialize Herwig7 generator configuration object for built-in/old-style matrix elements

from Herwig7_i.Herwig7_iConf import Herwig7
from Herwig7_i.Herwig7ConfigBuiltinME import Hw7ConfigBuiltinME

genSeq += Herwig7()
Herwig7Config = Hw7ConfigBuiltinME(genSeq, runArgs)

