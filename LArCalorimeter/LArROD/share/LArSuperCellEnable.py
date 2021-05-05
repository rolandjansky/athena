
from LArL1Sim.LArSCL1Getter import *
theLArSCL1Getter = LArSCL1Getter()
digitizationFlags.rndmSeedList.addSeed("LArSCL1Maker", 335242, 7306589 )

theLArSCL1Maker=LArSCL1Maker()
theLArSCL1Maker.RndmSvc = digitizationFlags.rndmSvc.get_Value()

from LArROD.LArSCellGetter import LArSCellGetter
theLArSCellGetter = LArSCellGetter()

streamRDO.ItemList+=["LArDigitContainer#LArDigitSCL2"]
streamRDO.ItemList+=["CaloCellContainer#SCellnoBCID"]
streamRDO.ItemList+=["CaloCellContainer#SCell"]

topSequence.remove(streamRDO)
topSequence += streamRDO
