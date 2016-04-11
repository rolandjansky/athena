//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <RootCoreUtils/RootUtils.h>

#include <TDirectory.h>
#include <TEfficiency.h>
#include <TH1.h>
#include <TTree.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace RCU
{
  bool SetDirectory (TObject *object, TDirectory *directory)
  {
    RCU_ASSERT (object != 0);

    TH1 *const hist = dynamic_cast<TH1*>(object);
    if (hist)
    {
      hist->SetDirectory (directory);
      return true;
    }

    TEfficiency *const efficiency = dynamic_cast<TEfficiency*>(object);
    if (efficiency)
    {
      efficiency->SetDirectory (directory);
      return true;
    }

    TTree *const tree = dynamic_cast<TTree*>(object);
    if (tree)
    {
      tree->SetDirectory (directory);
      return true;
    }

    return false;
  }
}
