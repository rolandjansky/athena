#______________________________________________________________________________________________________________________
# author: liza.mijovic@_nospam_cern.ch 
#
# jO effect: dump the values of all parameters PYTUNE can set - this jopts are meant for runnig with GET_TUNE_params.sh
#
# ______________________________________________________________________________________________________________________

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

from Pythia_i.Pythia_iConf import Pythia
topAlg += Pythia()
Pythia = topAlg.Pythia

theApp.EvtMax = 0

Pythia.Tune_Name="ATLAS_-1"
Pythia.Direct_call_to_pytune=REPIND
# if one want the call to be equivalen to Pythia.Tune_Name="PYTUNE_XXX"
# the ATLAS stable particles convention should also be added
Pythia.PygiveCommand += [ "mstj(22)=2" ]

PYDAT1_PARAMS=[ "MSTU", "PARU", "MSTJ", "PARJ" ]
PYPARS_PARAMS=[ "MSTP", "PARP", "MSTI", "PARI" ]
PYTUNE_PARAMS=PYDAT1_PARAMS+PYPARS_PARAMS

PQ_LIST=[]
for i in PYTUNE_PARAMS:
    PQ_LIST+=[i+"("+repr(x)+")=" for x in range(1,201)]

Pythia.PygiveCommand += PQ_LIST
Pythia.Param_Query_AfterInit += PQ_LIST
