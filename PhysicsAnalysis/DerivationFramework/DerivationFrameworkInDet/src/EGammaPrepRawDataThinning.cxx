/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkInDet/EGammaPrepRawDataThinning.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "FourMomUtils/P4Helpers.h"

using CLHEP::Hep3Vector;

DerivationFramework::EGammaPrepRawDataThinning::EGammaPrepRawDataThinning(const std::string& type, 
                                                                          const std::string& name, 
                                                                          const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_thinningSvc( "ThinningSvc", name ),
  m_regionSelector("RegSelSvc", name),
  m_electronsContainerName("Electrons"),
  m_photonsContainerName("Photons"),
  m_pixelClusterKey("PixelClusters"),
  m_sctClusterKey("SCT_Clusters"),
  m_trtDriftCircleKey("TRT_DriftCircles"),
  m_dr(0.5),
  m_minEtEg(0),
  m_maxDeltaEtaPRD(0.5),
  m_maxDeltaPhiPRD(0.5),
  m_maxDeltaZPRD(400)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningService"               , m_thinningSvc);
  declareProperty("RegSelSvc"                     , m_regionSelector);
  declareProperty("electronContainerName"         , m_electronsContainerName = "Electrons");
  declareProperty("photonContainerName"           , m_photonsContainerName   = "Photons");
  declareProperty("pixelClusterCollectionName"    , m_pixelClusterKey="PixelClusters" );
  declareProperty("sctClusterCollectionName"      , m_sctClusterKey="SCT_Clusters" );
  declareProperty("trtDriftCirclesCollectionName" , m_trtDriftCircleKey="TRT_DriftCircles" );
  declareProperty("deltaR"                        , m_dr=0.5);
  declareProperty("minEtEg"                       , m_minEtEg = 0 );
  declareProperty("maxDeltaEtaPRD"                , m_maxDeltaEtaPRD = 0.5 );
  declareProperty("maxDeltaPhiPRD"                , m_maxDeltaPhiPRD = 0.5 );
  declareProperty("maxDeltaZPRD"                  , m_maxDeltaZPRD   = 400 ); 
}

