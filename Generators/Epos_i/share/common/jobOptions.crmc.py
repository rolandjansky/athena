# ----------------------------------------------------------------------
# Example Epos JO file, adapted for Epos-1.99.crmc.r2790.
# Author: Sebastian Piec
#
# ! IMPORTANT !
# External config file crmc.param is automatically installed to Epos_i/run folder.
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
# Check existance of input files
# ----------------------------------------------------------------------
paramFile = "epos_crmc.param"

inputFiles = "qgsjet.dat \
              qgsjet.ncs \
              sectnu-II-03 \
              epos.initl \
              epos.iniev \
              epos.inirj \
              epos.inics \
              epos.inirj.lhc \
              epos.inics.lhc " 

os.system("mkdir tabs")

# get files from InstallArea
os.system("get_files %s" % inputFiles)
for fin in inputFiles.split():
  os.system("mv %s tabs/%s" % (fin, fin))


if not os.path.isfile( paramFile ):
  print "\"", paramFile, "\""," is missing!. Force exit..."
  # force exit
  sys.exit()
else:
  print "\"", paramFile, "\""," found!."

# ----------------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr
from AthenaServices.AthenaServicesConf import AtRndmGenSvc

ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["EPOS 174947321 159267993", "EPOS_INIT 820021 2347532"]

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# ----------------------------------------------------------------------
# Epos initialization 
# ----------------------------------------------------------------------
from Epos_i.Epos_iConf import Epos

epos = Epos()
epos.BeamMomentum     = -3500.0
epos.TargetMomentum   = 3500.0
epos.PrimaryParticle  = 1
epos.TargetParticle   = 1
epos.Model            = 0  # [0=EPOS_LHC (default), 1=EPOS_1.99, 2=QGSJET01, 6=Sibyll_2.1, 7=QGSJETII-04, 11=QGSJETII-03]
epos.ParamFile        = paramFile         # FIXME?
eps.LheOutput         = 0 # yes=1 no-0
epos.LheFile          = "epos.lhe"
epos.TabCreate        = 0   # force tab creation yes-1 no-0
job += epos

genSeq.ExtSvc += ["AtRndmGenSvc"]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3

# Number of events to be processed (default is 10)
#theApp.EvtMax = 200
evgenConfig.minevents = 100

# ----------------------------------------------------------------------
# Printing service
# ----------------------------------------------------------------------
from TruthExamples.TruthExamplesConf import DumpMC
#job += DumpMC()

# ----------------------------------------------------------------------
# Ntuple service output
# ----------------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
stream1.WritingTool = "AthenaOutputStreamTool"
stream1.OutputFile = "epos_events.pool.root"
stream1.TakeItemsFromInput = True
stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]
# ----------------------------------------------------------------------

