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

#include <cstdlib>
#include <iostream>
#include <memory>
#include <SampleHandler/fetch.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/SampleGrid.h>
#include <SampleHandler/SampleHandler.h>

//
// method implementations
//

/// effects: add a new grid sample to the sample handler
/// guarantee: strong
/// failures: out of memory II
void addSample (SH::SampleHandler& sh, const std::string& name)
{
  std::auto_ptr<SH::SampleGrid> sample (new SH::SampleGrid (name));
  sample->meta()->setString (SH::MetaFields::gridName, name);
  sample->meta()->setString (SH::MetaFields::gridFilter, SH::MetaFields::gridFilter_default);
  sh.add (sample.release());
}

int main ()
{
  // disabled right now
  return EXIT_SUCCESS;

  if (getenv ("ROOTCORE_AUTO_UT") != 0)
    return EXIT_SUCCESS;

  SH::SampleHandler sh;
  addSample (sh, "mc11_7TeV.107283.AlpgenJimmyWbbFullNp3_pt20.merge.NTUP_TOP.e887_a131_s1353_a133_r2780_p766_tid541230_00");
  addSample (sh, "mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.merge.NTUP_TOP.e1193_s1469_s1470_r3542_r3549_p1104_tid00886942_00");
  addSample (sh, "mc12_8TeV.164440.AlpgenJimmy_AUET2CTEQ6L1_ttbarlnlnNp0_baseline.merge.NTUP_TOP.e1527_s1499_s1504_r3658_r3549_p1294_tid01063260_00");
  fetchMetaData (sh, true);

  for (SH::SampleHandler::iterator sample = sh.begin(),
	 end = sh.end(); sample != end; ++ sample)
  {
    std::cout << (*sample)->name() << std::endl;
    std::cout << "   is data = " << (*sample)->meta()->castDouble (SH::MetaFields::isData, -1) << std::endl;
    std::cout << "   luminosity = " << (*sample)->meta()->castDouble (SH::MetaFields::lumi, -1) << std::endl;
    std::cout << "   cross section = " << (*sample)->meta()->castDouble (SH::MetaFields::crossSection, -1) << std::endl;
    std::cout << "   number of events = " << (*sample)->meta()->castDouble (SH::MetaFields::numEvents, -1) << std::endl;
    std::cout << "   k-factor = " << (*sample)->meta()->castDouble (SH::MetaFields::kfactor, -1) << std::endl;
    std::cout << "   filter efficiency = " << (*sample)->meta()->castDouble (SH::MetaFields::filterEfficiency, -1) << std::endl;
  }
}
