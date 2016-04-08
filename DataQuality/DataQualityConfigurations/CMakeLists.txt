################################################################################
# Package: DataQualityConfigurations
################################################################################

# Declare the package name:
atlas_subdir( DataQualityConfigurations )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/MergeConfigs.py scripts/UploadDQAMITag.py scripts/merge_all_han_configs.sh scripts/merge_some_han_configs.sh scripts/han-config-indent.sh scripts/han-config-check.sh scripts/han-config-parse.awk )

