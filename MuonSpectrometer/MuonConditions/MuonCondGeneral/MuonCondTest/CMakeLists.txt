################################################################################
# Package: MuonCondTest
################################################################################

# Declare the package name:
atlas_subdir( MuonCondTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondSvc
                          TestPolicy
                          PRIVATE
                          DetectorDescription/Identifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonCondTest
                     src/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MuonCondInterface MuonCondSvcLib Identifier )

# Install files from the package:
atlas_install_headers( MuonCondTest )
atlas_install_joboptions( share/*.py )

