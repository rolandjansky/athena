from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags


# various configuration special to commissioning, needed for all jobs
athenaCommonFlags.PoolESDInput.set_Value_and_Lock(['castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data08_cosmag/physics_IDCosmic/0091900/data08_cosmag.00091900.physics_IDCosmic.recon.ESD.o4_f73/data08_cosmag.00091900.physics_IDCosmic.recon.ESD.o4_f73._lb0001._sfo01._0001.1'])

rec.AutoConfiguration=['everything']


# all of these should now be autoconfigured - but i leave here incase 
#jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-04-00-00')
#globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-ES1C-000-00')
# for running over data with field on need this to be setto true
#from AthenaCommon.BFieldFlags import jobproperties
#jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
#jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
#jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)


#hardcoded output specification (corresponds to optional flags from runArgs in transform skeleton)
rec.RootNtupleOutput.set_Value_and_Lock("newcommission.ntuple.root")
#rec.RootHistoOutput.set_Value_and_Lock("monitoring.root")
athenaCommonFlags.EvtMax.set_Value_and_Lock(10)
#athenaCommonFlags.SkipEvents = 0

# run ID reconstruction
#from InDetRecExample.InDetJobProperties import InDetFlags
#jobproperties.InDetJobProperties.Enabled.set_Value_and_Lock(True)
#For version < 14.4.0: it requires to set in RecExCond/RecExCommon_DetFlags.py
# the following: DetFlags.makeRIO.ID_setOn()


# run Muons reconstruction
#from MuonRecExample.MuonRecFlags import muonRecFlags
#jobproperties.MuonRec.Enabled.set_Value_and_Lock(True)

# run combined muon reconstruction
#from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags 
#jobproperties.MuonCombinedRec.Enabled.set_Value_and_Lock(True)

#uncomment to do egamma
#rec.doEgamma.set_Value_and_Lock(True)
#from egammaRec.egammaRecFlags import jobproperties
#jobproperties.egammaRecFlags.Enabled.set_Value_and_Lock(True)

#but not others
rec.doJetMissingETTag.set_Value_and_Lock(False)
from JetRec.JetRecFlags import jetFlags
jetFlags.Enabled.set_Value_and_Lock(False)
from RecExConfig.RecAlgsFlags  import recAlgs
recAlgs.doMissingET.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
recAlgs.doMuonSpShower.set_Value_and_Lock(False)



rec.doHist.set_Value_and_Lock(True)
rec.doCBNT.set_Value_and_Lock(True)
rec.doWriteTAGCOM.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)	
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(True)

rec.readESD.set_Value_and_Lock(True)
rec.doESD.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
globalflags.InputFormat.set_Value_and_Lock('pool')

if not 'Debug' in dir():
    Debug=True  # change to True to have printout information

if Debug:
    # DEBUG/VERBOSE options
    rec.doNameAuditor=True
    rec.doPerfMon=True
    rec.doDetailedPerfMon=True
    rec.doDumpProperties=True
    rec.doDumpTES=True
    # rec.OutputLevel = INFO

# the main jobOpt
include("RecExCommission/RecExCommission.py")                


from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.monType='BS_ESD'
DQMonFlags.histogramFile.set_Value_and_Lock( "myMonitoringESD.root" )
include ("RecExCommon/RecExCommon_topOptions.py")


