################################################################################
# Package: TrigMenuRulebook
################################################################################

# Declare the package name:
atlas_subdir( TrigMenuRulebook )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/* )
atlas_install_scripts( scripts/runRuleBook.py scripts/cnvXML.py python/processRules.py )

