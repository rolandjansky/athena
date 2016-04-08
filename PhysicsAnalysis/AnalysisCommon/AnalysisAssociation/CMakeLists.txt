################################################################################
# Package: AnalysisAssociation
################################################################################

# Declare the package name:
atlas_subdir( AnalysisAssociation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Event/EventKernel
                          Event/FourMomUtils
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( AnalysisAssociation
                   src/*.cxx
                   PUBLIC_HEADERS AnalysisAssociation
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel EventKernel FourMomUtils GaudiKernel AnalysisUtilsLib )

