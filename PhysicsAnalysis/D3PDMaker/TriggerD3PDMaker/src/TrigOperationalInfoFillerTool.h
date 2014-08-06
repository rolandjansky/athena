// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigOperationalInfoFillerTool.h 316055 2010-08-17 16:26:31Z krasznaa $
#ifndef TRIGGERD3PDMAKER_TRIGOPERATIONALINFOFILLERTOOL_H
#define TRIGGERD3PDMAKER_TRIGOPERATIONALINFOFILLERTOOL_H

// STL include(s):
#include <vector>
#include <string>

// EDM include(s):
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool filling the information about express stream trigger chains
    *
    *         This tool can be used to fill information in the D3PD about which
    *         chains were responsible for putting an event into the express stream.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 316055 $
    * $Date: 2010-08-17 18:26:31 +0200 (Tue, 17 Aug 2010) $
    */
   class TrigOperationalInfoFillerTool :
      public BlockFillerTool< TrigOperationalInfoCollection > {

   public:
      /// Regular AlgTool constructor
      TrigOperationalInfoFillerTool( const std::string& type, const std::string& name,
                                     const IInterface* parent );

      /// Function booking the variables in the output
      virtual StatusCode book();
      /// Function filling the variables in the output
      virtual StatusCode fill( const TrigOperationalInfoCollection& info );

   private:
      /// Names of the chains causing the event to end up in the express stream
      std::vector< std::string >* m_chains;

   }; // class TrigOperationalInfoFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_TRIGOPERATIONALINFOFILLERTOOL_H
