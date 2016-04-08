################################################################################
# Package: CaloTTDetDescr
################################################################################

# Declare the package name:
atlas_subdir( CaloTTDetDescr )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/Identifier
                          PRIVATE
                          Calorimeter/CaloIdentifier )

# Component(s) in the package:
atlas_add_library( CaloTTDetDescr
                   src/*.cxx
                   PUBLIC_HEADERS CaloTTDetDescr
                   LINK_LIBRARIES SGTools Identifier
                   PRIVATE_LINK_LIBRARIES CaloIdentifier )

