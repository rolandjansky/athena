// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IBunchCrossingTool.h 511861 2012-07-31 08:40:04Z krasznaa $
#ifndef TRIGANALYSISINTERFACES_IBUNCHCROSSINGTOOL_H
#define TRIGANALYSISINTERFACES_IBUNCHCROSSINGTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// Local include(s):
#include "IIBunchCrossingTool.h"

/**
 *  @short The common trigger namespace for trigger analysis tools
 *
 *         This is the common namespace that all trigger analysis tools use by
 *         convention.
 *
 * $Revision: 511861 $
 * $Date: 2012-07-31 10:40:04 +0200 (Tue, 31 Jul 2012) $
 */
namespace Trig {

   /// The interface provided by IBunchCrossingTool
   static const InterfaceID IID_IBunchCrossingTool( "Trig::IBunchCrossingTool", 2, 0 );

   /**
    *  @short Interface for all the bunch crossing tools
    *
    *         This is the Athena inteface of all the bunch crossing tools. It inherits
    *         all its functions from Trig::IIBunchCrossingTool to provide the users
    *         with the same interface in Athena and outside of it.
    *
    *    @see Trig::IIBunchCrossingTool
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 511861 $
    * $Date: 2012-07-31 10:40:04 +0200 (Tue, 31 Jul 2012) $
    */
   class IBunchCrossingTool : public virtual IAlgTool,
                              public virtual IIBunchCrossingTool {

   public:
      /// Gaudi interface definition
      static const InterfaceID& interfaceID() { return IID_IBunchCrossingTool; }

   }; // class IBunchCrossingTool

} // namespace Trig

#endif // TRIGANALYSISINTERFACES_IBUNCHCROSSINGTOOL_H
