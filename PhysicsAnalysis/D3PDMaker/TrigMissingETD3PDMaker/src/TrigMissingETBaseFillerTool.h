// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingETBaseFillerTool.h 278740 2010-02-11 14:32:52Z krasznaa $
#ifndef TRIGMISSINGETD3PDMAKER_TRIGMISSINGETBASEFILLERTOOL_H
#define TRIGMISSINGETD3PDMAKER_TRIGMISSINGETBASEFILLERTOOL_H

// EDM include(s):
#include "TrigMissingEtEvent/TrigMissingETContainer.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool saving basic information about missing-ET objects
    *
    *         This tool saves the bare minimum of information about the
    *         missing-ET reconstructed in the HLT.
    *
    * @author Attila Krasznahorkay Jr.
    * @author Diego Casadei
    *
    * $Revision: 278740 $
    * $Date: 2010-02-11 15:32:52 +0100 (Thu, 11 Feb 2010) $
    */
   class TrigMissingETBaseFillerTool : public BlockFillerTool< TrigMissingETContainer > {

   public:
      /// Regular AlgTool constructor
      TrigMissingETBaseFillerTool( const std::string& type, const std::string& name,
                                   const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigMissingETContainer& met );

   private:
      float* m_ex; ///< X component of missing energy vector
      float* m_ey; ///< Y component of missing energy vector
      float* m_ez; ///< Z component of missing energy vector
      float* m_sumEt; ///< Sum of |Et|
      float* m_sumE; ///< Sum of |E|

      int*   m_flag; ///< Event flag: 0 (default) means OK

   }; // class TrigMissingETBaseFillerTool

} // namespace D3PD

#endif // TRIGMISSINGETD3PDMAKER_TRIGMISSINGETBASEFILLERTOOL_H
