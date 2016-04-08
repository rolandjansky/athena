#----------------------------------------------------------------------
#      jobOptions for MissingET package
#----------------------------------------------------------------------
from MissingET.MissingETConf import METAlg
topSequence += METAlg()

from MissingET.MissingETConf import METPartTruthTool
METPartTruth = METPartTruthTool('Truth')

#----------------------------------------------------------------------
# Name of MissingET AlgTools:	  
#----------------------------------------------------------------------
topSequence.METAlg.AlgTools = [
    METPartTruth.getFullName()
    ]

#----------------------------------------------------------------------
#  AlgTools  steering
#----------------------------------------------------------------------
#
# - METPartTruthTool
#
METPartTruth.etafullsim = 3.2;
METPartTruth.outKey="MET_Truth"

