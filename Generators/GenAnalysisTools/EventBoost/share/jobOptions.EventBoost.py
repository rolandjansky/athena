if not 'MyEvtMax' in dir():
    MyEvtMax = -1
print "JobSpecificParameter::MyEvtMax = %s" % MyEvtMax

if not 'MyInputCollections' in dir():
    print "ERROR: Please set MyInputCollections!!"
#print "JobSpecificParameter::MyInputCollections = %s" % MyInputCollections


################## Read events ###################

#import AthenaCommon.AtlasUnixGeneratorJob
import AthenaCommon.AtlasUnixStandardJob
import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/e/eklinkby/myfile.root"]

include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.defaultLimit = 99999999

theApp.EvtMax = -1
########### Modify events ############

from PartPropSvc.PartPropSvcConf import PartPropSvc
svcMgr += PartPropSvc()

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from EventBoost.EventBoostConf import EventBoost
job += EventBoost()

EventBoost = job.EventBoost
EventBoost.BeamInclination = 0. # micro radians. Default value: 0. at LHC start up, later: 142.5 taken from: https://edms.cern.ch/file/445830/5/Vol_1_Chapter_2.pdf.
EventBoost.GenEvtInputKey  = "GEN_EVENT"
EventBoost.GenEvtOutputKey = "GEN_EVENT_BOOSTED"
EventBoost.DoGaussianVertexSmearing = False
EventBoost.GaussianWidth       += [ 0.01, 0.01, 56.0] # mm  x, y, z. Default taken from: Simulation/G4Sim/MCTruth/MCTruth/VertexPositioner.hh 
EventBoost.GaussianMean        += [ 0.0, 0.0, 0.0] # mm  x, y, z
EventBoost.DoFlatVertexSmearing = False
EventBoost.FlatSmearingBoundaryMin += [ -0.01, -0.01, -0.01] #mm  x, y, z
EventBoost.FlatSmearingBoundaryMax += [  0.01,  0.01,  0.01] #mm  x, y, z
EventBoost.OutputLevel = 3



######### Streams & persistency #########
##################################################
# Setup output stream.                           #
##################################################
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
myOutStream = AthenaPoolOutputStream( "myOutStream" )
myOutStream.EvtConversionSvc = "AthenaPoolCnvSvc"
myOutStream.OutputFile = "boosted.pool.root"
myOutStream.ItemList += [ "EventInfo#*", "McEventCollection#*" ]
myOutStream.ForceRead=TRUE

##################################################
# Do consistency checks                          #
##################################################

#from TruthExamples.TruthExamplesConf import TestHepMC
#job += TestHepMC()
#job.TestHepMC.EnergyDifference = 0.001*GeV # tolerance on energy conservation
#job.TestHepMC.OutputLevel = WARNING

##################################################
# Make CBNT                                      #
##################################################

#include( "DC3_joboptions/GentoAtlfasttoCBNT.py" )
#Atlfast.members += ["Atlfast::TrackNtupleMaker/TrackNtupleMaker"]
#Atlfast.members += ["Atlfast::StandardNtupleMaker/StandardNtupleMaker"]
#include( "DC3_joboptions/GentoAtlfasttoAOD.py" )
#include( "TestAtlfast/POOLtoAtlfasttoCBNT.py" )

#CBNT_Atlfast.FillHistory = True
#CBNT_Atlfast.NtupleLocID="/FILE1/CBNT/t3333"




###############  Print algorithms ################
print "List of all top algorithms"
print theApp.TopAlg
print "List of all streams"
print theApp.OutStream
print "List of all stream types"
print theApp.OutStreamType


##################################################
