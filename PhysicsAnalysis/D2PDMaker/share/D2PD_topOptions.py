# =============================================================================
#  Name:        D2PD_topOptions.py
# 
#  Author:      Karsten Koeneke (DESY)
#  Created:     April 2009

#  Description: job options for all AOD->D2PD and D1PD->D2PD.
# 
#  Usage: Here, all neccessary job options for the D2PDs can be set.
#         To run, type:
#              athena D2PDMaker/D2PD_topOptions.py 2>&1 | tee log.txt
# =============================================================================


# ==============================================================================
# Include the job property flags for this package and for the RecExCommon package
# ==============================================================================
from D2PDMaker.D2PDFlags import D2PDFlags
from RecExConfig.RecFlags import rec


# ==============================================================================
# If you have your own DPD Maker scripts
# (see:
#                    share/D2PD_ExampleSimple*.py
# for examples),
# then just append your script (wherever it is) to this list:
#
#       rec.DPDMakerScripts.append("MyPackage/MyScript")
#
# The example scripts are appended below, so you can see how it works!
# ==============================================================================
#rec.DPDMakerScripts.append("D2PDMaker/CheckD2PD_jobOptions.py")
#rec.DPDMakerScripts.append("D2PDMaker/D2PD_Test.py")
#rec.DPDMakerScripts.append("D2PDMaker/D2PD_ExampleSimpleZee.py")
#rec.DPDMakerScripts.append("D2PDMaker/D2PD_ExampleSimpleZmumu.py")
#rec.DPDMakerScripts.append("D2PDMaker/D2PD_ExampleSimpleZtautau.py")
#rec.DPDMakerScripts.append("D2PDMaker/D2PD_ExampleSimpleHgamgam.py")
#rec.DPDMakerScripts.append("D2PDMaker/D2PD_ExampleSimpleWmunu.py")
#rec.DPDMakerScripts.append("D2PDMaker/D2PD_ZeeStream.py")
#rec.DPDMakerScripts.append("D2PDMaker/D2PD_WenuStream.py")
#rec.DPDMakerScripts.append("D2PDMaker/DAOD_ElLoose18.py")
#rec.DPDMakerScripts.append("D2PDMaker/DAOD_HighMDiJet.py")
#rec.DPDMakerScripts.append("D2PDMaker/DAODM_HighMDiJet.py")
rec.DPDMakerScripts.append("D2PDMaker/DAOD_H4lBremRec.py")


# For Higgs->4l signal MC
D2PDFlags.WriteDAOD_H4lBremRecStream.ApplySkimming = False

#(time gathena --profilerMode eventLoop --profilerInitEvent 15 --profilerOutput my.athena.profile  -c SKIPEVT=4850   ../share/D2PD_topOptions.py) >& log.txt &!


# ==============================================================================
# Load your input file
# ==============================================================================
from AthenaCommon.AthenaCommonFlags import jobproperties as jp
#jp.AthenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/project/PAT/data/data.physics_Egamma.AOD.pool.root"]

from glob import glob
myPath = "/eos/atlas/atlasgroupdisk/phys-higgs/dq2/mc12_8TeV/AOD/e1191_s1469_s1470_r3542_r3549/mc12_8TeV.160155.PowhegPythia8_AU2CT10_ggH125_ZZ4lep.merge.AOD.e1191_s1469_s1470_r3542_r3549_tid781451_00/*"
jp.AthenaCommonFlags.FilesInput = glob(myPath)
#jp.AthenaCommonFlags.FilesInput=["/eos/atlas/atlasgroupdisk/phys-higgs/dq2/mc12_8TeV/AOD/e1191_s1469_s1470_r3542_r3549/mc12_8TeV.160155.PowhegPythia8_AU2CT10_ggH125_ZZ4lep.merge.AOD.e1191_s1469_s1470_r3542_r3549_tid781451_00/AOD.781451._000019.pool.root.1"]

# ==============================================================================
# You can change the location and name of your output file with
# these three flags:
# ==============================================================================
#D2PDFlags.OutputDirectoryName = "/my/directory/with/enough/space/"




# ==============================================================================
# Say which parts of AODSelect you want to run, if any
# ==============================================================================
from RecExConfig.RecFlags import rec
rec.doAODSelect = True


# ==============================================================================
#           ---- NO NEED TO CHANGE ANYTHING BELOW THIS LINE !!! ----
# ==============================================================================


# ==============================================================================
# Set the number of events that you want to process
# ==============================================================================
if not vars().has_key('SKIPEVT'): SKIPEVT = 0
jp.AthenaCommonFlags.SkipEvents = SKIPEVT
if not vars().has_key('EVTMAX'): EVTMAX = -1
jp.AthenaCommonFlags.EvtMax = EVTMAX


# ==============================================================================
# Execute the dpd maker
# ==============================================================================
from RecExConfig.RecFlags import rec
# Turn off most of RecExCommon... (optional)
rec.doWriteTAG   = False 
rec.doHist       = False
#rec.LoadGeometry = False
#rec.doApplyAODFix.set_Value_and_Lock(False)

# Mandatory for ESD->DPD or AOD->DPD or DPD->DPD
rec.doDPD   = True


# For brem fit
if D2PDFlags.WriteDAOD_H4lBremRecStream() and D2PDFlags.WriteDAOD_H4lBremRecStream.DoBremRec :
    from RecExConfig.RecAlgsFlags  import recAlgs
    recAlgs.doEgammaBremReco.set_Value_and_Lock (True)
    include("egammaBremRec/egammaBremRec_preInclude.py")
    #recAlgs.doEgammaBremReco.set_Value_and_Lock(True)
    #include( "RecExCond/RecExCommon_flags.py" )
    #DetFlags.ID_setOn()
    #DetFlags.LAr_setOn()
    pass

#from AthenaCommon.DetFlags import DetFlags
#DetFlags.ID_setOn()


# Output file name handling. No need to change anything here
if len( D2PDFlags.OutputDirectoryName() ) >= 1 and not D2PDFlags.OutputDirectoryName().endswith("/") :
    D2PDFlags.OutputDirectoryName = D2PDFlags.OutputDirectoryName()+"/"    
    pass

# Print the used configuration to the log file
if rec.OutputLevel() <= INFO:
    print rec
    print D2PDFlags
    pass

# =============================================================================
# Final debugging options...
# Dump list of containers on StoreGate to output log
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True
#svcMgr.StoreGateSvc.Dump = True     # Prints the content of StoreGate (EventStore) 
#svcMgr.MetaDataStore.Dump = True    # Prints output metadata store content
#svcMgr.InputMetaDataStore.Dump=True # Prints input metadata store content
#theApp.Dlls  += ["TruthExamples" ]
#theApp.TopAlg += [ "DumpMC" ]    # These two lines will dump some
#DumpMC = Algorithm ( "DumpMC" )  # info about the MC truth
#DumpMCG4.McEventKey = "SpclMC"
#topSequence.InsertTruthToolLooper.TrueJetInserter.OutputLevel = DEBUG 
# =============================================================================

# The job starts here!
include ("RecExCommon/RecExCommon_topOptions.py")

if not vars().has_key('EVTPRINT'): EVTPRINT = 100
ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = EVTPRINT
