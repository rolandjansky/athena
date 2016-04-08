EvtMax=10
DetDescrVersion="ATLAS-DC3-01" 
PoolRDOInput=["rfio:/castor/cern.ch/atlas/csc/valiprod/sampleA/mc11/004100.T1_McAtNLO_top/digit/v11000201/mc11.004100.T1_McAtNLO_top.digit.RDO.v11000201._00029.pool.root"]
PoolESDOutput = "ESD.pool.root" # change file name and path appropriately
PoolAODOutput = "AOD.pool.root" # change file name and path appropriately
CollOutput = "TAG.pool.root"    # change the TAG output file name and path
doCBNT = False
doHist = False

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
# include ("RecExCommon/RecExCommon_flags.py")
# switch off ID, calo, or muons
# DetFlags.ID_setOff()
# DetFlags.Calo_setOff()
# DetFlags.Muon_setOff()

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
