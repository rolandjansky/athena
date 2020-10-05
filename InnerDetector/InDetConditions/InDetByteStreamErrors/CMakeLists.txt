################################################################################
# Package: InDetByteStreamErrors
################################################################################

# Declare the package name:
atlas_subdir( InDetByteStreamErrors )

# Component(s) in the package:
atlas_add_dictionary( InDetByteStreamErrorsDict
                      InDetByteStreamErrors/InDetByteStreamErrorsDict.h
                      InDetByteStreamErrors/selection.xml
                      LINK_LIBRARIES AthContainers Identifier )

atlas_add_library( InDetByteStreamErrors
                   src/*.cxx
                   PUBLIC_HEADERS InDetByteStreamErrors
                   LINK_LIBRARIES AthenaKernel EventContainers )

