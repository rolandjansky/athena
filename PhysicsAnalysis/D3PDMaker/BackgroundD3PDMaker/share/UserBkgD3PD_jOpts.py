
##set input content as ESD or AOD
from BackgroundD3PDMaker.BackgroundD3PDMakerFlags import BackgroundD3PDMakerFlagsSetESD
BackgroundD3PDMakerFlagsSetESD()
#from BackgroundD3PDMaker.BackgroundD3PDMakerFlags import BackgroundD3PDMakerFlagsSetAOD
#BackgroundD3PDMakerFlagsSetAOD()

# pp or HI
if rec.doHeavyIon:
    from BackgroundD3PDMaker.BackgroundD3PDMakerFlags import BackgroundD3PDMakerFlagsSetHI
    BackgroundD3PDMakerFlagsSetHI()
else:
    from BackgroundD3PDMaker.BackgroundD3PDMakerFlags import BackgroundD3PDMakerFlagsSetpp
    BackgroundD3PDMakerFlagsSetpp()


import glob
InputFiles = glob.glob( "myESD.pool.root" )

OutputFile = "custom.backgroundD3PD.root"

EvtMax = 100


## Set up the needed RecEx flags:
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock( InputFiles )
athenaCommonFlags.EvtMax.set_Value_and_Lock( EvtMax )

## Set up what the RecExCommon job should and shouldn't do:
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


## Let RecExCommon set everything up:
include( "RecExCommon/RecExCommon_topOptions.py" )

#########################################################################
#                                                                       #
#                     Now set up the D3PDMaker job                      #
#                                                                       #
#########################################################################

## Create a logger:
from AthenaCommon.Logging import logging
logger = logging.getLogger( "BackgroundD3PD" )

## Configure the D3PD parameters:
rec.doTruth.set_Value_and_Lock( False )
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.DoTrigger.set_Value_and_Lock( False ) # add trigger variables by hand...


##Uncomment if you want the unassociated hits
#from MinBiasD3PDMaker.MinBiasD3PDMakerConf import MinBiasD3PD__MinBiasD3PDPreProcessing
#MinBiasD3PDPreProcessingAlg = MinBiasD3PD__MinBiasD3PDPreProcessing( name = "MinBiasD3PDPreProcessingAlg", OutputLevel = WARNING )
#topSequence += MinBiasD3PDPreProcessingAlg

## Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

## Add BeamBackgroundFiller to the algorithm sequence.
from RecBackgroundAlgs.RecBackgroundAlgsConf import BeamBackgroundFiller
BeamBackgroundFiller1=BeamBackgroundFiller()
theJob+=BeamBackgroundFiller1

##Below is an example of running with two BeamBackgroundFiller instances,
##one for AntiKt4TopoEMJets and one for AntiKt6TopoEMJets.
BeamBackgroundFiller2=BeamBackgroundFiller('BeamBackgroundFiller2',jetContainerKey="AntiKt6TopoEMJets",BeamBackgroundKey="customBeamBackgroundData")
theJob+=BeamBackgroundFiller2


## Create the D3PD maker algorithm:
import D3PDMakerCoreComps
d3pdalg = D3PDMakerCoreComps.MakerAlg( "background", theJob, file = OutputFile,
                                       D3PDSvc = "D3PD::RootD3PDSvc" )


##
## Add variables
##

##Uncomment those variables you want in your D3PD

## Default is the usual Background D3PD
##
from BackgroundD3PDMaker.backgroundD3PD import backgroundD3PD
backgroundD3PD(d3pdalg)

## Pixel Clusters
##
#from TrackD3PDMaker.PixelClusterD3PDObject import PixelClusterD3PDObject
#d3pdalg += PixelClusterD3PDObject( 0 )

## Unassociated Hits
##
#from MinBiasD3PDMaker.UnassociatedHitsD3PDObject import UnassociatedHitsD3PDObject
#d3pdalg += UnassociatedHitsD3PDObject( 10 )

## Calo Info
##
##cells
#from CaloD3PDMaker.CellD3PDObject import CellD3PDObject
#d3pdalg += CellD3PDObject( 0 )
##clusters
#from CaloD3PDMaker.ClusterD3PDObject import ClusterD3PDObject
#d3pdalg += ClusterD3PDObject( 6 )


## Jets
##
from JetD3PDMaker.JetD3PDObject import JetD3PDObject
d3pdalg += JetD3PDObject (**_args(4, 'AntiKt4TopoEMJets', kw,
                                  sgkey='AntiKt4TopoEMJets',
                                  prefix='AntiKt4TopoEM_',
                                  include='BeamBackground',
                                  BeamBackground_BeamBackgroundKey='BeamBackgroundData' ))
d3pdalg += JetD3PDObject (**_args(4, 'AntiKt6TopoEMJets', kw,
                                  sgkey='AntiKt6TopoEMJets',
                                  prefix='AntiKt6TopoEM_',
                                  include='BeamBackground',
                                  BeamBackground_BeamBackgroundKey='customBeamBackgroundData' ))

## Beam Background Variables
##
from BackgroundD3PDMaker.BeamBackgroundD3PDObject import BeamBackgroundD3PDObject
d3pdalg += BeamBackgroundD3PDObject (**_args(2, 'BeamBackgroundData', kw,
                                             sgkey='BeamBackgroundData',
                                             prefix='bb_',
                                             bbJetIndex_Target='AntiKt4TopoEM_',
                                             bbClusterIndex_Target='cl_'))
d3pdalg += BeamBackgroundD3PDObject (**_args(2, 'BeamBackgroundData2', kw,
                                             sgkey='customBeamBackgroundData',
                                             prefix='bb2_',
                                             bbJetIndex_Target='AntiKt6TopoEM_',
                                             bbClusterIndex_Target='cl_'))


##
## End add variables
##

logger.info( "Configured the Background D3PDMaker algorithm" )

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = athenaCommonFlags.EvtMax()
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000000
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
if not hasattr( ServiceMgr, "AthenaEventLoopMgr" ):
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    ServiceMgr += AthenaEventLoopMgr()
ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100
