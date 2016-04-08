################################################################################
# Package: LArHV
################################################################################

# Declare the package name:
atlas_subdir( LArHV )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/IOVSvc
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelKernel
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArIdentifier )

# Component(s) in the package:
atlas_add_library( LArHV
                   src/*.cpp
                   PUBLIC_HEADERS LArHV
                   LINK_LIBRARIES SGTools GeoModelKernel IOVSvcLib StoreGateLib SGtests LArCablingLib
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities Identifier GaudiKernel LArIdentifier )

