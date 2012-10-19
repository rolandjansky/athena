# jobOption fragment for Truth tau creator and Tau Track Efficiency Alg

# make sure the THistSvc is setup correctly, when including this fragment
# use something similar to the following lines:
##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
#from GaudiSvc.GaudiSvcConf import THistSvc
#ServiceMgr += THistSvc()
#ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='TauIdAnalysis.aan.root' OPT='RECREATE'" ]
#from AnalysisTools.AnalysisToolsConf import AANTupleStream
#topSequence += AANTupleStream()
#topSequence.AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
#topSequence.AANTupleStream.OutputName = 'TauIdAnalysis.aan.root'
#topSequence.AANTupleStream.WriteInputDataHeader = True
#topSequence.AANTupleStream.OutputLevel = WARNING

###############################
# TauTrackTools options:

# Truth tau creator:
from TauTrackTools.TauTrackToolsConf import TauID__TruthTauCreatorAlg
truthTauCreatorAlg = TauID__TruthTauCreatorAlg()
topSequence += truthTauCreatorAlg
#truthTauCreatorAlg.OutputLevel = INFO
print truthTauCreatorAlg

# Tau Track Efficiency Alg:
from TauTrackTools.TauTrackToolsConf import TauID__TauTrackEfficiencyAlg
tauTrackEffAlg = TauID__TauTrackEfficiencyAlg()
topSequence += tauTrackEffAlg
#tauTrackEffAlg.OutputLevel = INFO
print tauTrackEffAlg

