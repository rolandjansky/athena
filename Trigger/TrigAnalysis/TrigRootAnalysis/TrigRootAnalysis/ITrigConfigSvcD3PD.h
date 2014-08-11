// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigConfigSvcD3PD.h 502443 2012-05-25 10:43:25Z krasznaa $
#ifndef TRIGROOTANALYSIS_ITRIGCONFIGSVCD3PD_H
#define TRIGROOTANALYSIS_ITRIGCONFIGSVCD3PD_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IService.h"

// Local include(s):
#include "IITrigConfigSvcD3PD.h"

namespace D3PD {

   /// The interface provided by ITrigConfigSvcD3PD
   static const InterfaceID IID_ITrigConfigSvcD3PD( "D3PD::ITrigConfigSvcD3PD", 1, 0 );

   /**
    *  @short Interface for the D3PD-based trigger configuration service
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 502443 $
    * $Date: 2012-05-25 12:43:25 +0200 (Fri, 25 May 2012) $
    */
   class ITrigConfigSvcD3PD : public virtual IService,
                              public virtual IITrigConfigSvcD3PD {

   public:
      /// Gaudi interface definition
      static const InterfaceID& interfaceID() { return IID_ITrigConfigSvcD3PD; }

   }; // class ITrigConfigSvcD3PD

} // namespace D3PD

#endif // TRIGROOTANALYSIS_ITRIGCONFIGSVCD3PD_H
