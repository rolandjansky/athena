# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# attention, if you change D3PD content here
# modify also share/ForwardDetectorsD3PDMaker_jobOptions.py accordingly

from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
from ForwardDetectorsD3PDMaker.ZdcD3PDObject import ZdcD3PDObject
from ForwardDetectorsD3PDMaker.ZdcDigitsD3PDObject import ZdcDigitsD3PDObject

from AthenaCommon.GlobalFlags import globalflags

print 'AO: Flags in ForwardDetectorsD3PD.py'
print 'globalflags'
print globalflags

def ForwardDetectorsD3PD( alg = None,
                          file = "ForwardDetectordD3PD.root",
                          tuplename = 'ForwardDetectorsD3PD',
                          **kw ):

  if not alg:
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    alg = MGMgr.NewRootStream( tuplename, file )
    pass

  ##Event Info
  alg += EventInfoD3PDObject( 10 )

  ##ZDC
  if globalflags.DataSource == "data":
    alg += ZdcD3PDObject( 10 )
    alg += ZdcDigitsD3PDObject( 10 )
    pass

  return alg
