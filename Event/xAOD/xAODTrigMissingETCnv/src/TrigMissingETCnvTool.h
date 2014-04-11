// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingETCnvTool.h 592539 2014-04-11 10:17:29Z krasznaa $
#ifndef xAODTrigMissingETCNV_TrigMissingETCNVTOOL_H
#define xAODTrigMissingETCNV_TrigMissingETCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigMissingETCnv/ITrigMissingETCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ITrigMissingETCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ITrigMissingETCnvTool.
    *
    * @author Gordon Watts <Gordon.Watts@cern.ch>
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 592539 $
    * $Date: 2014-04-11 12:17:29 +0200 (Fri, 11 Apr 2014) $
    */
   class TrigMissingETCnvTool : public AthAlgTool,
                                public virtual ITrigMissingETCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigMissingETCnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::TrigMissingETContainer
      virtual StatusCode convert( const TrigMissingETContainer* aod,
                                  xAOD::TrigMissingETContainer* xaod );

   }; // class TrigMissingETCnvTool

} // namespace xAODMaker

#endif // xAODTrigMissingETCNV_TrigMissingETCNVTOOL_H
