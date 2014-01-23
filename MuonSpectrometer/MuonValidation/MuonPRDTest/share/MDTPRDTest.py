from MuonPRDTest.MuonPRDTestConf import MDTPRDValAlg

myAlg = MDTPRDValAlg( "MDTPRDValAlg" )
#myAlg.OutputLevel = VERBOSE 
topSequence += myAlg

THistSvc = Service( "THistSvc")
ServiceMgr += THistSvc

RootTupleOutputFileName = 'MDTPRDTest.root'

#--------------------------------------------------------------  
# ntuples: AANT  (Athena Aware NTuples)
#-------------------------------------------------------------- 
if os.path.exists(RootTupleOutputFileName):
    os.remove(RootTupleOutputFileName)

THistSvc.Output += ["MuonHits DATAFILE='%s' OPT='NEW'" % RootTupleOutputFileName]

