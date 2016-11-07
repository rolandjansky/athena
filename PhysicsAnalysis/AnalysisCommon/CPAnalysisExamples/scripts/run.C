/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void run(string submitDir="output") {
   string origDir = submitDir;
   gROOT->ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C");
   SH::SampleHandler sh;
   SH::scanDir (sh, "/afs/cern.ch/user/j/jahreda/work/public/ntuples");
   sh.setMetaString ("nc_tree", "physics");
   sh.print ();
   EL::Job job;
   job.useD3PDReader();
   job.sampleHandler (sh);
   job.options()->setDouble (EL::Job::optCacheSize, 10*1024*1024);
   CPAnalysisExample *alg = new CPAnalysisExample;
   job.algsAdd (alg);
   job.options()->setDouble("elePtCut",15*1000);
   job.options()->setDouble("muPtCut",10*1000);
   job.options()->setDouble("metPtCut",30*1000);
   job.options()->setDouble("jetPtCut",25*1000);
   EL::DirectDriver driver;
   Long_t *id,*size,*flags,*mt;
   Int_t version(0);
   while (!gSystem->GetPathInfo(submitDir.c_str(),id,size,flags,mt)) {
      version++;
      submitDir = string(Form("%s_%d",origDir.c_str(),version));
   }
   cerr << "Submitting to " << submitDir << endl;
   driver.submit (job, submitDir);
}
