################################################################################
# Package: InDetRawData
################################################################################

# Declare the package name:
atlas_subdir( InDetRawData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/Identifier
                          Event/EventContainers
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetRawData
                   src/*.cxx
                   PUBLIC_HEADERS InDetRawData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks SGTools Identifier EventContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( InDetRawDataDict
                      InDetRawData/InDetRawDataCLASS_DEF.h
                      InDetRawData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks SGTools Identifier GaudiKernel InDetRawData EventContainers )

