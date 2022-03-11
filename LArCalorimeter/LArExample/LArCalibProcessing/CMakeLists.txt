# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArCalibProcessing )

# Necessary external(s):
find_package( cx_Oracle )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/LArCalib_*.py share/GetInputFiles.py share/LArHVCool2Ntuple_jobOptions.py share/LArTimePhysPrediction_jobOptions.py share/UseLArExtendedSubDetGrouping.py )
atlas_install_scripts( share/LAr_updateHVcalib_online.sh share/LAr_updateHVcalibNoise.sh share/LArNewCalib_*.py )
