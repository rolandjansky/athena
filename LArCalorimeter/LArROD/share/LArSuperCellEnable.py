

if ( "streamRDO" in dir() ):
  print "Enabling RDO production of SuperCells"

  include("LArROD/LArConfigureCablingSCFolder.py")

  from LArL1Sim.LArSCL1Getter import *
  theLArSCL1Getter = LArSCL1Getter()
  digitizationFlags.rndmSeedList.addSeed("LArSCL1Maker", 335242, 7306589 )

  theLArSCL1Maker=LArSCL1Maker()
  theLArSCL1Maker.RndmSvc = digitizationFlags.rndmSvc.get_Value()

  from LArROD.LArSCellGetter import LArSCellGetter
  theLArSCellGetter = LArSCellGetter()

  streamRDO.ItemList+=["LArDigitContainer#LArDigitSCL1"]
  streamRDO.ItemList+=["CaloCellContainer#SCell"]
  streamRDO.ItemList+=["CaloCellContainer#SCellnoBCID"]
  topSequence.remove(streamRDO)
  topSequence += streamRDO

if ( "StreamESD" in dir() ):
  print "Copying SuperCells from RDO to ESD"
  StreamESD.ItemList+=["LArDigitContainer#LArDigitSCL1"]; # This might need to be controlled later to avoid increase of ESD size
  StreamESD.ItemList+=["CaloCellContainer#SCell"]
  StreamESD.ItemList+=["CaloCellContainer#SCellnoBCID"]
if ( "StreamAOD" in dir() ):
  print "Copying SuperCells from ESD to AOD"
  StreamAOD.ItemList+=["LArDigitContainer#LArDigitSCL1"]; # This might need to be controlled later to avoid increase of AOD size
  StreamAOD.ItemList+=["CaloCellContainer#SCell"]         # This might need to be controlled later to avoid increase of AOD size
  StreamAOD.ItemList+=["CaloCellContainer#SCellnoBCID"]         # This might need to be controlled later to avoid increase of AOD size
