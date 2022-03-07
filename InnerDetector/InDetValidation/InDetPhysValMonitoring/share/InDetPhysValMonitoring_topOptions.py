# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

print("*********************************************************************************************************************************")
print("WARNING - You're running a recently deprecated IDPVM jobOptions: you're kindly invited to switch to the new runIDPVM.py executable")
print("WARNING - The IDPVM jobOptions you're currently running will soon be removed from the release")
print("*********************************************************************************************************************************")


# Define a few configuration arguments that can be passed by the user via the command line.
# Avoid the need of hacking the job option file 
def GetCustomAthArgs():
    from AthenaCommon.AthArgumentParser import AthArgumentParser
    IDPVMparser = AthArgumentParser(description='Parser for IDPVM configuration')
    IDPVMparser.add_argument("--doTightPrimary", help='also run tight-primary plots', action='store_true', default=False)
    IDPVMparser.add_argument("--doTracksInJets", help='also run tracks in jets', action='store_true', default=False)
    IDPVMparser.add_argument("--doTracksInBJets", help='also run tracks in jets', action='store_true', default=False)
    IDPVMparser.add_argument("--doTruthOrigin", help='make plots by track origin', action='store_true', default=False)
    IDPVMparser.add_argument("--doHitLevelPlots", help='make hit residual / eff plots', action='store_true', default=False)
    IDPVMparser.add_argument("--doPerAuthor", help='make plots by track author', action='store_true', default=False)
    IDPVMparser.add_argument("--doExpertPlots", help='run additional expert-level plots', action='store_true', default=False)
    IDPVMparser.add_argument("--doMuonMatchedTracks", help='run plots for tracks matched to true muons', action='store_true', default=False)
    IDPVMparser.add_argument("--doElectronMatchedTracks", help='run plots for tracks matched to true electrons', action='store_true', default=False)
    IDPVMparser.add_argument("--doTruthToRecoNtuple", help='output track-to-truth ntuple', action='store_true', default=False)
    IDPVMparser.add_argument("--hardScatterStrategy", help='Strategy to select the hard scatter. 0 = SumPt² 1 = SumPt 2 = Sumptw' , choices=["0","1", "2"], default="0")
    IDPVMparser.add_argument("--outputFile", help='Name of output file',default="M_output.root")
    IDPVMparser.add_argument("--HSFlag", help='Hard-scatter flag - decides what is used for truth matching', choices=['HardScatter', 'All', 'PileUp'],default="HardScatter")
    IDPVMparser.add_argument("--ancestorIDList", help='List of ancestor truth IDs to match.', default = [], nargs='+', type=int)
    IDPVMparser.add_argument("--requiredSiHits", help='Number of required truth sillicon hits', default = 1, type=int)
    return IDPVMparser.parse_args()

# Parse the arguments 
MyArgs = GetCustomAthArgs()

# Forward the args into the InDetPhysValFlags used to steer our job 
from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
InDetPhysValFlags.setTruthStrategy.set_Value_and_Lock(MyArgs.HSFlag)
InDetPhysValFlags.doExpertOutput.set_Value_and_Lock(MyArgs.doExpertPlots)
InDetPhysValFlags.doPhysValOutput.set_Value_and_Lock(not MyArgs.doExpertPlots)
InDetPhysValFlags.doValidateTruthToRecoNtuple.set_Value_and_Lock(MyArgs.doTruthToRecoNtuple)
InDetPhysValFlags.doValidateTracksInBJets.set_Value_and_Lock(MyArgs.doTracksInBJets)
InDetPhysValFlags.doValidateTracksInJets.set_Value_and_Lock(MyArgs.doTracksInJets)
InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(MyArgs.doTightPrimary)
InDetPhysValFlags.doTruthOriginPlots.set_Value_and_Lock(MyArgs.doTruthOrigin)
InDetPhysValFlags.doValidateMuonMatchedTracks.set_Value_and_Lock(MyArgs.doMuonMatchedTracks)
InDetPhysValFlags.doValidateElectronMatchedTracks.set_Value_and_Lock(MyArgs.doElectronMatchedTracks)
InDetPhysValFlags.doPerAuthorPlots.set_Value_and_Lock(MyArgs.doPerAuthor)
InDetPhysValFlags.doHitLevelPlots.set_Value_and_Lock(MyArgs.doHitLevelPlots)
InDetPhysValFlags.ancestorIDs.set_Value_and_Lock(MyArgs.ancestorIDList)
InDetPhysValFlags.requiredSiHits.set_Value_and_Lock(MyArgs.requiredSiHits)
InDetPhysValFlags.hardScatterStrategy.set_Value_and_Lock(int(MyArgs.hardScatterStrategy))

# Print the configuration
print(" ==== Final Phys Val flags to run === ")
print(InDetPhysValFlags)

# Set up the I/O and the RecExCommon infrastructure 
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSlimming.set_Value_and_Lock(False)
# Need to turn of explicitly some subsystems which otherwise
# cause difficulties when running on ESD
rec.doTrigger.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
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

