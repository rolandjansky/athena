// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigDecisionToolD3PD.h 502443 2012-05-25 10:43:25Z krasznaa $
#ifndef TRIGROOTANALYSIS_ITRIGDECISIONTOOLD3PD_H
#define TRIGROOTANALYSIS_ITRIGDECISIONTOOLD3PD_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// Local include(s):
#include "IITrigDecisionToolD3PD.h"

namespace D3PD {

   /// The interface provided by ITrigConfigSvcD3PD
   static const InterfaceID
   IID_ITrigDecisionToolD3PD( "D3PD::ITrigDecisionToolD3PD", 1, 0 );

   /**
    *  @short Interface for the D3PD-based trigger decision tool
    *
    *         This interface can be used in an Athena-based D3PD analysis to
    *         access the TDT.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 502443 $
    * $Date: 2012-05-25 12:43:25 +0200 (Fri, 25 May 2012) $
    */
   class ITrigDecisionToolD3PD : public virtual IAlgTool,
                                 public virtual IITrigDecisionToolD3PD {

   public:
      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigDecisionToolD3PD;
      }

   }; // class ITrigDecisionToolD3PD

} // namespace D3PD

#endif // TRIGROOTANALYSIS_ITRIGDECISIONTOOLD3PD_H
