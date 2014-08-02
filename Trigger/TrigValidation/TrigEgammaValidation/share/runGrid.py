
class Algorithm:
  def __init__(self, myname = "ZeeValidation"):
    self.name = myname;
    self.package = "TrigEgammaValidation";
    self.addTrigDecMaker = False;
    self.addTrigDecTool = False;
  
  def extra(self, file):
    pass;

  def generate(self, file):
    file.write("from %s.%sConf import %s\n" % (self.package, self.package, self.name));
    file.write("%sInstance = %s()\n" % (self.name, self.name));
    file.write("topSequence += %sInstance\n" % self.name);
    if (self.addTrigDecMaker == True):
      file.write("""
from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionMaker
topSequence += TrigDecisionMaker()
topSequence.TrigDecMaker.doL1 = True
topSequence.TrigDecMaker.doL2 = True
topSequence.TrigDecMaker.doEF = True
""");
    if (self.addTrigDecTool == True):
      file.write("""
from TrigDecision.TrigDecisionConf import TrigDec__TrigDecisionTool
tdt = TrigDec__TrigDecisionTool()
ToolSvc += tdt
""");
    self.extra(file);
    file.write("\n");
    
    
class AlgorithmNavSE(Algorithm):
  def __init__(self):
    Algorithm.__init__(self, "NavSEValidation");
    self.addTrigDecMaker = True;
    self.addTrigDecTool = True;

  def extra(self, file):
    file.write("""
topSequence.NavSEValidation.Chains = ['eNoCut', 'em15_passHLT', 'e10Ringer', 'em15i_passHLT', '2em15_passHLT', 'em20_passHLT', 'em20i_passHLT', '2em20_passHLT', 'em105_passHLT', '2e5_medium', '2e5_medium1', '2e6_medium', '2e6_medium1', 'e5_e7_medium', 'e5_e10_medium', 'e5_medium', 'e5_medium1', 'e6_medium', 'e6_medium1', 'e7_medium', 'e10_loose', 'e10_medium', '2e10_loose', 'e10_loose_passL2', 'e10_loose_passEF', 'e12_medium', 'e15_medium', 'e15_loose', 'e15_medium_passL2', 'e15_medium_passEF', 'e15_loose_passL2', 'e15_loose_passEF', 'e15i_medium', 'e15i_loose', 'e15i_medium_passL2', 'e15i_medium_passEF', '2e15_medium', '2e15i_medium', '2e15i_medium_passL2', '2e15i_medium_passEF', 'e17i_medium', 'e20_loose', 'e20_loose_passL2', 'e20_loose_passEF', 'e20_medium1', 'e20_medium1_passL2', 'e20_medium1_passEF', '2e20_loose', '2e20_loose_passL2', '2e20_loose_passEF', 'e20i_loose', 'e20i_loose_passL2', 'e20i_loose_passEF', 'e20i_medium1', 'e20i_medium1_passL2', 'e20i_medium1_passEF', '2e20i_loose', '2e20i_loose_passL2', '2e20i_loose_passEF', 'e25_loose', 'e25_medium1', 'e25i_loose', 'e25i_loose_passL2', 'e25i_loose_passEF', 'e25i_medium1', 'e25i_medium1_passL2', 'e25i_medium1_passEF', '2e25i_loose', '2e25i_loose_passL2', '2e25i_loose_passEF', 'e105_loose1', 'e140_loose1', '2e12_tight', 'e22i_tight', 'e55_loose1' ]
topSequence.NavSEValidation.TrigDecisionTool = tdt
""");

class AlgorithmNavOffline(Algorithm):
  def __init__(self):
    Algorithm.__init__(self, "NavOfflineValidation");
    self.addTrigDecMaker = True;
    self.addTrigDecTool = True;

  def extra(self, file):
    file.write("""
topSequence.NavOfflineValidation.Chains = ['eNoCut',  'em15_passHLT', 'e10Ringer', 'em15i_passHLT', '2em15_passHLT', 'em20_passHLT', 'em20i_passHLT', '2em20_passHLT', 'em105_passHLT', '2e5_medium', '2e5_medium1', '2e6_medium', '2e6_medium1', 'e5_e7_medium','e5_e10_medium', 'e5_medium', 'e5_medium1', 'e6_medium', 'e6_medium1', 'e7_medium', 'e10_loose', 'e10_medium', '2e10_loose', 'e10_loose_passL2', 'e10_loose_passEF', 'e12_medium', 'e15_medium', 'e15_loose', 'e15_medium_passL2', 'e15_medium_passEF', 'e15_loose_passL2', 'e15_loose_passEF', 'e15i_medium', 'e15i_loose', 'e15i_medium_passL2', 'e15i_medium_passEF', '2e15_medium', '2e15i_medium', '2e15i_medium_passL2', '2e15i_medium_passEF', 'e17i_medium', 'e20_loose', 'e20_loose_passL2', 'e20_loose_passEF', 'e20_medium1', 'e20_medium1_passL2', 'e20_medium1_passEF', '2e20_loose', '2e20_loose_passL2', '2e20_loose_passEF', 'e20i_loose', 'e20i_loose_passL2', 'e20i_loose_passEF', 'e20i_medium1', 'e20i_medium1_passL2', 'e20i_medium1_passEF', '2e20i_loose', '2e20i_loose_passL2', '2e20i_loose_passEF', 'e25_loose', 'e25_medium1', 'e25i_loose', 'e25i_loose_passL2', 'e25i_loose_passEF', 'e25i_medium1', 'e25i_medium1_passL2', 'e25i_medium1_passEF', '2e25i_loose', '2e25i_loose_passL2', '2e25i_loose_passEF', 'e105_loose1', 'e140_loose1', '2e12_tight', 'e22i_tight', 'e55_loose1' ]
topSequence.NavOfflineValidation.TrigDecisionTool = tdt
""");

