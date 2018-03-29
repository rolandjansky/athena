/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonStationIntersectSvc/MdtIntersectGeometry.h"

#include "TrkDriftCircleMath/MdtChamberGeometry.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonCondInterface/IMDTConditionsSvc.h"

#include "MuonIdHelpers/MdtIdHelper.h"

namespace Muon{

  
  MdtIntersectGeometry::MdtIntersectGeometry( const Identifier& chid, const MuonGM::MuonDetectorManager* detMgr,
					      IMDTConditionsSvc* mdtCondSvc ) : m_chid(chid), m_mdtGeometry(0), m_detMgr(detMgr), m_mdtSummarySvc(mdtCondSvc)
  {
    init();
  }

  MdtIntersectGeometry::MdtIntersectGeometry(const MdtIntersectGeometry &right) {
    m_chid = right.m_chid;
    m_transform = right.m_transform;
    m_intersect = right.m_intersect;
    m_mdtGeometry = new TrkDriftCircleMath::MdtChamberGeometry();
    *m_mdtGeometry = *right.m_mdtGeometry;
    m_detElMl0 = right.m_detElMl0;
    m_detElMl1 = right.m_detElMl1;
    m_detMgr = right.m_detMgr;
    m_mdtSummarySvc = right.m_mdtSummarySvc;
    m_mdtIdHelper = right.m_mdtIdHelper;
  }

  MdtIntersectGeometry& MdtIntersectGeometry::operator=(const MdtIntersectGeometry& right) {
    if (this!=&right) {
      m_chid = right.m_chid;
      m_transform = right.m_transform;
      m_intersect = right.m_intersect;
      if(m_mdtGeometry) delete m_mdtGeometry;
      m_mdtGeometry = new TrkDriftCircleMath::MdtChamberGeometry(*right.m_mdtGeometry);
      m_detElMl0 = right.m_detElMl0;
      m_detElMl1 = right.m_detElMl1;
      m_detMgr = right.m_detMgr;
      m_mdtSummarySvc = right.m_mdtSummarySvc;
      m_mdtIdHelper = right.m_mdtIdHelper;
    }
    return *this;
  } 

  MdtIntersectGeometry::~MdtIntersectGeometry(){
    delete m_mdtGeometry;
  }

  const MuonStationIntersect& MdtIntersectGeometry::intersection( const Amg::Vector3D& pos, const Amg::Vector3D& dir) const
  {
    if( !m_mdtGeometry ){
      std::cout << " MdtIntersectGeometry not correctly initialized  " << m_mdtIdHelper->print_to_string(m_chid) << std::endl;
      return m_intersect;
    }

    Amg::Vector3D  lpos = transform()*pos;
    Amg::Vector3D ldir = (transform().linear()*dir).unit();
    
    double dxdy = fabs(ldir.y()) > 0.001 ? ldir.x()/ldir.y() : 1000.;
    //if( dxdy == 1000. ){
    //  std::cout << " strang direction " << dir << std::endl;
    //}

    double lineAngle = atan2(ldir.z(),ldir.y());
    TrkDriftCircleMath::LocPos linePos( lpos.y(),lpos.z() );

    TrkDriftCircleMath::Line line( linePos, lineAngle );
    const TrkDriftCircleMath::DCVec& dcs = m_mdtGeometry->tubesPassedByLine( line );
    
    MuonStationIntersect::TubeIntersects intersects;

//     std::cout << " tubes intersected by line " << line << " -> " << dcs.size() << std::endl;
    TrkDriftCircleMath::DCCit dit = dcs.begin();
    TrkDriftCircleMath::DCCit dit_end = dcs.end();
    for( ; dit!=dit_end;++dit ){

      double xint = dxdy*( dit->position().x() - lpos.y() ) + lpos.x();
      Identifier tubeid = m_mdtIdHelper->channelID( m_chid, dit->id().ml()+1, dit->id().lay()+1, dit->id().tube()+1 );
      if( m_deadTubesML.find( m_mdtIdHelper->multilayerID(tubeid) ) != m_deadTubesML.end() ) {
        if( std::find( m_deadTubes.begin(), m_deadTubes.end(), tubeid ) != m_deadTubes.end() )
          continue;
      }
      double distWall = fabs(xint) - 0.5*tubeLength( dit->id().ml(), dit->id().lay(), dit->id().tube() );
//       if(   distWall > -500. ){
// 	std::cout << " muon close to tube end: " << *dit << std::endl;
// 	std::cout << " tube intersect " << distWall << std::endl;
//       }
      intersects.push_back( MuonTubeIntersect( tubeid, dit->dr(), distWall ) );
	
    }
    m_intersect.setTubeIntersects( intersects );

    return m_intersect;
  }

