################################################################################
# Package: GeoMaterial2G4
################################################################################

# Declare the package name:
atlas_subdir( GeoMaterial2G4 )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoModel/GeoModelKernel
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/GeoModel/GeoModelUtilities )

# External dependencies:
find_package( Geant4 )

# Component(s) in the package:
atlas_add_library( GeoMaterial2G4
                   src/*.cxx
                   PUBLIC_HEADERS GeoMaterial2G4
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} AthenaKernel GeoModelKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps GeoModelUtilities )