StatusCode DerivationFramework::EGammaPrepRawDataThinning::initialize(){

  ATH_CHECK( m_regionSelector.retrieve() );
 
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::EGammaPrepRawDataThinning::finalize(){

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::EGammaPrepRawDataThinning::doThinning() const {

  // retrive the PixelClusterContainer =============================   

  const InDet::PixelClusterContainer* pixelContainer = evtStore()->retrieve<const InDet::PixelClusterContainer> (m_pixelClusterKey);

  if ( !pixelContainer ) 
    {
      ATH_MSG_WARNING( "Container '" << m_pixelClusterKey 
                       << "' could not be retrieved from StoreGate !" 
                       << " Will NOT use this container for thinning in all subsequent processing!!!"
                       << " Please fix your job options..." );
      return StatusCode::FAILURE;   
    } 
  else
    {
      ATH_MSG_DEBUG( "Container '" << m_pixelClusterKey << "' retrieved from StoreGate" );
    }
  
  // retrive the SCT_ClusterContainer =============================

  const InDet::SCT_ClusterContainer* sctContainer = evtStore()->retrieve<const InDet::SCT_ClusterContainer> (m_sctClusterKey);
  if ( !sctContainer ) 
    {
      ATH_MSG_WARNING(  "Container '" << m_sctClusterKey 
                        << "' could not be retrieved from StoreGate !" 
                        << " Will NOT use this container for thinning in all subsequent processing!!!"
                        << " Please fix your job options..." );
      return StatusCode::FAILURE;
    }
  else
    {
      ATH_MSG_DEBUG( "Container '" << m_sctClusterKey << "' retrieved from StoreGate" );
    }
  
  // retrive the TRT_DriftCircleContainer =============================   

  const InDet::TRT_DriftCircleContainer* trtContainer = evtStore()->retrieve<const InDet::TRT_DriftCircleContainer> (m_trtDriftCircleKey);
  if ( !trtContainer ) 
    {
      ATH_MSG_WARNING( "Container '" << m_trtDriftCircleKey 
                       << "' could not be retrieved from StoreGate !" 
                       << " Will NOT use this container for thinning in all subsequent processing!!!"
                       << " Please fix your job options..." );
      return StatusCode::SUCCESS;
    }
  else
    {
      ATH_MSG_DEBUG( "Container '" << m_trtDriftCircleKey << "' retrieved from StoreGate" );
    }
    
  //Retrieve the vertex container to get the position of the vertex =====

  const Trk::Vertex* vertex=0;
  const VxContainer* importedVxContainer = 0;
  static const std::string containerName = "VxPrimaryCandidate";
  Hep3Vector VertexPos(0.0,0.0,0.0);
  if ( evtStore()->contains<VxContainer>(containerName))
    {
      if ( StatusCode::SUCCESS != evtStore()->retrieve(importedVxContainer,containerName))
        {
          // in general this means this container won't be around at all for this run
          ATH_MSG_WARNING( "No " << containerName
                           << " found in StoreGate."
                           << " Will use the default 0,0,0 vertex position instead!" );
          return StatusCode::SUCCESS;
        } 
      else
        {
          //Get the primary vertex 
          vertex = new Trk::RecVertex(importedVxContainer->at(0)->recVertex());
          VertexPos.setX(vertex->position().x());
          VertexPos.setY(vertex->position().y());
          VertexPos.setZ(vertex->position().z());
          
          ATH_MSG_DEBUG( "Primary Vetex found = " << vertex->position().x() 
                        << " " << vertex->position().y() 
                        << " " << vertex->position().z() ) ;
          
        }
    }
  
  // retrieve the electron collection =============================   
  
  const xAOD::ElectronContainer* electronContainer = evtStore()->retrieve < const xAOD::ElectronContainer > (m_electronsContainerName);
  if ( !electronContainer )
    {
      ATH_MSG_WARNING( "Container '" << m_electronsContainerName 
                       << "' could not be retrieved from StoreGate!" ); 
      return StatusCode::FAILURE;
    } 
  else
    {
      ATH_MSG_DEBUG( "Container '" << m_electronsContainerName
                     << "' retrieved from StoreGate" );
    }

   // retrieve the photon collection =============================    

  const xAOD::PhotonContainer* photonContainer = evtStore()->retrieve < const xAOD::PhotonContainer > (m_photonsContainerName);
  if ( !photonContainer )
    {
      ATH_MSG_WARNING( "Container '" << m_photonsContainerName 
                       << "' could not be retrieved from StoreGate!" ); 
      return StatusCode::FAILURE;
    } 
  else
    {
      ATH_MSG_DEBUG( "Container '" << m_photonsContainerName
                     << "' retrieved from StoreGate" );
    }

  // loop over electrons and for each electron mark the interesting tracks ==========
  
  std::vector<IdentifierHash> goodPixelsIDs; 
  std::vector<IdentifierHash> goodSCTClustersIDs; 
  std::vector<IdentifierHash> goodTRTDriftCirclesIDs; 
     
  xAOD::ElectronContainer::const_iterator eleItr    = electronContainer->begin();
  xAOD::ElectronContainer::const_iterator eleItrEnd = electronContainer->end();
  for (; eleItr != eleItrEnd; ++eleItr)
    {
      if ( (*eleItr)->pt() > m_minEtEg ) 
        {
          
          ATH_MSG_DEBUG(  "Electron at eta = " << (*eleItr)->eta() << " phi = " << (*eleItr)->phi() );          
          
          // get the list of pixels clusters collections associated to this object
         
          std::vector<IdentifierHash> el_goodPixelsIDs = findGoodIDObjects(PIXEL, (*eleItr)->p4(), VertexPos,
                                                                           m_maxDeltaEtaPRD,
                                                                           m_maxDeltaPhiPRD,
                                                                           m_maxDeltaZPRD);

          goodPixelsIDs.insert( goodPixelsIDs.end(), el_goodPixelsIDs.begin(), el_goodPixelsIDs.end() );
          ATH_MSG_DEBUG( "Selected Pixels collections (temporary size) ==>> " << goodPixelsIDs.size() );
          
          // get the list of SCT clusters collections associated to this object
         
          std::vector<IdentifierHash> el_goodSCTClustersIDs = findGoodIDObjects(SCT, (*eleItr)->p4(), VertexPos,
                                                                                m_maxDeltaEtaPRD,
                                                                                m_maxDeltaPhiPRD,
                                                                                m_maxDeltaZPRD);
                    
          goodSCTClustersIDs.insert( goodSCTClustersIDs.end(), el_goodSCTClustersIDs.begin(), el_goodSCTClustersIDs.end() );
          ATH_MSG_DEBUG( "Selected SCT collections (temporary size) ==>> " << goodSCTClustersIDs.size() );
          
          // get the list of TRT DriftCircles collections associated to this object

          std::vector<IdentifierHash> el_goodTRTDriftCirclesIDs = findGoodIDObjects(TRT, (*eleItr)->p4(), VertexPos,
                                                                                    m_maxDeltaEtaPRD,
                                                                                    m_maxDeltaPhiPRD,
                                                                                    m_maxDeltaZPRD);
                    
          goodTRTDriftCirclesIDs.insert( goodTRTDriftCirclesIDs.end(), el_goodTRTDriftCirclesIDs.begin(), el_goodTRTDriftCirclesIDs.end() );
          ATH_MSG_DEBUG( "Selected TRT collections (temporary size) ==>> " << goodTRTDriftCirclesIDs.size() );  
          
        }     
    }
  
  // loop over photons and for each photon mark the interesting tracks ==========
  
  xAOD::PhotonContainer::const_iterator phoItr    = photonContainer->begin();
  xAOD::PhotonContainer::const_iterator phoItrEnd = photonContainer->end();
  for (; phoItr != phoItrEnd; ++phoItr)
    {
      if ( (*phoItr)->pt() > m_minEtEg ) 
        {
          
          ATH_MSG_DEBUG(  "Photon at eta = " << (*phoItr)->eta() << " phi = " << (*phoItr)->phi() );          
                      
          // get the list of pixels clusters collections associated to this object
          
          std::vector<IdentifierHash> ph_goodPixelsIDs = findGoodIDObjects(PIXEL, (*phoItr)->p4(), VertexPos,
                                                                           m_maxDeltaEtaPRD,
                                                                           m_maxDeltaPhiPRD,
                                                                           m_maxDeltaZPRD);
          
          goodPixelsIDs.insert( goodPixelsIDs.end(), ph_goodPixelsIDs.begin(), ph_goodPixelsIDs.end() );
          ATH_MSG_DEBUG( "Selected Pixels collections (temporary size) ==>> " << goodPixelsIDs.size() );
          
          // get the list of SCT clusters collections associated to this object
          
          std::vector<IdentifierHash> ph_goodSCTClustersIDs = findGoodIDObjects(SCT, (*phoItr)->p4(), VertexPos,
                                                                                m_maxDeltaEtaPRD,
                                                                                m_maxDeltaPhiPRD,
                                                                                m_maxDeltaZPRD);
                    
          goodSCTClustersIDs.insert( goodSCTClustersIDs.end(), ph_goodSCTClustersIDs.begin(), ph_goodSCTClustersIDs.end() );
          ATH_MSG_DEBUG( "Selected SCT collections (temporary size) ==>> " << goodSCTClustersIDs.size() );
          
          // get the list of TRT DriftCircles collections associated to this object

          std::vector<IdentifierHash> ph_goodTRTDriftCirclesIDs = findGoodIDObjects(TRT, (*phoItr)->p4(), VertexPos,
                                                                                    m_maxDeltaEtaPRD,
                                                                                    m_maxDeltaPhiPRD,
                                                                                    m_maxDeltaZPRD);
                    
          goodTRTDriftCirclesIDs.insert( goodTRTDriftCirclesIDs.end(), ph_goodTRTDriftCirclesIDs.begin(), ph_goodTRTDriftCirclesIDs.end() );
          ATH_MSG_DEBUG( "Selected TRT collections (temporary size) ==>> " << goodTRTDriftCirclesIDs.size() );  
           
        }      
    }
  
  // now do the real thinning and keep all marked elements
    
  ATH_CHECK( thinPixelsClusters( pixelContainer, goodPixelsIDs ) );
  ATH_CHECK( thinSCTClusters( sctContainer, goodSCTClustersIDs ) );
  ATH_CHECK( thinTRTDriftCircles( trtContainer, goodTRTDriftCirclesIDs ) );
  
  // print some statistic

  ATH_MSG_DEBUG( " ======= Egamma PrepRawData thinning statistics: ====================================== ");
  ATH_MSG_DEBUG( " *      Pixels collections   = " << pixelContainer->numberOfCollections()  
                << "   Number of pixel collections kept = " << goodPixelsIDs.size() );
  ATH_MSG_DEBUG( " *      SCT collections      = " << sctContainer->numberOfCollections()    
                << "   Number of SCT collections kept   = " << goodSCTClustersIDs.size() );
  ATH_MSG_DEBUG( " *      TRT collections      = " << trtContainer->numberOfCollections() 
                << "   Number of TRT collections kept   = " << goodTRTDriftCirclesIDs.size() );
  ATH_MSG_DEBUG( " ====================================================================================== ");
    
  return StatusCode::SUCCESS;
}

// ========================================================
// For agiven electron/photon candidate this method
// marks the interesing tracks
// =======================================================

std::vector<IdentifierHash> DerivationFramework::EGammaPrepRawDataThinning::findGoodIDObjects(DETID detectorID,                               
                                                                                              TLorentzVector candHepLorentz,
                                                                                              CLHEP::Hep3Vector primaryVertex,
                                                                                              double maxDeltaEta,
                                                                                              double maxDeltaPhi,
                                                                                              double maxDeltaZ ) const
{
  
  ATH_MSG_DEBUG("===>> Calling findGoodIDObject for detector " << detectorID ) ;
	
  std::vector<IdentifierHash>  listOfHashIDs ;
   
  double roiZMin =  primaryVertex.z() - maxDeltaZ/2.;
  double roiZMax =  primaryVertex.z() + maxDeltaZ/2.;

  //// AAAARGH DON'T USE eta IN THIS WAY !!! 
  double roiEtaMin = candHepLorentz.Eta() - maxDeltaEta/2.;
  double roiEtaMax = candHepLorentz.Eta() + maxDeltaEta/2.;

  double roiPhiMin = candHepLorentz.Phi() - maxDeltaPhi/2.;
  double roiPhiMax = candHepLorentz.Phi() + maxDeltaPhi/2.;

  double roiPhi = candHepLorentz.Phi();

  ATH_MSG_DEBUG (  "ROI range = " << candHepLorentz.Eta() << " " <<  roiEtaMin << " " << roiEtaMax << " " <<
                  roiPhi << " " <<  roiPhiMin << " " <<  roiPhiMax << " " << 
                  primaryVertex.z() << " " <<  roiZMin << " " << roiZMax   ) ;
  
  // I commented this because I think the phicheck is done
  // inside the RoiDescriptor constructor

  // while ( roiPhi<-M_PI ) roiPhi += 2*M_PI;
  // while ( roiPhi> M_PI ) roiPhi -= 2*M_PI;

  //  while ( roiPhiMin<-M_PI ) roiPhiMin += 2*M_PI;
  // while ( roiPhiMin> M_PI ) roiPhiMin -= 2*M_PI;

  // is this correct ?

  //  while ( roiPhiMax<-M_PI ) roiPhiMin += 2*M_PI;
  // while ( roiPhiMax> M_PI ) roiPhiMin -= 2*M_PI;

  //  while ( roiPhiMax<-M_PI ) roiPhiMax += 2*M_PI;
  // while ( roiPhiMax> M_PI ) roiPhiMax -= 2*M_PI;

  RoiDescriptor roi( candHepLorentz.Eta(), roiEtaMin, roiEtaMax, 
		     candHepLorentz.Phi(), roiPhiMin, roiPhiMax, 
		     primaryVertex.z(), roiZMin, roiZMax );

  m_regionSelector->DetHashIDList( detectorID, roi, listOfHashIDs);
 
  return listOfHashIDs;

}

// ========================================================
// 
// =======================================================

StatusCode DerivationFramework::EGammaPrepRawDataThinning::thinPixelsClusters( const InDet::PixelClusterContainer* pixelCont,
                                                                               const std::vector<IdentifierHash> & goodPixelIDs ) const
{
    
  StatusCode sc(StatusCode::SUCCESS);
  
  // First, create the mask to be used for thinning
  IThinningSvc::Filter_t mask;
  
  // Then, iterate over the pixels to figure out which one should be kept
  // Create also some bookkeeping counters
  unsigned long nTotal  = 0;
  unsigned long nKeep   = 0;
  unsigned long nReject = 0;
  unsigned long nClustersTotal  = 0;
  unsigned long nClustersKeep   = 0;
  unsigned long nClustersReject = 0;

  // Loop over the PixelClusterContainer
  InDet::PixelClusterContainer::const_iterator pixelItr    = pixelCont->begin();
  InDet::PixelClusterContainer::const_iterator pixelItrEnd = pixelCont->end();
  for (; pixelItr != pixelItrEnd; ++pixelItr)
    {
      ++nTotal;
      
      ATH_MSG_DEBUG( "There are " << (*pixelItr)->size() 
                     << " entries in the PixelClusterContainer!" );
      const InDet::PixelClusterCollection *colNext = (*pixelItr);
  
      if (!colNext){ continue; }

      // Each PixelClusterContainer contains several hundred collections of the type
      // Vector<PixelCluster>
      // And each of these DataVector<PixelCluster> contains only a few (1-5 or so)
      // PixelClusters. Going to keep the whole DataVector<PixelCluster> if one of
      // its PixelClusters is near the interesting objects!
      
      bool keepThisCollection = false;
      unsigned long nClustersInThisContainer = 0;
      
      nClustersTotal += colNext->size();
      nClustersInThisContainer += colNext->size();
      
      // Determine if this ClusterCollection is interesting and should be kept
      if ( std::find(goodPixelIDs.begin(), goodPixelIDs.end(), colNext->identifyHash()) != goodPixelIDs.end() )
        {
          keepThisCollection = true;
          ++nKeep;
          nClustersKeep+= nClustersInThisContainer;
        }
      else
        {
          ++nReject;
          nClustersReject+= nClustersInThisContainer;
        }
      
      // Fill the thinning mask with the results      
      mask[colNext->identifyHash()] = keepThisCollection;
      
    } // End loop over PixelClusterContainer
  
  // Write out a statistics message
  
  ATH_MSG_DEBUG( " ======= PixelClusterThinning statistics: ====================================== ");
  ATH_MSG_DEBUG( " *      Pixels collections     = " << nTotal  << "    Number of pixel clusters      = " << nClustersTotal );
  ATH_MSG_DEBUG( " *      Pixel collections kept = " << nKeep   <<  "   Number of pixel clusters kept = " << nClustersKeep );
  ATH_MSG_DEBUG( " *      Pixel collections reje = " << nReject <<  "   Number of pixel clusters reje = " << nClustersReject );
  ATH_MSG_DEBUG( " ======================================== ====================================== ");
    
  // Perform the actual thinning
  ATH_CHECK( m_thinningSvc->filter(*pixelCont, mask, IThinningSvc::Operator::Or) );
  
  return sc;
  
}

// ========================================================
// 
// =======================================================

StatusCode DerivationFramework::EGammaPrepRawDataThinning::thinSCTClusters( const InDet::SCT_ClusterContainer* sctCont, 
                                                                            const std::vector<IdentifierHash> & goodSCTClustersIDs) const
{
    
  StatusCode sc(StatusCode::SUCCESS);
  
   // First, create the mask to be used for thinning
  IThinningSvc::Filter_t mask;
  
  // Then, iterate over the SCT_Clusters to figure out which one should be kept
  // Create also some bookkeeping counters
  unsigned long nTotal  = 0;
  unsigned long nKeep   = 0;
  unsigned long nReject = 0;
  unsigned long nClustersTotal  = 0;
  unsigned long nClustersKeep   = 0;
  unsigned long nClustersReject = 0;
  
  // Loop over the SCT_ClusterContainer
  InDet::SCT_ClusterContainer::const_iterator sctItr    = sctCont->begin();
  InDet::SCT_ClusterContainer::const_iterator sctItrEnd = sctCont->end();
  for (; sctItr != sctItrEnd; ++sctItr)
    {
      ++nTotal;
      ATH_MSG_DEBUG( "There are " << (*sctItr)->size() 
                     << " entries in the SCT_ClusterContainer!" );
      const InDet::SCT_ClusterCollection *colNext = (*sctItr);
      
      if (!colNext){ continue; }
      
      // Each SCT_ClusterContainer contains several hundred collections of the type
      // DataVector<SCT_Cluster>
      // And each of these DataVector<SCT_Cluster> contains only a few (1-5 or so)
      // SCT_Clusters. Going to keep the whole DataVector<SCT_Cluster> if one of
      // its SCT_Clusters is near the interesting objects!
      
      bool keepThisCollection = false;
      unsigned long nClustersInThisContainer = 0;
      
      nClustersTotal += colNext->size();
      nClustersInThisContainer += colNext->size();

      // Determine if this ClusterCollection is interesting and should be kept
      if (std::find(goodSCTClustersIDs.begin(), goodSCTClustersIDs.end(), colNext->identifyHash()) != goodSCTClustersIDs.end())
        {
          keepThisCollection = true;
          ++nKeep;
          nClustersKeep+= nClustersInThisContainer;
        }
      else
        {
          ++nReject;
          nClustersReject+= nClustersInThisContainer;
        }
      
      // Fill the thinning mask with the results
      mask[colNext->identifyHash()] = keepThisCollection;
    } // End loop over SCT_ClusterContainer
    // Update the counters

  // Write out a statistics message

  ATH_MSG_DEBUG( " ======= SCTClusterThinning statistics: ====================================== ");
  ATH_MSG_DEBUG( " *      SCT collections      = " << nTotal  << "    Number of SCT clusters      = " << nClustersTotal );
  ATH_MSG_DEBUG( " *      SCT collections kept = " << nKeep   <<  "   Number of SCT clusters kept = " << nClustersKeep );
  ATH_MSG_DEBUG( " *      SCT collections reje = " << nReject <<  "   Number of SCT clusters reje = " << nClustersReject );
  ATH_MSG_DEBUG( " ======================================== ====================================== ");
      
  // Perform the actual thinning

  ATH_CHECK( m_thinningSvc->filter(*sctCont, mask, IThinningSvc::Operator::Or) );
  
  return sc;    
    
}

// ========================================================
// 
// =======================================================

StatusCode DerivationFramework::EGammaPrepRawDataThinning::thinTRTDriftCircles( const InDet::TRT_DriftCircleContainer* trtCont,
                                                                                const std::vector<IdentifierHash> & goodTRTDriftCirclesIDs) const
{
    
  StatusCode sc(StatusCode::SUCCESS);
  
  IThinningSvc::Filter_t mask;

  unsigned long nTotal  = 0;
  unsigned long nKeep   = 0;
  unsigned long nReject = 0;
  unsigned long nClustersTotal  = 0;
  unsigned long nClustersKeep   = 0;
  unsigned long nClustersReject = 0;

  // Loop over the SCT_ClusterContainer
  InDet::TRT_DriftCircleContainer::const_iterator trtItr    = trtCont->begin();
  InDet::TRT_DriftCircleContainer::const_iterator trtItrEnd = trtCont->end();
  for (; trtItr != trtItrEnd; ++trtItr)
    {
      ++nTotal;
      ATH_MSG_DEBUG( "There are " << (*trtItr)->size() 
                     << " entries in the TRT_DriftCircleContainer!" );
      const InDet::TRT_DriftCircleCollection *colNext = (*trtItr);
      
      if (!colNext){ continue; }
      
      // Each TRT_DriftCircleContainer contains several hundred collections of the type
      // DataVector<TRT_DriftCircle>
      // And each of these DataVector<TRT_DriftCircle> contains only a few (1-5 or so)
      // TRT_DriftCircles. Going to keep the whole DataVector<TRT_DriftCircle> if one of
      // its TRT_DriftCircle is near the interesting objects!
      
      bool keepThisCollection = false;
      unsigned long nClustersInThisContainer = 0;

      nClustersTotal += colNext->size();
      nClustersInThisContainer += colNext->size();

      // Determine if this ClusterCollection is interesting and should be kept
      if (std::find(goodTRTDriftCirclesIDs.begin(), goodTRTDriftCirclesIDs.end(), colNext->identifyHash()) != goodTRTDriftCirclesIDs.end())
        {
          keepThisCollection = true;
          ++nKeep;
          nClustersKeep+= nClustersInThisContainer;
        }
      else
        {
          keepThisCollection = false;
          ++nReject;
          nClustersReject+= nClustersInThisContainer;
        }
      // Fill the thinning mask with the results
      mask[colNext->identifyHash()] = keepThisCollection;
    } // End loop over TRT_DriftCircles

  // Write out a statistics message
  
    // Write out a statistics message

  ATH_MSG_DEBUG( " ======= TRTClusterThinning statistics: ====================================== ");
  ATH_MSG_DEBUG( " *      TRT collections      = " << nTotal  << "    Number of TRT clusters      = " << nClustersTotal );
  ATH_MSG_DEBUG( " *      TRT collections kept = " << nKeep   <<  "   Number of TRT clusters kept = " << nClustersKeep );
  ATH_MSG_DEBUG( " *      TRT collections reje = " << nReject <<  "   Number of TRT clusters reje = " << nClustersReject );
  ATH_MSG_DEBUG( " ======================================== ====================================== ");
  
  // Perform the actual thinning
  ATH_CHECK( m_thinningSvc->filter(*trtCont, mask, IThinningSvc::Operator::Or) );
    
  return sc;    
    
}
