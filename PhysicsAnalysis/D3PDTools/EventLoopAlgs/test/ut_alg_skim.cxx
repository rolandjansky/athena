/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/Global.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <TSystem.h>
#include <EventLoop/DirectDriver.h>
#include <EventLoop/Job.h>
#include <EventLoop/OutputStream.h>
#include <EventLoopAlgs/AlgSelect.h>
#include <EventLoopAlgs/NTupleSvc.h>
#include <EventLoopAlgs/UnitTestSkim.h>
#include <RootCoreUtils/UnitTestDir.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>

//
// main program
//

using namespace EL;

int main ()
{
  RCU::UnitTestDir dir ("EventLoopAlgs", "AlgSkim");

  try
  {
    DirectDriver driver;
    SH::SampleHandler sh;

    std::auto_ptr<SH::SampleLocal> mysample;
    mysample.reset (new SH::SampleLocal ("dataset"));
    for (unsigned iter = 0, end = 3; iter != end; ++ iter)
    {
      std::ostringstream path;
      path << "$ROOTCOREBIN/data/EventLoop/test_ntuple" << iter << ".root";
      TString input = path.str();
      gSystem->ExpandPathName (input);
      mysample->add (input.Data());
    }
    mysample->meta()->setString ("nc_tree", "physics");
    sh.add (mysample.release());

    EL::Job job;
    job.sampleHandler (sh);
    job.outputAdd (OutputStream ("output"));
    std::auto_ptr<NTupleSvc> skim (new NTupleSvc ("output"));
    skim->copyBranch (".*");
    job.algsAdd (skim.release());
    std::auto_ptr<AlgSelect> select (new AlgSelect ("output"));
    select->addCut ("el_n%2==0");
    select->histName ("cut_flow");
    job.algsAdd (select.release());
    job.algsAdd (new UnitTestSkim);

    driver.submit (job, dir.path() + "/submit");
    return EXIT_SUCCESS;
  } catch (std::exception& e)
  {
    std::cout << "exception caught in unit test: " << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (std::string& e)
  {
    std::cout << "exception caught in unit test: " << e << std::endl;
    return EXIT_FAILURE;
  } catch (...)
  {
    std::cout << "unknown exception caught in unit test" << std::endl;
    return EXIT_FAILURE;
  }
}
