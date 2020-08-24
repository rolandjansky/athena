/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetPRD_Provider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDET_INDETASSOCIATIONTOOLS_INDETPRDPROVIDER_H
#define INDET_INDETASSOCIATIONTOOLS_INDETPRDPROVIDER_H 1

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// InDet includes; these are needed (cannot be fwd declared) for template type deduction
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
// Trk includes
#include "TrkToolInterfaces/IPRD_Provider.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"
#include "TrkPrepRawData/PrepRawDataCollection.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
 
class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID; 
class Identifier;
class IdentifierHash;
namespace InDet {
 
  /**
   @class InDet

   @brief PRD provider for the Inner detector needed, to optimize search for IdContext
       
   @author Andreas.Salzburger -at- cern.ch
   */
     
  class InDetPRD_Provider : public AthAlgTool, virtual public Trk::IPRD_Provider {

     public:     
        //** Constructor with parameters */
        InDetPRD_Provider( const std::string& t, const std::string& n, const IInterface* p );
 
       /** Athena algtool's Hooks */
       StatusCode  initialize();
       
       /** finalize */
       StatusCode  finalize();

       /** retrieve the PRD collection from StoreGate */
       StatusCode retrieveCollection();

       /** return the Prd given the Identifier - make a HashId out of the Id and return the associated PRD */
       const Trk::PrepRawData* prdFromIdentifier(const Identifier& ide, size_t& ndof ) const;
       
     private:
        /** templated method since used for Pixel/SCT/TRT */
        template < class PrdT > const Trk::PrepRawData* prdFromIdentifierContainer(
                            const Trk::PrepRawDataContainer< Trk::PrepRawDataCollection< PrdT > >& cont,
                            const Identifier& ideh, const IdentifierHash& ideHash ) const 
        {
          // find the collection
          const Trk::PrepRawDataCollection< PrdT >* prdCollection = cont.indexFindPtr(ideHash);
          if ( prdCollection == nullptr ){
              ATH_MSG_VERBOSE("PRD Collection to IdentifierHash could not be found. Return 0.");
              return 0;
          }
          // search for the PRD in the collection --- do a loop, can be done better with std::find probably
          const PrdT* prd = 0;
          // iterate through the collections
          typename Trk::PrepRawDataCollection< PrdT >::const_iterator prdIter  = prdCollection->begin();
          typename Trk::PrepRawDataCollection< PrdT >::const_iterator prdIterE = prdCollection->end();
          for ( ; prdIter != prdIterE; ++prdIter ){
              if ( (*prdIter)->identify() == ideh ){
                  prd = (*prdIter);
                  break;
              }
          }
          // return what you have
          return prd;
        }
         
        const AtlasDetectorID*                      m_idHelper;                     //! Helper to detect type of sub-detector from PRD->identify().
                                                    
        const PixelID*                              m_pixIdHelper;
        std::string                                 m_pixClusterContainerName;          
        const PixelClusterContainer*                m_pixClusterContainer;
        // For P->T converter of Pixel Clusters
        SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
        
        const SCT_ID*                               m_sctIdHelper;                      
        std::string                                 m_sctClusterContainerName;          
        const SCT_ClusterContainer*                 m_sctClusterContainer;
        // For P->T converter of SCT_Clusters
        SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
        
        const TRT_ID*                               m_trtIdHelper;                      
        std::string                                 m_trtDriftCircleContainerName;      
        const TRT_DriftCircleContainer*             m_trtDriftCircleContainer;

  };


} // end of namespace

#endif // INDET_INDETASSOCIATIONTOOLS_INDETPRDPROVIDER_H
