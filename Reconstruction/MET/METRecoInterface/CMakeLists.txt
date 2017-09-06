################################################################################
# Package: METRecoInterface
################################################################################

# Declare the package name:
atlas_subdir( METRecoInterface )

find_package( ROOT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODMissingET )

# Install files from the package:
atlas_install_headers( METRecoInterface )

atlas_add_dictionary( METRecoInterfaceDict
       		      METRecoInterface/METRecoInterfaceDict.h
	              METRecoInterface/selection.xml
	              INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
	              LINK_LIBRARIES ${ROOT_LIBRARIES} xAODMissingET AthContainers EventPrimitives )
