/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETASSOCIATIONBASE_P1_H
#define JETEVENT_JETASSOCIATIONBASE_P1_H

class JetAssociationBase_p1
{
  friend class JetAssociationBaseCnv_p1;
  
 public:
  JetAssociationBase_p1() {};
  ~JetAssociationBase_p1() = default;
  
  /// Stored in the base class
  unsigned int m_keyIndex{};
};

#endif

