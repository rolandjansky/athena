################################################################################
# Package: Scripts
################################################################################

# Declare the package name:
atlas_subdir( Scripts )

# Install files from the package:
atlas_install_scripts( share/get_joboptions share/get_files share/lookup_local_joboptions.sh share/lookup_joboptions.sh share/lookup_scripts.sh share/lookup_xmls.sh share/lookup_data.sh share/checkDbgSymbols.sh share/git-package-pseudomerge.py )
