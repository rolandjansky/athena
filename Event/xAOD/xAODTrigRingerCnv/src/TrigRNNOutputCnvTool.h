// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRNNOutputCnvTool.h 592334 2014-04-10 11:01:56Z krasznaa $
#ifndef XAODTRIGRINGERCNV_TRIGRNNOUTPUTCNVTOOL_H
#define XAODTRIGRINGERCNV_TRIGRNNOUTPUTCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigRingerCnv/ITrigRNNOutputCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ITrigRNNOutputCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IRingerRNNOutputCnvTool.
    *
    * @author joao victor da fonseca pinto <joao.victor.da.fonseca.pinto@cern.ch>
    *
    * $Revision:  $
    * $Date:  $
    */
   class TrigRNNOutputCnvTool : public AthAlgTool,
				  public virtual ITrigRNNOutputCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigRNNOutputCnvTool( const std::string& type, const std::string& name,
			      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::RingerRNNOutput
      virtual StatusCode convert( const TrigRNNOutputContainer* aod,
                                  xAOD::TrigRNNOutputContainer* xaod );

   }; // class TrigRNNOutputCnvTool

} // namespace xAODMaker

#endif // XAODMaker
