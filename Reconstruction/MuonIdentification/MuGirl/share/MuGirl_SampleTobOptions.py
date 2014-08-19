AllAlgs = False
doiPatRec = True
EvtMax = 10
readMuonDigit = False
doMuonIDCombined = False
makeESD = True
doWriteESD = False
doWriteAOD = False
doWriteTAG = False
doTruth = True
#doDumpTES = True
#doMdtHough = False
doMuGirlNTuple = True
MuGirlNTupleFile = "ntuple_006S.root"

DetDescrVersion = "Rome-Initial"

#50GeV muons
#PoolRDOInput = ["rfio:/castor/cern.ch/atlas/project/muon/validation/10.5.0/digit/valid.009999.digit.muminus_pt50_eta320._00001.pool.root"]

#10GeV muons
#PoolRDOInput = ["rfio:/castor/cern.ch/atlas/project/muon/validation/10.5.0/digit/valid.009999.digit.muminus_pt10_eta320._00001.pool.root"]

#10GeV anti muons
#PoolRDOInput = ["rfio:/castor/cern.ch/atlas/project/muon/validation/10.5.0/digit/valid.009999.digit.muplus_pt10_eta320._00001.pool.root"]

#6GeV muons
#PoolRDOInput = ["rfio:/castor/cern.ch/atlas/project/muon/validation/10.5.0/digit/valid.009999.digit.muminus_pt6_eta320._00001.pool.root"]

#6GeV anti muons
#PoolRDOInput = ["rfio:/castor/cern.ch/atlas/project/muon/validation/10.5.0/digit/valid.009999.digit.muplus_pt6_eta320._00001.pool.root"]

#PoolRDOInput = ["rfio:/castor/cern.ch/atlas/project/muon/validation/10.5.0/digit/valid.009999.digit.muminus_pt50_eta320._00001.pool.root"]
#PoolRDOInput = ["/home/shlomit/rome.004045.digit_1001.muminus_pt100_eta320._00012.pool.root"]
PoolRDOInput = ["/home/shlomit/rome.004908.digit_1001.muminus_pt6_eta30._00003.pool.root"]
#PoolRDOInput = ["/home/shlomit/rome.004912.digit_1001.muplus_pt10_eta30._00001.pool.root"]
#PoolRDOInput = ["/home/shlomit/rome.004919.digit_1001.muplus_pt20_eta320._00001.pool.root"]

#DetDescrVersion = "ATLAS-DC3-02"
#PoolRDOInput = ["/home/shlomit/mc11.004934.mu_pt100.digit.RDO.v11000401._00070.pool.root"]

include("RecExCommon/RecExCommon_flags.py")
DetFlags.Calo_setOff()

include("RecExCommon/RecExCommon_topOptions.py")

include("MuGirl/MuGirl_jobOptions.py")
MuGirl.OutputLevel = DEBUG

if doMuGirlNTuple:
    if not 'MuGirlNTupleFile' in dir():
        MuGirlNTupleFile = "ntuple_mugirl.root"
    NTupleSvc.Output = [ "FILE1  DATAFILE='ntuple.root' TYP='ROOT' OPT='NEW'",
                         "MUGIRL DATAFILE='"+MuGirlNTupleFile+"' TYP='ROOT' OPT='NEW'" ]

if doMdtHough:
    HoughMDTTool.OutputLevel = DEBUG

ToolSvc.TGC_LinearSegmentMakerTool.OutputLevel = DEBUG

include("MuGirlParticleCreator/MuGirlParticleCreator_jobOptions.py")
MuGirlToCombinedMuon.OutputLevel = DEBUG
 
StoreGate.ActivateHistory = False