  double MdtIntersectGeometry::tubeLength( int ml, int layer, int tube ) const
  {
    // shift by one to account for MuonGeoModel scheme
    tube  += 1;
    layer += 1;
    // handle case where first ml is dead
    if( ml == 1 && !m_detElMl1 ) return m_detElMl0->getActiveTubeLength( layer, tube );
    if( ml == 0 ) return m_detElMl0->getActiveTubeLength( layer, tube );
    else          return m_detElMl1->getActiveTubeLength( layer, tube );
  }


  void MdtIntersectGeometry::init()
  {

    m_mdtIdHelper = m_detMgr->mdtIdHelper();

    /* calculate chamber geometry
       it takes as input:
         distance between the first and second tube in the chamber within a layer along the tube layer (tube distance)
         distance between the first tube in the first layer and the first tube in the second layer along the tube layer (tube stagering)
	 distance between the first and second layer perpendicular to the tube layers (layer distance)
	 position of the first hit in ml 0 and ml 1 (2D in plane)
	 total number of multilayers
	 total number of layers
	 total number of tubes per layer for each multilayer
	 an identifier uniquely identifying the chamber
    */

    // get id
    int eta =  m_mdtIdHelper->stationEta(m_chid);
    int phi = m_mdtIdHelper->stationPhi(m_chid);
    int name = m_mdtIdHelper->stationName(m_chid);
    int isBarrel = m_mdtIdHelper->isBarrel(m_chid);
    TrkDriftCircleMath::MdtStationId  stationId( isBarrel, name, eta, phi );
    
    // get detEL for first ml (always there)
    Identifier firstIdml0 = m_mdtIdHelper->channelID( name,eta,phi,1,1,1 );
    Identifier firstIdml1;

    m_detElMl0 = m_detMgr->getMdtReadoutElement( firstIdml0 );
    m_detElMl1 = 0;
    
    if( !m_detElMl0 ) {
      std::cout << " no detEl for " << m_mdtIdHelper->print_to_string(m_chid) << std::endl;
      return;
    }

    // number of multilayers in chamber
    int nml = m_detElMl0->nMDTinStation();
    
    //if( nml != m_mdtIdHelper->numberOfMultilayers(m_chid) ){
    //  std::cout << " bad number of ML " << nml << "  " << m_mdtIdHelper->numberOfMultilayers(m_chid)
    //		<< "   " << m_mdtIdHelper->print_to_string(m_chid) << std::endl;
    //    }
    

    // treament of chambers with two ml
    if( nml == 2 ){
      firstIdml1 = m_mdtIdHelper->channelID( name,eta,phi,2,1,1 );
      m_detElMl1 = m_detMgr->getMdtReadoutElement( firstIdml1 ); 
    }
    
    // now evaluate result from IMDTConditionsSvc:
    // if one of the two ml is dead treat the chamber as a single ML station
    // if both are dead give a WARNING
    // check status of the two multilayers using the IMDTConditionsSvc
    bool goodMl0 = m_mdtSummarySvc->isGoodMultiLayer(firstIdml0);
    bool goodMl1 = m_detElMl1 ? m_mdtSummarySvc->isGoodMultiLayer(firstIdml1) : false;
    int firstMlIndex = 1;
    if( goodMl0 && !goodMl1 ){
      //std::cout << " Second multi layer dead in chamber " << m_mdtIdHelper->print_to_string(firstIdml1) << std::endl;
      nml = 1;
      m_detElMl1 = 0;
    }else if( !goodMl0 && goodMl1 ){
      //std::cout << " First multi layer dead in chamber " << m_mdtIdHelper->print_to_string(firstIdml0) << std::endl;
      nml = 1;
      // swap detEl1 and detEl0
      m_detElMl0 = m_detElMl1;
      m_detElMl1 = 0;
      firstIdml0 = firstIdml1;
      firstMlIndex = 2;
    }else if( !goodMl0 && !goodMl1 ) {
      //std::cout << " ERROR both multi layers dead! Why am I beng created! " << std::endl;
      return;
    }
    m_transform = m_detElMl0->GlobalToAmdbLRSTransform();

    
    // number of layers and tubes
    int nlay = m_detElMl0->getNLayers();
    int ntube0 = m_detElMl0->getNtubesperlayer();
    int ntube1 = m_detElMl1 ? m_detElMl1->getNtubesperlayer() : 0;


    // position first tube in ml 0 and 1
    Amg::Vector3D firstTubeMl0 = transform()*(m_detElMl0->tubePos( firstIdml0 ));
    Amg::Vector3D firstTubeMl1 = m_detElMl1 ? transform()*(m_detElMl1->tubePos( firstIdml1 )) : Amg::Vector3D();

    TrkDriftCircleMath::LocPos firstTube0( firstTubeMl0.y(), firstTubeMl0.z() );
    TrkDriftCircleMath::LocPos firstTube1( firstTubeMl1.y(), firstTubeMl1.z() );
        
    // position second tube in ml 0
    Identifier secondIdml0 = m_mdtIdHelper->channelID( name,eta,phi,firstMlIndex,1,2 );
    Amg::Vector3D secondTubeMl0 = transform()*(m_detElMl0->tubePos( secondIdml0 ));

    if(m_detElMl0) fillDeadTubes(m_detElMl0);
    if(m_detElMl1) fillDeadTubes(m_detElMl1);
	
    // position first tube in second layer ml 0 
    Identifier firstIdml0lay1 = m_mdtIdHelper->channelID( name,eta,phi,firstMlIndex,2,1 );
    Amg::Vector3D firstTubeMl0lay1 = transform()*(m_detElMl0->tubePos( firstIdml0lay1 ));
	
   
    double tubeDist = (secondTubeMl0 - firstTubeMl0).y();      // distance between tube in a given layer
    double tubeStage = (firstTubeMl0lay1 - firstTubeMl0).y();  // tube stagering distance
    double layDist = (firstTubeMl0lay1 - firstTubeMl0).z();    // distance between layers


    m_mdtGeometry = new TrkDriftCircleMath::MdtChamberGeometry(stationId,nml,nlay,ntube0,ntube1,firstTube0,firstTube1,
							       tubeDist,tubeStage,layDist,m_detElMl0->center().theta());
    
    // finally if the first ml is dead, configure the MdtChamberGeometry accordingly
    if( !goodMl0 && goodMl1 ) m_mdtGeometry->isSecondMultiLayer(true);
  }