class Job:
  def __init__(self):
    # To be overwritten
    self.sourceType = "RDO";
    self.GeoAtlas = "ATLAS-GEO-01-01-00";
    self.CondTag = "OFLCOND-SIM-00-00-00";
    self.source = "valid1.005144.PythiaZee.digit.RDO.e322_s454";
    self.site = "ANALY_LONG_BNL_ATLAS";
    self.algos = [Algorithm("ZeeValidation"), Algorithm("ZeeOfflineValidation")];
    self.runoffline = True;
    
  def generate(self, file):
    file.write("from AthenaCommon.AlgSequence import AlgSequence\n");
    file.write("topSequence = AlgSequence()\n");
    file.write("WhichInput = \"%s\"\n" % self.sourceType);
    if (self.runoffline == False):
      file.write("RunOffline = False\n");
    else:
      file.write("RunOffline = True\n");
    file.write("DetDescrVersion = \"%s\"\n" % self.GeoAtlas);
    file.write("EvtMax = -1\n");
    file.write("OutputLevel = INFO\n");
    file.write("include(\"TrigEgammaValidation/generalJobOption.py\")\n");
    file.write("theApp.serviceMgr().IOVDbSvc.GlobalTag = \"%s\"\n" % self.CondTag);
    for a in self.algos:
      a.generate(file);
    file.write("from GaudiSvc.GaudiSvcConf import THistSvc\n");
    file.write("svcMgr += THistSvc()\n");
    file.write("svcMgr.THistSvc.Output = [ \"Validation DATAFILE='Validation.root' OPT='RECREATE'\" ]\n");
    file.write("\n");
    pass;

  def getClassName(self):
    return self.__class__.__name__

class JobSE(Job):
  def __init__(self):
    Job.__init__(self)
    #self.GeoAtlas = "ATLAS-CSC-05-00-00";
    self.GeoAtlas = "ATLAS-GEO-02-01-00";
    #self.CondTag = "OFLCOND-CSC-01-00-00";
    self.CondTag = "OFLCOND-SIM-00-00-06";
    #self.source = "mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439";
    self.source = "mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s462_d153_tid044046";
    self.algos = [Algorithm("SingleElectronValidation"), Algorithm("SingleElectronOfflineValidation")];
    self.sourceType = "RDO"
    
class JobZee(Job):
  def __init__(self):
    Job.__init__(self)
    self.GeoAtlas = "ATLAS-CSC-01-02-00";
    self.CondTag = "OFLCOND-CSC-00-01-00";
    #self.source = "mc08.005144.PythiaZee.digit.RDO.e323_s400_d99";
    #self.source = "misal1_csc11.005144.PythiaZee.digit.RDO.v12003106";
    self.source = "misal1_mc12.005144.PythiaZee.digit.RDO.v12000801";
    #self.source = "mc08.005144.PythiaZee.digit.RDO.e323_s400_d99_tid021820";
    self.site = "AUTO";
    self.algos = [Algorithm("ZeeValidation"), Algorithm("ZeeOfflineValidation")];
    self.sourceType = "RDO"
    
class JobHgg(Job):
  def __init__(self):
    Job.__init__(self)
    self.GeoAtlas = "ATLAS-GEO-02-00-00";
    self.CondTag = "OFLCOND-SIM-00-00-00";
    #self.source = "valid3.006384.PythiaH120gamgam.digit.RDO.e322_s461";
    self.source = "valid1.006384.PythiaH120gamgam.digit.RDO.e322_s480";
    self.algos = [Algorithm("HggValidation"), Algorithm("HggOfflineValidation")];
    self.sourceType = "RDO"

class JobJet(Job):
  def __init__(self):
    Job.__init__(self)
    self.GeoAtlas = "ATLAS-CSC-01-02-00";
    self.CondTag = "OFLCOND-CSC-00-01-00";
    self.source = "misal1_mc12.005802.JF17_pythia_jet_filter.digit.RDO.v12003105";
    self.algos = [Algorithm("JetValidation")];
    self.sourceType = "RDO"

