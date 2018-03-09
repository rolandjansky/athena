################################################################################
# Package: LArCalibUtils
################################################################################

# Declare the package name:
atlas_subdir( LArCalibUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawConditions
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRawUtils
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArRecUtils
                          LArCalorimeter/LArCabling
                          TestBeam/TBEvent
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArBadChannelTool
                          LArCalorimeter/LArCOOLConditions
                          LArCalorimeter/LArSimEvent
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GSL )
find_package( ROOT COMPONENTS Minuit Core Tree MathCore Hist RIO pthread MathMore Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package( TBB )

# Component(s) in the package:
atlas_add_component( LArCalibUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${GSL_LIBRARIES} ${CORAL_LIBRARIES} ${EIGEN_LIBRARIES} ${TBB_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier GaudiKernel LArIdentifier LArRawConditions LArRawEvent LArRawUtilsLib LArRecConditions LArRecUtilsLib LArCablingLib TBEvent CaloDetDescrLib AthenaPoolUtilities xAODEventInfo LArBadChannelToolLib LArCOOLConditions LArSimEvent )

# Install files from the package:
atlas_install_headers( LArCalibUtils )
atlas_install_python_modules( python/*.py )

