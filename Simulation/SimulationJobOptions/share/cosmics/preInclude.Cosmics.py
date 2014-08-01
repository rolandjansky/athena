include.block('SimulationJobOptions/preInclude.Cosmics.py')
##########################################################
##
## SimulationJobOptions/preInclude.Cosmics.py
## Zach Marshall
##
## Set up the environment for cosmics simulation
##########################################################

## Use beamType job property to indicate this is a cosmics job
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType.get_Value() != 'cosmics':
    jobproperties.Beam.beamType = "cosmics"
print "Cosmics-specific configuration of Simulation and Digitization requested"

## Switch off EventFilter
from G4AtlasApps.SimFlags import simFlags
simFlags.EventFilter.set_Off()

## Now make the cosmics setup via callback functions

def cosmics_setup_filters():
    ## Define volumes
    atlasG4log.info("Setting up filter parameters")
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    MenuRecordEnvelopes = AtlasG4Eng.G4Eng.menu_RecordEnvelope()
    trt = PyG4Atlas.RecEnvelope('TRTBarrelEntryLayer', 'TRT::BarrelOuterSupport', 6)
    trteca = PyG4Atlas.RecEnvelope('TRTECAEntryLayer', 'TRT::WheelA', 6)
    trtecb = PyG4Atlas.RecEnvelope('TRTECBEntryLayer', 'TRT::WheelB', 6)
    sct = PyG4Atlas.RecEnvelope('SCTBarrelEntryLayer', 'SCT::ThShieldOuterCly', 6) # could be ThShieldInnerCly or Cyl..
    pixel = PyG4Atlas.RecEnvelope('PixelEntryLayer', 'Pixel::Pixel', 4)

    ## First filter volume
    if not simFlags.CosmicFilterVolumeName.statusOn:
        atlasG4log.warning("CosmicFilterVolumeName not set. Using default = %s" % simFlags.CosmicFilterVolumeName.get_Value())
        simFlags.CosmicFilterVolumeName.set_On()

    if simFlags.CosmicFilterVolumeName == "TRT_Barrel":
        atlasG4log.info('Setting recenv for TRT_Barrel')
        MenuRecordEnvelopes.add_RecEnvelope(trt)
    elif simFlags.CosmicFilterVolumeName == "TRT_EC":
        atlasG4log.info('Setting recenv for TRT_Endcaps')
        MenuRecordEnvelopes.add_RecEnvelope(trteca)
        MenuRecordEnvelopes.add_RecEnvelope(trtecb)
    elif simFlags.CosmicFilterVolumeName == "SCT_Barrel":
        atlasG4log.info('Setting recenv for SCT_Barrel')
        MenuRecordEnvelopes.add_RecEnvelope(sct)
    elif simFlags.CosmicFilterVolumeName == "Pixel":
        atlasG4log.info('Setting recenv for Pixel')
        MenuRecordEnvelopes.add_RecEnvelope(pixel)

    ## If second volume requested
    if simFlags.CosmicFilterVolumeName2.statusOn:
        if simFlags.CosmicFilterVolumeName2 == "TRT_Barrel":
            atlasG4log.info('Setting recenv2 for TRT_Barrel')
            MenuRecordEnvelopes.add_RecEnvelope(trt)
        elif simFlags.CosmicFilterVolumeName2 == "TRT_EC":
            atlasG4log.info('Setting recenv2 for TRT_Endcaps')
            MenuRecordEnvelopes.add_RecEnvelope(trteca)
            MenuRecordEnvelopes.add_RecEnvelope(trtecb)
        elif simFlags.CosmicFilterVolumeName2 == "SCT_Barrel":
            atlasG4log.info('Setting recenv2 for SCT_Barrel')
            MenuRecordEnvelopes.add_RecEnvelope(sct)
        elif simFlags.CosmicFilterVolumeName2 == "Pixel":
            atlasG4log.info('Setting recenv2 for Pixel')
            MenuRecordEnvelopes.add_RecEnvelope(pixel)

    # TODO: Does this work in a callback?
    if not simFlags.ISFRun:
        include("G4CosmicFilter/G4CosmicFilter.py")


def cosmics_modify_truth_strategies():
    ## Modifying truth strategies as requested by e/gamma group
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.listStrategies()
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Bremsstrahlung', 'CALO::CALO', 2)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Conversion', 'CALO::CALO', 2)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Decay', 'CALO::CALO', 2)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Ionization', 'CALO::CALO', 2)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('HadronicInteraction', 'CALO::CALO',2)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Compton', 'CALO::CALO', 2)


## Register the callbacks (at two different levels)
simFlags.InitFunctions.add_function("preInitG4", cosmics_setup_filters)
simFlags.InitFunctions.add_function("postInit", cosmics_modify_truth_strategies)
