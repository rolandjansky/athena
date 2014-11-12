// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRingsCnvTool.h  $
#ifndef XAODTRIGRINGERCNV_TRIGRINGERRINGSCNVTOOL_H
#define XAODTRIGRINGERCNV_TRIGRINGERRINGSCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigRingerCnv/ITrigRingerRingsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IRingerRingsCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IRingerRingsCnvTool.
    *
    * @author joao victor da fonseca pinto <joao.victor.da.fonseca.pinto@cern.ch>
    *
    * $Revision: $
    * $Date:  $
    */
   class TrigRingerRingsCnvTool : public AthAlgTool,
				  public virtual ITrigRingerRingsCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigRingerRingsCnvTool( const std::string& type, const std::string& name,
			      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::RingerRings
      virtual StatusCode convert( const RingerRingsContainer* aod,
                                  xAOD::TrigRingerRingsContainer* xaod );

   }; // class RingerRingsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGCALOCNV_RINGERRINGSCNVTOOL_H
