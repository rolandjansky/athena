################################################################################
# Package: LArCalibTools
################################################################################

# Declare the package name:
atlas_subdir( LArCalibTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawConditions
                          LArCalorimeter/LArRecConditions
                          PRIVATE
                          Calorimeter/CaloCondBlobObjs
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          LArCalorimeter/LArCOOLConditions
                          LArCalorimeter/LArCondUtils
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArTools )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( LArCalibTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CaloIdentifier AthenaBaseComps StoreGateLib SGtests GaudiKernel LArCablingLib LArIdentifier LArRawConditions LArRecConditions CaloCondBlobObjs AthenaKernel AthenaPoolUtilities EventInfo LArCOOLConditions LArRawEvent LArToolsLib )

# Install files from the package:
atlas_install_headers( LArCalibTools )
atlas_install_joboptions( share/*.py )

