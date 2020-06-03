/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         NimCTP.h  -  description
                            -------------------
   begin                : August 2010
   email                : eleanor.dobson@cern.ch
***************************************************************************/

#ifndef TRIGT1INTERFACES_NIMCTP_H
#define TRIGT1INTERFACES_NIMCTP_H

namespace LVL1 {

  /**
   *  This class defines the NIM items
   *
   * @author edobson
   */
  class NimCTP {

  public:
    NimCTP( unsigned int cableword0 = 0, unsigned int cableword1 = 0 , unsigned int cableword2 = 0 );
    ~NimCTP();

    unsigned int cableWord0() const;
    unsigned int cableWord1() const;
    unsigned int cableWord2() const;

    void SetCableWord0(unsigned int cableword0);
    void SetCableWord1(unsigned int cableword1);
    void SetCableWord2(unsigned int cableword2);

  private:
    unsigned int m_cableWord0;
    unsigned int m_cableWord1;
    unsigned int m_cableWord2;

  }; // class NimCTP

} // namespace LVL1

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
CLASS_DEF( LVL1::NimCTP, 22423433, 1 )

#endif // TRIGT1INTERFACES_NIMCTP_H
