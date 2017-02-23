###################################################
# Job options to run Charybdis/Herwig/Jimmy with
# release 13.0.X and after
#
# Modified: K.Loureiro, August 29, 2008
#           Added additional parameters for Charybdis_i
#
# It also runs well wiht 14.0.0.Y (April 3, 2008)
###################################################
###############################################################
#
# Job options file
#
#==============================================================
#
# last modified - 11/05/08 - Cano Ay [aycano@cern.ch]
# last modified - 9/09/2008 - Karina Loureiro [karina.Loureiro@cern.ch]

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

# make sure we are loading the ParticleProperty service
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 50
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["HERWIG 468703135 2145174067", "HERWIG_INIT 468703135 2145174067"]
# AtRndmGenSvc.ReadFromFile = true;

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Herwig_i.Herwig_iConf import Herwig
job += Herwig()
job.Herwig.HerwigCommand = ["modpdf 10042",
                            "maxpr 5",
                            "autpdf HWLHAPDF",
                            "msflag 1",  #calls jimmy
                            "jmbug 0",
                            "iproc charybdis",  # select user process
                            "charyb 2 6",       # total number of dimensions 
                            "charyb 3 5000",    # minimum mass
                            "charyb 4 14000",    # maximum mass
                            "charyb 5 2",       # number of partiles in remant decay
                            "charyb 6 1",       # time variation of temperature
                            "charyb 7 1",       # grey body factors
                            "charyb 8 1",       # kinematic limit
                            #Options below have their default values assigned to them
                            "charyb 13 2",      # MSSDEF=1 -> M_GT, MSSDEF=2 ->M_DL and MSSDEF=3 -> M_D
                            "charyb 14 0",      # Momentum scale for calling PDFs
                            "charyb 15 3",      # =1 no heavy particles, =2 t,W,Z, =3 t,W,Z,Higgs
                            "charyb 16 1",      # 0=no printout, 1=errors only, 2=errors+info
                            "charyb 17 5000"    # setting the beam energy
                            ]

from TruthExamples.TruthExamplesConf import DumpMC
job += DumpMC()


#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from PoolSvc.PoolSvcConf import PoolSvc
ServiceMgr += PoolSvc()

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
#Stream1.ForceRead=TRUE
Stream1.OutputFile = "charybdisHerwig.evgen.pool.root"
Stream1.ItemList += [ "EventInfo#*", "McEventCollection#*" ]

#---------------------------------------------------------------
    
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
#==============================================================
#
# End of job options file
#
###############################################################
