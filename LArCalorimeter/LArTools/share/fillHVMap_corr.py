#special script to fill HV corrected map
from LArCalibProcessing.TimeStampToRunLumi import TimeStampToRunLumi
from LArTools.LArHVMapDbFiller import LArHVMapDbFiller
from time import strptime
from calendar import timegm

#date="2010-03-03:14:01:00"
#ts=strptime(date+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
#TimeStamp=int(timegm(ts))*1000000000L
TimeStamp =1264091460*1000000000L

TimeStamp2=1272477600*1000000000L
from LArCalibProcessing.TimeStampToRunLumi import TimeStampToRunLumi


rlb=TimeStampToRunLumi(TimeStamp,2)
rlb2=TimeStampToRunLumi(TimeStamp2,2)

dbFiller =  LArHVMapDbFiller()
dbFiller.setFileName("HVLineToElectrode_v7old.data")
dbFiller.setBegin(rlb[0],rlb[1])
dbFiller.setEnd(rlb2[0],rlb2[1])
dbFiller.setFolderTag("LARIdentifierOflHVLineToElectrodeMap-UPD3-00")
dbFiller.genDb() 
