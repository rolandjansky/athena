# $Id$
#
# @file TruthD3PDMaker/share/atlfast1D3PD_topOptions.py
# @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
# @date Feb, 2010
# @brief Example for building a D3PD from atlfast1 containers.
#


###################################################################
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["AOD.pool.root"]
#athenaCommonFlags.FilesInput= ["/afs/cern.ch/user/b/bruneli/scratch0/temp/user09.RenaudBruneliere.106124.AlpgenJimmyGamNp2_pt20_v1.AOD.atlfast1._01101.pool.root"]
#athenaCommonFlags.FilesInput= ["/afs/cern.ch/user/b/bruneli/scratch0/temp/zee.AOD.pool.root"]

###################################################################
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'atlfast1.root'


###################################################################
# Define other job options here.
#

athenaCommonFlags.EvtMax = -1

# Set following flag to True if you want to use Corrected Atlfast1 containers
from TruthD3PDMaker.Atlfast1D3PDMakerFlags import Atlfast1D3PDMakerFlags
Atlfast1D3PDMakerFlags.UseAtlfast1Correction = False
if Atlfast1D3PDMakerFlags.UseAtlfast1Correction: Atlfast1D3PDMakerFlags.ContainerPrefix = 'Corrected'

# Example of changing D3PD maker flags.
# Redefine SG keys for atlfast1 running
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthSGKey     = 'GEN_EVENT,GEN_AOD,TruthEvent'
D3PDMakerFlags.PhotonSGKey    = Atlfast1D3PDMakerFlags.ContainerPrefix()+'AtlfastPhotonCollection'
D3PDMakerFlags.ElectronSGKey  = Atlfast1D3PDMakerFlags.ContainerPrefix()+'AtlfastElectronCollection'
D3PDMakerFlags.MuonSGKey      = Atlfast1D3PDMakerFlags.ContainerPrefix()+'AtlfastMuonCollection'
D3PDMakerFlags.JetSGKey       = Atlfast1D3PDMakerFlags.ContainerPrefix()+'AtlfastJetContainer'
D3PDMakerFlags.MissingETSGKey = 'AtlfastMissingEt'
D3PDMakerFlags.TauSGKey       = 'AtlfastTauJetContainer'
D3PDMakerFlags.TrackSGKey     = 'AtlfastTrackParticles'

# Fix by hand mismatch in collection name for jets
if Atlfast1D3PDMakerFlags.UseAtlfast1Correction: D3PDMakerFlags.JetSGKey = 'CorrectedAtlfastJetCollection' 


from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
ConfigureFromListOfKeys (['ProjectName',
                          'BeamType',
                          'RealOrSim',
                          'DoTruth',
                          'LumiFlags'])

###################################################################
# Configure RecExCommon.
#
# Need to set dummy values for atlfast1 when running on pure atlfast1 AODs
# if you are running on normal AODs (full simulation), comment next 4 lines
from AthenaCommon.GlobalFlags import GlobalFlags,globalflags
GlobalFlags.DetGeo.set_atlas()
globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-CSC-00-00-00')
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-CSC-02-00-00")

from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
include ("RecExCommon/RecExCommon_topOptions.py")


###################################################################
# Make the D3PD.
#
from TruthD3PDMaker.atlfast1D3PD import atlfast1D3PD
alg = atlfast1D3PD (tupleFileOutput)
