"""
@package   ZdcD3PDMaker_jobOption.py
@author    Marco Leite <leite@cern.ch>

         August 2010


        This is an example of a minimum job option file for creating a D3PD with ZDC information.
        Included here are the event info only. For other systems inclusion see the D3PD maker tree
        on ATLAS offline svn (PhysicsAnalysis/D3PDMaker).

        The ZdcD3PDMaker is part of the ForwardDetectorsD3PDMaker


        This started in HeavyIonD3PDMaker  in 15.6.9 Athena Release, now it's part of
        ForwardDetectorsD3PDMaker (Rel 17.0.0)
"""


doMC = False
DoReconstruction = False
DoTruth = False

from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *

import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc

ServiceMgr.MessageSvc.defaultLimit  = 9999999
outFileName = 'ZdcD3PD.root'

if doMC:
   DoTruth = True

###################################################################################################
# Load Global Flags and set defaults (import the new jobProperty globalflags)
###################################################################################################

DetDescrVersion = 'ATLAS-GEO-16-00-01'

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
globalflags.DetGeo = 'atlas'
globalflags.InputFormat = 'pool'
globalflags.DetDescrVersion = DetDescrVersion
globalflags.DataSource  = 'data' # 'geant4'#
#if doMC:
#   globalflags.DataSource  = 'geant4'
#jobproperties.Beam.beamType = 'collisions' # 'cosmics' # 'singlebeam' #
#if globalflags.DataSource != 'geant4':
#   DoTruth = False
#else:
#   globalflags.ConditionsTag = 'OFLCOND-SIM-BS7T-02'

globalflags.print_JobProperties()

# --- conditions tag

#if len(globalflags.ConditionsTag())!=0:
#   from IOVDbSvc.CondDB import conddb
#   conddb.setGlobalTag(globalflags.ConditionsTag())

# --- default is zero luminosity
#import AthenaCommon.SystemOfUnits as Units
#jobproperties.Beam.beamType           = 'collisions'

from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.solenoidOn=True



## Initialize the geometry
#  Necessary for the ZdcCabling Service
#
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

## Load algorithm to TopSequence
#
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

###################################################################################################
#D3PD maker setup
###################################################################################################

import D3PDMakerCoreComps
import EventCommonD3PDMaker

from   D3PDMakerCoreComps.D3PDObject import D3PDObject

## Event Info
#
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
##ZDC object
#
from ForwardDetectorsD3PDMaker.ZdcD3PDObject import ZdcD3PDObject
from ForwardDetectorsD3PDMaker.ZdcDigitsD3PDObject import ZdcDigitsD3PDObject

## MBTS object
#
from CaloD3PDMaker.MBTSD3PDObject import MBTSD3PDObject
from CaloD3PDMaker.MBTSTimeD3PDObject import MBTSTimeD3PDObject
from TrigMbD3PDMaker.MbtsContainerD3PDObject import MbtsContainerD3PDObject

## D3PD Maker
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.NewRootStream( "ZdcD3PD", tuple_name, "ZdcTree" )

alg += EventInfoD3PDObject (10)
alg += ZdcD3PDObject(0)
alg += ZdcDigitsD3PDObject(0)

alg += MBTSD3PDObject (10)
##Aggregate timing, time differences and counts
alg += MBTSTimeD3PDObject (10)


#Trigger
#not D3PD branch filler, adds trigger metadata to output D3PD file
from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata
addTrigConfMetadata(alg)
#raw info on what trig decision was made and at what level
from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
from TriggerD3PDMaker.BGCodeD3PDObject  import BGCodeD3PDObject
alg += TrigDecisionD3PDObject(10)
alg += BGCodeD3PDObject (10)

###################################################################################################
#ESD file for test. This will be kept on zdc castor area
###################################################################################################

ServiceMgr.EventSelector.InputCollections += \
["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/det-zdc/TestData/ESD.307459._000004.pool.root.1"]

theApp.EvtMax = -1
ServiceMgr.EventSelector.SkipEvents = 0


