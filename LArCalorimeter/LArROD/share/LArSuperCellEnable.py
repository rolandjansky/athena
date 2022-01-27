
from LArL1Sim.LArSCL1Getter import *
theLArSCL1Getter = LArSCL1Getter()

theLArSCL1Maker=LArSCL1Maker()

from LArROD.LArSCellGetter import LArSCellGetter
theLArSCellGetter = LArSCellGetter()

streamRDO.ItemList+=["LArDigitContainer#LArDigitSCL2"]
streamRDO.ItemList+=["CaloCellContainer#SCellnoBCID"]
streamRDO.ItemList+=["CaloCellContainer#SCell"]

topSequence.remove(streamRDO)
topSequence += streamRDO
