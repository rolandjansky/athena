import os
import os.path

online = True 
ReadCOOL=True
Type='Pedestal'

#LarDBConnection="<dbConnection>impl=cool;techno=sqlite;schema=/det/lar/project/databases/COMCOND200.db;X:COMP200:</dbConnection>"

from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.SingleVersion=True
larCondFlags.OFCShapeFolder = ""
# When release will be > 15.0.1, replace the 3 previous lines with :
# include("RecExOnline/SimpleLarCondFlags.py")

include("LArMonTools/LArMonCommonHeader_jobOptions.py")
include("LArMonTools/LArMonManager.py") 
svcMgr.IOVDbSvc.GlobalTag="COMCOND-MONC-001-00"

# Define ONE LArBadChannelMask for all Monitoring tools
ProblemsToMask=[
    "deadReadout","deadPhys","short","almostDead",
    "highNoiseHG","highNoiseMG","highNoiseLG","sporadicBurstNoise"
    ]

#if job == 'FebMon' :
include("LArMonTools/LArFEBMon_jobOptions.py")           

#cmd='is_ls -p %(partition)s -n Monitoring -v -R "Monitoring" | tail -5 ' % {"partition" : os.environ['TDAQ_PARTITION'] }
cmd='is_ls -p %(partition)s -n Monitoring -v -R "Monitoring"' % {"partition" : os.environ['TDAQ_PARTITION'] }


fi, fo = os.popen2(cmd,'t')
for line in fo: 
   job = line.strip().split(' ',15)[0]
   print('line=%(line)s job=%(job)s' % {"line" : line, "job": job})
   if job == 'DigitNoiseMon' :
       include("LArMonTools/LArDigitNoiseMonTool_jobOptions.py")
   if job == 'FebNoiseMon' :
       include("LArMonTools/LArFebNoiseMonTool_jobOptions.py")
   if job == 'FebNoiseMonAlt' :
       include("LArMonTools/LArFebNoiseMonAltTool_jobOptions.py")
   if job == 'OddCellMon' :
       include("LArMonTools/LArOddCellsMonTool_jobOptions.py")
fo.close

try:
    print('Trying to remove PoolCat_comcond_castor.xml')
    svcMgr.PoolSvc.ReadCatalog.remove("prfile:poolcond/PoolCat_comcond_castor.xml")
except ValueError:
    print('PoolCat_comcond_castor.xml cannot be removed')

svcMgr.PoolSvc.ReadCatalog+=["xmlcatalog_file:/det/lar/project/databases/cond09_data.000001.lar.COND/PoolFileCatalog.xml"]
svcMgr.PoolSvc.ReadCatalog+=["xmlcatalog_file:/det/lar/project/databases/cond08_data.000001.lar.COND/PoolFileCatalog.xml"]
svcMgr.PoolSvc.ReadCatalog+=["xmlcatalog_file:/det/lar/project/databases/comcond.000006.lar_conditions.recon.pool.v0000/PoolFileCatalog.xml"]
svcMgr.PoolSvc.ReadCatalog+=["xmlcatalog_file:/det/lar/project/databases/comcond.000004.lar_conditions.recon.pool.v0000/PoolFileCatalog.xml"]
svcMgr.PoolSvc.ReadCatalog+=["xmlcatalog_file:/det/lar/project/databases/oflcond.000003.conditions.simul.pool.v0000/PoolFileCatalog.xml"]
svcMgr.PoolSvc.ReadCatalog.remove("xmlcatalog_file:/det/lar/project/databases/poolcond/PoolCat_oflcond.xml")

include("LArMonTools/LArMyJob.py")

print(svcMgr.PoolSvc.ReadCatalog)

#
include("LArMonTools/LArMonCommonTrailer_jobOptions.py")
