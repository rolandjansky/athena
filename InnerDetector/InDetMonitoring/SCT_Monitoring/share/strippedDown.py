# job options for standalone running of NoiseMapBuilder

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()
ServiceMgr.MessageSvc.OutputLevel  = INFO

## Offline Conditions

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('DEFAULTCOND')

## GlobalFlags

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
#globalflags.DataSource = 'geant4'
globalflags.DataSource = 'data'
#globalflags.InputFormat = 'pool'
globalflags.InputFormat = 'bytestream'
globalflags.DetDescrVersion = 'ATLAS-CSC-02-00-00'
globalflags.print_JobProperties()

## DetFlags

from AthenaCommon.DetFlags import DetFlags 

DetFlags.all_setOff()
DetFlags.SCT_setOn()
DetFlags.Print()


## GeoModel

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

collection = []
if os.path.isfile("inputfilelist"):
  for line in open("inputfilelist"):
    collection.append(line.strip())
elif globalflags.InputFormat() == 'pool':
  collection = [ ] 
elif globalflags.InputFormat() == 'bytestream':
  collection = [ "/tmp/whitem/daq.detector.0039288.Physics.L1TT-det0.LB0000.SFO-1._0001.data" ] 
  #collection = [ "/afs/cern.ch/atlas/maxidisk/d158/idcalib_5802_fdr1_1.data" ] 
  #collection = ["/tmp/whitem/daq.m6_combined.0044316.physics.HLT_Cosmics_MU1.LB0000.SFO-1._0001.data","/tmp/whitem/daq.m6_combined.0044316.physics.HLT_Cosmics_MU1.LB0000.SFO-1._0002.data","/tmp/whitem/daq.m6_combined.0044316.physics.HLT_Cosmics_MU1.LB0000.SFO-1._0003.data","/tmp/whitem/daq.m6_combined.0044316.physics.HLT_Cosmics_MU1.LB0000.SFO-1._0004.data","/tmp/whitem/daq.m6_combined.0044316.physics.HLT_Cosmics_MU1.LB0000.SFO-1._0005.data","/tmp/whitem/daq.m6_combined.0044316.physics.HLT_Cosmics_MU1.LB0000.SFO-1._0006.data","/tmp/whitem/daq.m6_combined.0044316.physics.HLT_Cosmics_MU1.LB0000.SFO-1._0007.data","/tmp/whitem/daq.m6_combined.0044316.physics.HLT_Cosmics_MU1.LB0000.SFO-1._0008.data","/tmp/whitem/daq.m6_combined.0044316.physics.HLT_Cosmics_MU1.LB0000.SFO-1._0009.data"]
  #collection = [ "/tmp/whitem/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0001.data"]
  #collection=["castor:/castor/cern.ch/grid/atlas/datafiles/egamma/DC3.007040.singlepart_gamma_Et20/digit/120031/ideal0_mc12.007040.singlepart_gamma_Et20.digit.RDO.v12003108_tid005022._00001.pool.root"]
  #collection = [ "/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0001.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0002.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0003.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0004.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0005.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0006.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0007.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0008.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0009.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0010.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0011.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0012.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0013.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0014.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0015.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0016.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0017.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0018.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0019.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0020.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0021.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0022.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0023.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0024.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0025.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0026.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0027.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0028.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0029.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0030.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0031.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0032.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0033.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0034.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0035.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0036.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0037.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0038.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0039.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0040.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0041.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0042.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0043.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0044.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0045.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0046.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0047.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0048.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0049.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0050.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0051.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0052.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0053.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0054.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0055.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0056.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0057.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0058.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0059.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0060.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0061.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0062.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0063.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0064.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0065.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0066.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0067.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0068.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0069.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0070.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0071.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0072.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0073.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0074.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0075.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0076.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0077.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0078.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0079.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0080.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0081.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0082.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0083.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0084.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0085.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0086.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0087.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0088.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0089.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0090.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0091.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0092.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0093.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0094.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0095.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0096.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0097.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0098.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0099.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0100.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0101.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0102.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0103.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0104.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0105.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0106.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0107.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0108.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0109.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0110.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0111.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0112.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0113.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0114.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0115.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0116.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0117.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0118.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0119.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0120.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0121.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0122.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0123.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0124.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0125.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0126.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0127.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0128.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0129.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0130.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0131.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0132.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0133.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0134.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0135.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0136.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0137.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0138.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0139.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0140.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0141.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0142.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0143.data","/castor/cern.ch/grid/atlas/DAQ/M6/daq.m6_combined.0044094.physics.HLT_Cosmics_MU2.LB0000.SFO-1._0144.data"]
               

