/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_ISvxAssociation_p1_H
#define JETTAGINFOTPCNV_ISvxAssociation_p1_H

///
/// Store the required info for b-tagging assocaiations that are
/// a Navigable dude.
///

#include <string>

#include <string>

namespace Analysis {

  class ISvxAssociation_p1 {
  public:
    unsigned int m_keyIndex;
    std::string m_name;
  };
}

#endif
