include( "PartPropSvc/PartPropSvc.py" )
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#load the gen pool file with stable taus
import glob
files=glob.glob("/atlasdata/users/ndavidson/user09.NadiaMayDavidson.106062.McAtNloZtautau.evgen/*.pool.root")
###files=glob.glob("*.pool.root")
svcMgr.EventSelector.InputCollections = files

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#set-up tauola
from Tauolapp_i.Tauolapp_iConf import TauolaAlg
TauolaAlg=TauolaAlg( "TauolaAlg" )
#TauolaAlg.decay_mode_same(1); #tau- decay to electron + neutrinos
#TauolaAlg.decay_mode_opposite(2); #tau+ decays to muon + neutrinos
TauolaAlg.decay_mode_same = 1; #tau- decay to electron + neutrinos
TauolaAlg.decay_mode_opposite = 2; #tau+ decays to muon + neutrinos
TauolaAlg.decay_particle = 15; # decay particle
job += TauolaAlg


theApp.EvtMax = 10 #Number of events to process
theApp.SkipEvents = 10 #Number of events to process

###################################################################
#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
#
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.OutputFile = "tauola_decayed.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#GEN_EVENT']

#==============================================================

