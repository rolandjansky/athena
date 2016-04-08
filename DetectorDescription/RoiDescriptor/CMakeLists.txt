################################################################################
# Package: RoiDescriptor
################################################################################

# Declare the package name:
atlas_subdir( RoiDescriptor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/IRegionSelector )

# External dependencies:
find_package( Boost COMPONENTS program_options regex )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( RoiDescriptor
                   Root/RoiDescriptor.cxx
                   Root/RoiUtil.cxx
                   PUBLIC_HEADERS RoiDescriptor
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES IRegionSelector ${Boost_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( RoiDescriptorDict
                      RoiDescriptor/RoiDescriptorDict.h
                      RoiDescriptor/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} IRegionSelector RoiDescriptor )

