// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MissingETCnvTool.h 586557 2014-03-06 17:56:05Z krasznaa $
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
    *
    * $Revision: 586557 $
    * $Date: 2014-03-06 18:56:05 +0100 (Thu, 06 Mar 2014) $
    */
   class MissingETCnvTool : public AthAlgTool,
                            public virtual IMissingETCnvTool {

   public:
      /// Regular AlgTool constructor
      MissingETCnvTool( const std::string& type, const std::string& name,
                        const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function doing the translation
      virtual StatusCode add( const MissingET* aod,
                              xAOD::MissingETContainer* xaod );

   }; // class MissingETCnvTool

} // namespace xAODMaker

#endif // XAODMISSINGETCNV_MISSINGETCNVTOOL_H
