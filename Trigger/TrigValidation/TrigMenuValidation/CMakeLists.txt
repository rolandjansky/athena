################################################################################
# Package: TrigMenuValidation
################################################################################

# Declare the package name:
atlas_subdir( TrigMenuValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/TrigMenuValidation_RTT_*.py )
atlas_install_runtime( test/TrigMenuValidation_TestConfiguration.xml share/TrigMenuValidation_RTT_XMLUploadToDB.py scripts/XMLUploadToDB.sh share/TrigMenuValidation_RTT_XMLUploadToDBwithSettings.py scripts/XMLDumperFromAthenaWithSettings.sh )

