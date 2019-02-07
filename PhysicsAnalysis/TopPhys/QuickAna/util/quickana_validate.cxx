/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

// Suppressing unused typedef warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/program_options.hpp>
#pragma GCC diagnostic pop

#include <EventLoop/DirectDriver.h>
#include <EventLoop/Job.h>
#include <PATInterfaces/CorrectionCode.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/QuickAna.h>
#include <QuickAna/ValidationEL.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TStyle.h>
#include <TSystem.h>
#include <boost/program_options.hpp>
#include <fstream>
#include <xAODRootAccess/Init.h>

namespace po = boost::program_options;

//
// main program
//

int main (int argc, char **argv)
{
  // declare the return type of this function for ANA_CHECK
  ANA_CHECK_SET_TYPE (int);

  // needed for messaging macros
  using namespace ana::msgUserCode;

  // make unchecked status codes abort the job
  ana::enableStatusCodeFailure ();

  // initialize xAOD classes
  ANA_CHECK (xAOD::Init ());


  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("write-summary", "whether to write AthSummary.txt")
    ("submit-dir", po::value<std::string>(), "output file")
    ("el-input-file", po::value<std::vector<std::string>>(), "EventLoop input file")
    ("qa-no-default-def", "whether to turn off default object definitions")
    ("qa-muon-def", po::value<std::string>(), "value of muonDef")
    ("qa-electron-def", po::value<std::string>(), "value of electronDef")
    ("qa-photon-def", po::value<std::string>(), "value of photonDef")
    ("qa-tau-def", po::value<std::string>(), "value of tauDef")
    ("qa-jet-def", po::value<std::string>(), "value of jetDef")
    ("qa-fat-jet-def", po::value<std::string>(), "value of fatJetDef")
    ("qa-met-def", po::value<std::string>(), "value of metDef")
    ("qa-met2-def", po::value<std::string>(), "value of met2Def")
    ("qa-or-def", po::value<std::string>(), "value of orDef")
    ("qa-scheduler", po::value<std::string>(), "QuickAna scheduler");
  po::positional_options_description p;
  p.add("submit-dir", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
	    options(desc).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    return 1;
  }

  if (!vm.count("submit-dir"))
  {
    ATH_MSG_ERROR ("missing submission directory");
    return 1;
  }
  const std::string submitDir = vm["submit-dir"].as<std::string>();

  // create a new QuickAna tool
  ana::Configuration conf;

  // request the actual object definitions to use
  if (!vm.count ("qa-no-default-def"))
  {
    conf.muonDef = "default";
    conf.electronDef = "default";
    // conf.photonDef = "default";
    conf.tauDef = "default";
    conf.jetDef = "default";
    conf.fatJetDef = "default";
    conf.metDef = "default";
    conf.met2Def = "trackmet";
    conf.orDef = "default";
  }

  // Activate EventInfo def for all because crashes otherwise
  conf.eventinfoDef = "default";
  // We just need some files for testing. Do not use these in your analysis
  conf.muMcFiles = {
    //"dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
    //"dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root",
    "dev/SUSYTools/mc16a_defaults_buggy.NotRecommended.prw.root",
  };
  // Using test file from SUSYTools
  conf.muDataFiles = {"dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"};

  if (vm.count ("qa-muon-def"))
    conf.muonDef = vm["qa-muon-def"].as<std::string>();
  if (vm.count ("qa-electron-def"))
    conf.electronDef = vm["qa-electron-def"].as<std::string>();
  if (vm.count ("qa-photon-def"))
    conf.photonDef = vm["qa-photon-def"].as<std::string>();
  if (vm.count ("qa-tau-def"))
    conf.tauDef = vm["qa-tau-def"].as<std::string>();
  if (vm.count ("qa-jet-def"))
    conf.jetDef = vm["qa-jet-def"].as<std::string>();
  if (vm.count ("qa-fat-jet-def"))
    conf.fatJetDef = vm["qa-fat-jet-def"].as<std::string>();
  if (vm.count ("qa-met-def"))
    conf.metDef = vm["qa-met-def"].as<std::string>();
  if (vm.count ("qa-met2-def"))
    conf.met2Def = vm["qa-met2-def"].as<std::string>();
  if (vm.count ("qa-or-def"))
    conf.orDef = vm["qa-or-def"].as<std::string>();

  // set the optimized tool scheduler
  if (vm.count("qa-scheduler"))
    conf.schedulerDef = vm["qa-scheduler"].as<std::string>();

  // create a new sample handler to describe the data files we use
  std::vector<std::string> inputFiles;
  if (vm.count("el-input-file"))
    inputFiles = vm["el-input-file"].as<std::vector<std::string>>();
  if (inputFiles.empty())
  {
    const char *QUICKANA_UT_DATA = getenv ("QUICKANA_UT_DATA");
    if (!QUICKANA_UT_DATA)
      QUICKANA_UT_DATA = "/afs/cern.ch/atlas/project/PAT/xAODs";
    inputFiles.push_back (std::string (QUICKANA_UT_DATA) + "/r5591/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e1727_s1933_s1911_r5591/AOD.01494882._111853.pool.root.1");
  }
  SH::SampleHandler sh;
  std::unique_ptr<SH::SampleLocal> sample (new SH::SampleLocal ("sample"));
  for (auto& file : inputFiles)
    sample->add (file);
  sh.add (sample.release());

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
  std::unique_ptr<ana::ValidationEL> alg (new ana::ValidationEL);
  alg->setConfig (conf);
  alg->m_writeSummary = vm.count ("write-summary");
  job.algsAdd (alg.release());

  // make the driver we want to use:
  // this one works by running the algorithm directly:
  EL::DirectDriver driver;
  // we can use other drivers to run things on the Grid, with PROOF, etc.

  // process the job using the driver
  driver.submit (job, submitDir);

  gStyle->SetOptStat (1111111);

  std::set<std::string> histNames;
  std::unique_ptr<TFile> file (TFile::Open ((submitDir + "/hist-sample.root").c_str()));
  TDirectory *dir = dynamic_cast<TDirectory*>(file->Get ("val"));
  RCU_ASSERT (dir != 0);
  TObject *object = 0;
  for (TIter iter (dir->GetListOfKeys()); (object = iter.Next()); )
  {
    histNames.insert (object->GetName());
  }

  std::string webBase = submitDir + "/user/web";
  gSystem->MakeDirectory ((submitDir + "/user").c_str());
  gSystem->MakeDirectory (webBase.c_str());
  {
    TCanvas canvas;
    std::ofstream file ((webBase + "/index.html").c_str());
    for (auto& name : histNames)
    {
      TH1 *hist = dynamic_cast<TH1*>(dir->Get (name.c_str()));
      RCU_ASSERT (hist != 0);
      hist->Draw ();
      canvas.SetLogy (false);
      canvas.Print ((webBase + "/lin_" + name + ".gif").c_str());
      canvas.SetLogy ();
      canvas.Print ((webBase + "/log_" + name + ".gif").c_str());
      file << "<h1>" << name << "</h1>\n<img src=\"lin_" << name
	   << ".gif\" />\n<img src=\"log_" << name << ".gif\">\n\n";
    }
  }

  return EXIT_SUCCESS;
}
