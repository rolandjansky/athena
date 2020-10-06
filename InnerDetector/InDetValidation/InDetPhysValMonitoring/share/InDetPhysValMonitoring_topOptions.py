# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Define a few configuration arguments that can be passed by the user via the command line.
# Avoid the need of hacking the job option file 
def GetCustomAthArgs():
    from AthenaCommon.AthArgumentParser import AthArgumentParser
    IDPVMparser = AthArgumentParser(description='Parser for IDPVM configuration')
    IDPVMparser.add_argument("--doTightPrimary", help='also run tight-primary plots', action='store_true', default=False)
    IDPVMparser.add_argument("--doTracksInJets", help='also run tracks in jets', action='store_true', default=False)
    IDPVMparser.add_argument("--doTracksInBJets", help='also run tracks in jets', action='store_true', default=False)
    IDPVMparser.add_argument("--doExpertPlots", help='run additional expert-level plots', action='store_true', default=False)
    IDPVMparser.add_argument("--outputFile", help='Name of output file',default="M_output.root")
    IDPVMparser.add_argument("--HSFlag", help='Hard-scatter flag - decides what is used for truth matching', choices=['HardScatter', 'All', 'PileUp'],default="HardScatter")
    return IDPVMparser.parse_args()

# Parse the arguments 
MyArgs = GetCustomAthArgs()

# Forward the args into the InDetPhysValFlags used to steer our job 
from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
InDetPhysValFlags.setTruthStrategy.set_Value_and_Lock(MyArgs.HSFlag)
InDetPhysValFlags.doExpertOutput.set_Value_and_Lock(MyArgs.doExpertPlots)
InDetPhysValFlags.doPhysValOutput.set_Value_and_Lock(not MyArgs.doExpertPlots)
InDetPhysValFlags.doValidateTracksInBJets.set_Value_and_Lock(MyArgs.doTracksInBJets)
InDetPhysValFlags.doValidateTracksInJets.set_Value_and_Lock(MyArgs.doTracksInJets)
InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(MyArgs.doTightPrimary)

# Print the configuration
print(" ==== Final Phys Val flags to run === ")
print(InDetPhysValFlags)

# Set up the I/O and the RecExCommon infrastructure 
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSlimming.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
include( "AthenaPython/iread_file.py" )

# Add our monitoring manager to the topSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager",
  FileKey = "M_output",
  Environment = "altprod",
  ManualDataTypeSetup = True,
  DataType = "monteCarlo",
  ManualRunLBSetup = True,
  Run = 1,
  LumiBlock = 1  )
topSequence += monMan

# Now we can attach our IDPVM tool instances 
# To do this, we leverage the existing job options from PhysValMonitoring
include ("PhysValMonitoring/PhysValInDet_jobOptions.py") 
  
# finally, set up the infrastructure for writing our output
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["M_output DATAFILE='"+MyArgs.outputFile+"' OPT='RECREATE'"]
