/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelPrepDataToxAOD.cxx
//   Implementation file for class PixelPrepDataToxAOD
///////////////////////////////////////////////////////////////////

#include "PixelPrepDataToxAOD.h"

#include "InDetPrepRawData/PixelClusterContainer.h"

#include "xAODTracking/TrackMeasurementValidation.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTracking/TrackMeasurementValidationAuxContainer.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"


#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimData/InDetSimDataCollection.h"


#include "TMath.h" 
#include "CLHEP/Geometry/Point3D.h"

#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"

#define AUXDATA(OBJ, TYP, NAME) \
  static const SG::AuxElement::Accessor<TYP> acc_##NAME (#NAME);  acc_##NAME(*(OBJ))

/////////////////////////////////////////////////////////////////////
//
//         Constructor with parameters:
//
/////////////////////////////////////////////////////////////////////
PixelPrepDataToxAOD::PixelPrepDataToxAOD(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_PixelHelper(0),
  m_useSiHitsGeometryMatching(true),
  m_calibSvc("PixelCalibSvc", name),
  m_pixelBSErrorsSvc("PixelByteStreamErrorsSvc", name),
  m_firstEventWarnings(true),
  m_need_sihits{false}
{ 
  // --- Steering and configuration flags
 
  declareProperty("UseTruthInfo", m_useTruthInfo=false);
  declareProperty("UseSiHitsGeometryMatching", m_useSiHitsGeometryMatching=true);
  declareProperty("WriteSDOs", m_writeSDOs = true);
  declareProperty("WriteSiHits", m_writeSiHits = true);
  declareProperty("WriteNNinformation", m_writeNNinformation = true);
  declareProperty("WriteRDOinformation", m_writeRDOinformation = true);

  // --- Configuration keys
  declareProperty("SiClusterContainer",  m_clustercontainer_key = "PixelClusters");
  declareProperty("MC_SDOs", m_SDOcontainer_key = "PixelSDO_Map");
  declareProperty("MC_Hits", m_sihitContainer_key = "PixelHits");
  declareProperty("PRD_MultiTruth", m_multiTruth_key = "PRD_MultiTruthPixel");

  // --- Services and Tools
  declare(m_write_xaod);
  declare(m_write_offsets);

}

/////////////////////////////////////////////////////////////////////
//
//        Initialize method: 
//
/////////////////////////////////////////////////////////////////////
StatusCode PixelPrepDataToxAOD::initialize()
{
  ATH_CHECK( detStore()->retrieve(m_PixelHelper, "PixelID") );

  //make sure we don't write what we don't have
  if (not m_useTruthInfo) {
    m_writeSDOs = false;
    m_writeSiHits = false;
  }

  CHECK(m_calibSvc.retrieve());

  ATH_CHECK(m_condDCSStateKey.initialize());
  ATH_CHECK(m_condDCSStatusKey.initialize());
  ATH_CHECK(m_readKeyTemp.initialize());
  ATH_CHECK(m_readKeyHV.initialize());

  CHECK(m_pixelBSErrorsSvc.retrieve());

  ATH_CHECK(m_lorentzAngleTool.retrieve());

  ATH_CHECK(m_clustercontainer_key.initialize());
  m_need_sihits = (m_writeNNinformation || m_writeSiHits) && m_useTruthInfo;
  ATH_CHECK(m_sihitContainer_key.initialize(m_need_sihits));
  ATH_CHECK(m_SDOcontainer_key.initialize(m_writeSDOs));
  ATH_CHECK(m_multiTruth_key.initialize(m_useTruthInfo));

  m_write_xaod = m_clustercontainer_key.key();
  ATH_CHECK(m_write_xaod.initialize());
  m_write_offsets = m_clustercontainer_key.key() + "Offsets";
  ATH_CHECK(m_write_offsets.initialize());

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////
//
//        Execute method: 
//
/////////////////////////////////////////////////////////////////////
StatusCode PixelPrepDataToxAOD::execute() 
{
  //Mandatory. Require if the algorithm is scheduled.
  SG::ReadHandle<InDet::PixelClusterContainer> PixelClusterContainer(m_clustercontainer_key);
  
  if ( !PixelClusterContainer.isValid() )
  {
      ATH_MSG_ERROR("Failed to retrieve PixelClusterContainer with key" << PixelClusterContainer.key() );
      return StatusCode::FAILURE;
  }
  
  // Create the xAOD container and its auxiliary store:
  SG::WriteHandle<xAOD::TrackMeasurementValidationContainer> xaod(m_write_xaod);
  ATH_CHECK(xaod.record(std::make_unique<xAOD::TrackMeasurementValidationContainer>(),
			std::make_unique<xAOD::TrackMeasurementValidationAuxContainer>()));

  SG::WriteHandle<std::vector<unsigned int>> offsets(m_write_offsets);
  ATH_CHECK(offsets.record(std::make_unique<std::vector<unsigned int>>(m_PixelHelper->wafer_hash_max(), 0)));
  
  // Loop over the container
  unsigned int counter(0);
  
  for( const auto& clusterCollection : * PixelClusterContainer ){

    //Fill Offset container
    (*offsets)[clusterCollection->identifyHash()] = counter;

    // skip empty collections
    if( clusterCollection->empty() ) continue;
    
    // loop over collection and convert to xAOD
    for( const InDet::PixelCluster* prd : *clusterCollection ){    
      ++counter;

      Identifier clusterId = prd->identify();
      if ( !clusterId.is_valid() ) {
        ATH_MSG_WARNING("Pixel cluster identifier is not valid");
      }
            
      // create and add xAOD object
      xAOD::TrackMeasurementValidation* xprd = new xAOD::TrackMeasurementValidation();
      xaod->push_back(xprd);
      
      //Set Identifier
      xprd->setIdentifier( clusterId.get_compact() );

      //Set Global Position
      Amg::Vector3D gpos = prd->globalPosition();
      xprd->setGlobalPosition(gpos.x(),gpos.y(),gpos.z());

      //Set Local Position
      const Amg::Vector2D& locpos = prd->localPosition();

      // Set local error matrix
      xprd->setLocalPosition( locpos.x(),  locpos.y() ); 
      
      const Amg::MatrixX& localCov = prd->localCovariance();
      //std::cout << localCov <<  std::endl;
      if(localCov.size() == 1){
        //std::cout << "Size  == 1" << std::endl;
        xprd->setLocalPositionError( localCov(0,0), 0., 0. ); 
      } else if(localCov.size() == 4){
        //std::cout << "Size  == 2" << std::endl;
        xprd->setLocalPositionError( localCov(0,0), localCov(1,1), localCov(0,1) );     
      } else {
        //std::cout << "Size  == "<< localCov.size() << std::endl;
        xprd->setLocalPositionError(0.,0.,0.);
      }

      // Set vector of hit identifiers
      std::vector< uint64_t > rdoIdentifierList;
      for( const auto &hitIdentifier : prd->rdoList() ){
        rdoIdentifierList.push_back( hitIdentifier.get_compact() );
        //May want to addinformation about the individual hits here
      }
      xprd->setRdoIdentifierList(rdoIdentifierList);

      //Add pixel cluster properties
      AUXDATA(xprd,int,bec)          =   m_PixelHelper->barrel_ec(clusterId)   ;
      AUXDATA(xprd,int,layer)        =   m_PixelHelper->layer_disk(clusterId)  ;   
      AUXDATA(xprd,int,phi_module)   =   m_PixelHelper->phi_module(clusterId)  ;
      AUXDATA(xprd,int,eta_module)   =   m_PixelHelper->eta_module(clusterId)  ;
         
      //AUXDATA(xprd,int,col)         =  m_PixelHelper->eta_index(clusterId);
      //AUXDATA(xprd,int,row)         =  m_PixelHelper->phi_index(clusterId);
      AUXDATA(xprd,int,eta_pixel_index)         =  m_PixelHelper->eta_index(clusterId);
      AUXDATA(xprd,int,phi_pixel_index)         =  m_PixelHelper->phi_index(clusterId);
   

      const InDet::SiWidth cw = prd->width();
      AUXDATA(xprd,int,sizePhi) = (int)cw.colRow()[0];
      AUXDATA(xprd,int,sizeZ)   = (int)cw.colRow()[1];
      AUXDATA(xprd,int,nRDO)    = (int)prd->rdoList().size();
   
      AUXDATA(xprd,float,charge)  =  prd->totalCharge(); 
      AUXDATA(xprd,int,ToT)       =  prd->totalToT(); 
      AUXDATA(xprd,int,LVL1A)     =  prd->LVL1A(); 
   
      AUXDATA(xprd,char,isFake)      =  (char)prd->isFake(); 
      AUXDATA(xprd,char,gangedPixel) =  (char)prd->gangedPixel(); 
      AUXDATA(xprd,int,isSplit)      =  (int)prd->isSplit(); 
      AUXDATA(xprd,float,splitProbability1)  =  prd->splitProbability1(); 
      AUXDATA(xprd,float,splitProbability2)  =  prd->splitProbability2(); 

      // Need to add something to Add the NN splitting information
      if(m_writeNNinformation) addNNInformation( xprd,  prd, 7, 7);
      
      // Add information for each contributing hit
      if(m_writeRDOinformation) {
        IdentifierHash moduleHash = clusterCollection->identifyHash();
        AUXDATA(xprd,int,isBSError) = (int)m_pixelBSErrorsSvc->isActive(moduleHash);
        AUXDATA(xprd,int,DCSState) = SG::ReadCondHandle<PixelModuleData>(m_condDCSStateKey)->getModuleStatus(moduleHash);

        float temperature = 0.0;
        SG::ReadCondHandle<PixelDCSConditionsData>(m_readKeyTemp)->getValue(moduleHash,temperature);

        float deplVoltage = 0.0;

        float biasVoltage = 0.0;
        SG::ReadCondHandle<PixelDCSConditionsData>(m_readKeyHV)->getValue(moduleHash,biasVoltage);

        AUXDATA(xprd,float,BiasVoltage) = biasVoltage;
        AUXDATA(xprd,float,Temperature) = temperature;
        AUXDATA(xprd,float,DepletionVoltage) = deplVoltage;

        AUXDATA(xprd,float,LorentzShift) = (float)m_lorentzAngleTool->getLorentzShift(moduleHash);

        addRdoInformation(xprd,  prd);
      } 
  
  
      // Add the Detector element ID  --  not sure if needed as we have the informations above
      const InDetDD::SiDetectorElement* de = prd->detectorElement();
      uint64_t detElementId(0);
      if(de){
        Identifier detId = de->identify();
        if ( detId.is_valid() ) {
          detElementId = detId.get_compact();
        }
      }
      AUXDATA(xprd,uint64_t,detectorElementID) = detElementId;
      
      // Use the MultiTruth Collection to get a list of all true particle contributing to the cluster
      if (m_useTruthInfo) {
	SG::ReadHandle<PRD_MultiTruthCollection> prdmtColl(m_multiTruth_key);
        std::vector<int> barcodes;
        auto range = prdmtColl->equal_range(clusterId);
        for (auto i = range.first; i != range.second; ++i) {
	  barcodes.push_back( i->second.barcode() );
        }
        AUXDATA(xprd,std::vector<int>, truth_barcode) = barcodes;
      }
      
      std::vector< std::vector< int > > sdo_tracks;
      // Use the SDO Collection to get a list of all true particle contributing to the cluster per readout element
      //  Also get the energy deposited by each true particle per readout element   
      if(m_writeSDOs) {
	SG::ReadHandle<InDetSimDataCollection> sdoCollection(m_SDOcontainer_key);
	if ( sdoCollection.isValid() )
	{
	    sdo_tracks = addSDOInformation(xprd, prd, *sdoCollection);
	}
	else if ( m_firstEventWarnings )
	{
	    ATH_MSG_WARNING("SDO information requested, but SDO collection not available!");
	}
      }
    
      // Now Get the most detailed truth from the SiHits
      // Note that this could get really slow if there are a lot of hits and clusters
      if (m_need_sihits) {
	SG::ReadHandle<SiHitCollection> sihitCollection(m_sihitContainer_key);
	if ( sihitCollection.isValid() )
	{
	    const std::vector<SiHit> matched_hits = findAllHitsCompatibleWithCluster(prd, *sihitCollection, sdo_tracks);
	    
	    if (m_writeSiHits) {
		if (!m_writeSDOs)
		    ATH_MSG_WARNING("Si hit truth information requested, but SDO collection not available!");
		addSiHitInformation(xprd, prd, matched_hits); 
	    }
	    
	    if (m_writeNNinformation) {
		if (!m_writeSDOs)
		    ATH_MSG_WARNING("Si hit truth information requested, but SDO collection not available!");
		addNNTruthInfo(xprd, prd, matched_hits);
	    }
	}
	else if ( m_firstEventWarnings )
	{
	    ATH_MSG_WARNING("SiHit information requested, but SiHit collection not available!");
	}
      }
    }
  }
  
  for ( auto clusItr = xaod->begin(); clusItr != xaod->end(); clusItr++ ) {
      AUXDATA(*clusItr,char,broken) = false;
  }

  static const SG::AuxElement::Accessor<int> acc_layer ("layer");
  static const SG::AuxElement::Accessor<int> acc_phi_module ("phi_module");
  static const SG::AuxElement::Accessor<int> acc_eta_module ("eta_module");
  static const SG::AuxElement::Accessor<std::vector<int> > acc_sihit_barcode ("sihit_barcode");
  for ( auto clusItr = xaod->begin(); clusItr != xaod->end(); clusItr++ )
  {
      auto pixelCluster = *clusItr;
      int layer = acc_layer(*pixelCluster);
      std::vector<int> barcodes = acc_sihit_barcode(*pixelCluster);

      for ( auto clusItr2 = clusItr + 1; clusItr2 != xaod->end(); clusItr2++ )
      {
	  auto pixelCluster2 = *clusItr2;
	  if ( acc_layer(*pixelCluster2) != layer )
	      continue;
	  if ( acc_eta_module(*pixelCluster) != acc_eta_module(*pixelCluster2) )
	      continue;
	  if ( acc_phi_module(*pixelCluster) != acc_phi_module(*pixelCluster2) )
	      continue;

	  std::vector<int> barcodes2 = acc_sihit_barcode(*pixelCluster2);
	  
	  bool broken = false;
	  for ( auto bc : barcodes )
	  {
	      for ( auto bc2 : barcodes2 )
	      {
		  if ( bc2 == bc )
		  {
		      broken = true;
                      static const SG::AuxElement::Accessor<char> acc_broken ("broken");
		      acc_broken(*pixelCluster)  = true;
		      acc_broken(*pixelCluster2) = true;
		      break;
		  }
	      }
	      if ( broken ) 
		  break;
	  }
      }
  }

  ATH_MSG_DEBUG( " recorded PixelPrepData objects: size " << xaod->size() );

  m_firstEventWarnings = false;

  return StatusCode::SUCCESS;
}


std::vector< std::vector< int > > PixelPrepDataToxAOD::addSDOInformation( xAOD::TrackMeasurementValidation* xprd,
									  const InDet::PixelCluster* prd,
									  const InDetSimDataCollection& sdoCollection ) const
{
  std::vector<int> sdo_word;
  std::vector< std::vector< int > > sdo_depositsBarcode;
  std::vector< std::vector< float > > sdo_depositsEnergy;
  // find hit
  for( const auto &hitIdentifier : prd->rdoList() ){
    auto pos = sdoCollection.find(hitIdentifier);
    if( pos != sdoCollection.end() ) {
      sdo_word.push_back( pos->second.word() ) ;
      
      std::vector< int > sdoDepBC;
      std::vector< float > sdoDepEnergy;
      for( auto deposit : pos->second.getdeposits() ){
        if(deposit.first){
          sdoDepBC.push_back( deposit.first->barcode());
        } else {
          sdoDepBC.push_back( -1 );   
        }
        sdoDepEnergy.push_back( deposit.second  );
        ATH_MSG_DEBUG(" SDO Energy Deposit " << deposit.second  ) ;
      }
      sdo_depositsBarcode.push_back( sdoDepBC );
      sdo_depositsEnergy.push_back( sdoDepEnergy );
    }
  }
  AUXDATA(xprd,std::vector<int>,sdo_words)  = sdo_word;
  AUXDATA(xprd,std::vector< std::vector<int> >,sdo_depositsBarcode)  = sdo_depositsBarcode;
  AUXDATA(xprd,std::vector< std::vector<float> >,sdo_depositsEnergy) = sdo_depositsEnergy;
  
  return sdo_depositsBarcode;
}



void  PixelPrepDataToxAOD::addSiHitInformation( xAOD::TrackMeasurementValidation* xprd, 
						const InDet::PixelCluster* prd,
						const std::vector<SiHit> & matchingHits ) const
{

  int numHits = matchingHits.size();

  std::vector<float> sihit_energyDeposit(numHits,0);
  std::vector<float> sihit_meanTime(numHits,0);
  std::vector<int>   sihit_barcode(numHits,0);
  std::vector<int>   sihit_pdgid(numHits,0);
  
  std::vector<float> sihit_startPosX(numHits,0);
  std::vector<float> sihit_startPosY(numHits,0);
  std::vector<float> sihit_startPosZ(numHits,0);

  std::vector<float> sihit_endPosX(numHits,0);
  std::vector<float> sihit_endPosY(numHits,0);
  std::vector<float> sihit_endPosZ(numHits,0);

  int hitNumber(0);
  const InDetDD::SiDetectorElement* de = prd->detectorElement();
  if(de){
    for ( auto sihit : matchingHits ) {          
      sihit_energyDeposit[hitNumber] =  sihit.energyLoss() ;
      sihit_meanTime[hitNumber] =  sihit.meanTime() ;
      sihit_barcode[hitNumber] =  sihit.particleLink().barcode() ;
      if(sihit.particleLink().isValid()){
        sihit_pdgid[hitNumber]   = sihit.particleLink()->pdg_id();
      }
    
      // Convert Simulation frame into reco frame
      const HepGeom::Point3D<double>& startPos=sihit.localStartPosition();

      Amg::Vector2D pos= de->hitLocalToLocal( startPos.z(), startPos.y() );
      sihit_startPosX[hitNumber] =  pos[0];
      sihit_startPosY[hitNumber] =  pos[1];
      sihit_startPosZ[hitNumber] =  startPos.x();
 

      const HepGeom::Point3D<double>& endPos=sihit.localEndPosition();
      pos= de->hitLocalToLocal( endPos.z(), endPos.y() );
      sihit_endPosX[hitNumber] =  pos[0];
      sihit_endPosY[hitNumber] =  pos[1];
      sihit_endPosZ[hitNumber] =  endPos.x();
      ++hitNumber;
    }
  }

  AUXDATA(xprd,std::vector<float>,sihit_energyDeposit) = sihit_energyDeposit;
  AUXDATA(xprd,std::vector<float>,sihit_meanTime) = sihit_meanTime;
  AUXDATA(xprd,std::vector<int>,sihit_barcode) = sihit_barcode;
  AUXDATA(xprd,std::vector<int>,sihit_pdgid) = sihit_pdgid;
  
  AUXDATA(xprd,std::vector<float>,sihit_startPosX) = sihit_startPosX;
  AUXDATA(xprd,std::vector<float>,sihit_startPosY) = sihit_startPosY;
  AUXDATA(xprd,std::vector<float>,sihit_startPosZ) = sihit_startPosZ;

  AUXDATA(xprd,std::vector<float>,sihit_endPosX) = sihit_endPosX;
  AUXDATA(xprd,std::vector<float>,sihit_endPosY) = sihit_endPosY;
  AUXDATA(xprd,std::vector<float>,sihit_endPosZ) = sihit_endPosZ;


}






std::vector<SiHit> PixelPrepDataToxAOD::findAllHitsCompatibleWithCluster( const InDet::PixelCluster* prd, 
                                                                          const SiHitCollection& collection,
									  std::vector< std::vector< int > > & trkBCs ) const
{
  ATH_MSG_VERBOSE( "Got " << collection.size() << " SiHits to look through" );
  std::vector<SiHit>  matchingHits;
    
  // Check if we have detector element  --  needed to find the local position of the SiHits
  const InDetDD::SiDetectorElement* de = prd->detectorElement();
  if(!de)
    return matchingHits;

  std::vector<const SiHit* >  multiMatchingHits;
  
  for ( const auto&  siHit : collection) {
    // Check if it is a Pixel hit
    if( !siHit.isPixel() )
      continue;
    
    //Check if it is on the correct module
    Identifier clusterId = prd->identify();
  
    if( m_PixelHelper->barrel_ec(clusterId) != siHit.getBarrelEndcap() ||
        m_PixelHelper->layer_disk(clusterId)!= siHit.getLayerDisk()    ||  
        m_PixelHelper->phi_module(clusterId)!= siHit.getPhiModule()    ||
        m_PixelHelper->eta_module(clusterId)!= siHit.getEtaModule() ) 
       continue;

    // Now we have all hits in the module that match lets check to see if they match the cluster
    // Must be within +/- 1 hits of any hit in the cluster to be included
    ATH_MSG_DEBUG("Hit is on the same module");
    
    if ( m_useSiHitsGeometryMatching )
    {
	HepGeom::Point3D<double>  averagePosition =  siHit.localStartPosition() + siHit.localEndPosition();
	averagePosition *= 0.5;
	Amg::Vector2D pos = de->hitLocalToLocal( averagePosition.z(), averagePosition.y() );
	InDetDD::SiCellId diode = de->cellIdOfPosition(pos);
	
	for( const auto &hitIdentifier : prd->rdoList() ){
	    ATH_MSG_DEBUG("Truth Phi " <<  diode.phiIndex() << " Cluster Phi " <<   m_PixelHelper->phi_index( hitIdentifier ) );
	    ATH_MSG_DEBUG("Truth Eta " <<  diode.etaIndex() << " Cluster Eta " <<   m_PixelHelper->eta_index( hitIdentifier ) );
	    if( abs( int(diode.etaIndex()) - m_PixelHelper->eta_index( hitIdentifier ) ) <=1  
		&& abs( int(diode.phiIndex()) - m_PixelHelper->phi_index( hitIdentifier ) ) <=1 ) 
	    {
		multiMatchingHits.push_back(&siHit);   
		break;
	    }
	}
    }
    else
    {
	bool foundHit = false;
	for ( const auto barcodeSDOColl : trkBCs )
	{
	    for ( const auto barcode : barcodeSDOColl )
	    {
		if ( siHit.particleLink().barcode() == barcode )
		{
		    multiMatchingHits.push_back(&siHit);   
		    foundHit = true;
		    break;   
		}
	    }
	    if ( foundHit )
		break;
	}
    }
  }
  //Now we will now make 1 SiHit for each true particle if the SiHits "touch" other 
  std::vector<const SiHit* >::iterator siHitIter  = multiMatchingHits.begin();
  std::vector<const SiHit* >::iterator siHitIter2 = multiMatchingHits.begin();
  ATH_MSG_DEBUG( "Found " << multiMatchingHits.size() << " SiHit " );
  for ( ; siHitIter != multiMatchingHits.end(); siHitIter++) {
    const SiHit* lowestXPos  = *siHitIter;
    const SiHit* highestXPos = *siHitIter;


    // We will merge these hits
    std::vector<const SiHit* > ajoiningHits;
    ajoiningHits.push_back( *siHitIter );
    
    siHitIter2 = siHitIter+1;    
    while ( siHitIter2 != multiMatchingHits.end() ) {
      // Need to come from the same truth particle 
            
      if( (*siHitIter)->particleLink().barcode() != (*siHitIter2)->particleLink().barcode() ){
        ++siHitIter2;
        continue;
      }
      
      // Check to see if the SiHits are compatible with each other.
      if (fabs((highestXPos->localEndPosition().x()-(*siHitIter2)->localStartPosition().x()))<0.00005 &&
          fabs((highestXPos->localEndPosition().y()-(*siHitIter2)->localStartPosition().y()))<0.00005 &&
          fabs((highestXPos->localEndPosition().z()-(*siHitIter2)->localStartPosition().z()))<0.00005 )
      {
        highestXPos = *siHitIter2;
        ajoiningHits.push_back( *siHitIter2 );
        // Dont use hit  more than once
        siHitIter2 = multiMatchingHits.erase( siHitIter2 );
      }else if (fabs((lowestXPos->localStartPosition().x()-(*siHitIter2)->localEndPosition().x()))<0.00005 &&
                fabs((lowestXPos->localStartPosition().y()-(*siHitIter2)->localEndPosition().y()))<0.00005 &&
                fabs((lowestXPos->localStartPosition().z()-(*siHitIter2)->localEndPosition().z()))<0.00005)
      {
        lowestXPos = *siHitIter2;
        ajoiningHits.push_back( *siHitIter2 );
        // Dont use hit  more than once
        siHitIter2 = multiMatchingHits.erase( siHitIter2 );
      } else {
        ++siHitIter2;
      }
    }
    
    if( ajoiningHits.size() == 0){
      ATH_MSG_WARNING("This should really never happen");
      continue;
    }else if(ajoiningHits.size() == 1){
      // Copy Si Hit ready to return
      matchingHits.push_back( *ajoiningHits[0] );
      continue;
    } else {
    //  Build new SiHit and merge information together.  
      ATH_MSG_DEBUG("Merging " << ajoiningHits.size() << " SiHits together." );
      
      
      float energyDep(0);
      float time(0);
      for( auto& siHit :  ajoiningHits){
        energyDep += siHit->energyLoss();
        time += siHit->meanTime();    
      }
      time /= (float)ajoiningHits.size();
       
      matchingHits.push_back(  SiHit(lowestXPos->localStartPosition(), 
                                     highestXPos->localEndPosition(),
                                     energyDep,
                                     time,
                                     (*siHitIter)->particleLink().barcode(),
                                     0, // 0 for pixel 1 for Pixel
                                     (*siHitIter)->getBarrelEndcap(),
                                     (*siHitIter)->getLayerDisk(),
                                     (*siHitIter)->getEtaModule(),
                                     (*siHitIter)->getPhiModule(),
                                     (*siHitIter)->getSide() ) );
     ATH_MSG_DEBUG("Finished Merging " << ajoiningHits.size() << " SiHits together." );

    }
  } 
  

  return matchingHits;
  
}

void PixelPrepDataToxAOD::addRdoInformation(xAOD::TrackMeasurementValidation* xprd, 
                                           const InDet::PixelCluster* pixelCluster ) const
{
  ATH_MSG_VERBOSE( " Starting creating input from cluster "   );


  const std::vector<Identifier>& rdos  = pixelCluster->rdoList();  

  const std::vector<float> chList     = pixelCluster->chargeList();
  const std::vector<int>  totList     = pixelCluster->totList();

  // std::vector<int>  rowList;
  // std::vector<int>  colList;
  std::vector<int>  etaIndexList;
  std::vector<int>  phiIndexList;
  std::vector<float> CTerm;
  std::vector<float> ATerm;
  std::vector<float> ETerm;


  ATH_MSG_VERBOSE( "Number of RDOs: " << rdos.size() );
  
  //Itererate over all elements hits in the cluster and fill the charge and tot matricies 
  std::vector<Identifier>::const_iterator rdosBegin = rdos.begin();
  std::vector<Identifier>::const_iterator rdosEnd = rdos.end();

  ATH_MSG_VERBOSE(" Putting together the n. " << rdos.size() << " rdos into a matrix.");

  for (; rdosBegin!= rdosEnd; ++rdosBegin)
  {
    Identifier rId =  *rdosBegin;
    // rowList.push_back( m_PixelHelper->phi_index(rId) );
    // colList.push_back( m_PixelHelper->eta_index(rId) );  
    phiIndexList.push_back( m_PixelHelper->phi_index(rId) );
    etaIndexList.push_back( m_PixelHelper->eta_index(rId) );  

    // charge calibration parameters
    CTerm.push_back( m_calibSvc->getQ2TotC(rId) );
    ATerm.push_back( m_calibSvc->getQ2TotA(rId) );
    ETerm.push_back( m_calibSvc->getQ2TotE(rId) );

  }//end iteration on rdos


  // AUXDATA(xprd, std::vector<int>, rdo_row)  = rowList;
  // AUXDATA(xprd, std::vector<int>, rdo_col)  = colList;
  AUXDATA(xprd, std::vector<int>,rdo_phi_pixel_index)  = phiIndexList;
  AUXDATA(xprd, std::vector<int>,rdo_eta_pixel_index)  = etaIndexList;
  AUXDATA(xprd, std::vector<float>,rdo_charge)  = chList;
  AUXDATA(xprd, std::vector<int>,rdo_tot)  = totList;
  
  AUXDATA(xprd, std::vector<float>,rdo_Cterm) = CTerm;
  AUXDATA(xprd, std::vector<float>,rdo_Aterm) = ATerm;
  AUXDATA(xprd, std::vector<float>,rdo_Eterm) = ETerm;

}


void PixelPrepDataToxAOD::addNNInformation(xAOD::TrackMeasurementValidation* xprd, 
                                           const InDet::PixelCluster* pixelCluster, 
                                           const unsigned int sizeX, const unsigned int sizeY ) const
{
  ATH_MSG_VERBOSE( " Starting creating input from cluster "   );

  const InDetDD::SiDetectorElement* de = pixelCluster->detectorElement();
  if (de==0) {
    ATH_MSG_ERROR("Could not get detector element");
    return;
  }


  const InDetDD::PixelModuleDesign* design(dynamic_cast<const InDetDD::PixelModuleDesign*>(&de->design()));
	if (not design) {
		ATH_MSG_WARNING("PixelModuleDesign was not retrieved in function 'addNNInformation'");
		return;
	}
  const std::vector<Identifier>& rdos  = pixelCluster->rdoList();  

  const std::vector<float>& chList     = pixelCluster->chargeList();
  const std::vector<int>&  totList     = pixelCluster->totList();

  ATH_MSG_VERBOSE( "Number of RDOs: " << rdos.size() );
  ATH_MSG_VERBOSE( "Number of charges: " << chList.size() );
  ATH_MSG_VERBOSE( "Number of TOT: " << totList.size() );

 
  //Calculate the centre of the cluster
  int phiPixelIndexMin, phiPixelIndexMax, etaPixelIndexMin, etaPixelIndexMax;
  InDetDD::SiCellId cellIdWeightedPosition= getCellIdWeightedPosition( pixelCluster, &phiPixelIndexMin, &phiPixelIndexMax, &etaPixelIndexMin, &etaPixelIndexMax);

  if (!cellIdWeightedPosition.isValid())
  {
    ATH_MSG_WARNING( "Weighted position is on invalid CellID." );
  }

  int etaPixelIndexWeightedPosition=cellIdWeightedPosition.etaIndex();
  int phiPixelIndexWeightedPosition=cellIdWeightedPosition.phiIndex();


  ATH_MSG_DEBUG(" weighted pos phiPixelIndex: " << phiPixelIndexWeightedPosition << " etaPixelIndex: " << etaPixelIndexWeightedPosition ); 

  // SiLocalPosition PixelModuleDesign::positionFromColumnRow(const int column, const int row) const;
  //
  // Given row and column index of diode, returns position of diode center
  // ALTERNATIVE/PREFERED way is to use localPositionOfCell(const SiCellId & cellId) or 
  // rawLocalPositionOfCell method in SiDetectorElement.
  // DEPRECATED (but used in numerous places)
  //
  // Comment by Hide (referring the original comment in the code) : 2015-02-04
  // I automatically replaced column to etaPixelIndex and row to phiPixelIndex here. It was bofore:
  // InDetDD::SiLocalPosition siLocalPosition( design->positionFromColumnRow(columnWeightedPosition,rowWeightedPosition) ); 
  //
  // Then I assume the argument of column/row in this function is in offline manner, not the real hardware column/row.
  // 
  InDetDD::SiLocalPosition w = design->positionFromColumnRow(etaPixelIndexWeightedPosition,phiPixelIndexWeightedPosition);


  double localEtaPixelIndexWeightedPosition = w.xEta();
  double localPhiPixelIndexWeightedPosition    = w.xPhi();

  int centralIndexX=(sizeX-1)/2;
  int centralIndexY=(sizeY-1)/2;



  // Check to see if the cluster is too big for the NN

  if (abs(phiPixelIndexWeightedPosition-phiPixelIndexMin)>centralIndexX ||
      abs(phiPixelIndexWeightedPosition-phiPixelIndexMax)>centralIndexX)
  {
    ATH_MSG_DEBUG(" Cluster too large phiPixelIndexMin " << phiPixelIndexMin << " phiPixelIndexMax " << phiPixelIndexMax << " centralX " << centralIndexX);
    //return;
  }

  if (abs(etaPixelIndexWeightedPosition-etaPixelIndexMin)>centralIndexY ||
      abs(etaPixelIndexWeightedPosition-etaPixelIndexMax)>centralIndexY)
  {
    ATH_MSG_DEBUG(" Cluster too large etaPixelIndexMin" << etaPixelIndexMin << " etaPixelIndexMax " << etaPixelIndexMax << " centralY " << centralIndexY);
    //return;
  }

  std::vector< std::vector<float> > matrixOfToT (sizeX, std::vector<float>(sizeY,0) );
  std::vector< std::vector<float> > matrixOfCharge(sizeX, std::vector<float>(sizeY,0));
  std::vector<float> vectorOfPitchesY(sizeY,0.4);


  //Itererate over all elements hits in the cluster and fill the charge and tot matrices 
  std::vector<Identifier>::const_iterator rdosBegin = rdos.begin();
  std::vector<Identifier>::const_iterator rdosEnd = rdos.end();
  auto charge = chList.begin();    
  auto tot = totList.begin();    

  ATH_MSG_VERBOSE(" Putting together the n. " << rdos.size() << " rdos into a matrix.");

  for (; rdosBegin!= rdosEnd; ++rdosBegin)
  {
    
    Identifier rId =  *rdosBegin;
    int absphiPixelIndex = m_PixelHelper->phi_index(rId)-phiPixelIndexWeightedPosition    + centralIndexX;
    int absetaPixelIndex = m_PixelHelper->eta_index(rId)-etaPixelIndexWeightedPosition + centralIndexY;
    if (charge != chList.end()){
      ATH_MSG_VERBOSE( " Phi Index: " << m_PixelHelper->phi_index(rId) << " absphiPixelIndex: " << absphiPixelIndex << " eta Idx: " << m_PixelHelper->eta_index(rId) << " absetaPixelIndex: " << absetaPixelIndex << " charge " << *charge );
    }
    if (absphiPixelIndex <0 || absphiPixelIndex >= (int)sizeX)
    {
      ATH_MSG_DEBUG(" problem with index: " << absphiPixelIndex << " min: " << 0 << " max: " << sizeX);
      continue;
    }
    
    if (absetaPixelIndex <0 || absetaPixelIndex >= (int)sizeY)
    {
      ATH_MSG_DEBUG(" problem with index: " << absetaPixelIndex << " min: " << 0 << " max: " << sizeY);
      continue;
    }

    InDetDD::SiCellId  cellId = de->cellIdFromIdentifier(*rdosBegin);
    InDetDD::SiDiodesParameters diodeParameters = design->parameters(cellId);
    float pitchY = diodeParameters.width().xEta();
  
    if ( (not totList.empty()) && tot    != totList.end()) {
      matrixOfToT[absphiPixelIndex][absetaPixelIndex]   =*tot;
      ++tot;
    } else matrixOfToT[absphiPixelIndex][absetaPixelIndex]   = -1;

    if ( (not chList.empty()) && charge != chList.end()){
     matrixOfCharge[absphiPixelIndex][absetaPixelIndex]=*charge;
     ++charge;
    } else matrixOfCharge[absphiPixelIndex][absetaPixelIndex] = -1;
  
    if (pitchY > 0.1)
    {
      vectorOfPitchesY[absetaPixelIndex]=pitchY;
    }
  }//end iteration on rdos
  

  ATH_MSG_VERBOSE( " End RDO LOOP " );

  // Using the centre of the module and beam spot calculate
  // the incidence angles of the tracks
  const Amg::Vector2D& prdLocPos = pixelCluster->localPosition();
  InDetDD::SiLocalPosition centroid(prdLocPos);

  Amg::Vector3D globalPos = de->globalPosition(centroid);
  Amg::Vector3D trackDir = globalPos; // - beamSpotPosition; 
  trackDir.normalize();

  Amg::Vector3D module_normal = de->normal();
  Amg::Vector3D module_phiax  = de->phiAxis();
  Amg::Vector3D module_etaax  = de->etaAxis();

  // Calculate the phi incidence angle
  float trkphicomp  = trackDir.dot(module_phiax);
  float trketacomp  = trackDir.dot(module_etaax);
  float trknormcomp = trackDir.dot(module_normal);
  double bowphi     = atan2(trkphicomp,trknormcomp);
  double boweta     = atan2(trketacomp,trknormcomp);
  double tanl = m_lorentzAngleTool->getTanLorentzAngle(de->identifyHash());
  if(bowphi > TMath::Pi()/2) bowphi -= TMath::Pi();
  if(bowphi < -TMath::Pi()/2) bowphi += TMath::Pi();
  int readoutside = design->readoutSide();
  double angle = atan(tan(bowphi)-readoutside*tanl);


  // Calculate the theta incidence angle
  ATH_MSG_VERBOSE( " Angle theta bef corr: " << boweta );
  if (boweta>TMath::Pi()/2.) boweta-=TMath::Pi();
  if (boweta<-TMath::Pi()/2.) boweta+=TMath::Pi();


  ATH_MSG_VERBOSE(" Angle phi: " << angle << " theta: " << boweta );
  ATH_MSG_VERBOSE(" PhiPixelIndexWeightedPosition: " << phiPixelIndexWeightedPosition << " EtaPixelIndexWeightedPosition: " << etaPixelIndexWeightedPosition );

  // store the matrixOfToT in a vector  
  std::vector<float> vectorOfCharge(sizeX*sizeY,0);
  std::vector<float> vectorOfToT(sizeX*sizeY,0);
  int counter(0);
  for (unsigned int u=0;u<sizeX;u++)
  {
    for (unsigned int s=0;s<sizeY;s++)
    {    
      vectorOfToT[counter]    = matrixOfToT[u][s];
      vectorOfCharge[counter] = matrixOfCharge[u][s];
      ++counter;
    }
  }

  ATH_MSG_VERBOSE( "matrixOfToT converted in a std::vector<float>  " );

  ATH_MSG_VERBOSE( "... and saved  " );
  // Add information to xAOD
  AUXDATA(xprd, int, NN_sizeX) = sizeX;
  AUXDATA(xprd, int, NN_sizeY) = sizeY;

  AUXDATA(xprd, float, NN_phiBS) = angle;
  AUXDATA(xprd, float, NN_thetaBS) = boweta;

  AUXDATA(xprd, std::vector<float>, NN_matrixOfToT)      = vectorOfToT;
  AUXDATA(xprd, std::vector<float>, NN_matrixOfCharge)   = vectorOfCharge;
  AUXDATA(xprd, std::vector<float>, NN_vectorOfPitchesY) = vectorOfPitchesY;
  
  
  AUXDATA(xprd, int, NN_etaPixelIndexWeightedPosition) = etaPixelIndexWeightedPosition;
  AUXDATA(xprd, int, NN_phiPixelIndexWeightedPosition) = phiPixelIndexWeightedPosition;

  AUXDATA(xprd, float, NN_localEtaPixelIndexWeightedPosition) = localEtaPixelIndexWeightedPosition;
  AUXDATA(xprd, float, NN_localPhiPixelIndexWeightedPosition) = localPhiPixelIndexWeightedPosition;

  ATH_MSG_VERBOSE( "NN training Written" );
}

void  PixelPrepDataToxAOD::addNNTruthInfo(  xAOD::TrackMeasurementValidation* xprd,
                                            const InDet::PixelCluster* pixelCluster, 
                                            const std::vector<SiHit> & matchingHits ) const
{


  unsigned int numberOfSiHits = matchingHits.size();
  
  std::vector<float> positionsX(numberOfSiHits,0);
  std::vector<float> positionsY(numberOfSiHits,0);

  std::vector<float> positions_indexX(numberOfSiHits,0);
  std::vector<float> positions_indexY(numberOfSiHits,0);

  std::vector<float> theta(numberOfSiHits,0);
  std::vector<float> phi(numberOfSiHits,0);

  std::vector<int>   barcode(numberOfSiHits,0);
  std::vector<int>   pdgid(numberOfSiHits,0);
  std::vector<float> chargeDep(numberOfSiHits,0);
  std::vector<float> truep(numberOfSiHits,0);

  std::vector<float> pathlengthX(numberOfSiHits,0);
  std::vector<float> pathlengthY(numberOfSiHits,0);
  std::vector<float> pathlengthZ(numberOfSiHits,0);

  std::vector<int>   motherBarcode(numberOfSiHits,0);
  std::vector<int>   motherPdgid(numberOfSiHits,0);



  // Check if we have detector element  --  needed to find the local position of the SiHits
  const InDetDD::SiDetectorElement* de = pixelCluster->detectorElement();
  if(!de)
    return;

  InDetDD::SiCellId cellIdWeightedPosition = getCellIdWeightedPosition( pixelCluster );

  const InDetDD::PixelModuleDesign* design(dynamic_cast<const InDetDD::PixelModuleDesign*>(&de->design()));
  if (not design) {
		ATH_MSG_WARNING("PixelModuleDesign was not retrieved in function 'addNNTruthInfo'");
		return;
	}
  // lorentz shift correction    
  double shift = m_lorentzAngleTool->getLorentzShift(de->identifyHash());
  unsigned hitNumber(0);
  for( auto& siHit : matchingHits ){
    
    HepGeom::Point3D<double> averagePosition = (siHit.localStartPosition() + siHit.localEndPosition()) * 0.5;
    
    ATH_MSG_VERBOSE("Truth Part X: " << averagePosition.y() << " shift " << shift << " Y: " << averagePosition.z() );

    // position lorentz shift corrected
    float YposC = averagePosition.y()-shift;

    if (fabs(YposC)>design->width()/2 && 
        fabs(averagePosition.y())<design->width()/2)
   { 
      if (YposC>design->width()/2)
      {
        YposC=design->width()/2-1e-6;
      } else if (YposC<-design->width()/2)
      {
        YposC=-design->width()/2+1e-6;
      }
    }
        
    positionsX[hitNumber] = YposC;
    positionsY[hitNumber] = averagePosition.z();

    HepGeom::Point3D<double> deltaPosition = siHit.localEndPosition() - siHit.localStartPosition();
    
    pathlengthX[hitNumber] = deltaPosition.y();
    pathlengthY[hitNumber] = deltaPosition.z();
    pathlengthZ[hitNumber] = deltaPosition.x();


    
    InDetDD::SiLocalPosition siLocalTruthPosition(averagePosition.z(),YposC ) ;
    InDetDD::SiCellId cellIdOfTruthPosition =design->cellIdOfPosition(siLocalTruthPosition);

    int truthEtaIndex =  cellIdOfTruthPosition.etaIndex();
    int truthPhiIndex =  cellIdOfTruthPosition.phiIndex();

    InDetDD::SiDiodesParameters diodeParameters = design->parameters(cellIdOfTruthPosition);
    double pitchY = diodeParameters.width().xEta();
    double pitchX = diodeParameters.width().xPhi();

    // pixel center
    // SiLocalPosition PixelModuleDesign::positionFromColumnRow(const int column, const int row) const;
    //
    // Given row and column index of diode, returns position of diode center
    // ALTERNATIVE/PREFERED way is to use localPositionOfCell(const SiCellId & cellId) or 
    // rawLocalPositionOfCell method in SiDetectorElement.
    // DEPRECATED (but used in numerous places)
    //
    // Comment by Hide (referring the original comment in the code) : 2015-02-04
    // I automatically replaced column to etaPixelIndex and row to phiPixelIndex here. It was bofore:
    // InDetDD::SiLocalPosition siLocalPosition( design->positionFromColumnRow(truthColumn,truthRow) ); 
    //
    // Then I assume the argument of column/row in this function is in offline manner, not the real hardware column/row.
    // 
    InDetDD::SiLocalPosition siLocalPositionCenter(design->positionFromColumnRow(truthEtaIndex,truthPhiIndex)); 
    double pixelCenterY = siLocalPositionCenter.xEta();
    double pixelCenterX = siLocalPositionCenter.xPhi();
    

    // truth index
    double truthIndexY =  truthEtaIndex + (averagePosition.z() - pixelCenterY)/pitchY;
    double truthIndexX =  truthPhiIndex + (YposC               - pixelCenterX)/pitchX;

    positions_indexX[hitNumber] = truthIndexX - cellIdWeightedPosition.phiIndex();
    positions_indexY[hitNumber] = truthIndexY - cellIdWeightedPosition.etaIndex();

    HepGeom::Point3D<double> diffPositions = (siHit.localEndPosition() - siHit.localStartPosition());
    double bowphi = atan2( diffPositions.y(), diffPositions.x() );
   

    //Truth Track incident angle theta
    theta[hitNumber] = atan2(diffPositions.z() ,diffPositions.x());
    //Truth track incident angle phi -- correct for lorentz angle
    float tanlorentz = m_lorentzAngleTool->getTanLorentzAngle(de->identifyHash());
  
    int readoutside = design->readoutSide();
    phi[hitNumber] = atan(tan(bowphi)-readoutside*tanlorentz);
    
    if(siHit.particleLink().isValid()){
      barcode[hitNumber] = siHit.particleLink().barcode(); 
      
      auto particle = siHit.particleLink();
      pdgid[hitNumber]   = particle->pdg_id();
      truep[hitNumber]  = particle->momentum().rho();
      if ( particle->production_vertex() ){
        auto vertex =  particle->production_vertex();
        if( vertex->particles_in_const_begin() !=  vertex->particles_in_const_end() ){
          motherBarcode[hitNumber] =  (*vertex->particles_in_const_begin())->barcode();
          motherPdgid[hitNumber]    =  (*vertex->particles_in_const_begin())->pdg_id();
        }
      }
    }
    chargeDep[hitNumber] = siHit.energyLoss() ;
    
    ++hitNumber;
  }


  AUXDATA(xprd, std::vector<float>, NN_positionsX) = positionsX;
  AUXDATA(xprd, std::vector<float>, NN_positionsY) = positionsY;

  AUXDATA(xprd, std::vector<float>, NN_positions_indexX) = positions_indexX;
  AUXDATA(xprd, std::vector<float>, NN_positions_indexY) = positions_indexY;

  AUXDATA(xprd, std::vector<float>, NN_theta)     = theta;
  AUXDATA(xprd, std::vector<float>, NN_phi)       = phi;

  AUXDATA(xprd, std::vector<int>, NN_barcode)     = barcode;
  AUXDATA(xprd, std::vector<int>, NN_pdgid)       = pdgid;
  AUXDATA(xprd, std::vector<float>, NN_energyDep) = chargeDep;
  AUXDATA(xprd, std::vector<float>, NN_trueP)     = truep;

  AUXDATA(xprd, std::vector<int>, NN_motherBarcode) = motherBarcode;
  AUXDATA(xprd, std::vector<int>, NN_motherPdgid)   = motherPdgid;
 

  AUXDATA(xprd, std::vector<float>, NN_pathlengthX) = pathlengthX;
  AUXDATA(xprd, std::vector<float>, NN_pathlengthY) = pathlengthY;
  AUXDATA(xprd, std::vector<float>, NN_pathlengthZ) = pathlengthZ;


}




InDetDD::SiCellId PixelPrepDataToxAOD::getCellIdWeightedPosition(  const InDet::PixelCluster* pixelCluster,
                                                                   int *rphiPixelIndexMin,
                                                                   int *rphiPixelIndexMax,
                                                                   int *retaPixelIndexMin,
                                                                   int *retaPixelIndexMax   ) const
{

  const InDetDD::SiDetectorElement* de = pixelCluster->detectorElement();
  if (de==0) {
    ATH_MSG_ERROR("Could not get detector element");
    return InDetDD::SiCellId();
  }

  const InDetDD::PixelModuleDesign* design(dynamic_cast<const InDetDD::PixelModuleDesign*>(&de->design()));
  if (not design) {
    ATH_MSG_WARNING("PixelModuleDesign was not retrieved in function 'getCellIdWeightedPosition'");
    return InDetDD::SiCellId();
  }
  const std::vector<Identifier>& rdos  = pixelCluster->rdoList();  

  ATH_MSG_VERBOSE( "Number of RDOs: " << rdos.size() );
  const std::vector<float>& chList     = pixelCluster->chargeList();

  ATH_MSG_VERBOSE( "Number of charges: " << chList.size() );
  std::vector<Identifier>::const_iterator rdosBegin = rdos.begin();
  std::vector<Identifier>::const_iterator rdosEnd = rdos.end();

  auto charge = chList.begin();    

  InDetDD::SiLocalPosition sumOfWeightedPositions(0,0,0);
  double sumOfCharge=0;

  int phiPixelIndexMin =  99999;
  int phiPixelIndexMax = -99999;
  int etaPixelIndexMin =  99999;
  int etaPixelIndexMax = -99999;

  for (; rdosBegin!= rdosEnd; ++rdosBegin, ++charge)
  {
  
    Identifier rId =  *rdosBegin;
    int phiPixelIndex = m_PixelHelper->phi_index(rId);
    int etaPixelIndex = m_PixelHelper->eta_index(rId);
  
    ATH_MSG_VERBOSE(" Adding pixel phiPixelIndex: " << phiPixelIndex << " etaPixelIndex: " << etaPixelIndex << " charge: " << *charge );
    
    // SiLocalPosition PixelModuleDesign::positionFromColumnRow(const int column, const int row) const;
    //
    // Given row and column index of diode, returns position of diode center
    // ALTERNATIVE/PREFERED way is to use localPositionOfCell(const SiCellId & cellId) or 
    // rawLocalPositionOfCell method in SiDetectorElement.
    // DEPRECATED (but used in numerous places)
    //
    // Comment by Hide (referring the original comment in the code): 2015-02-04
    // I automatically replaced column to etaPixelIndex and row to phiPixelIndex here. It was bofore:
    // InDetDD::SiLocalPosition siLocalPosition( design->positionFromColumnRow(column,row) ); 
    //
    // Then I assume the argument of column/row in this function is in offline manner, not the real hardware column/row.
    // 
    InDetDD::SiLocalPosition siLocalPosition( design->positionFromColumnRow(etaPixelIndex,phiPixelIndex) ); 
    ATH_MSG_VERBOSE ( "Local Position: Row = " << siLocalPosition.xRow() << ", Col = " << siLocalPosition.xColumn() );
  
    sumOfWeightedPositions += (*charge)*siLocalPosition;
    sumOfCharge += (*charge);

    if (phiPixelIndex < phiPixelIndexMin)
      phiPixelIndexMin = phiPixelIndex; 
     
    if (phiPixelIndex > phiPixelIndexMax)
      phiPixelIndexMax = phiPixelIndex;
    
    if (etaPixelIndex < etaPixelIndexMin)
      etaPixelIndexMin = etaPixelIndex;
      
    if (etaPixelIndex > etaPixelIndexMax)
      etaPixelIndexMax = etaPixelIndex;

  }
  sumOfWeightedPositions /= sumOfCharge;

  ATH_MSG_VERBOSE ( "Wighted position: Row = " << sumOfWeightedPositions.xRow() << ", Col = " << sumOfWeightedPositions.xColumn() );

  if(rphiPixelIndexMin) *rphiPixelIndexMin = phiPixelIndexMin;
  if(rphiPixelIndexMax) *rphiPixelIndexMax = phiPixelIndexMax;
  if(retaPixelIndexMin) *retaPixelIndexMin = etaPixelIndexMin;
  if(retaPixelIndexMax) *retaPixelIndexMax = etaPixelIndexMax;

  //what you want to know is simple:
  //just the phiPixelIndex and etaPixelIndex of this average position!

  InDetDD::SiCellId cellIdWeightedPosition=design->cellIdOfPosition(sumOfWeightedPositions);


  return cellIdWeightedPosition;
  
}



/////////////////////////////////////////////////////////////////////
//
//        Finalize method: 
//
/////////////////////////////////////////////////////////////////////
StatusCode PixelPrepDataToxAOD::finalize()
{
  return StatusCode::SUCCESS;
}
