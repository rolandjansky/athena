## Base config for Sherpa
from Sherpa_i.Sherpa_iConf import Sherpa_i
genSeq += Sherpa_i()
evgenConfig.generators = ["Sherpa"]
genSeq.Sherpa_i.PluginCode = ""

## General ATLAS parameters
genSeq.Sherpa_i.BaseFragment = """
BEAMS: 2212
BEAM_ENERGIES: $(EBEAMSETBYATHENA)

MAX_PROPER_LIFETIME: 10.0
HEPMC_TREE_LIKE: 1
PRETTY_PRINT: Off
EXTERNAL_RNG: Atlas_RNG
"""

## Cap event weights at |w|<10 in unweighted evgen; set H-event shower mode
## in the soft region to avoid instable weights 
genSeq.Sherpa_i.BaseFragment += """
OVERWEIGHT_THRESHOLD: 10
MC@NLO:
  HPSMODE: 0
"""

particledata = {
    '5': {'mass': '4.95', 'width': '0.'}, # mb consistent with McProductionCommonParametersMC15 and https://cds.cern.ch/record/2047636
    '6': {'mass': '1.725E+02', 'width': '1.32E+00'},
    '15': {'mass': '1.777', 'width': '2.26735e-12'},
    '23': {'mass': '91.1876', 'width': '2.4952'},
    '24': {'mass': '80.399', 'width': '2.085'},
}
sin2thetaw='0.23113'
try:
    USE_PDG_VALUES
except NameError:
    USE_PDG_VALUES = False
if USE_PDG_VALUES:
    from EvgenProdTools.offline_dict import parameters
    for id, values in particledata:
        particledata[i] = parameters['particles'][id]
    sin2thetaw = parameters['EW_parameters']['SIN2THETHAW']
genSeq.Sherpa_i.BaseFragment += """
PARTICLE_DATA:
"""
for id, values in particledata.items():
    genSeq.Sherpa_i.BaseFragment += """
  {id}:
    Mass: {mass}
    Width: {width}
""".format(id=id, mass=values['mass'], width=values['width'])
## ToDo: Include partial widths for H/W/Z in EvgenProdTools/python/offline_dict.py ?

## Switch to EW_SCHEME=0 to be able to set PDG value of thetaW
genSeq.Sherpa_i.BaseFragment += """
EW_SCHEME: UserDefined
SIN2THETAW: {}
""".format(sin2thetaw)


## SM decay branching ratios
genSeq.Sherpa_i.BaseFragment += """
HARD_DECAYS:
  Enabled: true
  Channels:
    "6 -> 24 5":  { Width: 1.32 }
    "-6 -> -24 -5":  { Width: 1.32 }
    "25 -> 5 -5":  { Width: 2.35e-3 }
    "25 -> 15 -15":  { Width: 2.57e-4 }
    "25 -> 13 -13":  { Width: 8.91e-7 }
    "25 -> 4 -4":  { Width: 1.18e-4 }
    "25 -> 3 -3":  { Width: 1.00e-6 }
    "25 -> 21 21":  { Width: 3.49e-4 }
    "25 -> 22 22":  { Width: 9.28e-6 }
    "24 -> 2 -1":  { Width: 0.7041 }
    "24 -> 4 -3":  { Width: 0.7041 }
    "24 -> 12 -11":  { Width: 0.2256 }
    "24 -> 14 -13":  { Width: 0.2256 }
    "24 -> 16 -15":  { Width: 0.2256 }
    "-24 -> -2 1":  { Width: 0.7041 }
    "-24 -> -4 3":  { Width: 0.7041 }
    "-24 -> -12 11":  { Width: 0.2256 }
    "-24 -> -14 13":  { Width: 0.2256 }
    "-24 -> -16 15":  { Width: 0.2256 }
    "23 -> 1 -1":  { Width: 0.3828 }
    "23 -> 2 -2":  { Width: 0.2980 }
    "23 -> 3 -3":  { Width: 0.3828 }
    "23 -> 4 -4":  { Width: 0.2980 }
    "23 -> 5 -5":  { Width: 0.3828 }
    "23 -> 11 -11":  { Width: 0.0840 }
    "23 -> 12 -12":  { Width: 0.1663 }
    "23 -> 13 -13":  { Width: 0.0840 }
    "23 -> 14 -14":  { Width: 0.1663 }
    "23 -> 15 -15":  { Width: 0.0840 }
    "23 -> 16 -16":  { Width: 0.1663 }
"""

## OpenLoops parameters
import os
genSeq.Sherpa_i.BaseFragment += """
OL_PARAMETERS:
  preset: 2
  write_parameters: 1
OL_PREFIX: {}
""".format(str(os.environ['OPENLOOPSPATH']))
