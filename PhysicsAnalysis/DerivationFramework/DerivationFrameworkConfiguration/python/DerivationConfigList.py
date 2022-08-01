# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# All derivation framework formats must be listed here

# Example formats
# Skimming example
from DerivationFrameworkExamples.TEST1 import TEST1Cfg
# Skimming with strings example
from DerivationFrameworkExamples.TEST2 import TEST2Cfg
# Thinning example
from DerivationFrameworkExamples.TEST3 import TEST3Cfg
# Slimming example
from DerivationFrameworkExamples.TEST4 import TEST4Cfg
# Decoration example
from DerivationFrameworkExamples.TEST5 import TEST5Cfg
# Pre-selection example
from DerivationFrameworkExamples.TEST6 import TEST6Cfg

# Truth (EVNT->xAOD) formats
# TRUTH0 - complete copy of HepMC to xAOD truth
from DerivationFrameworkMCTruth.TRUTH0 import TRUTH0Cfg
# TRUTH1 - extended common ATLAS truth for analysis
from DerivationFrameworkMCTruth.TRUTH1 import TRUTH1Cfg
# TRUTH3 - standard common ATLAS truth for analysis
from DerivationFrameworkMCTruth.TRUTH3 import TRUTH3Cfg

# Common unskimmed formats for Run 3 physics analysis
# PHYS - uncalibrated, full slimming list
from DerivationFrameworkPhys.PHYS import PHYSCfg
# PHYSLITE - calibrated physics analysis objects, reduced slimming list
from DerivationFrameworkPhys.PHYSLITE import PHYSLITECfg

# Physics validation for run 3
# PHYSVAL - large bulk of the variables from AOD plus PHYS augmentations
from DerivationFrameworkPhysicsValidation.PHYSVAL import PHYSVALCfg

# Higgs derivations
# HIGG1D1 Higgs->gammagamma derivation
from DerivationFrameworkHiggs.HIGG1D1 import HIGG1D1Cfg

# LLP derivations
from DerivationFrameworkLLP.LLP1 import LLP1Cfg

# BLS derivations
from DerivationFrameworkBPhys.BPHY1 import BPHY1Cfg
from DerivationFrameworkBPhys.BPHY5 import BPHY5Cfg

# Avoids compilation warnings from Flake8
__all__ = ['TEST1Cfg','TEST2Cfg','TEST3Cfg','TEST4Cfg','TEST5Cfg','TEST6Cfg',
           'TRUTH0Cfg','TRUTH1Cfg','TRUTH3Cfg',
           'PHYSCfg','PHYSLITECfg',
           'PHYSVALCfg',
           'HIGG1D1Cfg',
           'LLP1Cfg',
           'BPHY1Cfg', 'BPHY5Cfg']

