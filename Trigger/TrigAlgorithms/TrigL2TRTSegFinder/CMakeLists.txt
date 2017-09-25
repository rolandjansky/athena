################################################################################
# Package: TrigL2TRTSegFinder
################################################################################

# Declare the package name:
atlas_subdir( TrigL2TRTSegFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/RegionSelector
                          Event/ByteStreamCnvSvcBase
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          DetectorDescription/IRegionSelector
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          Trigger/TrigEvent/TrigSteeringEvent )

# Component(s) in the package:
atlas_add_component( TrigL2TRTSegFinder
                     src/TrigTRTSegFinder.cxx
                     src/components/TrigL2TRTSegFinder_entries.cxx
                     LINK_LIBRARIES AthenaBaseComps RegionSelectorLib ByteStreamCnvSvcBaseLib GaudiKernel InDetPrepRawData InDetRecToolInterfaces TrkGeometry TrkSegment TrkExInterfaces TrigInDetEvent TrigInterfacesLib TrigTimeAlgsLib IRegionSelector InDetRIO_OnTrack MagFieldInterfaces TrkSurfaces TrkParameters TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( TrigL2TRTSegFinder )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt share/*.py )

