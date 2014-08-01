###############################################################################
#
# PythiaB_Master.py
# Author: ATLAS B-physics group
# Use this script to execute standard production job options.
#
###############################################################################

#------------------------------------------------------------------------------
# General Application Configuration options
#------------------------------------------------------------------------------

import AthenaCommon.AtlasUnixGeneratorJob
include( "PartPropSvc/PartPropSvc.py" )
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

#------------------------------------------------------------------------------
# Events to execute
#------------------------------------------------------------------------------

theApp.EvtMax = 100
ServiceMgr.EventSelector.RunNumber  = 108524
ServiceMgr.EventSelector.FirstEvent = 1

#------------------------------------------------------------------------------
# Random seeds initialization
#------------------------------------------------------------------------------

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
AtRndmGenSvc = AtRndmGenSvc()
ServiceMgr  += AtRndmGenSvc

# See EvgenJobTransforms/Evgen_randomseeds.py
AtRndmGenSvc.Seeds = ['PYTHIA 47898993 0', 'PYTHIA_INIT 820021 2347532']

#------------------------------------------------------------------------------
# Output level threshold: 1=VERBOSE 2=DEBUG 3=INFO 4=WARNING 5=ERROR 6=FATAL
#------------------------------------------------------------------------------

ServiceMgr.MessageSvc.OutputLevel  = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999 # all messages

#------------------------------------------------------------------------------
# HERE INCLUDE THE PRODUCTION JOB-OPTIONS FRAGMENT
#------------------------------------------------------------------------------

include( "MC11.108424.PythiaB_Bs_Jpsi_mu0mu0_phi_KK_nopTcuts_PhysAngles.py" )

# Set CMS energy
PythiaB.PythiaCommand += [ "pyinit win 7000"]

#------------------------------------------------------------------------------
# POOL persistency
#------------------------------------------------------------------------------

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )

# 2101 = EventInfo, 133273 = MCTruth (HepMC)
Stream1.ItemList  += [ "2101#*", "133273#*" ]
Stream1.OutputFile = "evgen.pool.root"

###############################################################################
#
# End of PythiaB_Master.py job-options
#
###############################################################################
