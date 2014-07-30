// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IN4MLinkContainerFlagFillerTool.h 542661 2013-03-27 00:35:13Z ssnyder $
#ifndef EVENTCOMMOND3PDMAKER_IN4MLINKCONTAINERFLAGFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_IN4MLINKCONTAINERFLAGFILLERTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "EventKernel/INavigable4Momentum.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "D3PDMakerInterfaces/IObjGetterTool.h"

namespace D3PD {

   /**
    *  @short Tool for flagging objects that have been selected in some way
    *
    *         The DPD making tools can usually create INav4MomLinkCollection
    *         objects which point to objects of interest. (For instance
    *         passing some selection cuts.) This tool helps in flagging
    *         the objects in some container based on whether they also
    *         appear in this preselected INav4MomLinkCollection
    *         container.
    *
    *         The object getter tools and the flag names can be freely
    *         configured.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 542661 $
    * $Date: 2013-03-27 01:35:13 +0100 (Wed, 27 Mar 2013) $
    */
   class IN4MLinkContainerFlagFillerTool :
      public BlockFillerTool< INavigable4Momentum > {

   public:
      /// Regular Gaudi AlgTool constructor
      IN4MLinkContainerFlagFillerTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent );

      /// Function initializing the tool
      virtual StatusCode initialize();

      /// Function booking the variable(s) of this block
      virtual StatusCode book();
      /// Function filling the variable(s) of this block
      virtual StatusCode fill( const INavigable4Momentum& p );

   private:
      /// The flags created in the D3PD
      std::vector< bool* > m_flags;

      /// The flag names, configured as a property
      std::vector< std::string > m_flagNames;

      /// The container getters that are used to compute the flags
      ToolHandleArray< IObjGetterTool > m_getters;

   }; // class IN4MLinkContainerFlagFillerTool

} // namespace D3PD

#endif // EVENTCOMMOND3PDMAKER_IN4MLINKCONTAINERFLAGFILLERTOOL_H
