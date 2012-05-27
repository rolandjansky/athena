// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingETComponentFillerTool.h 278740 2010-02-11 14:32:52Z krasznaa $
#ifndef TRIGMISSINGETD3PDMAKER_TRIGMISSINGETCOMPONENTFILLERTOOL_H
#define TRIGMISSINGETD3PDMAKER_TRIGMISSINGETCOMPONENTFILLERTOOL_H

// EDM include(s):
#include "TrigMissingEtEvent/TrigMissingETContainer.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool saving the components of the missing-ET objects
    *
    *         This tool saves the components of the HLT missing-ET calculation.
    *
    * @author Attila Krasznahorkay Jr.
    * @author Diego Casadei
    *
    * $Revision: 278740 $
    * $Date: 2010-02-11 15:32:52 +0100 (Thu, 11 Feb 2010) $
    */
   class TrigMissingETComponentFillerTool : public BlockFillerTool< TrigMissingETContainer > {

   public:
      /// Regular AlgTool constructor
      TrigMissingETComponentFillerTool( const std::string& type, const std::string& name,
                                        const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigMissingETContainer& met );

   private:
      std::vector< std::string >*    m_name; ///< Name of component
      std::vector< float >*          m_ex; ///< Ex from component
      std::vector< float >*          m_ey; ///< Ey from component
      std::vector< float >*          m_ez; ///< Ez from component
      std::vector< float >*          m_sumEt; ///< Sum of |Et| from component
      std::vector< float >*          m_sumE; ///< Sum of |E| from component
      std::vector< float >*          m_calib0; ///< Constant term for calibration of component
      std::vector< float >*          m_calib1; ///< Scale for calibration of component
      std::vector< short >*          m_sumOfSigns; ///< Sum of energy signs for calibration of comp.
      std::vector< unsigned short >* m_usedChannels; ///< Number of used channels in component
      std::vector< short >*          m_status; ///< Status flag for component

   }; // class TrigMissingETComponentFillerTool

} // namespace D3PD

#endif // TRIGMISSINGETD3PDMAKER_TRIGMISSINGETCOMPONENTFILLERTOOL_H
