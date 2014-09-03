
#Job options needed to enable LAr alignment

include("DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

from IOVDbSvc.CondDB import conddb

if  conddb.isMC:
  #Monte Carlo case:
  conddb.addFolder("LAR_OFL","/LAR/Align")
  conddb.addFolder("LAR_OFL","/LAR/LArCellPositionShift")
else:
  #Regular offline data processing
  conddb.addFolder("LAR_ONL","/LAR/Align")
  conddb.addFolder("LAR_ONL","/LAR/LArCellPositionShift")
     
print "including share/LArAlignable"
