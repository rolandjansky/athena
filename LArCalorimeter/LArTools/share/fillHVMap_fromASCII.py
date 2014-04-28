from LArCalibProcessing.TimeStampToRunLumi import TimeStampToRunLumi
from LArTools.LArHVMapDbFiller import LArHVMapDbFiller
from time import strptime
from calendar import timegm

date="2011-11-10:16:00:00"
ts=strptime(date+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
TimeStamp=int(timegm(ts))*1000000000L
#TimeStamp=1272477600*1000000000L
from LArCalibProcessing.TimeStampToRunLumi import TimeStampToRunLumi

rlb=TimeStampToRunLumi(TimeStamp,2)

dbFiller =  LArHVMapDbFiller()
dbFiller.setFileName("HVLineToElectrode_v14.data")
dbFiller.setBegin(rlb[0],0)
dbFiller.setFolderTag("LARIdentifierOflHVLineToElectrodeMap-UPD3-00")
dbFiller.genDb() 
