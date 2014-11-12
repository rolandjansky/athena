// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IRingerRNNOutputCnvTool.h  $
#ifndef XAODTRIGRINGERCNV_ITRIGRNNOUTPUTCNVTOOL_H
#define XAODTRIGRINGERCNV_ITRIGRNNOUTPUTCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRNNOutputContainer.h"

// Forward declaration(s):
class TrigRNNOutputContainer;

namespace xAODMaker {

   /// The interface provided by IRingerRNNOutputCnvTool
   static const InterfaceID
   IID_ITrigRNNOutputCnvTool( "xAODMaker::ITrigRNNOutputCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigRNNOutput from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         RingerRNNOutput object from an existing AOD RingerRNNOutput container into
    *         an xAOD::RingerRNNOutput.
    *
    * @author joao victor da fonseca pinto <joao.victor.da.fonseca.pinto@cern.ch>
    *
    * $Revision: $
    * $Date:  $
    */
   class ITrigRNNOutputCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::RingerRNNOutputContainer
      virtual StatusCode convert( const TrigRNNOutputContainer* aod,
				  xAOD::TrigRNNOutputContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigRNNOutputCnvTool;
      }

   }; // class ITrigRNNOutputCnvTool

} // namespace xAODMaker

#endif // XAODTRIGRINGERCNV_ITRIGRNNOUTPUTCNVTOOL_H
