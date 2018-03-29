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
#include <SampleHandler/MetaDataQuery.h>
#include <SampleHandler/fetch.h>

//
// method implementations
//

using namespace SH;

int main ()
{
  // disabled right now
  return EXIT_SUCCESS;


  if (getenv ("ROOTCORE_AUTO_UT") != 0)
    return EXIT_SUCCESS;

  MetaDataQuery query;
  query.samples.push_back (MetaDataSample ("mc11_7TeV.107283.AlpgenJimmyWbbFullNp3_pt20.merge.NTUP_TOP.e887_a131_s1353_a133_r2780_p766_tid541230_00"));
  query.samples.push_back (MetaDataSample ("mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.merge.NTUP_TOP.e1193_s1469_s1470_r3542_r3549_p1104_tid00886942_00"));
  query.samples.push_back (MetaDataSample ("mc12_8TeV.164440.AlpgenJimmy_AUET2CTEQ6L1_ttbarlnlnNp0_baseline.merge.NTUP_TOP.e1527_s1499_s1504_r3658_r3549_p1294_tid01063260_00"));
  fetchMetaData (query);

  std::cout << query.messages << std::endl;
  for (std::size_t iter = 0, end = query.samples.size(); iter != end; ++ iter)
  {
    std::cout << query.samples[iter].name << std::endl;
    if (!query.samples[iter].source.empty())
      std::cout << "   source=" << query.samples[iter].source << std::endl;
    else
      std::cout << "   no source given" << std::endl;
    if (!query.samples[iter].warning.empty())
      std::cout << "   warning=" << query.samples[iter].warning << std::endl;
    std::cout << "   unknown=" << query.samples[iter].unknown << std::endl;
    if (query.samples[iter].isData != -1)
      std::cout << "   is-data=" << query.samples[iter].isData << std::endl;
    else
      std::cout << "   is-data not given" << std::endl;
    if (query.samples[iter].luminosity != -1)
      std::cout << "   luminosity=" << query.samples[iter].luminosity << std::endl;
    else
      std::cout << "   no luminosity given" << std::endl;
    if (query.samples[iter].crossSection != -1)
      std::cout << "   cross section=" << query.samples[iter].crossSection << std::endl;
    else
      std::cout << "   no cross section given" << std::endl;
    if (query.samples[iter].nevents != -1)
      std::cout << "   number of events=" << query.samples[iter].nevents << std::endl;
    else
      std::cout << "   no number of events given" << std::endl;
    if (query.samples[iter].kfactor != -1)
      std::cout << "   k-factor=" << query.samples[iter].kfactor << std::endl;
    else
      std::cout << "   no k-factor given" << std::endl;
    if (query.samples[iter].filterEfficiency != -1)
      std::cout << "   filter efficiency=" << query.samples[iter].filterEfficiency << std::endl;
    else
      std::cout << "   no filter efficiency given" << std::endl;
  }
}
