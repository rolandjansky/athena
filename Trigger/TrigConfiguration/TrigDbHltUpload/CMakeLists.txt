# $Id: CMakeLists.txt 739093 2016-04-09 15:34:36Z krasznaa $
################################################################################
# Package: TrigDbHltUpload
################################################################################

# Declare the package name:
atlas_subdir( TrigDbHltUpload )

# External dependencies:
find_package( Oracle )
find_package( cx_Oracle )

# Install files from the package:
atlas_install_scripts( share/ConvertHLTSetup_txt2xml.py
  share/prepareOnlineTriggerConfig.py
  share/oldPrepareOnlineTriggerConfig.py
  share/menumerge.py share/*.sh )
atlas_install_python_modules( python/*.py )
atlas_install_xmls( XML/*.dtd )
