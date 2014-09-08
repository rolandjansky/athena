
from IOVDbSvc.CondDB import conddb
if not conddb.folderRequested('/LAR/IdentifierOfl/OnOffIdMap_SC') :
  conddb.addFolder("LAR_OFL","<tag>LARIdentifierOflOnOffIdMap_SC-000</tag>/LAR/IdentifierOfl/OnOffIdMap_SC")

from LArL1Sim.LArSCL1Getter import *
theLArSCL1Getter = LArSCL1Getter()
digitizationFlags.rndmSeedList.addSeed("LArSCL1Maker", 335242, 7306589 )

theLArSCL1Maker=LArSCL1Maker()
theLArSCL1Maker.RndmSvc = digitizationFlags.rndmSvc.get_Value()

from LArROD.LArSCellGetter import LArSCellGetter
theLArSCellGetter = LArSCellGetter()

streamRDO.ItemList+=["LArDigitContainer#LArDigitSCL1"]
streamRDO.ItemList+=["CaloCellContainer#SCell"]
topSequence.remove(streamRDO)
topSequence += streamRDO
