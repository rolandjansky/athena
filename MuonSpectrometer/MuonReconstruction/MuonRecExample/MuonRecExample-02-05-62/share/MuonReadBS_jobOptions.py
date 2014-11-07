include.block("MuonRecExample/MuonReadBS_jobOptions.py")
from MuonRecExample.MuonRecUtils import logMuon
logMuon.warning("include(\"MuonRecExample/MuonReadBS_jobOptions.py\") is deprecated. "
                "Please use instead: include(\"MuonCnvExample/MuonReadBS_jobOptions.py\")")

# forward include to MuonCvnExample
include("MuonCnvExample/MuonReadBS_jobOptions.py")
