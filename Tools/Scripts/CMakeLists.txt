################################################################################
# Package: Scripts
################################################################################

# Declare the package name:
atlas_subdir( Scripts )

# this line failed automatic conversion in cmt2cmake :
# setup_script $(SETUPCOMPLETIONROOT)/cmt/completion/setup_completion_main

# Install files from the package:
atlas_install_scripts( share/get_joboptions share/get_files share/lookup_local_joboptions.sh share/lookup_joboptions.sh share/lookup_scripts.sh share/lookup_xmls.sh share/lookup_data.sh share/checkDbgSymbols.sh )

athena_env( SET DO_EXT_ATHENA_TAB_COMPLETION 0
            SET SETUPCOMPLETIONROOT \\\${SCRIPTSROOT} )

