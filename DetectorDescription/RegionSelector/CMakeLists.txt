################################################################################
# Package: RegionSelector
################################################################################

# Declare the package name:
atlas_subdir( RegionSelector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/RegSelLUT
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          Tools/PathResolver )

# Component(s) in the package:
atlas_add_library( RegionSelectorLib
                   src/*.cxx
                   PUBLIC_HEADERS RegionSelector
                   LINK_LIBRARIES SGTools IRegionSelector Identifier GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities RegSelLUT PathResolver )

atlas_add_component( RegionSelector
                     src/components/*.cxx
                     LINK_LIBRARIES SGTools IRegionSelector Identifier GaudiKernel AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities RegSelLUT PathResolver RegionSelectorLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

