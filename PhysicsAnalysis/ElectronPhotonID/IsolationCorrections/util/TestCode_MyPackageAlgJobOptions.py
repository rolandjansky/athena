#Skeleton joboption for a simple analysis job
import AthenaRootComps.ReadAthenaxAODHybrid

#---- Minimal job options -----

jps.AthenaCommonFlags.AccessMode = "ClassAccess"              #Choose from TreeAccess,BranchAccess,ClassAccess,AthenaAccess,POOLAccess
#jps.AthenaCommonFlags.TreeName = "MyTree"                    #when using TreeAccess, must specify the input tree name

jps.AthenaCommonFlags.HistOutputs = ["MYSTREAM:myfile.root"]  #register output files like this. MYSTREAM is used in the code

athAlgSeq += CfgMgr.MyPackageAlg()                               #adds an instance of your alg to the main alg sequence


#---- Options you could specify on command line -----
jps.AthenaCommonFlags.EvtMax=20                          #set on command-line with: --evtMax=-1
jps.AthenaCommonFlags.SkipEvents=0                       #set on command-line with: --skipEvents=0
jps.AthenaCommonFlags.FilesInput = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/DAOD_PHYSVAL/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.DAOD_PHYSVAL.e5458_s3126_r9364_r9315_AthDerivation-21.2.1.0.root"]        #set on command-line with: --filesInput=...


include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above

