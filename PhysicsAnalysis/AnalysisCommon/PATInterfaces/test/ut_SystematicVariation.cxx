/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#undef NDEBUG
#include <PATInterfaces/SystematicVariation.h>

#include <RootCoreUtils/Assert.h>
#include <cmath>

using namespace CP;

//
// method implementations
//

int main ()
{
  {
    SystematicVariation var = SystematicVariation::makeContinuousEnsemble ("TEST");
    RCU_ASSERT (var.name() == "TEST__continuous");
    RCU_ASSERT (var == SystematicVariation ("TEST", SystematicVariation::CONTINUOUS));
    RCU_ASSERT (var.isContinuousEnsemble());
    RCU_ASSERT (var.isEnsemble());
  }
  {
    SystematicVariation var = SystematicVariation ("TEST");
    RCU_ASSERT (var.isContinuousEnsemble() == false);
  }
  {
    SystematicVariation var = SystematicVariation::makeToyEnsemble ("TEST");
    RCU_ASSERT (var.name() == "TEST__toy_ensemble");
    RCU_ASSERT (var.isToyEnsemble());
    RCU_ASSERT (var.isEnsemble());
  }
  {
    SystematicVariation var = SystematicVariation ("TEST");
    RCU_ASSERT (var.isToyEnsemble() == false);
  }
  {
    SystematicVariation var = SystematicVariation::makeToyVariation ("TEST", 3, 2.1234);
    RCU_ASSERT (var.name() == "TEST__toy_3_2scale1234");
    RCU_ASSERT (var.getToyVariation().first == 3);
    RCU_ASSERT (rint (var.getToyVariation().second * 10000) == 21234);
  }
  {
    bool fail = false;
    try
    {
      SystematicVariation ("adfsadf__toy_3_adf").getToyVariation ();
    } catch (...)
    {
      fail = true;
    }
    RCU_ASSERT (fail == true);
  }
  {
    bool fail = false;
    try
    {
      SystematicVariation ("adfsadf__toy_asf_3scale1745").getToyVariation ();
    } catch (...)
    {
      fail = true;
    }
    RCU_ASSERT (fail == true);
  }
  {
    bool fail = false;
    try
    {
      SystematicVariation ("adfsadf__to_3_3scale1745").getToyVariation ();
    } catch (...)
    {
      fail = true;
    }
    RCU_ASSERT (fail == true);
  }
}
