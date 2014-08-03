
# Python job to configure JetTagAna
# Laurent Vacavant 2005/10/12

if not "AnalysisExamples" in theApp.Dlls:
	theApp.Dlls += [ "AnalysisExamples" ]

from AnalysisExamples.AnalysisExamplesConf import JetTagAna
MyJetTagAna = JetTagAna(
	name="MyJetTagAna",
        #SelectBFromRun = run_number # use only u-jets from this run
        #SelectUFromRun = run_number # use only u-jets from this run
	OutputLevel=INFO)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += MyJetTagAna
		
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["fileJetTagAna DATAFILE='tagana.root' OPT='RECREATE'"]
