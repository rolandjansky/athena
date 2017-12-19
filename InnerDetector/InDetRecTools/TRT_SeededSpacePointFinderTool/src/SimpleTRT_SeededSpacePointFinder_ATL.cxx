/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_SeededSpacePointFinder_ATL
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 04/2007 Martin Siebel
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <set>
#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h" 
#include "TRT_SeededSpacePointFinderTool/SimpleTRT_SeededSpacePointFinder_ATL.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
#include "TrkSurfaces/Surface.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "RoiDescriptor/RoiDescriptor.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SimpleTRT_SeededSpacePointFinder_ATL::SimpleTRT_SeededSpacePointFinder_ATL
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_assoTool("InDet::InDetPRD_AssociationToolGangedPixels"),
    m_useAssoTool(true),
    m_useROI(true),
    m_maxLayers(3),
    m_maxHoles(1),
    m_perigeeCut(200),
    m_directionPhiCut(0.05),
    m_directionEtaCut(0.5),
    m_positionPhiCut(5.),
    m_positionZ_Cut(250.),
    m_pRegionSelector("RegSelSvc",n),
    m_sctId(nullptr),
    m_trtId(nullptr)
{


  declareInterface<ITRT_SeededSpacePointFinder>(this);

  declareProperty("RestrictROI"           ,m_useROI                );
  declareProperty("AssociationTool"       ,m_assoTool              );
  declareProperty("MaxLayers"             ,m_maxLayers             );
  declareProperty("MaxHoles"              ,m_maxHoles              );
  declareProperty("PerigeeCut"            ,m_perigeeCut            );
  declareProperty("DirectionPhiCut"       ,m_directionPhiCut       );
  declareProperty("DirectionEtaCut"       ,m_directionEtaCut       );
  declareProperty("PositionPhiCut"        ,m_positionPhiCut        );
  declareProperty("PositionZ_Cut"         ,m_positionZ_Cut         );

}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::SimpleTRT_SeededSpacePointFinder_ATL::~SimpleTRT_SeededSpacePointFinder_ATL()
{}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SimpleTRT_SeededSpacePointFinder_ATL::initialize()
{

  // msg().setLevel(outputLevel());

  // Get association tool
  //

  StatusCode sc = AthAlgTool::initialize(); 

  m_useAssoTool = false;

  if(!m_assoTool.empty())
    {
      sc = m_assoTool.retrieve(); 
      if(sc.isFailure()) 
	{
	  msg(MSG::FATAL)<<"Could not get "<<m_assoTool<<endmsg; 
	  return StatusCode::FAILURE;
	}
      else
	m_useAssoTool=true;
    }	

  // get region selector
  sc = m_pRegionSelector.retrieve();
  if( sc.isFailure() ) 
    {
      msg(MSG::FATAL) << "Failed to retrieve RegionSelector Service";
      return sc;
    }

  sc = detStore()->retrieve(m_sctId, "SCT_ID");
  if (sc.isFailure())
    {
      msg(MSG::ERROR) << "Could not get SCT_ID helper !" << endmsg;
      return StatusCode::FAILURE;
    }

  sc = detStore()->retrieve(m_trtId, "TRT_ID");
  if (sc.isFailure())
    {
      msg(MSG::ERROR) << "Could not get TRT_ID helper !" << endmsg;
      return StatusCode::FAILURE;
    }

  setupLookUpTable();

  ATH_CHECK(m_spacepointsSCTname.initialize());
  ATH_CHECK(m_spacepointsOverlapname.initialize());

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SimpleTRT_SeededSpacePointFinder_ATL::finalize()
{
   StatusCode sc = AthAlgTool::finalize(); 
   return sc;
}
//============================================================================================

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SimpleTRT_SeededSpacePointFinder_ATL::newRegion
(const std::vector<IdentifierHash>& /*vPixel*/, const std::vector<IdentifierHash>& /*vSCT*/)
{
}

std::list<std::pair<const Trk::SpacePoint*, const Trk::SpacePoint*> >* 
InDet::SimpleTRT_SeededSpacePointFinder_ATL::find2Sp(const Trk::TrackParameters& directionTRT)
{
  /** The main method. This method is called from outside to form pairs of SCT SpacePoints which could seed
      Si tracks linked to the TRT TrackParameter given as input. iModus is a dummy variable to satisfy the 
      interface.

      This method calls several private methods to perform specific tasks. These are:

      getHashesInROI : If the restriction of the Region Of Intereset is used, this method returns a set of
                       IdHashes of SCT modules within the ROI

      getSpacePointsInROI : This Method retrieves the SpacePointCollections from StoreGate and fills a map
                            with the SpacePoints. The map key is a number which identifies the layer or disc
                            the SpacePoint is on. 		       

      combineSpacePoints : The retrieved and sorted SpacePoints are combined to pairs. Pairs are built only of
                           SpacePoints which are on the layer or disc pairs specified in the internal look-up table.			    

      The method returns a pointer to an internal list of SpacePoint pairs, the list may very well be empty nevertheless.
  */			   


  msg(MSG::VERBOSE) << "Enter getListOfSpacePointPairs, TrackParameter given is: " << endmsg;
  msg(MSG::VERBOSE) << directionTRT << endmsg;

  // clear output buffer
  m_listOfSpacePointPairsBuffer.clear();

  // IdHash in ROI
  std::set<IdentifierHash> setOfSCT_Hashes;

  if (m_useROI)
    {
      // fill IdHashes in ROI
      getHashesInROI(directionTRT,setOfSCT_Hashes);
      msg(MSG::VERBOSE) << "Retrieved " << setOfSCT_Hashes.size() << " potentially interesting detector elements." << endmsg;
    }

  if ( !m_useROI || setOfSCT_Hashes.size()>0)
    {
      // map of < SCT layer number, SP* >
      std::multimap<int,const Trk::SpacePoint*> relevantSpacePoints; 

      // fill the map of relevant SP as defined by hashes from ROI
      int modulTRT = TRT_Module(directionTRT);
      getSpacePointsInROI(setOfSCT_Hashes, modulTRT, relevantSpacePoints);

      msg(MSG::VERBOSE) << "Retrieved " << relevantSpacePoints.size() << " potentially interesting SpacePoints" << endmsg;

      // build pairs of the relevant SP according to the look-up table
      combineSpacePoints(relevantSpacePoints, directionTRT, modulTRT);

      /* output for debug purposes, deactivated now. Once development is finished, it will be removed.
       */
      Amg::Vector3D r0 = directionTRT.position();
      Amg::Vector3D v0(directionTRT.momentum());
      msg(MSG::VERBOSE) << "------------------------------------------------------------------------------------------" << endmsg;
      msg(MSG::VERBOSE) << "Final SpacePoint pairs: " << m_listOfSpacePointPairsBuffer.size() << endmsg;
      msg(MSG::VERBOSE) << "   Position of initial vector:  ( " << r0.x() << ", " << r0.y() << ", "<< r0.z() << " ) " << endmsg;
      msg(MSG::VERBOSE) << "   Direction of initial vector:  ( " << v0.unit().x() << ", " << v0.unit().y() << ", "<< v0.unit().z() << " ) , phi = " 
	    << v0.phi() <<  "   theta = " << v0.theta() << "    eta = "<< v0.eta() << endmsg;
      msg(MSG::VERBOSE) << "------------------------------------------------------------------------------------------" << endmsg;
      msg(MSG::VERBOSE) << "   Direction of space Point vectors: "<< endmsg;
      msg(MSG::VERBOSE) << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . " << endmsg;
      for (std::list<std::pair<const Trk::SpacePoint*, const Trk::SpacePoint*> >::iterator it = m_listOfSpacePointPairsBuffer.begin();
	   it != m_listOfSpacePointPairsBuffer.end(); ++it)
	{
	  Amg::Vector3D s1 = it->first->globalPosition();
	  Amg::Vector3D s2 = it->second->globalPosition();
	  Amg::Vector3D s1s2 = s2-s1;  // vector from s1 to s2
	  msg(MSG::VERBOSE) << "   Positions:  ( " << s1.x() << " , "<< s1.y() << " , "<< s1.z() << " ) " << endmsg;
	  msg(MSG::VERBOSE) << "               ( " << s2.x() << " , "<< s2.y() << " , "<< s2.z() << " ) " << endmsg;
	  msg(MSG::VERBOSE) << "   direction:  ( " 
		<< s1s2.unit().x() << ", " << s1s2.unit().y() << ", " << s1s2.unit().z() <<" ) , phi = "
		<< s1s2.phi() <<  "   theta = " << s1s2.theta() << "    eta = "<< s1s2.eta() << endmsg;
	  msg(MSG::VERBOSE) << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . " << endmsg;
	}
      msg(MSG::VERBOSE) << "------------------------------------------------------------------------------------------" << endmsg;
    }

  return &m_listOfSpacePointPairsBuffer;
}

//=====================================================================================================

void InDet::SimpleTRT_SeededSpacePointFinder_ATL::getHashesInROI(const Trk::TrackParameters& directionTRT, std::set<IdentifierHash>& setOfSCT_Hashes)
{
  /** This Method finds the IdHashes of SCT modules inside a Region Of Interest around the TRT track parameter. 
      First the allowed tolerance in phi and eta is inquired, then a vector with the relevant hashes is retrieved from the RegionSelectorService.
      In order to be able to search conveniently for hashes later on, they are then copied into a STL set, which is then returned via call by reference.
  */


  //double eta = directionTRT.eta();
  //double phi = directionTRT.position().phi();
  double phi = directionTRT.parameters()[Trk::phi];
  double eta = directionTRT.position().eta();
  double deltaPhi = 0.;
  double deltaEta = 0.;

  // define tolerance for ROI
  //getSearchRange(directionTRT, deltaPhi, deltaEta);
  getSearchRange(deltaPhi, deltaEta);

  // retrieve SCT hashes in Region of interest  
  std::vector<IdentifierHash> listOfSCT_Hashes;

  //// AAAARGH!!! No checking that you are within the correct phi range???
  RoiDescriptor roi(  eta-deltaEta, eta+deltaEta, phi-deltaPhi, phi+deltaPhi);

  m_pRegionSelector->DetHashIDList(SCT, roi, listOfSCT_Hashes);

  // copy Hashes into Set to be able to search them
  for (std::vector<IdentifierHash>::const_iterator it = listOfSCT_Hashes.begin();
       it != listOfSCT_Hashes.end();
       ++it)
    setOfSCT_Hashes.insert(*it);
}

//=====================================================================================================

void InDet::SimpleTRT_SeededSpacePointFinder_ATL::getSpacePointsInROI(std::set<IdentifierHash>& setOfSCT_Hashes, int modulTRT, std::multimap<int,const Trk::SpacePoint*>& relevantSpacePoints)
{
  /** This method retrieves the SpacePoints in a given region of interest. The region of interest is defined by the hashes in the first argument of the method.

      The second argument specifies the part of the TRT in which the TRT track parameter is situated. This number is used to inquire the index of the look-up table, which layers
      of the SCT are relevant for the pair building process. Only SpacePointCollections from relevant layers or discs are looped over in order to save time.

      If the restriction of the ROI is used, it is inquired for every SpacePointCollection, if the corresponding SCT module is listed in the set of IdHashes which defines the ROI.
      In this case, only Collections which satisfy this are looped over.

      Relevant SpacePoints are then stored in a map. Key of the map is a number which identifies the SCT layer the SpacePoints are on.

      In a last step, the SpacePointOverlapCollection is retrieved and looped over. SpacePoints in the ROI (if use is specified) are added to the list of SpacePoints

      If the AssociationTool is used, only SpacePoints are stored, which do not contain a PRD that is not yet registered in the AssociationTool, i.e. which is not already used by the 
      inside-out tracking.
  */


  const std::set<IdentifierHash>::const_iterator endSCT_Hashes = setOfSCT_Hashes.end();
  
  // retrieve SP Container
  SG::ReadHandle<SpacePointContainer> spacepointsSCT(m_spacepointsSCTname);
  if(spacepointsSCT.isValid()) 
    {
      // loop over SP collections in SP container
      SpacePointContainer::const_iterator itCont  =  spacepointsSCT->begin();
      SpacePointContainer::const_iterator endCont =  spacepointsSCT->end  ();
      for(; itCont != endCont; ++itCont) 
	{
	  bool acceptCollection = true;

	  if (m_useROI)
	    {
	      // Check if IdHash of the Collection is in the list of ROI hashes
	      IdentifierHash idHash = (*itCont)->identifyHash();
	      acceptCollection = ( idHash.is_valid() && setOfSCT_Hashes.find( idHash ) != endSCT_Hashes );
	    }

	  int SCT_LayerNumber = -99;
	  if (acceptCollection)
	    {
	      // check if the module is on a relevant layer
	      Identifier id  = (*itCont)->identify();
	      int detRegionIndex = m_sctId->barrel_ec(id) ;
	      if ( detRegionIndex == -2 ||  detRegionIndex == 0 ||  detRegionIndex == 2 )
		{
		  // conversion factors for layer numbering
		  int detLayerOffset = 1;	      
		  int detLayerSign   = 1;
		  if (detRegionIndex == 0) detLayerOffset = 10;
		  if (detRegionIndex == -2) detLayerSign = -1;
		  
		  // retrieve layer number from IdHelper
		  int detLayerIndex = m_sctId->layer_disk(id);
		  SCT_LayerNumber =  detLayerSign*(detLayerIndex+detLayerOffset);
		}
	      else
		{
		  msg(MSG::VERBOSE) << "detRegionIndex " << detRegionIndex << " recieved. " << endmsg;
		  acceptCollection = false;
		}
	    }

	  if (acceptCollection)
	    {
	      if (modulTRT > -14 && modulTRT < 15 )
		{
		  std::set<int>::const_iterator pos = m_modulLookupTableIndex[modulTRT+SIMPLE_TRT_INDEX_OFFSET].find(SCT_LayerNumber);
		  if ( pos == m_modulLookupTableIndex[modulTRT+SIMPLE_TRT_INDEX_OFFSET].end() ) acceptCollection = false;
		}
	      else
		{
		  msg(MSG::WARNING) << "Received TRTmodul number " << modulTRT << endmsg;
		  acceptCollection = false;
		}
	    }

	  if (acceptCollection)
	    {	      
	      // Loop over SP Collection and add SP if they are not yet used (or if this does not matter). 
	      SpacePointCollection::const_iterator itColl  = (*itCont)->begin();
	      SpacePointCollection::const_iterator endColl = (*itCont)->end  ();
	      for(; itColl != endColl; ++itColl) 
		if (    !m_useAssoTool 
			|| !(    m_assoTool->isUsed(*((*itColl)->clusterList().first))
				 || m_assoTool->isUsed(*((*itColl)->clusterList().second)) ) )
		  {
		    relevantSpacePoints.insert(std::make_pair( SCT_LayerNumber   ,*itColl));
		    msg(MSG::VERBOSE) << "Added SpacePoint for layer " << SCT_LayerNumber << " at  ( " 
			  << (*itColl)->globalPosition().x() << " , " 
			  << (*itColl)->globalPosition().y() << " , " 
			  << (*itColl)->globalPosition().z() << " ) " << endmsg; 
		  }
	    }
	}
    }

  // retrieve the overlap collection
  SG::ReadHandle<SpacePointOverlapCollection> spacepointsOverlap(m_spacepointsOverlapname);
  if(spacepointsOverlap.isValid()) 
    {

      // Loop over Overlap SP
      SpacePointOverlapCollection::const_iterator  itColl  = spacepointsOverlap->begin();
      SpacePointOverlapCollection::const_iterator endColl = spacepointsOverlap->end  ();
      for (; itColl != endColl; ++itColl) 
	{
	  // check if SP is in ROI
	  std::pair<IdentifierHash, IdentifierHash> idHashPair = (*itColl)->elementIdList();
	  if (    !m_useROI  
	       || (idHashPair.first.is_valid()  && setOfSCT_Hashes.find( idHashPair.first )  != endSCT_Hashes)
	       || (idHashPair.second.is_valid() && setOfSCT_Hashes.find( idHashPair.second ) != endSCT_Hashes) )
	  {

	    // find out if one of the Clusters has already been used, if relevant
	    if(m_useAssoTool)
	      {
		bool u1=false; 
		bool u2=false;
		const Trk::PrepRawData* p1=(*itColl)->clusterList().first; 
		u1=m_assoTool->isUsed(*p1);
		const Trk::PrepRawData* p2=(*itColl)->clusterList().second;
		u2=m_assoTool->isUsed(*p2);
		if(u1 || u2) continue;
	      }
	    
	    // retrieve identifier and fill SP in SP buffer.
	    Identifier id = (*itColl)->associatedSurface().associatedDetectorElementIdentifier();
	    if ( id.is_valid() )
	      {
		int detRegionIndex = m_sctId->barrel_ec(id) ;	      
		if ( detRegionIndex == -2 ||  detRegionIndex == 0 ||  detRegionIndex == 2 )
		  {
		    int detLayerOffset = 1;	      
		    int detLayerSign   = 1;
		    if (detRegionIndex == 0) detLayerOffset = 10;
		    if (detRegionIndex == -2) detLayerSign = -1;
		    int detLayerIndex = m_sctId->layer_disk(id);
		    
		    relevantSpacePoints.insert(std::make_pair(detLayerSign*(detLayerIndex+detLayerOffset),*itColl));
		    msg(MSG::VERBOSE) << "Added OverlapSpacePoint for layer " << detLayerSign*(detLayerIndex+detLayerOffset) << " at  ( " 
			  << (*itColl)->globalPosition().x() << " , " 
			  << (*itColl)->globalPosition().y() << " , " 
			  << (*itColl)->globalPosition().z() << " ) " << endmsg; 
		  }
		else
		  msg(MSG::VERBOSE) << "detRegionIndex " << detRegionIndex << " recieved. " << endmsg;
	      }
	    else
	      msg(MSG::DEBUG) << "Invalid Id from OverlapCollection recieved. " << endmsg;
	  }
	}
    }
}


//=====================================================================================================

//void InDet::SimpleTRT_SeededSpacePointFinder_ATL::getSearchRange(const Trk::TrackParameters& directionTRT, double& deltaPhi, double& deltaEta)
void InDet::SimpleTRT_SeededSpacePointFinder_ATL::getSearchRange(double& deltaPhi, double& deltaEta)
{
  /** This method defines the tolerance for the ROI around the TRT track parameter. At the moment it is set constant, but it should be considered to
      define it in dependence on the track parameter. In order to suppress warnings for unused variables, the method signature including the track parameter
      as an argument is replaced for the time being by a signature without the track parameter.
  */

  // first guess: the ROI tolerance is constant
  deltaPhi = 0.05;
  deltaEta = .2;
}

//=====================================================================================================

void InDet::SimpleTRT_SeededSpacePointFinder_ATL::combineSpacePoints(const std::multimap<int,const Trk::SpacePoint*>& relevantSpacePoints,
								     const Trk::TrackParameters& directionTRT, int modulNumber)
{
  /** This method combines SpacePoints contained in the first argument to pairs which are then stored in the internal list of SpacePoint pairs.

      Only SpacePoints which are on two different SCT layers or wheels specified as pair in the look-up table are combined. The pairing is only accepted, if
      if satisfies quality cuts specified in the method pairIsOk.

      The second argument specifies the TRT track parameter which is used for quality cuts on the SpacePoint pairs. 
      The third argument specifies the part of the TRT the track parameter comes from. This number is used to inquire the relevant pairings of SCT
      layers from the look-up table.       

     
  */
  
  if ( modulNumber+SIMPLE_TRT_INDEX_OFFSET < 29 && -1 < modulNumber+SIMPLE_TRT_INDEX_OFFSET)
    {
      modulLookupTable::const_iterator  itTable = m_modulLookupTable[modulNumber+SIMPLE_TRT_INDEX_OFFSET].begin();
      modulLookupTable::const_iterator endTable = m_modulLookupTable[modulNumber+SIMPLE_TRT_INDEX_OFFSET].end();
      for ( ; itTable != endTable ; ++itTable)
	{
	  msg(MSG::VERBOSE) << " Combining Space Poinst from modules " << itTable->first << " and " << itTable->second << endmsg; 
	  // SPs from SCT layer 1
	  std::pair< std::multimap<int,const Trk::SpacePoint*>::const_iterator,
	    std::multimap<int,const Trk::SpacePoint*>::const_iterator  >  
	    range1 = relevantSpacePoints.equal_range(itTable->first);
	  //	  if (range1.second != relevantSpacePoints.end() ) ++(range1.second);  // transforming the 2nd pointer to an end of range

	  //SPs from SCT layer 2
	  std::pair< std::multimap<int,const Trk::SpacePoint*>::const_iterator,
	    std::multimap<int,const Trk::SpacePoint*>::const_iterator  >  
	    range2 = relevantSpacePoints.equal_range(itTable->second);
	  //	  if (range2.second != relevantSpacePoints.end() ) ++(range2.second);  // transforming the 2nd pointer to an end of range

	  // add the SP pairs
	  for ( std::multimap<int,const Trk::SpacePoint*>::const_iterator it1 = range1.first; 
		it1 != range1.second ; ++it1 )	      
	    for ( std::multimap<int,const Trk::SpacePoint*>::const_iterator it2 = range2.first; 
		  it2 != range2.second ; ++it2 )	      
	      if ( pairIsOk(it1->second,it2->second,directionTRT) )
		m_listOfSpacePointPairsBuffer.push_back(std::make_pair(it2->second,it1->second));

	}
    }
  else
    msg(MSG::WARNING) << "TRT module not in look-up table --> no SP pairs formed" << endmsg;

}
//=====================================================================================================

bool InDet::SimpleTRT_SeededSpacePointFinder_ATL::pairIsOk(const Trk::SpacePoint* sp1, const Trk::SpacePoint* sp2, const Trk::TrackParameters& directionTRT)
{
  /** This method applies quality cuts on a given pairing of SpacePoints. The quality cuts at the moment are:

     - the closest approach of the line connecting the two SpacePoints to the beam axis in (x,y) has to be smaller than a given limit.
     - the direction in phi of the line connecting the two SpacePoints must not deviate from the phi direction of the TRT track parameter by more than a given tolerance.

     If the cuts are satisfied, true is returned, false otherwise.
  */


  Amg::Vector3D s1 = sp1->globalPosition();
  Amg::Vector3D s2 = sp2->globalPosition();
  //Amg::Vector3D r0 = directionTRT.position();
  Amg::Vector3D v0(directionTRT.momentum());
  Amg::Vector3D s1s2 = s2-s1;  // vector from s1 to s2
  
  msg(MSG::VERBOSE) << "Checking Space Point Pair at ( "<< s1.x() << ", " << s1.y() << ", " << s1.z()
	<< " )  and  ( "<< s2.x() << ", " << s2.y() << ", " << s2.z() << " )" << endmsg;

  // Cut on closest approach to z-axis
  double t = ( s1.x()-s2.x() )*( s1.x()-s2.x() ) + ( s1.y()-s2.y() )*( s1.y()-s2.y() ) ;
  if (t)
    {
      t =  ( s1.x()*(s2.x()-s1.x()) + s1.y()*(s2.y()-s1.y()) ) / t;
      Amg::Vector3D perigee = s1 + t*(s1-s2);
      msg(MSG::VERBOSE) << " closest approach to beam-pipe at ( "<< perigee.x() << ", " << perigee.y() << ", " << perigee.z() << " )    transversal: " << perigee.perp()<<endmsg;
      if (perigee.perp() > m_perigeeCut ) return false;
    }
  msg(MSG::VERBOSE) << " Passed cut on r-phi impact parameter" <<endmsg;


  // Cut on angle between s1-s2 and parameter direction
  double diffPhi= fabs(s1s2.phi()-v0.phi());

  if (diffPhi > CLHEP::pi ) diffPhi = 2.*CLHEP::pi - diffPhi;
  if (diffPhi > CLHEP::pi/2. ) diffPhi = CLHEP::pi - diffPhi;
  msg(MSG::VERBOSE) << "Phi directions differ by "<< diffPhi << endmsg;
  if ( diffPhi > m_directionPhiCut ) return false;

  msg(MSG::VERBOSE) << " Passed cut on direction phi deviation" <<endmsg;

  // cut on eta deviation
  //if (fabs(s1s2.eta()-v0.eta()) > m_directionEtaCut) return false;
  //msg(MSG::VERBOSE) << " Passed cut on direction eta deviation" <<endmsg;


  /*
  // Cut on angle between s1-s2 and parameter direction
  double cosAng = s1s2.mag()*v0.mag();
  if (cosAng)
    {
      cosAng = fabs(v0.dot(s1s2)/cosAng);
      msg(MSG::VERBOSE) << "Cosine of angle between SP-extrap. and momentum is " << cosAng << endmsg;
      if ( cosAng < cosAngleCut ) return false;
    }
  msg(MSG::VERBOSE) << " Passed cut on direction deviation" <<endmsg;
  */

  /*
  // Cut on closest approach to parameter position
  Trk::GlobalPosition s1r0 = r0-s1;  // vector from s1 to r0
  Trk::GlobalPosition h = s1r0 - s1r0.dot(s1s2)/s1s2.mag();  // the part of s1r0 perp. to s1s2
  msg(MSG::VERBOSE) << "closest approach to Parameter Position is ( " << h.x() << ", "<< h.y() << ", " << h.z() << " )"<<endmsg; 
  if (h.x()*h.x() + h.y()*h.y() > positionPhiCutSquared || fabs(h.z()) > positionZ_Cut) return false;
  
  msg(MSG::VERBOSE) << " Passed cut on extrapolation to TRT" <<endmsg;
  */
  return true;
}
//=====================================================================================================
void InDet::SimpleTRT_SeededSpacePointFinder_ATL::setupLookUpTable()
{
  /** This method sets up the look-up table. This table is an array of lists of integer pairs.
      The index of the array corresponds to a number which identifies the part of the TRT the
      TRT track parameter comes from, the two integer numbers in the pairs identify the SCT layers
      or wheels from which SpacePoints have to be paired if the TRT track parameter comes from the
      corresponding TRT part.

      The numbering scheme for the TRT detectors:
          0     ->  barrel straws
       1 -  14  ->  forward wheels, counted from inside out
      -1 - -14  ->  backward wheels, -1 the innermost

      The numbering scheme for the SCT detector (first number ATLAS, second number within this Tool):

		          ____backw_________________________     __barrel__    ________forw_____________
		 Index =   8   7   6   5   4   3   2   1   0       3 (13)      0  1  2  3  4  5  6  7  8 
		          -9  -8  -7  -6  -5  -4  -3  -2  -1       2 (12)      1  2  3  4  5  6  7  8  9 
			   			                   1 (11)
						                   0 (10)


     The numbering scheme used here deviates from the numbering used in the ATLAS Detector description for two reasons:
       - In order to be fast, the identification has to be done by one id number only. ATLAS id is based on two information: 
         the barrel-endcap flag and the modul number. As the modul numbers alone are ambiguous (forward, backward, barrel may feature
         the same module numbers), an unambiguous id number is derived from both informations.

      - As anyway a specific numbering scheme has to be employed here, it is designed to be anti-symmetric with respect to forward and
        backward direction. This allows a convenient automatic set-up of the backward part of the look-up table, once the forward part is
        defined and thus reduces the likelihood of mistakes. In order to compensate for negative module numbers, an offset is added when they are
        used as index to the look-up table, because C++ does not allow negative indices (as FORTRAN would do ... :-) ) 
      
	Depending on the number of allowed holes, i.e. basically the number of SCT layers or wheels omitted between the SpacePoints of a pair,
        additional pairings of SCT layers are added to the look-up table.
   
  */

  m_modulLookupTable[0+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,13));
  if (m_maxHoles > 0 )
    {
      m_modulLookupTable[0+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,12));
      m_modulLookupTable[0+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,13));
      m_modulLookupTable[0+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,12));
    }
  if (m_maxHoles > 1 )
    {
      m_modulLookupTable[0+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,13));
      m_modulLookupTable[0+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,11));
    }

  m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,13));
  m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,12));
  m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,11));
  m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(13,1));
  m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,1));
  m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,1));
  m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,1));
  m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(1,2));
  if (m_maxHoles > 0 )
    {
      m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,13));
      m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,12));
      m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(13,2));
      m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,2));
      m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,2));
      m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,2));
    }
  if (m_maxHoles > 1 )
    {
      m_modulLookupTable[1+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,13));
    }

  //  TRT wheels 2 and 3
  for (int i = 2; i<4 ; ++ i)
    {
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,13));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(13,1));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,1));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,1));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,1));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(1,2));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(2,3));
      if (m_maxHoles > 0 )
	{
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,12));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(13,2));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,2));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,2));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,2));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(1,3));
	}
      if (m_maxHoles > 1 )
	{
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,11));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,12));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(13,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,3));
	}
    }

  // TRT wheels 4 - 6
  for (int i = 4 ; i < 7 ; ++i )
    {
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,13));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(13,1));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,1));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,1));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,1));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(1,2));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(2,3));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(3,4));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(4,5));
      if (m_maxHoles > 0 )
	{
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,13));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,12));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(13,2));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,2));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,2));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,2));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(1,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(2,4));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(3,5));
	}
      if (m_maxHoles > 1 )
	{
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(13,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(11,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(10,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(1,4));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(2,5));
	}
    }


  // TRT wheels 7 - 9
  for ( int i = 7 ;  i < 10 ; ++i)
    {
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(2,3));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(3,4));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(4,5));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(5,6));  
      if (m_maxHoles > 0 )
	{
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(1,2));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(1,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(2,4));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(3,5));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(4,6));
	}
      if (m_maxHoles > 1 )
	{
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(13,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(12,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(1,4));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(2,5));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(3,6));
	}
    }


  // TRT wheels 10 - 12
  for (int i = 10 ; i < 13 ; ++i )
    {
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(4,5));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(5,6));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(6,7));
      if (m_maxHoles > 0 )
	{
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(3,4));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(3,5));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(4,6));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(5,7));
	}
      if (m_maxHoles > 1 )
	{
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(2,3));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(2,4));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(2,5));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(3,6));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(4,7));
	}
    }


  // TRT - wheels 13 and 14
  for (int i = 13 ; i < 15 ; ++i )
    {
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(5,6));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(6,7));
      m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(7,8));
      if (m_maxHoles > 0 )
	{
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(4,5));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(4,6));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(5,7));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(6,8));
	}
      if (m_maxHoles > 0 )
	{
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(3,4));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(3,5));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(3,6));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(4,7));
	  m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(5,8));
	}
    }


  // mirror the endcap information
  for ( int iTable = 1 ; iTable < 15 ; ++iTable )
    {
      std::list<std::pair<int,int> >::const_iterator  itList = m_modulLookupTable[iTable+SIMPLE_TRT_INDEX_OFFSET].begin();
      std::list<std::pair<int,int> >::const_iterator endList = m_modulLookupTable[iTable+SIMPLE_TRT_INDEX_OFFSET].end();
      for ( ; itList != endList ; ++itList )
	{
	  int i = itList->first;
	  int j = itList->second;
	  if (i<10) i*=-1;   // don't change the sign of the barrel modules...
	  if (j<10) j*=-1;
	  m_modulLookupTable[-1*iTable+SIMPLE_TRT_INDEX_OFFSET].push_back(std::make_pair(i,j));
	}
    }	  

  // fill the short index for the lookup table
  for ( int iTable = -14 ; iTable < 15 ; ++iTable )
    {
      std::list<std::pair<int,int> >::const_iterator  itList = m_modulLookupTable[iTable+SIMPLE_TRT_INDEX_OFFSET].begin();
      std::list<std::pair<int,int> >::const_iterator endList = m_modulLookupTable[iTable+SIMPLE_TRT_INDEX_OFFSET].end();
      for ( ; itList != endList ; ++itList )
	{
	  m_modulLookupTableIndex[iTable+SIMPLE_TRT_INDEX_OFFSET].insert(itList->first);
	  m_modulLookupTableIndex[iTable+SIMPLE_TRT_INDEX_OFFSET].insert(itList->second);
	}
    }

  printLookupTable();

}