class JobNavSE(Job):
  def __init__(self):
    Job.__init__(self)
    self.GeoAtlas = "ATLAS-CSC-05-00-00";
    self.CondTag = "OFLCOND-CSC-01-00-00";
    self.source = "mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439";
    self.algos = [AlgorithmNavSE(), AlgorithmNavOffline()];
    self.sourceType = "RDO"
    
import os;

# The only lines one shall ever change!
myjob = [JobSE(), JobZee(), JobHgg(), JobJet(), JobNavSE()];
user = "DaniloEnoqueFerreiraDeLima";
tmpDir = "/rscratch/pcatr16/dferreir/AutoTEV/";
outDir = os.environ["PWD"]+"/output";
refDir = "/afs/cern.ch/user/d/dferreir/public/references_grid/";


thisDir = os.environ["PWD"];

import time;
todaytimestamp = time.time();


file = [False for i in myjob];
seq = [False for i in myjob];
for i in range(0, len(myjob)):
  seq[i] = open("seq_"+str(i)+".txt", 'w');
  file[i] = open("myTopOptions_"+str(i)+".py", 'w');
  myjob[i].generate(file[i]);
  file[i].close();
  seq[i].write("""
### Step1
pathena --inDS %s --outDS user09.%s.%s.TrigEgammaValidation.v%s myTopOptions_%s.py --site %s --long --nFilesPerJob 5 --nFiles 10000

### Step2
cd %s
rm -rf *.root
source /afs/cern.ch/atlas/offline/external/GRID/ddm/DQ2Clients/setup.sh
dq2-get -f "*.root*" user09.%s.%s.TrigEgammaValidation.v%s

### Step3
cd %s
rm -rf %s_%s
mkdir %s_%s
cd %s_%s
cp %s/ReFinalize.C %s/ProcessValidation.C %s/CKolmogorov.C %s/CompareFilesKolmogorov.C .
rm -f tmp.C
touch tmp.C
echo "{" >> tmp.C
echo "  gROOT->ProcessLine(\\".L ReFinalize.C+\\");" >> tmp.C
echo "  ReFinalize(\\"Validation_%s.root\\", \\"%s/user09.%s.%s.TrigEgammaValidation.v%s/\\");" >> tmp.C
echo "  gROOT->ProcessLine(\\".q\\");" >> tmp.C
echo "}" >> tmp.C
root -l -b tmp.C

### Step4
cd %s/%s_%s
rm -f tmp.C
touch tmp.C
echo "{" >> tmp.C
echo "  gROOT->ProcessLine(\\".L ProcessValidation.C+\\");" >> tmp.C
echo "  ProcessValidationNew(\\"Validation_%s.root\\", \\"%s/%s_%s.root\\");" >> tmp.C
echo "  gROOT->ProcessLine(\\".q\\");" >> tmp.C
echo "}" >> tmp.C
root -l -b tmp.C

### Step5
cd %s/%s_%s
rm -f tmp.C
touch tmp.C
echo "{" >> tmp.C
echo "  gROOT->ProcessLine(\\".L CKolmogorov.C+\\");" >> tmp.C
echo "  TestIt(\\"Validation_%s.root\\", \\"%s/%s_%s.root\\", \\"results.txt\\");" >> tmp.C
echo "  gROOT->ProcessLine(\\".q\\");" >> tmp.C
echo "}" >> tmp.C
root -l -b tmp.C

### Sequence
nTry = 5
while nTry > 0:
  Step1.execute()
  result = Step1.result()
  if ((result.status == 0) and (result.Failed == 0)):
    break
  nTry -= 1

result = Step1.result()
if (result.status == 0):
  Step2.execute()
  result = Step2.result()
  if (result.status == 0):
    Step3.execute()
    result = Step3.result()
    if (result.status == 0):
      Step4.execute()
      result = Step4.result()
      if (result.status == 0):
        Step5.execute()
        result = Step5.result()
	
""" % (myjob[i].source, user, myjob[i].getClassName(), todaytimestamp, str(i), myjob[i].site,
       tmpDir, user, myjob[i].getClassName(), todaytimestamp,
       outDir, myjob[i].source,myjob[i].getClassName(), myjob[i].source, myjob[i].getClassName(), myjob[i].source,myjob[i].getClassName(), thisDir, thisDir, thisDir, thisDir, str(i), tmpDir, user, myjob[i].getClassName(), todaytimestamp,
       outDir, myjob[i].source,myjob[i].getClassName(), str(i), refDir, myjob[i].source, myjob[i].getClassName(),
       outDir, myjob[i].source, myjob[i].getClassName(), str(i), refDir, myjob[i].source, myjob[i].getClassName()));

print "Now run \"psequencer seq_i.txt\" for all i"


