/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <SystematicsHandles/SelectionHelpers.h>

#include <PATCore/TAccept.h>

//
// method implementations
//

namespace EL
{
  SelectionType selectionFromBool (bool accept)
  {
    if (accept)
      return selectionAccept();
    else
      return selectionReject();
  }



  SelectionType selectionFromAccept (const Root::TAccept& accept)
  {
    return ~SelectionType (accept.getCutResultInvertedBitSet().to_ulong());
  }
}
