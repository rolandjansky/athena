# jO version showing how to manually set the crosssection value for the MetaData printout
# using the SetCrossSection property

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

# --> use the following pythia tune if CTEQ6l1 pdf has been used
include ( "MC10JobOptions/MC10_PythiaMC09p_Common.py" )

Pythia.PythiaCommand += [ "pyinit user madgraph",
                          "pyinit pylisti -1","pyinit pylistf 1","pyinit dumpr 1 2",
                          "pydat1 parj 90 20000", # Turn off FSR.
                          "pydat3 mdcy 15 1 0"    # Turn off tau decays.
                        ]

# set x before filtering efficiency in [nb] manually;
#Pythia.SetCrossSection=3190.0

## ... Tauola
include ( "MC10JobOptions/MC10_Tauola_Fragment.py" )

## ... Photos
include ( "MC10JobOptions/MC10_Photos_Fragment.py" )

# MultiLeptonFilter
from GeneratorFilters.GeneratorFiltersConf import MultiLeptonFilter
topAlg += MultiLeptonFilter()

MultiLeptonFilter = topAlg.MultiLeptonFilter
MultiLeptonFilter.Ptcut = 15.*GeV
MultiLeptonFilter.Etacut = 2.7
MultiLeptonFilter.NLeptons = 1

#---------------------------------------------------------------
# POOL / Root output
#---------------------------------------------------------------
try:
    StreamEVGEN.RequireAlgs += [ "MultiLeptonFilter" ]
except Exception, e:
    pass

from MC10JobOptions.PythiaEvgenConfig import evgenConfig
#from MC10JobOptions.MadGraphEvgenConfig import evgenConfig

evgenConfig.generators += ["MadGraph", "Pythia"]
evgenConfig.inputfilebase = 'MadGraph'
try:
  if runArgs.ecmEnergy == 7000.0:
    evgenConfig.inputfilebase = 'group10.phys-gener.MadGraph.117960.TT_250_010_7TeV.TXT.v3'
  if runArgs.ecmEnergy == 8000.0:
    evgenConfig.inputfilebase = 'group10.phys-gener.MadGraph.117960.TT_250_010_8TeV.TXT.v1'
  if runArgs.ecmEnergy == 10000.0:
    evgenConfig.inputfilebase = 'group10.phys-gener.MadGraph.117960.TT_250_010_10TeV.TXT.v1'
  if runArgs.ecmEnergy == 14000.0:
    evgenConfig.inputfilebase = 'group10.phys-gener.MadGraph.117960.TT_250_010_14TeV.TXT.v1'
except NameError:
  pass
evgenConfig.efficiency = 0.08

phojf=open('./pythia_card.dat', 'w')
phojinp = """
! exclusive or inclusive matching
      IEXCFILE=0
      showerkt=T
      qcut=20
      """
phojf.write(phojinp)
phojf.close()
