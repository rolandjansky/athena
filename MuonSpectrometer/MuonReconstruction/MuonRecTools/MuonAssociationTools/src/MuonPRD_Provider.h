/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonPRD_Provider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUON__MUONASSOCIATIONTOOLS_MUONPRDPROVIDER_H
#define MUON__MUONASSOCIATIONTOOLS_MUONPRDPROVIDER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Muon includes
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepData.h"
// Trk includes
#include "TrkToolInterfaces/IPRD_Provider.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"
#include "TrkPrepRawData/PrepRawDataCollection.h"
// Identifier
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
 
namespace Muon {
 
  /**
     @class Muon

     @brief PRD provider for the Inner detector needed, to optimize search for IdContext
       
     @author Andreas.Salzburger -at- cern.ch
  */
     
  class MuonPRD_Provider : public AthAlgTool, virtual public Trk::IPRD_Provider {

  public:     
    //** Constructor with parameters */
    MuonPRD_Provider( const std::string& t, const std::string& n, const IInterface* p );
 
    /** Athena algtool's Hooks */
    StatusCode  initialize();
       
    /** finalize */
    StatusCode  finalize();

    /** retrieve the PRD collection from StoreGate */
    StatusCode retrieveCollection();

    /** return the Prd given the Identifier - make a HashId out of the Id and return the associated PRD */
    const Trk::PrepRawData* prdFromIdentifier(const Identifier& ide, size_t& ndof ) const;
       
  private:
    /** templated method since used for all technologies */
    template < class PrdT > const Trk::PrepRawData* prdFromIdentifierContainer(
									       const MuonPrepDataContainer< MuonPrepDataCollection< PrdT > >& cont,
									       const Identifier& ideh, const IdentifierHash& ideHash ) const 
    {
      // find the collection
      typename MuonPrepDataContainer< MuonPrepDataCollection< PrdT > >::const_iterator prdCollIter = cont.indexFind(ideHash);
      if ( prdCollIter == cont.end() ){
	ATH_MSG_VERBOSE("PRD Collection to IdentifierHash could not be found. Return 0.");
	return 0;
      }
      const MuonPrepDataCollection< PrdT >* prdCollection = (*prdCollIter);
      // search for the PRD in the collection --- do a loop, can be done better with std::find probably
      const PrdT* prd = 0;
      // iterate through the collections
      typename MuonPrepDataCollection< PrdT >::const_iterator prdIter  = prdCollection->begin();
      typename MuonPrepDataCollection< PrdT >::const_iterator prdIterE = prdCollection->end();
      for ( ; prdIter != prdIterE; ++prdIter ){
	if ( (*prdIter)->identify() == ideh ){
	  prd = (*prdIter);
	  break;
	}
      }
      // return what you have
      return prd;
    }
    
    ToolHandle<MuonIdHelperTool> m_idHelper;
    const MmIdHelper*                   m_mmIdHelper;
    const sTgcIdHelper*                 m_stgcIdHelper;
    std::string         m_keyTgc;
    std::string         m_keyRpc;
    std::string         m_keyCsc;
    std::string         m_keyMdt;
    std::string         m_keyMM;
    std::string         m_keysTgc;

    mutable const RpcPrepDataContainer* m_rpcPrds;
    mutable const TgcPrepDataContainer* m_tgcPrds;
    mutable const CscPrepDataContainer* m_cscPrds;
    mutable const MdtPrepDataContainer* m_mdtPrds;
    mutable const MMPrepDataContainer*  m_mmPrds;
    mutable const sTgcPrepDataContainer* m_stgcPrds;

    
  };


} // end of namespace

#endif // INDET_INDETASSOCIATIONTOOLS_INDETPRDPROVIDER_H
