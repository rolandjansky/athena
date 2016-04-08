################################################################################
# Package: JetRecCalo
################################################################################

# Declare the package name:
atlas_subdir( JetRecCalo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthToolSupport/AsgTools
                          Control/CxxUtils
                          GaudiKernel
                          Reconstruction/Jet/JetInterface
                          Reconstruction/Jet/JetUtils
                          TileCalorimeter/TileConditions
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Control/AthenaKernel
                          LArCalorimeter/LArRecConditions )

# Component(s) in the package:
atlas_add_component( JetRecCalo
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloIdentifier AsgTools CxxUtils GaudiKernel JetInterface JetUtils TileConditionsLib CaloDetDescrLib CaloEvent AthenaKernel LArRecConditions )

# Install files from the package:
atlas_install_headers( JetRecCalo )

