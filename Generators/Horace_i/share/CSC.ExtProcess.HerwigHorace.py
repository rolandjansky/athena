###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )


#--------------------------------------------------------------
# Generator
#--------------------------------------------------------------

# ... Main generator : Herwig
from AthenaCommon.AlgSequence import AlgSequence 
topAlg = AlgSequence("TopAlg") 

from Herwig_i.Herwig_iConf import Herwig
topAlg += Herwig()

Herwig = topAlg.Herwig
Herwig.HerwigCommand = ["iproc horace",
                        "modpdf 10042",
                        "autpdf HWLHAPDF", 
                        "msflag 1",
                        "jmbug 0",
                        "jmueo 1",
                        "ptjim 4.91",
                        "jmrad 73 1.8",
                        "pltcut 0.0000000000333",
                        "ptmin 10.",
                        "prsof 0",
                        "rmass 198 80.425",
                        "rmass 199 80.425",
                        "rmass 200 91.19",
                        "gamw 2.124",
                        "gamz 2.495"]

#--------------------------------------------------------------
# Filter
#--------------------------------------------------------------

from GeneratorFilters.GeneratorFiltersConf import LeptonFilter
topAlg += LeptonFilter()

LeptonFilter = topAlg.LeptonFilter
LeptonFilter.Ptcut = 5000.
LeptonFilter.Etacut = 2.8

#---------------------------------------------------------------
# POOL / Root output
#---------------------------------------------------------------

try:
     Stream1.RequireAlgs +=  [ "LeptonFilter" ]
except Exception, e:
     pass

#==============================================================
#
# End of job options file
#
###############################################################
#--------------------------------------------------------------
# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
from EvgenJobOptions.HerwigEvgenConfig import evgenConfig
evgenConfig.efficiency = 0.71
