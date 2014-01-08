/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbROT_H
#define MbROT_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbPOT.h"


  /**
   @class MbROT

   This class is an absract class for managing a ROT

  @author samusog@cern.ch
  
  */

class MbROT: public MbPOT{
public:
    MbROT();
    virtual ~MbROT();

   /** Set ID of the PRD */
   void SetLinkPrd(int LinkPrd) ;

   /** Get ID of the PRD */
   int GetLinkPrd()  const ;

protected:
///////////////////////////////////

  int m_LinkPrd ; //!< ID of the PRD

};

#endif
