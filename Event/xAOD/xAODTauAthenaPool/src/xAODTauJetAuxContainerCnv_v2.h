// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetAuxContainerCnv_v2.h 632169 2014-11-29 15:13:34Z krasznaa $
#ifndef XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_V2_H
#define XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_V2_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EDM include(s):
#include "xAODTau/versions/TauJetAuxContainer_v2.h"
#include "xAODTau/TauJetAuxContainer.h"

/// Converter class used for reading xAOD::TauJetAuxContainer_v2
///
/// This class implements the meat of the v1->vX conversion for xAOD::TauJet.
/// It translates the payload from the v1 object to the latest type.
/// The converter is implemented in a separate class in order to make it
/// visible to the BS reading code as well.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 632169 $
/// $Date: 2014-11-29 09:13:34 -0600 (Sat, 29 Nov 2014) $
///
class xAODTauJetAuxContainerCnv_v2 :
   public T_AthenaPoolTPCnvBase< xAOD::TauJetAuxContainer,
                                 xAOD::TauJetAuxContainer_v2 > {

public:
   /// Default constructor
   xAODTauJetAuxContainerCnv_v2();

   /// Function converting from the old type to the current one
   virtual void persToTrans( const xAOD::TauJetAuxContainer_v2* oldObj,
                             xAOD::TauJetAuxContainer* newObj,
                             MsgStream& log );
   /// Dummy function inherited from the base class
   virtual void transToPers( const xAOD::TauJetAuxContainer*,
                             xAOD::TauJetAuxContainer_v2*,
                             MsgStream& log );

  inline void setKey(const std::string& k){ m_key=k;}

private:
  std::string m_key;
}; // class xAODTauJetAuxContainer_v2

#endif // XAODTAUATHENAPOOL_XAODTAUJETAUXCONTAINERCNV_V2_H