  void MdtIntersectGeometry::fillDeadTubes(const MuonGM::MdtReadoutElement* mydetEl){

    if( (mydetEl->getStationName()).find("BMG") != std::string::npos ) {
      PVConstLink cv = mydetEl->getMaterialGeom(); // it is "Multilayer"
      int nGrandchildren = cv->getNChildVols();
      if(nGrandchildren <= 0) return;

      Identifier detElId = mydetEl->identify();

      int name = m_mdtIdHelper->stationName(detElId);
      int eta = m_mdtIdHelper->stationEta(detElId);
      int phi = m_mdtIdHelper->stationPhi(detElId);
      int ml = m_mdtIdHelper->multilayer(detElId);

      for(int layer = 1; layer <= mydetEl->getNLayers(); layer++){
         for(int tube = 1; tube <= mydetEl->getNtubesperlayer(); tube++){
            bool tubefound = false;
            for(int kk=0; kk < nGrandchildren; kk++) {
               auto idOfChildVol = cv->getIdOfChildVol(kk);
               int tubegeo = idOfChildVol % 100;
               int layergeo = ( idOfChildVol - tubegeo ) / 100;
               if( tubegeo == tube && layergeo == layer ) {
                 tubefound=true;
                 break;
               }
               if( layergeo > layer ) break; // don't loop any longer if you cannot find tube anyway anymore
            }
            if(!tubefound) {
              Identifier deadTubeId = m_mdtIdHelper->channelID( name, eta, phi, ml, layer, tube );
              Identifier deadTubeMLId = m_mdtIdHelper->multilayerID( deadTubeId );
              m_deadTubes.push_back( deadTubeId );
              m_deadTubesML.insert( deadTubeMLId );
              // std::cout << " MdtIntersectGeometry: adding dead tube (" << tube  << "), layer(" <<  layer 
              //           << "), phi(" << phi << "), eta(" << eta << "), name(" << name 
              //           << ") and adding multilayerId(" << deadTubeMLId << ")." <<std::endl;
            }
         }
      }
    }
  }

}
