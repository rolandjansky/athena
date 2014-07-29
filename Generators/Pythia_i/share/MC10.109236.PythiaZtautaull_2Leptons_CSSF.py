#________________________________________________________________________________________
#
# Auth.: liza.mijovic_at_nospam.cern.ch
# example how to use CrossSectionScaleFactor property that multiplites x-section 
# for the MetaData printout by a num. factor
#
# use-case: Pythia Z->tautau -> lept. decay with 2lepEF (lep=e,mu)
#           tau BR determined by Tauola and is not picked by Pythia => wrong AMI value
#           => take BR into account at jO level by setting CrossSectionScaleFactor
#
#________________________________________________________________________________________
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

# ... Pythia
include ( "MC10JobOptions/MC10_Pythia_Common.py" )
#________________________________________________________________________________________
# Proc. setup
#________________________________________________________________________________________
Pythia.PythiaCommand += [ "pysubs msel 0",
                          "pydat1 parj 90 20000", # Turn off FSR.
                          "pydat3 mdcy 15 1 0",   # Turn off tau decays.
                          "pysubs msub 1 1",      # Create Z bosons.
                          "pysubs ckin 1 60.0",   # Lower invariant mass.
                          "pydat3 mdme 174 1 0",
                          "pydat3 mdme 175 1 0",
                          "pydat3 mdme 176 1 0",
                          "pydat3 mdme 177 1 0",
                          "pydat3 mdme 178 1 0",
                          "pydat3 mdme 179 1 0",
                          "pydat3 mdme 182 1 0",    # Switch for Z->ee.
                          "pydat3 mdme 183 1 0",
                          "pydat3 mdme 184 1 0",    # Switch for Z->mumu.
                          "pydat3 mdme 185 1 0",
                          "pydat3 mdme 186 1 1",    # Switch for Z->tautau.
                          "pydat3 mdme 187 1 0",
                          "pydat3 mdme 188 1 0",
                          "pydat3 mdme 189 1 0"
                          ]
#________________________________________________________________________________________
# Part. Decay done by Gen-s other than Pythia
#________________________________________________________________________________________
# ... Tauola
include ( "MC10JobOptions/MC10_Tauola_Fragment_LeptonicDecay.py" )
# multiplicative factor for AMI MetaData
Pythia.CrossSectionScaleFactor=0.1277 

# ... Photos
include ( "MC10JobOptions/MC10_Photos_Fragment.py" )
#________________________________________________________________________________________
# Filter
#________________________________________________________________________________________

from GeneratorFilters.GeneratorFiltersConf import MultiLeptonFilter
topAlg += MultiLeptonFilter()

MultiLeptonFilter = topAlg.MultiLeptonFilter
MultiLeptonFilter.Ptcut = 5000.
MultiLeptonFilter.Etacut = 3.0
MultiLeptonFilter.NLeptons = 2

try:
    StreamEVGEN.RequireAlgs += [ "MultiLeptonFilter" ]
except Exception, e:
    pass            
#________________________________________________________________________________________
from MC10JobOptions.PythiaEvgenConfig import evgenConfig
evgenConfig.efficiency = 0.34
#________________________________________________________________________________________
