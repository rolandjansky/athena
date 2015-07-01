/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetPRD_Provider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASRECOTOOLS_PLANARCLUSTERPROVIDER_H
#define ISF_FATRASRECOTOOLS_PLANARCLUSTERPROVIDER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// ISF includes
#include "ISF_FatrasEvent/PlanarClusterContainer.h"
// Trk includes
#include "TrkToolInterfaces/IPRD_Provider.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"
#include "TrkPrepRawData/PrepRawDataCollection.h"
// Identifier
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
 
class AtlasDetectorID;
class PixelID;
class SCT_ID;
  
namespace iFatras {
 
  /**
   @class iFatras

   @brief PRD provider for the planar cluster
       
   @author Noemi.Calace -at- cern.ch
   */
     
  class PlanarClusterProvider : public AthAlgTool, virtual public Trk::IPRD_Provider {
    
  public:     
    //** Constructor with parameters */
    PlanarClusterProvider ( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Athena algtool's Hooks */
    StatusCode  initialize();
    
    /** finalize */
    StatusCode  finalize();
    
    /** retrieve the PRD collection from StoreGate */
    StatusCode retrieveCollection();
    
    /** return the Prd given the Identifier - make a HashId out of the Id and return the associated PRD */
    const Trk::PrepRawData* prdFromIdentifier(const Identifier& ide, size_t& ndof ) const;
    
  private:
    /** templated method since used for further developments */
    template < class PrdT > const Trk::PrepRawData* prdFromIdentifierContainers(
										std::vector < const Trk::PrepRawDataContainer< Trk::PrepRawDataCollection< PrdT > >* >& coll,
										const Identifier& ideh, const IdentifierHash& ideHash ) const 
      {
	const PrdT* prd = 0;
	// loop on the containers
	typename std::vector < const Trk::PrepRawDataContainer< Trk::PrepRawDataCollection< PrdT > >* >::const_iterator pmtVecCollIter  = coll.begin();
	typename std::vector < const Trk::PrepRawDataContainer< Trk::PrepRawDataCollection< PrdT > >* >::const_iterator pmtVecCollIterE = coll.end();
	for ( ; pmtVecCollIter != pmtVecCollIterE; ++pmtVecCollIter ){
	  // find the collection
	  typename Trk::PrepRawDataContainer< Trk::PrepRawDataCollection< PrdT > >::const_iterator prdCollIter = (*pmtVecCollIter)->indexFind(ideHash);
	  if (!(*prdCollIter))
	    continue;
	    
	  const Trk::PrepRawDataCollection< PrdT >* prdCollection = (*prdCollIter);
	  // search for the PRD in the collection --- do a loop, can be done better with std::find probably
	  // iterate through the collections
	  typename Trk::PrepRawDataCollection< PrdT >::const_iterator prdIter  = prdCollection->begin();
	  typename Trk::PrepRawDataCollection< PrdT >::const_iterator prdIterE = prdCollection->end();
	  for ( ; prdIter != prdIterE; ++prdIter ){
	    if ( (*prdIter)->identify() == ideh ){
	      prd = (*prdIter);
	      break;
	    }
	  }
	}
	
	// return what you have
	if (!prd) // if no prd is found
	  ATH_MSG_VERBOSE("PRD Collection to IdentifierHash could not be found. Return 0.");
	
	return prd;
      }
    
    const AtlasDetectorID*                      m_idHelper;                     //! Helper to detect type of sub-detector from PRD->identify().
    
    const PixelID*                              m_pixIdHelper;
    
    const SCT_ID*                               m_sctIdHelper;                      
    
    std::vector<std::string>                                        m_planarClusterContainerNames;          
    mutable std::vector<const iFatras::PlanarClusterContainer*>*    m_planarClusterContainers;     

    bool                                        m_usePixel;
    bool                                        m_useSCT;
    
  };


} // end of namespace

#endif // ISF_FATRASRECOTOOLS_PLANARCLUSTERPROVIDER_H