//=====================================================================================================

MsgStream& InDet::SimpleTRT_SeededSpacePointFinder_ATL::dump( MsgStream& out ) const
{
  out << "to be implemented soon..." << std::endl;

  return out;
}
//=====================================================================================================

std::ostream& InDet::SimpleTRT_SeededSpacePointFinder_ATL::dump( std::ostream& out ) const
{
  out << "to be implemented soon..." << std::endl;
  return out;
}

 
//=====================================================================================================

void InDet::SimpleTRT_SeededSpacePointFinder_ATL::printLookupTable()
{
  /** This method provides a printout of the look-up table for debugging purposes. It is called in the setup. */


  msg(MSG::VERBOSE) << "=====================================================================================================================" << endmsg;
  msg(MSG::VERBOSE) << "  Module Lookup table: " << endmsg;
  msg(MSG::VERBOSE) << "---------------------------------------------------------------------------------------------------------------------" << endmsg;
  for (int i = -14; i<15 ; ++i)
    {
      msg(MSG::VERBOSE) << "Module "<< i << "  : " << endmsg;
      std::list<std::pair<int,int> >::const_iterator  itList = m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].begin();
      std::list<std::pair<int,int> >::const_iterator endList = m_modulLookupTable[i+SIMPLE_TRT_INDEX_OFFSET].end();
      for ( ; itList != endList ; ++itList )
	msg(MSG::VERBOSE) << " (" << itList->first << ", " << itList->second << ") " ; 
      msg(MSG::VERBOSE) << "  " << endmsg;
      std::set<int>::const_iterator  itIndex = m_modulLookupTableIndex[i+SIMPLE_TRT_INDEX_OFFSET].begin();
      std::set<int>::const_iterator endIndex = m_modulLookupTableIndex[i+SIMPLE_TRT_INDEX_OFFSET].end();
      msg(MSG::VERBOSE) << "Needs Layers: { " ;
      for ( ; itIndex != endIndex ; ++ itIndex )
	msg(MSG::VERBOSE) << *itIndex << ", " ;
      msg(MSG::VERBOSE) << " } " << endmsg;
      msg(MSG::VERBOSE) << "...................................................................................................................." << endmsg;

    }
  msg(MSG::VERBOSE) << "=====================================================================================================================" << endmsg;

}

