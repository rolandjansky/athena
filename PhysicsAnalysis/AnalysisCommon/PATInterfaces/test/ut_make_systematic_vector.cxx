/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#undef NDEBUG

#include <PATInterfaces/MakeSystematicsVector.h>

#include <RootCoreUtils/Assert.h>

#include <iostream>

using namespace CP;

//
// method implementations
//

int main ()
{
  {
    MakeSystematicsVector sys;

    SystematicSet sysList;
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 1);
    RCU_ASSERT (sys.result("")[0] == SystematicSet ());
  }
  {
    MakeSystematicsVector sys;

    SystematicSet sysList;
    sysList.insert (SystematicVariation ("MYSYS", SystematicVariation::CONTINUOUS));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 3);
    RCU_ASSERT (sys.result("")[0] == SystematicSet ());
    SystematicSet set1;
    set1.insert (SystematicVariation ("MYSYS", 1));
    RCU_ASSERT (sys.result("")[1] == set1);
    SystematicSet set2;
    set2.insert (SystematicVariation ("MYSYS", -1));
    RCU_ASSERT (sys.result("")[2] == set2);
  }
  {
    MakeSystematicsVector sys;

    SystematicSet sysList;
    sysList.insert (SystematicVariation ("MYSYS", 1));
    sysList.insert (SystematicVariation ("MYSYS", -1));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 3);
    RCU_ASSERT (sys.result("")[0] == SystematicSet ());
    RCU_ASSERT (!(sys.result("")[1] == sys.result("")[2]));

    SystematicSet set1;
    set1.insert (SystematicVariation ("MYSYS", 1));
    RCU_ASSERT (sys.result("")[1] == set1 || sys.result("")[2] == set1);
    SystematicSet set2;
    set2.insert (SystematicVariation ("MYSYS", -1));
    RCU_ASSERT (sys.result("")[1] == set2 || sys.result("")[2] == set2);
  }
  {
    MakeSystematicsVector sys;

    SystematicSet sysList;
    sysList.insert (SystematicVariation ("MYSYS"));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 2);
    RCU_ASSERT (sys.result("")[0] == SystematicSet ());

    SystematicSet set1;
    set1.insert (SystematicVariation ("MYSYS"));
    RCU_ASSERT (sys.result("")[1] == set1);
  }
  {
    MakeSystematicsVector sys;
    sys.addGroup ("second");
    sys.setPattern ("MYSYS");

    SystematicSet sysList;
    sysList.insert (SystematicVariation ("MYSYS", SystematicVariation::CONTINUOUS));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 1);
    RCU_ASSERT (sys.result("second").size() == 2);
    RCU_ASSERT (sys.result("")[0] == SystematicSet ());
    SystematicSet set1;
    set1.insert (SystematicVariation ("MYSYS", 1));
    RCU_ASSERT (sys.result("second")[0] == set1);
    SystematicSet set2;
    set2.insert (SystematicVariation ("MYSYS", -1));
    RCU_ASSERT (sys.result("second")[1] == set2);
  }
  {
    MakeSystematicsVector sys;
    sys.addGroup ("");
    sys.setPattern ("MYSYS");

    SystematicSet sysList;
    sysList.insert (SystematicVariation ("MYSYS", SystematicVariation::CONTINUOUS));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 3);
    RCU_ASSERT (sys.result("")[0] == SystematicSet ());
    SystematicSet set1;
    set1.insert (SystematicVariation ("MYSYS", 1));
    RCU_ASSERT (sys.result("")[1] == set1);
    SystematicSet set2;
    set2.insert (SystematicVariation ("MYSYS", -1));
    RCU_ASSERT (sys.result("")[2] == set2);
  }
  {
    MakeSystematicsVector sys;
    sys.addGroup ("second");
    sys.setPattern ("MYSYS");
    sys.addGroup ("third");
    sys.setPattern ("MYSYS");

    SystematicSet sysList;
    sysList.insert (SystematicVariation ("MYSYS", SystematicVariation::CONTINUOUS));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 1);
    RCU_ASSERT (sys.result("second").size() == 0);
    RCU_ASSERT (sys.result("third").size() == 2);
  }
  {
    MakeSystematicsVector sys;
    sys.addGroup ("second");
    sys.setPattern ("MYSY.*");

    SystematicSet sysList;
    sysList.insert (SystematicVariation ("MYSYS", SystematicVariation::CONTINUOUS));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 1);
    RCU_ASSERT (sys.result("second").size() == 2);
  }
  {
    MakeSystematicsVector sys;
    sys.addGroup ("second");
    sys.setPattern ("MYSY");

    SystematicSet sysList;
    sysList.insert (SystematicVariation ("MYSYS", SystematicVariation::CONTINUOUS));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 3);
    RCU_ASSERT (sys.result("second").size() == 0);
  }
  {
    MakeSystematicsVector sys;
    sys.setSigma (5);

    SystematicSet sysList;
    sysList.insert (SystematicVariation ("MYSYS", SystematicVariation::CONTINUOUS));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 3);
    RCU_ASSERT (sys.result("")[0] == SystematicSet ());
    SystematicSet set1;
    set1.insert (SystematicVariation ("MYSYS", 5));
    RCU_ASSERT (sys.result("")[1] == set1);
    SystematicSet set2;
    set2.insert (SystematicVariation ("MYSYS", -5));
    RCU_ASSERT (sys.result("")[2] == set2);
  }
  {
    MakeSystematicsVector sys;
    sys.addGroup ("second");
    sys.setPattern ("MYSYS.*");
    sys.setToys (5);

    SystematicSet sysList;
    sysList.insert (SystematicVariation ("MYSYS1", SystematicVariation::CONTINUOUS));
    sysList.insert (SystematicVariation ("MYSYS2", SystematicVariation::CONTINUOUS));
    sysList.insert (SystematicVariation ("MYSYS3", SystematicVariation::CONTINUOUS));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 1);
    RCU_ASSERT (sys.result("second").size() == 5);
    for (auto& mysys : sys.result("second"))
      RCU_ASSERT (mysys.size() == 3);

    // these are the systematics I got on my home machine, they should
    // be the same everywhere though.
    std::vector<std::string> names1 ({ "MYSYS1__0down0232", "MYSYS1__1up6983", "MYSYS1__0down3621", "MYSYS1__0down2449", "MYSYS1__0up0397" });
    std::vector<std::string> names2 ({ "MYSYS2__0down8379", "MYSYS2__0down0795", "MYSYS2__2down1523", "MYSYS2__1up0418", "MYSYS2__0up4289" });
    std::vector<std::string> names3 ({ "MYSYS3__1up2419", "MYSYS3__1up4446", "MYSYS3__1up7961", "MYSYS3__1up4007", "MYSYS3__0down301" });

    for (unsigned index = 0; index != 5; ++ index)
    {
      for (auto& mysys : sys.result("second")[index])
      {
	// std::cout << mysys.name() << std::endl;
	std::string myname;
	if (mysys.basename() == "MYSYS1")
	  myname = names1.at(index);
	else if (mysys.basename() == "MYSYS2")
	  myname = names2.at(index);
	else if (mysys.basename() == "MYSYS3")
	  myname = names3.at(index);
	else
	  RCU_ASSERT0 (("extra systematic: " + mysys.name()).c_str());

	if (myname != mysys.name())
	  RCU_ASSERT0 (("expected systematic " + myname + " but found " + mysys.name()).c_str());
      }
    }
  }
  {
    MakeSystematicsVector sys;
    sys.addGroup ("second");
    sys.useForNominal ();

    SystematicSet sysList;
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 0);
    RCU_ASSERT (sys.result("second").size() == 1);
  }

  {
    MakeSystematicsVector sys;
    sys.addGroup ("second");
    sys.setToys (5);

    SystematicSet sysList;
    sysList.insert (SystematicVariation::makeContinuousEnsemble ("test"));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 3);
    RCU_ASSERT (sys.result("second").size() == 0);
  }
  {
    MakeSystematicsVector sys;
    sys.addGroup ("second");
    sys.setToys (5);

    SystematicSet sysList;
    sysList.insert (SystematicVariation::makeToyEnsemble ("test"));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 1);
    RCU_ASSERT (sys.result("second").size() == 5);
  }
  {
    MakeSystematicsVector sys;
    sys.addGroup ("second");
    sys.setPattern (".*");
    sys.setToys (5);

    SystematicSet sysList;
    sysList.insert (SystematicVariation::makeToyEnsemble ("test"));
    sysList.insert (SystematicVariation::makeToyEnsemble ("test2"));
    sysList.insert (SystematicVariation::makeContinuousEnsemble ("test3"));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 1);
    RCU_ASSERT (sys.result("second").size() == 5);
    RCU_ASSERT (sys.result("second")[0].size() == 3);
  }
  {
    MakeSystematicsVector sys;
    sys.setToys (5);
    sys.setSigma (2);
    sys.addGroup ("second");
    sys.useForNominal ();

    SystematicSet sysList;
    sysList.insert (SystematicVariation::makeToyEnsemble ("test"));
    sys.calc (sysList);

    RCU_ASSERT (sys.result("").size() == 5);
    RCU_ASSERT (sys.result("")[0].size() == 1);
    RCU_ASSERT (*sys.result("")[0].begin() == SystematicVariation::makeToyVariation ("test", 1, 2));
    RCU_ASSERT (sys.result("")[1].size() == 1);
    RCU_ASSERT (*sys.result("")[1].begin() == SystematicVariation::makeToyVariation ("test", 2, 2));
    RCU_ASSERT (sys.result("second").size() == 1);
  }
}
