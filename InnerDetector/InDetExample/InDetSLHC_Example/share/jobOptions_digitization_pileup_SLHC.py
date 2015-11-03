#==============================================================
# JobOptions for digitisation and analysis
# may be two thing to change:
# 1) the name of the input and output files
# 2) the number of event processed
#==============================================================

import os

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS14T-ATLAS-00"

include("InDetSLHC_Example/preInclude.SLHC.py")

include("InDetSLHC_Example/preInclude.SiliconOnly.py")

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-00-00'

#--------------------------------------------------------------
# Pileup configuration
#--------------------------------------------------------------
mergeMcEventCollection = Algorithm( "MergeMcEventCollection" )
mergeMcEventCollection.DoSlimming=True

from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=int(os.environ['EVTMAX'])
jobproperties.AthenaCommonFlags.SkipEvents    = int(os.environ['EVTSKIP'])
jobproperties.AthenaCommonFlags.PoolHitsInput=[ os.environ['INFILENAME'] ]
jobproperties.AthenaCommonFlags.PoolRDOOutput=os.environ['OUTFILENAME']

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties 
jobproperties.Digitization.doInDetNoise         = False
jobproperties.Digitization.doMinimumBias        = True
jobproperties.Digitization.doCavern             = False
jobproperties.Digitization.bunchSpacing         = 25
jobproperties.Digitization.numberOfCollisions   = float(os.environ['NPILEUP'])
jobproperties.Digitization.numberOfCavern       = 0
jobproperties.Digitization.initialBunchCrossing = -0
jobproperties.Digitization.finalBunchCrossing   = 0
jobproperties.Digitization.numberOfBeamHalo     = 0.0
jobproperties.Digitization.numberOfBeamGas      = 0.00
jobproperties.Digitization.overrideMetadata     = ['SimulatedDetectors']

# construct list of input pileup files
cols = []
pileuplist = open(os.environ['INPILEUPLIST'])
for line in pileuplist:
  cols += [ line.lstrip().rstrip() ]

# then we randomize the list
import random
random.shuffle(cols)
jobproperties.Digitization.minBiasInputCols     = cols

#setting the pilup seed
from Digitization.DigitizationFlags import jobproperties
jobproperties.Digitization.rndmSeedOffset1=int(os.environ['SEEDNUMBER1'])
jobproperties.Digitization.rndmSeedOffset2=int(os.environ['SEEDNUMBER2'])

theApp.OutputLevel = INFO

#from Digitization.DigitizationFlags import jobproperties 
#jobproperties.Digitization.doInDetNoise=False

include ( "Digitization/Digitization.py" )

include("InDetSLHC_Example/postInclude.SLHC_Digitization.py")

MessageSvc = Service( "MessageSvc" )
#increase the number of letter reserved to the alg/tool name from 18 to 30
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message per alg
MessageSvc.defaultLimit = 9999999  # all messages
# Set output level threshold among DEBUG, INFO, WARNING, ERROR, FATAL 
MessageSvc.OutputLevel = WARNING

# SLHC Flags -----------------------------------------------------
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version = os.environ['LAYOUT']

#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()
