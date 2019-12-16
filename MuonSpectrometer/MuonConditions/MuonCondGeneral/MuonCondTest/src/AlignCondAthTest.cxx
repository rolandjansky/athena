/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonCondTest/AlignCondAthTest.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

AlignCondAthTest::AlignCondAthTest(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator){
}
 
StatusCode AlignCondAthTest::initialize(){
//

  ATH_MSG_INFO( "in initialize()"  );
  if (StatusCode::SUCCESS != detStore()->retrieve(m_MuonDetMgrDS)) {
    ATH_MSG_FATAL("Couldn't load MuonDetectorManager");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_idHelperSvc.retrieve());

  ATH_CHECK(m_DetectorManagerKey.initialize());

  return StatusCode::SUCCESS;

}
 
StatusCode AlignCondAthTest::execute() {
//

  ATH_MSG_INFO( " AlignCondAthTest in execute()"  );

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
  const MuonGM::MuonDetectorManager* MuonDetMgr{*DetectorManagerHandle}; 
  if(MuonDetMgr==nullptr){
    ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
    return StatusCode::FAILURE; 
  } 

  bool MdtPrint = true;
  bool RpcPrint = true;
  bool TgcPrint = true;
  bool CscPrint = true;

  if(MdtPrint) {
    std::cout << "************************ BEGIN: MdtReadoutElements from Detector Store **************************" << std::endl;
    std::ofstream* foutDS = new std::ofstream("MdtOut_DS.txt");
    if(checkMdtGeometry(m_MuonDetMgrDS, foutDS).isFailure()) return StatusCode::FAILURE;
    foutDS->close(); delete foutDS;
    std::cout << "************************ END: MdtReadoutElements from Detector Store **************************" << std::endl;
    std::cout << "************************ BEGIN: MdtReadoutElements from Conditions Store **************************" << std::endl;
    std::ofstream* foutCS = new std::ofstream("MdtOut_CS.txt");
    if(checkMdtGeometry(MuonDetMgr, foutCS).isFailure()) return StatusCode::FAILURE;
    foutCS->close(); delete foutCS;
    std::cout << "************************ END: MdtReadoutElements from Conditions Store **************************" << std::endl;
  }
  if(RpcPrint) {
    std::cout << "************************ BEGIN: RpcReadoutElements from Detector Store **************************" << std::endl;
    std::ofstream* foutDS = new std::ofstream("RpcOut_DS.txt");
    if(checkRpcGeometry(m_MuonDetMgrDS, foutDS).isFailure()) return StatusCode::FAILURE;
    foutDS->close(); delete foutDS;
    std::cout << "************************ END: RpcReadoutElements from Detector Store **************************" << std::endl;
    std::cout << "************************ BEGIN: RpcReadoutElements from Conditions Store **************************" << std::endl;
    std::ofstream* foutCS = new std::ofstream("RpcOut_CS.txt");
    if(checkRpcGeometry(MuonDetMgr, foutCS).isFailure()) return StatusCode::FAILURE;
    foutCS->close(); delete foutCS;
    std::cout << "************************ END: RpcReadoutElements from Conditions Store **************************" << std::endl;
  }
  if(TgcPrint) {
    std::cout << "************************ BEGIN: TgcReadoutElements from Detector Store **************************" << std::endl;
    std::ofstream* foutDS = new std::ofstream("TgcOut_DS.txt");
    if(checkTgcGeometry(m_MuonDetMgrDS, foutDS).isFailure()) return StatusCode::FAILURE;
    foutDS->close(); delete foutDS;
    std::cout << "************************ END: TgcReadoutElements from Detector Store **************************" << std::endl;
    std::cout << "************************ BEGIN: TgcReadoutElements from Conditions Store **************************" << std::endl;
    std::ofstream* foutCS = new std::ofstream("TgcOut_CS.txt");
    if(checkTgcGeometry(MuonDetMgr, foutCS).isFailure()) return StatusCode::FAILURE;
    foutCS->close(); delete foutCS;
    std::cout << "************************ END: TgcReadoutElements from Conditions Store **************************" << std::endl;
  }
  if(CscPrint) {
    std::cout << "************************ BEGIN: CscReadoutElements from Detector Store **************************" << std::endl;
    std::ofstream* foutDS = new std::ofstream("CscOut_DS.txt");
    if(checkCscGeometry(m_MuonDetMgrDS, foutDS).isFailure()) return StatusCode::FAILURE;
    foutDS->close(); delete foutDS;
    std::cout << "************************ END: CscReadoutElements from Detector Store **************************" << std::endl;
    std::cout << "************************ BEGIN: CscReadoutElements from Conditions Store **************************" << std::endl;
    std::ofstream* foutCS = new std::ofstream("CscOut_CS.txt");
    if(checkCscGeometry(MuonDetMgr, foutCS).isFailure()) return StatusCode::FAILURE;
    foutCS->close(); delete foutCS;
    std::cout << "************************ END: CscReadoutElements from Conditions Store **************************" << std::endl;
  }
  // if(checkCscGeometry().isFailure()) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
//
}
 
