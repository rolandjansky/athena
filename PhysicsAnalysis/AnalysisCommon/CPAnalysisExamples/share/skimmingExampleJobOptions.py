#Author: Will Buttinger

#This joboption shows a few examples of how you could define an event selection at the joboption level. 
#You can use these techniques in, for example, the skimxAODJobOptions.py 

theApp.EvtMax=10
import AthenaPoolCnvSvc.ReadAthenaPool
myfile = os.environ.get("ASG_TEST_FILE_MC", "/afs/cern.ch/user/a/asgbase/patspace/xAODs/r6630/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.AOD.e3698_s2608_s2183_r6630_tid05352803_00/AOD.05352803._000242.pool.root.1")
svcMgr.EventSelector.InputCollections = [myfile] 
algseq = CfgMgr.AthSequencer("AthAlgSeq")
include("AthAnalysisBaseComps/SuppressLogging.py") #optional to suppress amount of output seen from core services

#1. Using an existing selector algorithm
#Here we use the PyEvtFilter, which is a python-based selection algorithm

from GaudiSequencer.PyComps import PyEvtFilter
algseq +=  PyEvtFilter("MyFilter", evt_list=[(222510,187960),(222510,185506)])

#2. Using the expression evaluation code
#Here we will use the CutTool tool, which is effectively an interface to the ExpressionEvaluation code for performing relatively generic selections
#A tool needs to be executed by an algorithm in order to run. We need an algorithm which knows how to execute the CutTool, which implements the ISkimmingTool interface. the DerivationFramework__DerivationKernel is one such algorithm, but there is also the dedicated CutAlg class. We'll use the DerivationKernel here, because it can easily be extended with multiple cut tools.

ToolSvc += CfgMgr.CutTool("MyCutTool",Cut="EventInfo.eventTypeBitmask==1 && count( (Electrons.pt>20*GeV) && (Electrons.Medium) ) >= 1")
algseq += CfgMgr.DerivationFramework__DerivationKernel("MyKernel",SkimmingTools=[ToolSvc.MyCutTool])


#3. Write your own in-situ python algorithm
# You can write a mini python athena algorithm right in the joboptions, and use that
# Here is an alg that selects events with at least 3 electrons

import AthenaPython.PyAthena as PyAthena
class MyAlg (PyAthena.Alg):
    def execute(self): 
        self.setFilterPassed(False) #this is the method that algorithms must use to signal if they have 'passed' or not, if they behave as skimmers/filters
        print "runNumber = %d , eventNumber = %d\n" % (self.evtStore["EventInfo"].runNumber(),self.evtStore["EventInfo"].eventNumber())
        if( self.evtStore["Electrons"].size()>2 ): self.setFilterPassed(True)
        return PyAthena.StatusCode.Success


algseq += MyAlg("MyAlg")


 

