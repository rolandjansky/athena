################################################################################
# Package: PerfMonEvent
################################################################################

# Declare the package name:
atlas_subdir( PerfMonEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# External dependencies:
find_package( PythonLibs )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( PerfMonEvent
                   src/PyStore.cxx
                   src/PyChrono.cxx
                   src/DataModel.cxx
                   src/MemStatsHooks.cxx
                   src/MallocStats.cxx
                   PUBLIC_HEADERS PerfMonEvent
                   INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${PYTHON_LIBRARIES} GaudiKernel rt
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( PerfMonEventDict
                      PerfMonEvent/PerfMonEventDict.h
                      PerfMonEvent/selection.xml
                      INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${PYTHON_LIBRARIES} ${ROOT_LIBRARIES} rt GaudiKernel PerfMonEvent )

