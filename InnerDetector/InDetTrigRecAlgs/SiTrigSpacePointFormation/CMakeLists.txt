################################################################################
# Package: SiTrigSpacePointFormation
################################################################################

# Declare the package name:
atlas_subdir( SiTrigSpacePointFormation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Trigger/TrigSteer/TrigInterfaces
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/IRegionSelector
                          Event/EventPrimitives
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecAlgs/SiSpacePointFormation
                          InnerDetector/InDetRecTools/SiSpacePointTool
                          InnerDetector/InDetTrigRecTools/InDetTrigToolInterfaces
                          Tracking/TrkEvent/TrkSpacePoint
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigTimeAlgs )

include_directories(src)

# Component(s) in the package:
atlas_add_component( SiTrigSpacePointFormation
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES Identifier GaudiKernel InDetPrepRawData TrigInterfacesLib AthenaBaseComps IRegionSelector EventPrimitives InDetIdentifier InDetReadoutGeometry SiSpacePointFormationLib SiSpacePointToolLib TrkSpacePoint TrigSteeringEvent TrigTimeAlgsLib InDetTrigToolInterfacesLib )

# Install files from the package:
atlas_install_headers( SiTrigSpacePointFormation )
atlas_install_python_modules( python/*.py )

