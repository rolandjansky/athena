## JobOption fragment for Herwig 7.0.1

from Herwig7_i.Herwig7_iConf import Herwig7
genSeq += Herwig7()
evgenConfig.generators += ["Herwig7"]

try:
  from Herwig7_i import config as hw
except:
  raise RuntimeError("""
The release you have set up features the re-designed Herwig7 interface in athena which makes use of Herwig7 versions >= 7.0.4.
The JobOptions, that you are trying to run, were written for and can only be used with the now discontinued interface for Herwig7 version 7.0.1, but can't be run with the new interface.
Please check if you would like to
* use more recent JobOptions, that are compatible with the new Herwig7 interface
or
* set up a different release, that includes the old interface for Herwig7
Please have a look at https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/Herwig7ForAtlas for more information on release numbers and the corresponding interface versions.
""")

genSeq.Herwig7.Commands += hw.atlas_parameter_cmds().splitlines()

genSeq.Herwig7.Commands += [
  "## fix for global default settings until released with Herwig7 itself",
  "set /Herwig/Shower/GtoQQbarSplitFn:AngularOrdered Yes",
  "set /Herwig/Shower/GammatoQQbarSplitFn:AngularOrdered Yes",
  "",
  "## fix for GeV-mass photon radiation",
  "set /Herwig/Shower/GammatoQQbarSudakov:Alpha /Herwig/Shower/AlphaQED",
  "",
  "## fix for initial-state (backward evolution) splitting",
  "set /Herwig/Shower/QtoGammaQSudakov:Alpha /Herwig/Shower/AlphaQED",
  ""
]
