/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void xExample_run(string submitDir="output") {
   string origDir = submitDir;
   gROOT->ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C");
   xAOD::Init().ignore();
   SH::SampleHandler sh;
   SH::DiskListLocal list ("/afs/cern.ch/atlas/project/PAT/tutorial/cern-oct2014/r5591");
   SH::scanDir (sh,list,"AOD.01494882._113691.pool.root.1" );
   sh.setMetaString ("nc_tree", "CollectionTree");
   sh.print ();
   EL::Job job;
   job.sampleHandler (sh);
   //job.options()->setDouble (EL::Job::optCacheSize, 10*1024*1024);
   xExample *alg = new xExample;
   job.algsAdd (alg);
   EL::DirectDriver driver;
   cerr << "Submitting to " << submitDir << endl;
   driver.submit (job, submitDir);
}
