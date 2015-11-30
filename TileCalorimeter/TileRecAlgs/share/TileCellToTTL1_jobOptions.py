
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
    
from TileRecAlgs.TileRecAlgsConf import TileCellToTTL1
theTileCellToTTL1 = TileCellToTTL1()
theTileCellToTTL1.OutputLevel = INFO
topSequence += theTileCellToTTL1;

