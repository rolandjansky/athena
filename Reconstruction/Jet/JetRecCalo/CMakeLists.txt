# $Id: CMakeLists.txt 757523 2016-06-24 15:20:43Z krasznaa $
################################################################################
# Package: JetRecCalo
################################################################################

# Declare the package name:
atlas_subdir( JetRecCalo )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
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
atlas_add_library( JetRecCaloLib
   JetRecCalo/*.h
   INTERFACE
   PUBLIC_HEADERS JetRecCalo
   LINK_LIBRARIES GaudiKernel CaloIdentifier TileConditionsLib CxxUtils JetUtils
   AsgTools JetInterface )

atlas_add_component( JetRecCalo
   src/*.cxx src/components/*.cxx
   LINK_LIBRARIES GaudiKernel CaloDetDescrLib CaloEvent AthenaKernel
   LArRecConditions JetRecCaloLib )
