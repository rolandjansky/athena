################################################################################
# Package: LArCondUtils
################################################################################

# Declare the package name:
atlas_subdir( LArCondUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArElecCalib
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaKernel
                          Database/RDBAccessSvc
                          Database/RegistrationServices
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArGeoModel/LArHV
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawConditions
                          LArCalorimeter/LArRecConditions )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( LArCondUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel CaloDetDescrLib CaloIdentifier CaloGeoHelpers AthenaKernel LArCablingLib LArHV LArReadoutGeometry LArIdentifier LArRawConditions LArRecConditions )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/LArEmLvl1.data share/LArHecLvl1.data share/LArFcalLvl1.data )
atlas_install_scripts( share/LArBuildHVPathologies.sh share/fillPileUpNoiseLumi.py share/dumpHVPathFromNtuple.py )