//=====================================================================================================

int InDet::SimpleTRT_SeededSpacePointFinder_ATL::TRT_Module(const Trk::TrackParameters& directionTRT)
{
  /** This method returns the id number of the TRT part, the given trackparameter comes from following the
      numbering scheme applied in this Tool (see documentation to setupLookupTable).
  */

  // find out from which SCT part the segment is built
  Identifier id  ;
 
    id = directionTRT.associatedSurface().associatedDetectorElementIdentifier();
 
  if (!id.is_valid())
    {
      msg(MSG::WARNING) << " Id not valid "<<endmsg;
      return -999;
    }
  int modulNumber;
  int detRegionIndex = m_trtId->barrel_ec(id) ;  
  if ( detRegionIndex == -1 || detRegionIndex == 1 )
    modulNumber = 0;  // central TRT
  else if ( detRegionIndex == -2 || detRegionIndex == 2 )
    {
      modulNumber = m_trtId->layer_or_wheel(id) + 1  ; // forward/bachward wheels
      modulNumber *= (detRegionIndex/2) ;  // add sign to distinguish forward and backward
    }
  else
    {
      msg(MSG::WARNING) << "TRT barrel-endcap id not in {-2;-1;1;2}" << endmsg;
      return -999;
    }

  return modulNumber;

}
