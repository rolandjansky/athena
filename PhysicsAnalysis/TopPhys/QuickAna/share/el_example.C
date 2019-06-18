/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void el_example (const std::string& submitDir, const std::string& inputFilePath)
{
  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // create a new sample handler to describe the data files we use
  SH::SampleHandler sh;

  // scan for datasets in the given directory
  // this works if you are on lxplus, otherwise you'd want to copy over files
  // to your local machine and use a local path.  if you do so, make sure
  // that you copy all subdirectories and point this to the directory
  // containing all the files, not the subdirectories.

  // MC single file:
  SH::DiskListLocal list (inputFilePath);
  SH::scanSingleDir (sh, "sample", list);

  // set the name of the tree in our files
  // in the xAOD the TTree containing the EDM containers is "CollectionTree"
  sh.setMetaString ("nc_tree", "CollectionTree");

  // further sample handler configuration may go here

  // print out the samples we found
  sh.print ();

  // this is the basic description of our job
  EL::Job job;
  job.sampleHandler (sh);
  job.options()->setString (EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_athena);
  job.options()->setDouble (EL::Job::optMaxEvents, 100);

  // add our algorithm to the job
  ana::ELExample *alg = new ana::ELExample;
  alg->eventinfoDef = "default";
  alg->electronDef = "default";
  alg->muonDef = "default";
  alg->tauDef = "default";
  alg->jetDef = "default";

  // We just need some files for testing. Do not use these in your analysis
  alg->muMcFiles = {
    "dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
    "dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root",
  };
  // Using test file from SUSYTools
  alg->muDataFiles = {"dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"};

  job.algsAdd (alg);

  // make the driver we want to use:
  // this one works by running the algorithm directly:
  EL::DirectDriver driver;
  // we can use other drivers to run things on the Grid, with PROOF, etc.

  // process the job using the driver
  driver.submit (job, submitDir);
}
