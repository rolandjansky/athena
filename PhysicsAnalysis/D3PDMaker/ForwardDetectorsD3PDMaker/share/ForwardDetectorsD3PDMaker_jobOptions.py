# attention, if you change D3PD content here
# modify also python/ForwardDetectorsD3PD.py accordingly

# Output names
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.AthenaCommonFlags.PoolAODOutput = "%s.aod.pool.root" % outfile
jobproperties.AthenaCommonFlags.PoolESDOutput = "%s.esd.pool.root" % outfile
jobproperties.Rec.RootNtupleOutput            = "%s.cbnt.root" % outfile

include ("RecExCond/RecExCommon_flags.py")
include ("RecExCommon/RecExCommon_topOptions.py")

tuple_name = "%s.D3PD.root" % outfile
from ForwardDetectorsD3PDMaker.ForwardDetectorsD3PDMakerFlags import jobproperties
if jobproperties.ForwardDetectorsD3PDMakerFlags.FileName.statusOn:
  tuple_name = jobproperties.ForwardDetectorsD3PDMakerFlags.FileName()
  pass

# Create the D3PD stream:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.NewRootStream( "ForwardDetectorsD3PD", tuple_name )

##Event Info
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
alg += EventInfoD3PDObject( 10 )

##ZDC
##Energy and time information
from ForwardDetectorsD3PDMaker.ZdcD3PDObject import ZdcD3PDObject
from ForwardDetectorsD3PDMaker.ZdcDigitsD3PDObject import ZdcDigitsD3PDObject
if globalflags.DataSource == "data":
    alg += ZdcD3PDObject( 10 )
    alg += ZdcDigitsD3PDObject( 10 )
    pass
