################################################################################
# Package: InDetByteStreamErrors
################################################################################

# Declare the package name:
atlas_subdir( InDetByteStreamErrors )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          Event/EventContainers
                          DetectorDescription/Identifier )

# this line failed automatic conversion in cmt2cmake :
# use AtlasReflex          AtlasReflex-*              External - no_auto_imports

# Component(s) in the package:
atlas_add_dictionary( InDetByteStreamErrorsDict
                      InDetByteStreamErrors/InDetByteStreamErrorsDict.h
                      InDetByteStreamErrors/selection.xml
                      LINK_LIBRARIES AthContainers Identifier )

atlas_add_library( InDetByteStreamErrors
                   src/*.cxx
                   PUBLIC_HEADERS InDetByteStreamErrors
                   LINK_LIBRARIES AthenaKernel )

