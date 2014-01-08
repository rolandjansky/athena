/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyPosDigiChoice_H
#define MboyPosDigiChoice_H


#include "MboyCoreEvt/MboyCoreEvtMisc.h"

  /**
   @class MboyPosDigiChoice

   This class is an utility class to specify choice on treatement of
   positions associated to digits

  @author samusog@cern.ch
  
  */

class MboyPosDigiChoice{
public:
    MboyPosDigiChoice();
    virtual ~MboyPosDigiChoice();

public:
///////////////////////////////////

   /**Set choice on treatement of positions for MDT */
   void SetPosOffMDT (int PosOffMDT) ; 

   /**Set choice on treatement of positions for RPC */
   void SetPosOffRPC (int PosOffRPC) ; 

   /**Set choice on treatement of positions for TGC */
   void SetPosOffTGC (int PosOffTGC) ; 

   /**Set choice on treatement of positions for CSC */
   void SetPosOffCSC (int PosOffCSC) ; 

   /**Get choice on treatement of positions for MDT */
   int GetPosOffMDT () const ; 

   /**Get choice on treatement of positions for RPC */
   int GetPosOffRPC () const ; 

   /**Get choice on treatement of positions for TGC */
   int GetPosOffTGC () const ; 

   /**Get choice on treatement of positions for CSC */
   int GetPosOffCSC () const ; 

private:
///////////////////////////////////
// Flag to zerofy digits position
   int m_PosOffMDT ; //!< choice on treatement of positions for MDT
   int m_PosOffRPC ; //!< choice on treatement of positions for RPC
   int m_PosOffTGC ; //!< choice on treatement of positions for TGC
   int m_PosOffCSC ; //!< choice on treatement of positions for CSC

};

#endif
