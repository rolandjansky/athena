if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-GEO-11-01-00"
    print "MuonGeoModelTest/simulGeantinoHits Setting now DetDescrVersion to ", DetDescrVersion
else:
    print "MuonGeoModelTest/simulGeantinoHits DetDescrVersion already set to ", DetDescrVersion

# now it's necessary to specify the global condition tag
from AthenaCommon.GlobalFlags import globalflags
if not 'ConditionsTag' in dir():
   ConditionsTag="OFLCOND-SIM-01-00-00"
   print "MuonGeoModelTest/simulGeantinoHits Setting now ConditionsTag to ", ConditionsTag
else:
    print "MuonGeoModelTest/simulGeantinoHits DetDescrVersion already set to ", ConditionsTag
                    
#globalflags.ConditionsTag = 'OFLCOND-SIM-01-00-00'
globalflags.ConditionsTag = ConditionsTag


#==============================================================
#
# Job options file for Geant4 Simulations
# modified version of jobOptions.G4Atlas_Sim.py (in Simulation/G4Atlas/G4AtlasApps/share)
#
# Atlas simulation 
#
__version__="$Revision: 1.5 $"
#==============================================================

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
#
DetFlags.simulate.Truth_setOn()
#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.PoolHitsOutput='MuonHits.root'
athenaCommonFlags.EvtMax=1000

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.SimLayout.set_Value(DetDescrVersion) # specific value 
#SimFlags.SimLayout.set_On()                  # use the default value
#  sets the EtaPhi, VertexSpread and VertexRange checks on
SimFlags.EventFilter.set_On()  
#
print "Reading alignment constants from DB"
from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC')
from MuonCondTool.MuonCondToolConf import MuonAlignmentDbTool
MuonAlignmentDbTool = MuonAlignmentDbTool("MGM_AlignmentDbTool")
MuonAlignmentDbTool.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                      "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                      "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                      "/MUONALIGN/TGC/SIDEA",
                                      "/MUONALIGN/TGC/SIDEC"]
ToolSvc += MuonAlignmentDbTool
MGM_AlignmentDbTool = ToolSvc.MGM_AlignmentDbTool
MGM_AlignmentDbTool.OutputLevel=DEBUG

from AtlasGeoModel.MuonGM import GeoModelSvc
#GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.01.01.Initial.Light"
MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]
MuonDetectorTool.UseConditionDb = 1
MuonDetectorTool.OutputLevel=VERBOSE


## Run ParticleGenerator
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: constant 13',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: flat -3.0 3.0',
             'phi: flat  0 6.283186',
             'e: constant 10000']
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
topSeq.ParticleGenerator.orders = sorted(spgorders)
from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
ServiceMgr += AtRanluxGenSvc()
ServiceMgr.AtRanluxGenSvc.Seeds = ["SINGLE 2040160768 443921183"]


## Add G4 sim framework alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3


