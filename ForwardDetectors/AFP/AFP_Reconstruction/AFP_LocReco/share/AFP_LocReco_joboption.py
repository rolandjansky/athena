#==============================================================
# Job options file for the ALFA_LocRec package
#==============================================================
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.AlgSequence         import AlgSequence
TopLocRecSeq  = AlgSequence()

#-- SiD part ------------------------------------------------------------

from AFP_LocReco.AFP_LocRecoConf      import AFP_SIDLocReco
TopLocRecSeq += AFP_SIDLocReco("AFP_SIDLocReco")

# select between a real data or a simulation mode - Simulation = 0, RealData = 1
if globalflags.DataSource()=='data':
        AFP_SIDLocReco.DataType = 1
else:
        AFP_SIDLocReco.DataType = 0

# select algo for tracker reco
AFP_SIDLocReco.ListAlgoSID = ["SIDBasicKalman"]
AFP_SIDLocReco.AmpThresh = 5000.	

#-- TiD part ------------------------------------------------------------

from AFP_LocReco.AFP_LocRecoConf      import AFP_TDLocReco
TopLocRecSeq += AFP_TDLocReco("AFP_TDLocReco")

# select between a real data or a simulation mode - Simulation = 0, RealData = 1
if globalflags.DataSource()=='data':
        AFP_TDLocReco.DataType = 1
else:
        AFP_TDLocReco.DataType = 0

# select algo for timing reco and parameters
AFP_TDLocReco.ListAlgoTD = ["TDZVertex"]
AFP_TDLocReco.AmpThresh = 10.
