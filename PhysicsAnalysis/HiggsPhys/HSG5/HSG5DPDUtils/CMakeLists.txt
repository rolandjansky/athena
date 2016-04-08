################################################################################
# Package: HSG5DPDUtils
################################################################################

# Declare the package name:
atlas_subdir( HSG5DPDUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaPython
                          PRIVATE
                          Event/NavFourMom
                          GaudiKernel
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( HSG5DPDUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps NavFourMom GaudiKernel JetEvent muonEvent )

# Install files from the package:
atlas_install_headers( HSG5DPDUtils )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

