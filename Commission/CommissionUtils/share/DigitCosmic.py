# DetDescrVersion="Rome-Final"
DetDescrVersion="ATLAS-DC3-02"

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()

# doCaloNoise = True 
doCaloNoise = False 

# disable default Tile Opt Filter
doTileHitToDigit=False
doTileDigitToRawChannel=False
include ("Digitization/AtlasDigitization.py") 

# and use testbeam-like Fit Filter method for Tile
doTileHitToDigit=True
doTileDigitToRawChannel=True
include ("TileSimAlgs/TileDigitizationCosmics_jobOptions.py") 

if not 'inputfile' in dir() :
 inputfile=  "cosmic.simul.pool.root"

if not 'outputfile' in dir() :
 outputfile ="cosmic.digit.pool.root" 

EventSelector.InputCollections=[inputfile]
Stream1.OutputFile = outputfile 

theApp.Dlls += ["GeneratorObjectsAthenaPoolPoolCnv"] 
theApp.Dlls += ["G4SimAthenaPOOLPoolCnv"] 
theApp.Dlls += ["CommissionAthenaPoolPoolCnv"] 
include ("CommissionEvent/CommissionEventDict_joboptions.py")

digitmaker1.SkipNoHit=True
# digitmaker1.PileUp=True
digitmaker1.UseTriggerTime  = True; 
digitmaker1.TriggerTimeToolName  = "CosmicTriggerTimeTool" 

# TileHitVecToCnt.PileUp=True
TileHitVecToCnt.UseTriggerTime  = True; 
TileHitVecToCnt.TriggerTimeToolName  = "CosmicTriggerTimeTool" 

# insert ComTimeRec at the beginning of the TopAlgs.
theApp.Dlls += ["CommissionRec"] 
topAlgs = theApp.TopAlg ; 

theApp.TopAlg = ["ComTimeRec"] 
theApp.TopAlg += topAlgs

ComTimeRec = Algorithm("ComTimeRec") 


Stream1.AcceptAlgs = ["digitmaker1"] 
# Stream1.AcceptAlgs = ["TileHitVecToCnt"]
Stream1.ItemList += ["McEventCollection#*"] 
Stream1.ItemList += ["LArHitContainer#*"] 
Stream1.ItemList += ["TileHitVector#*"]
Stream1.ItemList += ["TileHitContainer#*"]
Stream1.ItemList += ["TileTTL1Container#*"]
# Stream1.ItemList += ["TimedTrackRecordCollection#*"]
Stream1.ItemList += ["ComTime#*"]
# Stream1.ItemList += ["LArDigitContainer#*"] 
# Stream1.ItemList += ["TileDigitsContainer#*"] 

theApp.EvtMax = 2000
IOVDbSvc.ManageConnections = TRUE

MessageSvc.infoLimit = 1000000 
theApp.Dlls += ["CommissionUtils"]
PileUpMergeSvc = Service("PileUpMergeSvc") 
PileUpMergeSvc.TriggerTimeTool = "CosmicTriggerTimeTool" 

# MessageSvc.OutputLevel = DEBUG 
# ToolSvc.CosmicTriggerTimeTool.OutputLevel = DEBUG
# EventSelector.OutputLevel  = DEBUG 

GeoModelSvc.IgnoreTagDifference = True
DetDescrCnvSvc.MuonIDFileName = "IdDictMuonSpectrometer_R.01.xml"
