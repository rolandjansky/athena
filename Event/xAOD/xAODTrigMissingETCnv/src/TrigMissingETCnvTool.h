// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
    */
   class TrigMissingETCnvTool : public AthAlgTool,
                                public virtual ITrigMissingETCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigMissingETCnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function that fills an existing xAOD::TrigMissingETContainer
      virtual StatusCode convert( const TrigMissingETContainer* aod,
                                  xAOD::TrigMissingETContainer* xaod ) const override;

   }; // class TrigMissingETCnvTool

} // namespace xAODMaker

#endif // xAODTrigMissingETCNV_TrigMissingETCNVTOOL_H
