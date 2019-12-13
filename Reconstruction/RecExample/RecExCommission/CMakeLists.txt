################################################################################
# Package: RecExCommission
################################################################################

# Declare the package name:
atlas_subdir( RecExCommission )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/RecExCommissionData_links.sh share/RecExCommission_links.sh share/RecExCommissionData_links.csh )

