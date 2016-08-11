################################################################################
# Package: AFP_RawEv
################################################################################

# Declare the package name:
atlas_subdir( AFP_RawEv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
			  Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Simulation/HitManagement
                          PRIVATE
                          Control/StoreGate
                          Event/EventContainers
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AFP_RawEv
                   src/*.cxx
                   PUBLIC_HEADERS AFP_RawEv
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers)

atlas_add_dictionary( AFP_RawEvDict
                      AFP_RawEv/AFP_RawEvDict.h
                      AFP_RawEv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AFP_RawEv )

