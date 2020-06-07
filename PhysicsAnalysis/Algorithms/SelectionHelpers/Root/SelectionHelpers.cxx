/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionHelpers.h>

#include <PATCore/AcceptData.h>

//
// method implementations
//

namespace CP
{
  SelectionType selectionFromBool (bool accept)
  {
    if (accept)
      return selectionAccept();
    else
      return selectionReject();
  }



  SelectionType selectionFromAccept (const asg::AcceptData& accept)
  {
    return ~SelectionType (accept.getCutResultInvertedBitSet().to_ulong());
  }



  ANA_MSG_SOURCE (msgSelectionHelpers, "selectionHelpers")
}
