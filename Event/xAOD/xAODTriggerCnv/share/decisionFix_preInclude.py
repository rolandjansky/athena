# $Id: decisionFix_preInclude.py 626691 2014-11-06 16:27:15Z krasznaa $
#
# This preInclude can be used with transformation jobs that need to
# fix the xAOD::TrigDecision payload in DC14 files that hold corrupt
# objects.
#

# Access the main algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Create the xAOD::TrigDecision object from the TrigDecision one:
from xAODTriggerCnv.xAODTriggerCnvConf import xAODMaker__TrigDecisionCnvAlg
topSequence += xAODMaker__TrigDecisionCnvAlg( "TrigDecisionCnvAlg" )
topSequence.TrigDecisionCnvAlg.xAODKey = "xTrigDecision"
