################################################################################
# Package: TrigConfOffline
################################################################################

# Declare the package name:
atlas_subdir( TrigConfOffline )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigConfiguration/TrigConfStorage )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( TrigConfL1DataDict
                      TrigConfOffline/TrigConfL1DataDict.h
                      TrigConfOffline/selectionL1Data.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigConfHLTData TrigConfL1Data TrigConfStorage )

atlas_add_dictionary( TrigConfHLTDataDict
                      TrigConfOffline/TrigConfHLTDataDict.h
                      TrigConfOffline/selectionHLTData.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigConfHLTData TrigConfL1Data TrigConfStorage )

atlas_add_dictionary( TrigConfStorageDict
                      TrigConfOffline/TrigConfStorageDict.h
                      TrigConfOffline/selectionStorage.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigConfHLTData TrigConfL1Data TrigConfStorage )

# Install files from the package:
atlas_install_headers( TrigConfOffline )
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/LoadTriggerMenuFromXML.py )

