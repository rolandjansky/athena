// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGETCNV_MISSINGETCNVTOOL_H
#define XAODMISSINGETCNV_MISSINGETCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODMissingETCnv/IMissingETCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation of the IMissingETCnvTool interface
    *
    *         Documentation to be written...
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    */
   class MissingETCnvTool : public AthAlgTool,
                            public virtual IMissingETCnvTool {

   public:
      /// Regular AlgTool constructor
      MissingETCnvTool( const std::string& type, const std::string& name,
                        const IInterface* parent );

      /// Function doing the translation
      virtual StatusCode add( const MissingET* aod,
                              xAOD::MissingETContainer* xaod ) override;

   }; // class MissingETCnvTool

} // namespace xAODMaker

#endif // XAODMISSINGETCNV_MISSINGETCNVTOOL_H
