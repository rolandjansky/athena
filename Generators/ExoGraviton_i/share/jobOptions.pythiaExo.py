###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.Logging import logging
evgenLog = logging.getLogger('csc_evgen')

# special setup for event generation
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO

# Number of events to be processed (default is 10)
theApp.EvtMax = 10

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512","PYTHIA_INIT 820021 2347532"]

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Pythia_i.Pythia_iConf import Pythia
job +=Pythia()

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
job.Pythia.PythiaCommand = ["pyinit user exograviton"]


#Graviton parameters to be specified by the user:
# 1:number of extra dimensions
# 2:Process number(1110:qqbar->gG/ 1111:qg->qG/ 1112:gg->gG)
# 3:Total Centre of Mass Energy in GeV.
# 4:D-dimensional Planck scale, MD ,in GeV.
# 5:PtCut of the jet in the final state, in GeV.
# 6:Maximum Mass of the Graviton,in GeV.
# 7:Maximum value for cross-section(in pb).
# 8:CutOff mode (treatment of the effective theory when s_hat > MD^2).

#  The default values are : "grav 1 2","grav 2 1111", "grav 3 7000","grav 4 3500","grav 5 350","grav 6 3500","grav 7 2.5E+1", "grav 8 0 " .
#  Note that Process number 1111 is the most dominant sub-process in a p-p collider: qg->qG .
#  It is recommanded to set GravMaxMass = MD. Above this value, the low-energy effective field theory is not trustable.
#  If the maximum x-section you set is lower than a generated event, Pythia will update this value(See Pythia Manual for further information).
#  Different Cutoff Modes implemented :
#    0 : all events are kept.
#    1 : events with s_hat < MD^2 are given the usual weight (equal to the corresponding differential cross-section), and events with s_hat > MD^2 are given a weight equal to "differential x-section * (MD^4/s_hat^2)"
#    2 : only events with s_hat < MD^2 are kept, and the rest are given a weight of zero.

job.Pythia.PythiaCommand += ["grav 1 2","grav 2 1111",
                             "grav 3 7000","grav 4 3500",
                             "grav 5 350","grav 6 3500",
                             "grav 7 2.5E+1", "grav 8 0"]

job.Pythia.PythiaCommand += ["pyinit pylistf 1","pyinit dumpr 0 3",
                             "pystat 1"]

#----------------------------------------------------------------
from TruthExamples.TruthExamplesConf import DumpMC
#job += DumpMC()
 

#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#from AthenaServices.AthenaServicesConf import AthenaOutputStream
from PoolSvc.PoolSvcConf import PoolSvc
from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc

Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.ItemList += [ "EventInfo#*",
                      "McEventCollection#*" ]                        
Stream1.OutputFile = "Pythia_Graviton.pool.root"
#==============================================================
#
# End of job options file
#
###############################################################