if globalflags.InputFormat() == 'pool':
  import AthenaPoolCnvSvc.ReadAthenaPool
  ServiceMgr.EventSelector.InputCollections = collection
elif globalflags.InputFormat() == 'bytestream':
  include("InDetRecExample/InDetReadBS_jobOptions.py")
  ServiceMgr.EventSelector.Input = collection
  
#--------------------------------------------------------------
# include SCT Clusterization
#--------------------------------------------------------------
from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
PixelConfigCondAlg.UseCalibConditions = False

from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
InDetClusterMakerTool = InDet__ClusterMakerTool(name = "InDetClusterMakerTool")
ToolSvc += InDetClusterMakerTool
print      InDetClusterMakerTool
#
# SCT_ClusteringTool
#
from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name          = "InDetSCT_ClusteringTool",
                                                       globalPosAlg  = InDetClusterMakerTool)
print      InDetSCT_ClusteringTool
#
# SCT_Clusterization algorithm
#
from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
InDetSCT_Clusterization = InDet__SCT_Clusterization(name                = "InDetSCT_Clusterization",
                                                          clusteringTool      = InDetSCT_ClusteringTool,
                                                          #ChannelStatus       = InDetSCT_ChannelStatusAlg,
                                                          DataObjectName      = "SCT_RDOs",
                                                          ClustersName        = "SCT_Clusters")
topSequence += InDetSCT_Clusterization
print          InDetSCT_Clusterization

from GaudiSvc.GaudiSvcConf import THistSvc

if not hasattr(ServiceMgr, 'THistSvc'):
    ServiceMgr += THistSvc()

THistSvc = ServiceMgr.THistSvc
MonitorOutput="GLOBAL"
RootHistOutputFilename="TimeDependentHists.root"
if os.path.exists(RootHistOutputFilename):
        os.remove(RootHistOutputFilename)
THistSvc.Output += [MonitorOutput+" DATAFILE='"+RootHistOutputFilename+"'OPT='NEW'"]

#include("SCT_Monitoring/SCTMonitor_topOptions.py")

theApp.EvtMax = -1

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager("SCTMonManager")

SCTMonMan = topSequence.SCTMonManager
SCTMonMan.FileKey = "GLOBAL"
SCTMonMan.ManualDataTypeSetup = True
SCTMonMan.DataType= "monteCarlo"
SCTMonMan.Environment         = "user"
SCTMonMan.ManualRunLBSetup    = True
SCTMonMan.Run                 = 1
SCTMonMan.LumiBlock           = 1

from SCT_Monitoring.SCT_MonitoringConf import SCTTimeDependentMonTool
SCTTimeDependentMonTool = SCTTimeDependentMonTool("SCTTimeDependentMonTool")
#ToolSvc+=SCTTimeDependentMonTool
#SCTMonMan.AthenaMonTools += [SCTTimeDependentMonTool]

CBNTAthenaAware=True
include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )

from SCT_Monitoring.SCT_MonitoringConf import SCT_Monitoring__SCTTimeDependentCBNT
CBNT_AthenaAware += SCT_Monitoring__SCTTimeDependentCBNT()


if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["AANT DATAFILE='SCT_CBNT.root' OPT='RECREATE'"]
theApp.Dlls   += [ "AnalysisTools" ]
theApp.TopAlg += [ "AANTupleStream" ]
AANTupleStream = Algorithm( "AANTupleStream" )
#AANTupleStream.ExtraRefNames = [ "StreamESD","StreamRDO" ]
AANTupleStream.OutputName = 'SCT_CBNT.root'
AANTupleStream.ExistDataHeader=False



