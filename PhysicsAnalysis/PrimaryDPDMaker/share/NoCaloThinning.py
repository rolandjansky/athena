###
# Special fragment to turn off (python) calo thinning from the jobConfig argument of job transforms

from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDFlags_EGammaStream import primEGammaDPD
primEGammaDPD.ApplyCaloCellThinning=False

