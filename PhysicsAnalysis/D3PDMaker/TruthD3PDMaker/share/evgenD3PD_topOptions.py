# $Id$
#
# @file TruthD3PDMaker/share/evgenD3PD_topOptions.py
# @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
# @date Apr, 2010
# @brief Example for building a D3PD from event generation file.
#


###################################################################
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput= ["EVNT.pool.root"]
athenaCommonFlags.FilesInput= ["/afs/cern.ch/user/b/bruneli/scratch0/temp/mc10_7TeV.114611.SherpaW5jetstoenu30GeV.evgen.EVNT.e711_tid274882_00/EVNT.274882._000157.pool.root.1"]

###################################################################
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'evgen.root'


###################################################################
# Define other job options here.
#

athenaCommonFlags.EvtMax = -1

# Example of changing D3PD maker flags.
# Redefine SG keys for evgen running
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthSGKey = 'GEN_EVENT,GEN_AOD,TruthEvent'

from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
ConfigureFromListOfKeys (['ProjectName',
                          'BeamType',
                          'RealOrSim',
                          'DoTruth',
                          'LumiFlags'])

###################################################################
# Configure RecExCommon.
#
# Need to set dummy values for when running on pure EVNT files
# if you are running on normal AODs (full simulation), comment next 4 lines
from AthenaCommon.GlobalFlags import GlobalFlags,globalflags
GlobalFlags.DetGeo.set_atlas()
globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-DR-BS7T-ANom-12')
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-GEO-10-00-00")

from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
rec.doForwardDet.set_Value_and_Lock(False)
rec.readAOD.set_Value_and_Lock(True)

# The following flags are mandatory if you build jets
from JetRec.JetRecFlags import jetFlags
jetFlags.noStandardConfig.set_Value_and_Lock(True)
jetFlags.evgenJets.set_Value_and_Lock(True)

include ("RecExCommon/RecExCommon_topOptions.py")

# Build truth missing Et
include( "ParticleBuilderOptions/MissingEtTruth_jobOptions.py" )
METPartTruth.TruthCollectionName="GEN_EVENT"
topSequence.METAlg+=METPartTruth

###################################################################
# Make the D3PD.
#
from TruthD3PDMaker.evgenD3PD import evgenD3PD
alg = evgenD3PD (tupleFileOutput)
