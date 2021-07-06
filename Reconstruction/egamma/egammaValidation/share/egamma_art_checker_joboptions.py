#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
#


from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon import CfgMgr
from AthenaCommon.AlgSequence import AlgSequence
from GaudiSvc.GaudiSvcConf import THistSvc
from AthenaCommon.GlobalFlags import jobproperties
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags
import AthenaPoolCnvSvc.ReadAthenaPool

#Tools 
MCClassifier = CfgMgr.MCTruthClassifier(
    name="MCTruthClassifier",
    ParticleCaloExtensionTool="")

LooseLH = CfgMgr.AsgElectronLikelihoodTool(
    name="LooseLH",
    WorkingPoint="LooseLHElectron")

MediumLH = CfgMgr.AsgElectronLikelihoodTool(
    name="MediumLH",
    WorkingPoint="MediumLHElectron")

TightLH = CfgMgr.AsgElectronLikelihoodTool(
    name="TightLH",
    WorkingPoint="TightLHElectron")

Loose_Photon = CfgMgr.AsgPhotonIsEMSelector(
    name="Loose_Photon",
    WorkingPoint="LoosePhoton")

Tight_Photon = CfgMgr.AsgPhotonIsEMSelector(
    name="Tight_Photon",
    WorkingPoint="TightPhoton")

IsoFixedCutTight = CfgMgr.CP__IsolationSelectionTool(
    name="IsoFixedCutTight", 
    PhotonWP="FixedCutTight")

IsoFixedCutTightCaloOnly = CfgMgr.CP__IsolationSelectionTool(
    name="IsoFixedCutTightCaloOnly",
    PhotonWP="FixedCutTightCaloOnly")

IsoFixedCutLoose = CfgMgr.CP__IsolationSelectionTool(
    name="IsoFixedCutLoose",
    PhotonWP="FixedCutLoose")

# Ouput Message Level
svcMgr.MessageSvc.OutputLevel = INFO

Geometry = "ATLAS-R2-2016-01-00-01"
globalflags.DetGeo.set_Value_and_Lock('atlas')
DetFlags.detdescr.all_setOn()
DetFlags.Forward_setOff()
DetFlags.ID_setOff()

jobproperties.Global.DetDescrVersion = Geometry

# We need the following two here to properly have
# Geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion
include("CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include("LArDetDescr/LArDetDescr_joboptions.py")


# ART File
testFile = 'Nightly_AOD_'+particleType+'.pool.root'
svcMgr.EventSelector.InputCollections = [testFile]


job = CfgMgr.AthSequencer("AthAlgSeq")
job += CfgMgr.EgammaMonitoring(
    'MonitoringAlg',
    sampleType=particleType,
    MCTruthClassifier=MCClassifier,
    LooseLH=LooseLH,
    MediumLH=MediumLH,
    TightLH=TightLH,
    Loose_Photon=Loose_Photon,
    Tight_Photon=Tight_Photon,
    IsoFixedCutTight=IsoFixedCutTight,
    IsoFixedCutLoose=IsoFixedCutLoose,
    IsoFixedCutTightCaloOnly=IsoFixedCutTightCaloOnly,
)


theApp.EvtMax = 60000


outputFile = 'Nightly-monitoring_'+particleType+'.hist.root'
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output = [
    "MONITORING DATAFILE='"+outputFile+"' OPT='RECREATE'"]


print("\n\nALL OK\n\n")
