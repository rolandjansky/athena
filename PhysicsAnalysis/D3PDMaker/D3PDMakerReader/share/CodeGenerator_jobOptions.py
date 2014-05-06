# $Id: CodeGenerator_jobOptions.py 348546 2011-03-01 15:09:56Z krasznaa $

#
# Define an input file. To know which variables the D3PDObject-s would've
# created, we have to use an actual input file...
#
if not "InputFiles" in dir():
    InputFiles = [ "AOD.pool.root" ]

# We need to process exactly one event:
EvtMax = 1

# Set up the needed RecEx flags:
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock( InputFiles )
athenaCommonFlags.EvtMax.set_Value_and_Lock( EvtMax )

# Set up what the RecExCommon job should and shouldn't do:
from RecExConfig.RecFlags import rec
rec.AutoConfiguration = [ "everything" ]
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteESD.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doESD.set_Value_and_Lock( False )
rec.doAOD.set_Value_and_Lock( False )
rec.doDPD.set_Value_and_Lock( False )
rec.doHist.set_Value_and_Lock( False )
rec.doPerfMon.set_Value_and_Lock( False )
rec.doForwardDet.set_Value_and_Lock( False )
rec.doEdmMonitor.set_Value_and_Lock( False )

# Let RecExCommon set everything up:
include( "RecExCommon/RecExCommon_topOptions.py" )

#########################################################################
#                                                                       #
#                     Now set generate the sources                      #
#                                                                       #
#########################################################################

from AthenaCommon.AppMgr import ServiceMgr
import D3PDMakerReader
service = D3PDMakerReader.RootReaderD3PDSvc( "RootReaderD3PDSvc" )
service.Version = 2
service.OutputLevel = 1
ServiceMgr += service

d3pdalg = D3PDMakerReader.MultiReaderAlg( "ReaderAlg", Directory = "../test",
                                          TuplePath = "D3PDReader/Test",
                                          D3PDSvc = service )

from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
d3pdalg += EventInfoD3PDObject( 10 )

from TrigMuonD3PDMaker.TrigMuonD3PD import TrigMuonD3PDObjects
TrigMuonD3PDObjects( d3pdalg, addNaviInfo = False )

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.DoTrigger = False

from egammaD3PDMaker.ElectronD3PDObject import ElectronD3PDObject
d3pdalg += ElectronD3PDObject( 10 )

from egammaD3PDMaker.PhotonD3PDObject import PhotonD3PDObject
d3pdalg += PhotonD3PDObject( 10 )

# Do some of the additional setup:
from AthenaCommon.AppMgr import theApp, ServiceMgr
theApp.EvtMax = athenaCommonFlags.EvtMax()
ServiceMgr.MessageSvc.OutputLevel = INFO
