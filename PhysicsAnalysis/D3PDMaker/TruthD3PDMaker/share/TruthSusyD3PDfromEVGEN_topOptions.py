# @file TruthD3PDMaker/share/evgenD3PD_topOptions.py
# @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
# @date Apr, 2010
# @brief Example for building a D3PD from event generation file.

###################################################################
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.FilesInput= ["EVNT.pool.root"]
#athenaCommonFlags.FilesInput= ["EVNT.289844._000396.pool.root.1"]
athenaCommonFlags.FilesInput= ["/afs/phas.gla.ac.uk/user/p/pmullen/sw-test/EVNTFiles/mc12_8TeV.107283.AlpgenJimmy_AUET2CTEQ6L1_WbbNp3.evgen.EVNT.e1601_tid01025138_00/EVNT.01025138._000002.pool.root.1",
"/afs/phas.gla.ac.uk/user/p/pmullen/sw-test/EVNTFiles/mc12_8TeV.107283.AlpgenJimmy_AUET2CTEQ6L1_WbbNp3.evgen.EVNT.e1601_tid01025138_00/EVNT.01025138._000001.pool.root.1",
"/afs/phas.gla.ac.uk/user/p/pmullen/sw-test/EVNTFiles/mc12_8TeV.107283.AlpgenJimmy_AUET2CTEQ6L1_WbbNp3.evgen.EVNT.e1601_tid01025138_00/EVNT.01025138._000003.pool.root.1",
"/afs/phas.gla.ac.uk/user/p/pmullen/sw-test/EVNTFiles/mc12_8TeV.107283.AlpgenJimmy_AUET2CTEQ6L1_WbbNp3.evgen.EVNT.e1601_tid01025138_00/EVNT.01025138._000004.pool.root.1",
"/afs/phas.gla.ac.uk/user/p/pmullen/sw-test/EVNTFiles/mc12_8TeV.107283.AlpgenJimmy_AUET2CTEQ6L1_WbbNp3.evgen.EVNT.e1601_tid01025138_00/EVNT.01025138._000005.pool.root.1",
"/afs/phas.gla.ac.uk/user/p/pmullen/sw-test/EVNTFiles/mc12_8TeV.107283.AlpgenJimmy_AUET2CTEQ6L1_WbbNp3.evgen.EVNT.e1601_tid01025138_00/EVNT.01025138._000006.pool.root.1",
"/afs/phas.gla.ac.uk/user/p/pmullen/sw-test/EVNTFiles/mc12_8TeV.107283.AlpgenJimmy_AUET2CTEQ6L1_WbbNp3.evgen.EVNT.e1601_tid01025138_00/EVNT.01025138._000007.pool.root.1",
"/afs/phas.gla.ac.uk/user/p/pmullen/sw-test/EVNTFiles/mc12_8TeV.107283.AlpgenJimmy_AUET2CTEQ6L1_WbbNp3.evgen.EVNT.e1601_tid01025138_00/EVNT.01025138._000008.pool.root.1",
"/afs/phas.gla.ac.uk/user/p/pmullen/sw-test/EVNTFiles/mc12_8TeV.107283.AlpgenJimmy_AUET2CTEQ6L1_WbbNp3.evgen.EVNT.e1601_tid01025138_00/EVNT.01025138._000009.pool.root.1",
"/afs/phas.gla.ac.uk/user/p/pmullen/sw-test/EVNTFiles/mc12_8TeV.107283.AlpgenJimmy_AUET2CTEQ6L1_WbbNp3.evgen.EVNT.e1601_tid01025138_00/EVNT.01025138._0000010.pool.root.1"]

###################################################################
# Define the output file here.
#
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags
if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'susy.root'
TruthD3PDFlags.TruthD3PDOutputFileName = tupleFileOutput


###################################################################
# Define other job options here.
#

#athenaCommonFlags.EvtMax = 1000
athenaCommonFlags.EvtMax = -1


from RecExConfig.RecFlags import rec
rec.doHist=False
rec.doMonitoring=False
rec.doCBNT=False
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False	

include("TruthD3PDMaker/TruthD3PDfromEVGEN_preInclude.py")

include("RecExCommon/RecExCommon_topOptions.py")
#from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
#D3PDMakerFlags.TruthSGKey = 'GEN_EVENT,GEN_AOD,TruthEvent'
#D3PDMakerFlags.DoTrigger  = False
#D3PDMakerFlags.TruthWriteBHadrons=True
#D3PDMakerFlags.TruthWriteEverything=True
