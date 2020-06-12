################################################################################
# Package: CalibrationDataInterface
################################################################################

# Declare the package name:
atlas_subdir( CalibrationDataInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PhysicsAnalysis/JetTagging/JetTagCalibration )

# External dependencies:
find_package( ROOT COMPONENTS Cint Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

find_package( Boost ) # header-only so no libraries needed

# Component(s) in the package:
atlas_add_root_dictionary( CalibrationDataInterfaceLib
                           CalibrationDataInterfaceLibDictSource
                           ROOT_HEADERS
			   CalibrationDataInterface/CalibrationDataContainer.h
			   CalibrationDataInterface/CalibrationDataVariables.h
			   CalibrationDataInterface/CalibrationDataInterfaceBase.h
			   CalibrationDataInterface/CalibrationDataInterfaceROOT.h
			   CalibrationDataInterface/CalibrationDataEigenVariations.h
			   CalibrationDataInterface/CalibrationDataBackend.h
			   CalibrationDataInterface/CalibrationDataUtilities.h
			   CalibrationDataInterface/CalibrationDataInternals.h Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT Boost )

atlas_add_library( CalibrationDataInterfaceLib
                   Root/*.cxx
                   ${CalibrationDataInterfaceLibDictSource}
                   PUBLIC_HEADERS CalibrationDataInterface

                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} )

# Install files from the package:
atlas_install_joboptions( share/*.py )
