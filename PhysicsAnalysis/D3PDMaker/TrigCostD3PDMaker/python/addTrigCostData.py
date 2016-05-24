# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: addTrigCostD3PD.py tamartin
#
# Add a trig cost D3PD output stream

def addTrigCostData(mode = "COST", fileName = "trig_cost.root", costConfigL2 = False, costConfigEF = False, costConfigHLT = True):

    # Set up a logger:
    from AthenaCommon.Logging import logging
    addTrigCostData_msg = logging.getLogger( 'addTrigCostData' )

    # Construct the stream and file names for the Trigger D3PD:
    streamName = "StreamNTUP_TRIG" + mode
    addTrigCostData_msg.info( "Configuring Trigger Cost D3PD from bytestream with streamName '%s' and fileName '%s'" % ( streamName, fileName ) )

    # Create the D3PD stream(s):
    # We dont actually use the default stream! 
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    d3pdalg = MSMgr.NewRootStream( streamName, fileName, "dummy" )
      
    # As we are running from bytestream, need to disable data headers.
    from AthenaCommon.AlgSequence import AlgSequence
    theJob = AlgSequence()
    fullStreamName = "StreamNTUP_TRIG" + mode + "AANTStream"
    if hasattr(theJob, fullStreamName):
        streamRef = getattr( theJob, fullStreamName )
        streamRef.ExistDataHeader = False
        streamRef.WriteInputDataHeader = False

    # The tool needs a special D3PDSvc in which the indexing is turned off for the TTree-s:
    _costD3PDSvcName = "TrigCostD3PDSvc_" + mode
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr( ServiceMgr, _costD3PDSvcName ):
        from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
        ServiceMgr += D3PD__RootD3PDSvc( _costD3PDSvcName )
    else:
        addTrigCostData_msg.info( "The private D3PDSvc for the cost monitoring TTree already exists" )
    _costD3PDSvc = getattr( ServiceMgr, _costD3PDSvcName )
    _costD3PDSvc.MasterTree = ""
    _costD3PDSvc.IndexMajor = ""

    # Add the tool:
    _costD3PDToolName = "TrigCostD3PDMakerTool_" + mode
    _tuplePath = "/StreamNTUP_TRIG" + mode + "/trig_cost"
    if not _costD3PDToolName in [ t.name() for t in d3pdalg.MetadataTools ]:
        import TrigCostD3PDMaker
        _trigCostTool = TrigCostD3PDMaker.TrigCostD3PDMakerTool( _costD3PDToolName, D3PDSvc = _costD3PDSvc, dir = _tuplePath )
        _trigCostTool.mode = mode
        if (costConfigL2 == True):
            _trigCostTool.keyEvent = "HLT_TrigMonEventCollection_OPI_L2_monitoring_event"
            _trigCostTool.prefix = "TrigCostL2_"
        elif (costConfigEF == True):
            _trigCostTool.keyEvent = "HLT_TrigMonEventCollection_OPI_EF_monitoring_event"
            _trigCostTool.prefix = "TrigCostEF_"
        elif (costConfigHLT == True):
            _trigCostTool.keyEvent = "HLT_TrigMonEventCollection_OPI_HLT_monitoring_event"
            _trigCostTool.prefix = "TrigCostHLT_"
        d3pdalg.MetadataTools += [ _trigCostTool ]

    if (mode != "EBWEIGHT"):
      from TrigCostD3PDMaker.TrigCostConfMetadata import addTrigCostConfMetadata
      addTrigCostConfMetadata( d3pdalg, costConfigL2, costConfigEF, costConfigHLT, _tuplePath )
