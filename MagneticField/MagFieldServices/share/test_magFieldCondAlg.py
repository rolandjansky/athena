# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Job options to test magnetic field conditions algs with varying currents.
#

# Testing IOVs and currents: (since LB, solenoid, toroids)
# Default test - should read both mag field files, and turn off fields for events 5 to 9, and back on for 10 to 14
currents = [(0, 7730, 20400),
            (5, 0, 0),
            (10, 7730, 20400)]

# Optional test: only toroid is on for whole run. Scale factor for solenoid will become 1 at event 5,
# but solenoid field will still be off
# currents = [(0, 0, 20400),
#             (5, 7730, 20400),
#             (10, 7730, 20400)]

# Optional test: only solenoid is on for whole run. Scale factor for toroid will become 1 at event 5,
# but toroid field will still be off
# currents = [(0, 7730, 0),
#             (5, 7730, 20400),
#             (10, 7730, 20400)]

# Folder name
folder = '/EXT/DCS/MAGNETS/SENSORDATA'

def createDB():
   """Create sqlite file with DCS currents"""
   import os
   os.environ['CLING_STANDARD_PCH'] = 'none' #See bug ROOT-10789
   from PyCool import cool
   from CoolConvUtilities import AtlCoolLib, AtlCoolTool

   # Cleanup previous file
   import os
   if os.path.isfile("magfield.db"):
      os.remove("magfield.db")

   db = cool.DatabaseSvcFactory.databaseService().createDatabase('sqlite://;schema=magfield.db;dbname=CONDBR2')
   spec = cool.RecordSpecification()
   spec.extend("value", cool.StorageType.Float)
   spec.extend("quality_invalid", cool.StorageType.Bool)
   f = AtlCoolLib.ensureFolder(db, folder, spec, AtlCoolLib.athenaDesc(True, 'CondAttrListCollection'))

   for v in currents:
      sol = cool.Record(spec)
      sol['value'] = v[1]
      sol['quality_invalid'] = False
      tor = cool.Record(spec)
      tor['value'] = v[2]
      tor['quality_invalid'] = False
      f.storeObject(v[0], cool.ValidityKeyMax, sol, 1)  # channel 1
      f.storeObject(v[0], cool.ValidityKeyMax, tor, 3)  # channel 3

   # print database content
   act = AtlCoolTool.AtlCoolTool(db)
   print (act.more(folder))


# Create sqlite file with DCS currents
createDB()

# basic job configuration
import AthenaCommon.AtlasUnixGeneratorJob  # noqa: F401
from AthenaCommon.AppMgr import theApp, ServiceMgr as svcMgr
from AthenaCommon.AlgSequence import AthSequencer, AlgSequence

# Increment LBN every event
from McEventSelector import McEventSelectorConf
svcMgr += McEventSelectorConf.McEventSelector('EventSelector', EventsPerLB=1)

# Conditions folder setup
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource='data'
globalflags.ConditionsTag='CONDBR2-BLKPA-RUN2-01'

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(globalflags.ConditionsTag())
conddb.addFolder('GLOBAL', '/GLOBAL/BField/Maps <noover/>', className="CondAttrListCollection")
conddb.addFolder('magfield.db', folder, className='CondAttrListCollection')

# Magnetic field algorithm setup
from AthenaCommon.CfgGetter import getAlgorithm
condSeq = AthSequencer("AthCondSeq")
condSeq += getAlgorithm( "AtlasFieldMapCondAlg" )
condSeq += getAlgorithm( "AtlasFieldCacheCondAlg" )
condSeq.AtlasFieldCacheCondAlg.LockMapCurrents = False

from MagFieldUtils.MagFieldUtilsConf import MagField__CondReader
topSequence = AlgSequence()
topSequence += MagField__CondReader("MagFieldCondReader")

theApp.EvtMax = currents[-1][0]+5   # 5 events per IOV
