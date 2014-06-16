# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.SystemOfUnits import GeV
from EventShapeTools.EventEtClusterDensityConfig import configureJetAreaDensity, getDefaultEtDensityClusterDict
from EventShapeRec.EventShapeRecConf import EventEtDensityTester
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaServices.TheUserDataSvc import TheUserDataSvc
svcMgr += TheUserDataSvc("UserDataSvc")

# -- common tool settings
commonDict = dict(getDefaultEtDensityClusterDict())
commonDict['Algorithm']             = "kt"
commonDict['JetAreaMethod']         = "VoronoiArea"
commonDict['VoronoiEffectiveRfact'] = 0.9
commonDict['InclusivePtMin']        = 0.*GeV
commonDict['EtaMin']                = -2.
commonDict['EtaMax']                =  2.
commonDict['UseAreaFourMomentum']   = True
del commonDict['SignalState']

    
def suffix (radius, calibrated):
    suff = 'Kt%d' % int(radius*10 + 0.5)
    return suff +  ( 'LC' if calibrated else 'EM')
    
def areaDensityTool (radius, calibrated):

    toolName = "ClusterRho"+ suffix (radius, calibrated)

    ss = 'CALIBRATED' if calibrated else 'UNCALIBRATED'
    if not hasattr(ToolSvc, toolName):
        rhoTool = configureJetAreaDensity (toolName,
                                           Radius = radius,
                                           SignalState = ss,
                                           **commonDict)
    else:
        rhoTool = getattr(ToolSvc, toolName)
    return rhoTool

def calculateSimpleEventDensity(Radius = 0.4,SignalState="UNCALIBRATED", UseAreaFourMomentum=True, seq=AlgSequence() ):
    """This function schedule an algorithm which compute a simple event et density, rho, variable and then stores it in userStore()
    The key in userStore is in the form ClusterRhoKtXY where
       - X = Radius*10
       - Y = EM if SignalState="UNCALIBRATED" and LC in other cases.
   for example "ClusterRhoKt4EM"

   Subsequent calls of this function will simply schedule new calculations from the same athena algorithm.
    """

    # Check if an alg already exists 
    if hasattr( seq, "EventEtDensityTester" ):
        alg  = seq.EventEtDensityTester
    elif hasattr( AlgSequence(), "EventEtDensityTester" ):
        alg  = seq.EventEtDensityTester
    else:
        alg = EventEtDensityTester("EventEtDensityTester", FillHistos=False, FillEventStore=True, VertexContainerKey="PrimaryVertices")
        seq += alg        

    rhoTool= areaDensityTool(Radius, SignalState=="CALIBRATED" )

    alg.EventEtDensityTools += [ rhoTool ] 
    