StatusCode AlignCondAthTest::finalize() {
//

  ATH_MSG_INFO( "in finalize()"  );
//
  return StatusCode::SUCCESS;
//
}
  
StatusCode AlignCondAthTest::checkMdtGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout) 
{
  for( int i1 = 0;i1 < manager->NMdtStatType; ++i1 ){
    for( int i2 = 0;i2 < manager->NMdtStatEta; ++i2 ){
      for( int i3 = 0;i3 < manager->NMdtStatPhi; ++i3 ){
	for( int i4 = 0;i4 < manager->NMdtMultilayer; ++i4 ){
	  const MuonGM::MdtReadoutElement* detEl = manager->getMdtReadoutElement(i1,i2,i3,i4);
	  if( !detEl ) continue;
	  
	  (*fout) << " New " << m_idHelperSvc->mdtIdHelper().print_to_string(detEl->identify())
		  << " nlayers " << detEl->getNLayers() << " ntubes " << detEl->getNtubesperlayer() << std::endl
		  << Amg::toString( detEl->transform(),6 ) << std::endl;
	  
	  for( int tl = 0; tl<detEl->getNLayers();++tl ){
	    for( int t = 0; t<detEl->getNtubesperlayer();++t ){
	      const Trk::Surface& surf = detEl->surface(tl+1,t+1);
	      (*fout) << " New tube: layer " << tl << " tube " << t << std::endl
		      << " X, Y, Z:       " << detEl->tubePos(i4+1, tl+1, t+1).x() << ", " << detEl->tubePos(i4+1, tl+1, t+1).y() << ", " << detEl->tubePos(i4+1, tl+1, t+1).z() << std::endl
		      << " Local X, Y, Z: " << detEl->localTubePos(i4+1, tl+1, t+1).x() << ", " << detEl->localTubePos(i4+1, tl+1, t+1).y() << ", " << detEl->localTubePos(i4+1, tl+1, t+1).z() << std::endl
		      << Amg::toString( surf.transform(),6 ) << Amg::toString( detEl->fromIdealToDeformed(i4, tl+1, t+1) ) << std::endl;
	    }
	  }
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode AlignCondAthTest::checkRpcGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout) 
{

  for( int i1 = 0;i1<manager->NRpcStatType; ++i1 ){
    for( int i2 = 0;i2<manager->NRpcStatEta; ++i2 ){
      for( int i3 = 0;i3<manager->NRpcStatPhi; ++i3 ){
  	for( int i4 = 0;i4<manager->NDoubletR; ++i4 ){
  	  for( int i5 = 0;i5<manager->NDoubletZ; ++i5 ){
  	    const MuonGM::RpcReadoutElement* detEl = manager->getRpcReadoutElement(i1,i2,i3,i4,i5);
  	    if( !detEl ) continue;
	    
	    Identifier id = detEl->identify();
	    const Amg::Vector3D stripPos = detEl->stripPos(id);
	    const Amg::Vector3D localStripPos = detEl->localStripPos(id);
  	    (*fout) << " New " << m_idHelperSvc->rpcIdHelper().print_to_string(detEl->identify())
  		    << " NphiStripPanels " << detEl->NphiStripPanels() << std::endl
		    << " X, Y, Z:       " << stripPos.x() << ", " << stripPos.y() << ", " << stripPos.z() << std::endl
		    << " Local X, Y, Z: " << localStripPos.x() << ", " << localStripPos.y() << ", " << localStripPos.z() << std::endl
		    << Amg::toString( detEl->transform(),6 ) << std::endl;
	    
  	    for( int dbPhi=1;dbPhi<=detEl->NphiStripPanels();++dbPhi ){
  	      for( int gp = 1; gp<=2;++gp ){
  		for( int mp = 0; mp<2;++mp ){
  		  const Trk::Surface& surf = detEl->surface( detEl->surfaceHash(dbPhi,gp,mp) );
  		  (*fout) << " New layer " << dbPhi << " gp " << gp << " measPhi " << mp << std::endl
  			  << Amg::toString( surf.transform(),6 ) << std::endl;
  		}
  	      }
  	    }
  	  }
  	}
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode AlignCondAthTest::checkTgcGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout) 
{

  for( int i1 = 0;i1<manager->NTgcStatType; ++i1 ){
    for( int i2 = 0;i2<manager->NTgcStatEta; ++i2 ){
      for( int i3 = 0;i3<manager->NTgcStatPhi; ++i3 ){
	
	const MuonGM::TgcReadoutElement* detEl = manager->getTgcReadoutElement(i1,i2,i3);
	if( !detEl ) continue;

	Identifier id = detEl->identify();
	const Amg::Vector3D stripPos = detEl->stripPos(id);
	const Amg::Vector3D localStripPos = detEl->localStripPos(id);
	(*fout) << " New " << m_idHelperSvc->tgcIdHelper().print_to_string(detEl->identify()) << std::endl 
		<< " X, Y, Z:       " << stripPos.x() << ", " << stripPos.y() << ", " << stripPos.z() << std::endl
		<< " Local X, Y, Z: " << localStripPos.x() << ", " << localStripPos.y() << ", " << localStripPos.z() << std::endl
		<< Amg::toString( detEl->transform(),6 ) << std::endl;
	
	for( int gp = 0; gp<2;++gp ){
	  for( int mp = 0; mp<2;++mp ){
	    const Trk::Surface& surf = detEl->surface(2*gp+mp);
	    (*fout) << " New layer: gp " << gp << " measPhi " << mp << std::endl
		    << Amg::toString( surf.transform(),6 ) << std::endl;
	  }
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode AlignCondAthTest::checkCscGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout) 
{
  for( int i1 = 0;i1<manager->NCscStatType; ++i1 ){
    for( int i2 = 0;i2<manager->NCscStatEta; ++i2 ){
      for( int i3 = 0;i3<manager->NCscStatPhi; ++i3 ){
	const MuonGM::CscReadoutElement* detEl = manager->getCscReadoutElement(i1,i2,i3,1);
	if( !detEl ) continue;
	      
	Identifier id = detEl->identify();
	const Amg::Vector3D stripPos = detEl->stripPos(id);
	const Amg::Vector3D localStripPos = detEl->localStripPos(id);
	(*fout) << " New " << m_idHelperSvc->tgcIdHelper().print_to_string(detEl->identify()) << std::endl 
		<< " X, Y, Z:       " << stripPos.x() << ", " << stripPos.y() << ", " << stripPos.z() << std::endl
		<< " Local X, Y, Z: " << localStripPos.x() << ", " << localStripPos.y() << ", " << localStripPos.z() << std::endl
		<< Amg::toString( detEl->transform(),6 ) << std::endl;

	for( int gp = 0; gp<4;++gp ){
	  for( int mp = 0; mp<2;++mp ){
	    const Trk::Surface& surf = detEl->surface(2*gp+mp);
	    (*fout) << " New layer: gp " << gp << " measPhi " << mp << std::endl
		    << Amg::toString( surf.transform(),6 ) << std::endl;
	  }
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}

