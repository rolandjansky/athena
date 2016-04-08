################################################################################
# Package: TriggerMenuPython
################################################################################

# Declare the package name:
atlas_subdir( TriggerMenuPython )

# Install files from the package:
atlas_install_python_modules( python/*.py RatesRulebook/*.py scripts/cnvXml_express.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/dumpMenu.py scripts/createMenuXMLFiles.sh scripts/trigconf_dumphltchain.py scripts/trigconf_find_related.py scripts/printL1CableMap.py scripts/addPS.py )
atlas_install_xmls( data/*.xml )

