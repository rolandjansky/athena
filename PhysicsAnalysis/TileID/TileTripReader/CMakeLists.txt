################################################################################
# Package: TileTripReader
################################################################################

# Declare the package name:
atlas_subdir( TileTripReader )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PRIVATE
                          Control/AthenaKernel
                          Event/EventKernel
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 PyROOT )

# Component(s) in the package:
atlas_add_library( TileTripReaderLib
                   Root/*.cxx
                   PUBLIC_HEADERS TileTripReader
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps PATCoreLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel EventKernel xAODEventInfo GaudiKernel PathResolver )

atlas_add_component( TileTripReader
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps PATCoreLib AthenaKernel EventKernel xAODEventInfo GaudiKernel PathResolver TileTripReaderLib )

atlas_add_dictionary( TileTripReaderDict
   TileTripReader/TileTripReaderDict.h
   TileTripReader/selection.xml
   LINK_LIBRARIES ${ROOT_LIBRARIES} TileTripReaderLib )

 # Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_xmls( data/*.root )

