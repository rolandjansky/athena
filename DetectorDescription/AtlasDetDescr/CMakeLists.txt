################################################################################
# Package: AtlasDetDescr
################################################################################

# Declare the package name:
atlas_subdir( AtlasDetDescr )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/IdDict
                          DetectorDescription/Identifier
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AtlasDetDescr
                   src/AtlasDetectorID.cxx
                   src/AtlasDetectorIDHelper.cxx
                   src/AtlasRegionHelper.cxx
                   PUBLIC_HEADERS AtlasDetDescr
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools IdDict Identifier
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( AtlasDetDescrDict
                      AtlasDetDescr/AtlasDetDescrDict.h
                      AtlasDetDescr/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools IdDict Identifier GaudiKernel AtlasDetDescr )

