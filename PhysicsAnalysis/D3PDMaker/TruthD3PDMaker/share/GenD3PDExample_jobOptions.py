
###################################################################3
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput = [ "AOD.pool.root" ]
athenaCommonFlags.FilesInput= ["/afs/phas.gla.ac.uk/user/p/pmullen/sw-test/EVNTFiles/mc12_8TeV.107283.AlpgenJimmy_AUET2CTEQ6L1_WbbNp3.evgen.EVNT.e1601_tid01025138_00/EVNT.01025138._000002.pool.root.1"]


###################################################################3
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'gen_objects.root'


###################################################################3
# Define other job options here.
#

athenaCommonFlags.EvtMax = 100

###################################################################
# Configure RecExCommon.
#

from RecExConfig.RecFlags import rec
rec.AutoConfiguration.set_Value_and_Lock( [ "everything" ] )
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doForwardDet.set_Value_and_Lock(False)
include ("RecExCommon/RecExCommon_topOptions.py")

###################################################################3
# Make the D3PD.
#
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.NewRootStream( "StreamNTUP_GEN", tupleFileOutput, "gend3pd" )

from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags

from TruthD3PDAnalysis.GenObjectsFilterTool import *

from TruthD3PDMaker.GenEventD3PDObject import GenEventD3PDObject
alg += GenEventD3PDObject( 10, filter = AllTrackFilterTool(),
                           pileup_CollectionGetterRegistry=\
                               alg.name()+'_CollectionGetterRegistry')

from TruthD3PDMaker.GenVertexD3PDObject import GenVertexD3PDObject
alg += GenVertexD3PDObject( 10, filter = AllTrackFilterTool() )

from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
alg += GenParticleD3PDObject( 10, filter = AllTrackFilterTool() )

### stable charged particle with perigee info
from TruthD3PDMaker.GenParticleD3PDObject import GenTruthTrackD3PDObject
alg += GenTruthTrackD3PDObject( 10, filter = TruthTrackFilterTool() )

### you can link to the gen particle (e.g from tracks or btag truth lepton info)
### using the gen particle getter label: TruthD3PDKeys.GenParticleGetterLabel()
