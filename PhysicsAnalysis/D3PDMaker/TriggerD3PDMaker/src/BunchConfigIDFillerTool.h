// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfigIDFillerTool.h 451905 2011-08-03 11:49:54Z krasznaa $
#ifndef TRIGGERD3PDMAKER_BUNCHCONFIGIDFILLER_H
#define TRIGGERD3PDMAKER_BUNCHCONFIGIDFILLER_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"

// Trigger include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingConfProvider.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool saving the bunch structure setup configuration ID for each event
    *
    *         Each entry in the bunch structure metadata TTree is indexed by a
    *         configuration ID. This tool saves the variable in the event-level tree
    *         which specifies which entry from the metadata tree should be used to
    *         analyze the event.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 451905 $
    * $Date: 2011-08-03 13:49:54 +0200 (Wed, 03 Aug 2011) $
    */
   class BunchConfigIDFillerTool : public BlockFillerTool< void > {

   public:
      /// Regular AlgTool constructor
      BunchConfigIDFillerTool( const std::string& type, const std::string& name,
                               const IInterface* parent );

      /// Regular AlgTool initialization function
      virtual StatusCode initialize();

      /// Function booking the variables in the output
      virtual StatusCode book();
      /// Function filling the variables for the output
      virtual StatusCode fill();

   private:
      /// Handle to the configuration provider
      ToolHandle< Trig::IBunchCrossingConfProvider > m_bcConfProv;

      Trig::IBunchCrossingConfProvider::configid_type* m_configId; ///< Configuration ID

   }; // class BunchConfigIDFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_BUNCHCONFIGIDFILLER_H
