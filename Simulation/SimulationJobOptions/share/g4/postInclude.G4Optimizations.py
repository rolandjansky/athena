##########################################################
#
# SimulationJobOptions/postInclude.G4Optimizations.py
# Marilena Bandieramonte
#
# Geant4 Optimization Options for Run3
#  
##########################################################

###########################################################
#
# Configure correct volumes for the beam pipe killer
# The beam pipe killer itself is actived through SimulationJobOptions/preInclude.BeamPipeKill.py
# More info: https://its.cern.ch/jira/browse/ATLASSIM-5337
#
###########################################################

if hasattr(simFlags, 'BeamPipeSimMode' ):
    if(simFlags.BeamPipeSimMode() == "FastSim"):
        ToolSvc.G4AtlasDetectorConstructionTool.RegionCreators['BeampipeFwdCutPhysicsRegionTool'].VolumeList = ['BeamPipe::SectionF198','BeamPipe::SectionF199','BeamPipe::SectionF200']
        print(ToolSvc.G4AtlasDetectorConstructionTool.RegionCreators['BeampipeFwdCutPhysicsRegionTool'])


###########################################################
#
# Configure the assembly beam pipe
# More info: https://its.cern.ch/jira/browse/ATLASSIM-3773 and https://its.cern.ch/jira/browse/ATLPHYSVAL-660
# This is not validated as of early August 2021, so kept commented out!
#
###########################################################
#ServiceMgr.GeoModelSvc.DetectorTools['BeamPipeDetectorTool'].BeamPipeMode='AssemblyBeamPipe'
#print(ServiceMgr.GeoModelSvc)

###########################################################
#
# EMEC custom solid SLICES variant
# Use the SLICES variant of the EMEC custom solid
# both for the inner and outer wheel. 
# More info: https://its.cern.ch/jira/browse/ATLASSIM-3778
#
###########################################################

from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
LArDetectorToolNV.EMECVariantInner="Slices"
LArDetectorToolNV.EMECVariantOuter="Slices"
