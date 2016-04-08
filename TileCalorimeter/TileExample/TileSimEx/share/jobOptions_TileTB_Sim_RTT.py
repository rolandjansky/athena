
include("TileSimEx/jobOptions_TileTB_Sim.py")

svcMgr.StatusCodeSvc.AbortOnError = False

if not 'doD3PD' in dir():
    doD3PD = True

if not 'doHitNtuple' in dir():
    doHitNtuple = True

theApp.HistogramPersistency = "ROOT"

if doD3PD:
    
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    alg = MSMgr.NewRootStream( "AANT", "tiletb%s.d3pd.root" % FileSuffix, "truth" )     
    topSeq.AANTAANTStream.WriteInputDataHeader = False
    topSeq.AANTAANTStream.ExistDataHeader = False

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    D3PDMakerFlags.TruthSGKey = 'TruthEvent'

    from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys
    from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags

    from TruthD3PDAnalysis.GenObjectsFilterTool import *

    from TruthD3PDMaker.GenVertexD3PDObject import GenVertexD3PDObject
    alg += GenVertexD3PDObject( 10, filter = AllTrackFilterTool() )

    from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
    alg += GenParticleD3PDObject( 10, filter = AllTrackFilterTool() )

    from CaloD3PDMaker.TileHitD3PDObject import TileHitD3PDObject
    alg += TileHitD3PDObject( 10 )

if doHitNtuple:

    if not hasattr(svcMgr, 'NtupleSvc'):
        from GaudiSvc.GaudiSvcConf import NTupleSvc
        svcMgr += NTupleSvc()
    NTupleSvc = svcMgr.NTupleSvc
    NTupleSvc.Output = [ "NTUP DATAFILE='tiletb%s.ntup.root' OPT='NEW'" % FileSuffix ]   

    #--- Create TileCal h32 ntuple with all G4 hits ---------
    from TileRec.TileRecConf import TileHitVecToNtuple
    theTileHitToNtuple = TileHitVecToNtuple()
    topSeq += theTileHitToNtuple
    theTileHitToNtuple.MaxLength=99999
    theTileHitToNtuple.TileHitVector = 'TileHitVec'
    theTileHitToNtuple.NTupleLoc = '/NTUP'
