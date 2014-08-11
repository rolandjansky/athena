// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionToolD3PD.h 502443 2012-05-25 10:43:25Z krasznaa $
#ifndef TRIGROOTANALYSIS_TRIGDECISIONTOOLD3PD_H
#define TRIGROOTANALYSIS_TRIGDECISIONTOOLD3PD_H

// ROOT include(s):
#include <TNamed.h>
#include <TString.h>

// Local include(s):
#include "IITrigDecisionToolD3PD.h"
#include "TrigConfigSvcD3PD.h"
#include "DataAccess.h"
#include "ConfigAccess.h"
#include "ChainGroupHandling.h"
#include "TrigDecisionFunctions.h"
#include "Conditions.h"

// Forward declaration(s):
class TTree;

namespace D3PD {

   /**
    *  @short Tool for interpreting trigger information in the D3PDs
    *
    *         This tool can be used to interpret the trigger information from the
    *         D3PDs, which is stored in an encoded way. The way in which the data
    *         is stored is explained in
    *
    *         https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerD3PDMaker
    *
    *         Currently the only implemented feature is to ask the tool if a trigger
    *         item passed various conditions.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 502443 $
    * $Date: 2012-05-25 12:43:25 +0200 (Fri, 25 May 2012) $
    */
   class TrigDecisionToolD3PD : public virtual TNamed,
                                public virtual D3PD::IITrigDecisionToolD3PD,
                                public D3PD::Trig::DataAccess,
                                public D3PD::Trig::ConfigAccess,
                                public D3PD::Trig::ChainGroupHandling,
                                public D3PD::Trig::TrigDecisionFunctions {

   public:
      /// Constructor specifying the D3PD trees that the tool should act on
      TrigDecisionToolD3PD( ::TTree* eventTree = 0, ::TTree* confTree = 0,
                            const ::TString& prefix = "trig_" );

      ClassDef( D3PD::TrigDecisionToolD3PD, 0 )

   }; // class TrigDecisionToolD3PD

} // namespace D3PD

#endif // TRIGROOTANALYSIS_TRIGDECISIONTOOLD3PD_H
