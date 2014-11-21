// Dear emacs, this is -*- c++ -*-                                                                                                                                                       

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IRODHeaderCnvTool.h 576052 2013-12-18 09:13:50Z morrisj $
#ifndef XAODTRIGL1CALOCNV_IRODHEADERCNVTOOL_H
#define XAODTRIGL1CALOCNV_IRODHEADERCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigL1Calo/RODHeaderContainer.h"

// Forward declaration(s):
namespace LVL1{
  class RODHeader;
}

namespace xAODMaker {

   /// The interface provided by IRODHeaderCnvTool
   static const InterfaceID
   IID_IRODHeaderCnvTool( "xAODMaker::IRODHeaderCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating RODHeaderContainer from an ESD
    *
    *         This interface is implemented by the tool that converts the
    *         Trigger/TrigT1/TrigT1CaloEvent/RODHeader objects from an existing ESD container into
    *         an xAOD::RODHeaderContainer.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576052 $
    * $Date: 2013-12-18 10:13:50 +0100 (Wed, 18 Dec 2013) $
    */
   class IRODHeaderCnvTool : public virtual IAlgTool {
   public:
      /// Function that fills an existing xAOD::RODHeaderContainer
      virtual StatusCode convert( const DataVector<LVL1::RODHeader>* esd,
                                  xAOD::RODHeaderContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IRODHeaderCnvTool;
      }

   }; // class IRODHeaderCnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_IRODHEADERCNVTOOL_H
