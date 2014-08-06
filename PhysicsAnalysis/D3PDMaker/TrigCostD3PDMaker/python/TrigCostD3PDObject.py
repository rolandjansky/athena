# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigCostD3PDObject.py 2013-03-04 tamartin $
#
# D3PD Object saving information from the TrigEvent/TrigMonEvent object
# accessed during trigger reprocessing. Saves all details of ROS access
# and chain execuition times for use in rate plotting, rate prediction
# and cost monitoring
#

# This file should be included as in TrigCostD3PDMaker_prodJobOFragment.py

from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
import TrigCostD3PDMaker

# We allow missing for when running on AthenaMT / PT where we only have one of the two collections.
TrigCostD3PDObject = make_SG_D3PDObject( "TrigMonEventCollection", #Data type
                                         "HLT_OPI_L2_monitoring_event", #Default SG key
                                         "TrigMonL2_", #Default prefix
                                         "TrigCostD3PDObject", #Objact name
                                         True ) #AllowMissing

# TrigMonEvent data is stored in a DataVector. The D3PD maker needs to be told which entry to save.
# By default this will be the first entry (0)
# FailGracefully determins the behaviour of the algorithm if there is no data present at the specified
# entry in the vector.

TrigCostD3PDObject.defineBlock( 2, "event", TrigCostD3PDMaker.TrigCostBaseFillerTool, DataVectorEntryToSave=0, FailGracefully=False )

TrigCostD3PDObject.defineBlock( 0, "chain", TrigCostD3PDMaker.TrigCostChainFillerTool, DataVectorEntryToSave=0, FailGracefully=False )

TrigCostD3PDObject.defineBlock( 0, "l1", TrigCostD3PDMaker.TrigCostL1FillerTool, DataVectorEntryToSave=0, FailGracefully=False )

TrigCostD3PDObject.defineBlock( 1, "rob", TrigCostD3PDMaker.TrigCostROBFillerTool, DataVectorEntryToSave=0, FailGracefully=False )

TrigCostD3PDObject.defineBlock( 1, "roi", TrigCostD3PDMaker.TrigCostRoIFillerTool, DataVectorEntryToSave=0, FailGracefully=False )

TrigCostD3PDObject.defineBlock( 1, "sequence", TrigCostD3PDMaker.TrigCostSequenceFillerTool, DataVectorEntryToSave=0, FailGracefully=False )

TrigCostD3PDObject.defineBlock( 1, "trigElement", TrigCostD3PDMaker.TrigCostTrigElementFillerTool, DataVectorEntryToSave=0, FailGracefully=False )
