################################################################################
# Package: TrigDbHltUpload
################################################################################

# Declare the package name:
atlas_subdir( TrigDbHltUpload )

# External dependencies:
find_package( Oracle )

# Install files from the package:
atlas_install_scripts( share/ConvertHLTSetup_txt2xml.py share/prepareOnlineTriggerConfig.py share/oldPrepareOnlineTriggerConfig.py share/menumerge.py share/*.sh )
atlas_install_xmls( python/*.py )
atlas_install_xmls( XML/*.dtd )

