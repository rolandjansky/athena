## @file MuonCnvExample/share/MuonReadBS_jobOptions.py
#
# @brief jobOptions for data conversion from bytestream to RDO for offline (i.e. decode full event)
#
# Used when reading raw date file (bytestream format).
# Adds the algorithms that convert the full muon bytestream data in RDO collections.
# The actual conversion tools and services used by the algorithms are configured in python/MuonReadBSConfig.py
# and python/MuonRecTools.py.
# There is one algorithm per detector technology and a specific algorithm is added or not depending on the flags<br>
# <code>DetFlags.readRODBS.MDT_on(), RPC_on(), TGC_on() and CSC_on()</code><br>

include.block("MuonCnvExample/MuonReadBS_jobOptions.py")

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.CfgGetter import getAlgorithm
from MuonRecExample.MuonRecFlags import muonRecFlags

# ================= MDT ===========================
if DetFlags.readRDOBS.MDT_on() and muonRecFlags.makePRDs():
    topSequence += getAlgorithm("MuonMdtRawDataProvider")
    
# ================= RPC ===========================
if DetFlags.readRDOBS.RPC_on() and muonRecFlags.makePRDs():
    topSequence += getAlgorithm("MuonRpcRawDataProvider")

# ================= TGC ===========================
if DetFlags.readRDOBS.TGC_on() and muonRecFlags.makePRDs():
    topSequence += getAlgorithm("MuonTgcRawDataProvider")

# ================= CSC ===========================
if DetFlags.readRDOBS.CSC_on() and muonRecFlags.makePRDs():
    topSequence += getAlgorithm("MuonCscRawDataProvider")
