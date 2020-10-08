/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTest/AlignCondAthTest.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonAlignmentData/CorrContainer.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

AlignCondAthTest::AlignCondAthTest(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_alinePrint(true),
  m_blinePrint(true),
  m_mdtPrint(true),
  m_rpcPrint(true),
  m_tgcPrint(true),
  m_cscPrint(true)
{
  declareProperty( "ALinePrint", m_alinePrint );
  declareProperty( "BLinePrint", m_blinePrint );
  declareProperty( "MdtPrint", m_mdtPrint );
  declareProperty( "RpcPrint", m_rpcPrint );
  declareProperty( "TgcPrint", m_tgcPrint );
  declareProperty( "CscPrint", m_cscPrint );
}
 
StatusCode AlignCondAthTest::initialize(){

  ATH_CHECK(detStore()->retrieve(m_MuonDetMgrDS));

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

  if(m_alinePrint) {
    std::cout << "************************ BEGIN: ALines from Detector Store **************************" << std::endl;
    std::ofstream* foutDS = new std::ofstream("ALines_DS.txt");
    if(checkALines(m_MuonDetMgrDS, foutDS).isFailure()) return StatusCode::FAILURE;
    foutDS->close(); delete foutDS;
    std::cout << "************************ END: ALines from Detector Store **************************" << std::endl;
    std::cout << "************************ BEGIN: ALines from Conditions Store **************************" << std::endl;
    std::ofstream* foutCS = new std::ofstream("ALines_CS.txt");
    if(checkALines(MuonDetMgr, foutCS).isFailure()) return StatusCode::FAILURE;
    foutCS->close(); delete foutCS;
    std::cout << "************************ END: ALines from Conditions Store **************************" << std::endl;
  }
  if(m_blinePrint) {
    std::cout << "************************ BEGIN: BLines from Detector Store **************************" << std::endl;
    std::ofstream* foutDS = new std::ofstream("BLines_DS.txt");
    if(checkBLines(m_MuonDetMgrDS, foutDS).isFailure()) return StatusCode::FAILURE;
    foutDS->close(); delete foutDS;
    std::cout << "************************ END: BLines from Detector Store **************************" << std::endl;
    std::cout << "************************ BEGIN: BLines from Conditions Store **************************" << std::endl;
    std::ofstream* foutCS = new std::ofstream("BLines_CS.txt");
    if(checkBLines(MuonDetMgr, foutCS).isFailure()) return StatusCode::FAILURE;
    foutCS->close(); delete foutCS;
    std::cout << "************************ END: BLines from Conditions Store **************************" << std::endl;
  }
  if(m_mdtPrint) {
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
  if(m_rpcPrint) {
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
  if(m_tgcPrint) {
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
  if(m_cscPrint) {
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

StatusCode AlignCondAthTest::checkALines(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout) 
{
  for (const auto& [ALineId, ALine] : *manager->ALineContainer()) {
    std::string stationType;
    int jff,jzz,job;
    ALine.getAmdbId(stationType,jff,jzz,job);
    float s,z,t,ths,thz,tht;
    ALine.getParameters(s,z,t,ths,thz,tht);
    
    (*fout) << "A " << std::setiosflags(std::ios::fixed|std::ios::right)
	    << std::setw(4) << stationType  <<" " 
	    << std::setw(2) << jff  <<"  " 
	    << std::setw(2) << jzz  <<" " 
	    << std::setw(2) << job  <<" "
	    << std::setw(6) << std::setprecision(3) <<  s   <<" "  // here cm !
	    << std::setw(6) << std::setprecision(3) <<  z   <<" "  // here cm !
	    << std::setw(6) << std::setprecision(3) <<  t   <<" "  // here cm !
	    << std::setw(6) << std::setprecision(6) << ths  <<" " 
	    << std::setw(6) << std::setprecision(6) << thz  <<" " 
	    << std::setw(6) << std::setprecision(6) << tht  <<" "
	    << &ALineId << std::endl;

  }

  return StatusCode::SUCCESS;
}

StatusCode AlignCondAthTest::checkBLines(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout) 
{
  for (const auto& [BLineId, BLine] : *manager->BLineContainer()) {
    std::string stationType;
    int jff,jzz,job;
    BLine.getAmdbId(stationType,jff,jzz,job);
    float bs,bp,bn,sp,sn,tw,pg,tr,eg,ep,en;
    BLine.getParameters(bs,bp,bn,sp,sn,tw,pg,tr,eg,ep,en);
    
    (*fout) << "B " << std::setiosflags(std::ios::fixed|std::ios::right)
	    << std::setw(4) << stationType  <<" " 
	    << std::setw(2) << jff  <<" " 
	    << std::setw(2) << jzz  <<" " 
	    << std::setw(2) << job  <<"  "
	    << std::setw(6) << std::setprecision(6) <<  bs  <<" "
	    << std::setw(6) << std::setprecision(6) <<  bp  <<" "
	    << std::setw(6) << std::setprecision(6) <<  bn  <<" "
	    << std::setw(6) << std::setprecision(6) <<  sp  <<" " 
	    << std::setw(6) << std::setprecision(6) <<  sn  <<" " 
	    << std::setw(6) << std::setprecision(6) <<  tw  <<" "
	    << std::setw(6) << std::setprecision(6) <<  pg  <<" "
	    << std::setw(6) << std::setprecision(6) <<  tr  <<" "
	    << std::setw(6) << std::setprecision(6) <<  eg  <<" "
	    << std::setw(6) << std::setprecision(6) <<  ep  <<" "
	    << std::setw(6) << std::setprecision(6) <<  en  <<" "
	    << BLineId << std::endl;
  }

  return StatusCode::SUCCESS;
}

StatusCode AlignCondAthTest::checkMdtGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout) 
{
  for( int i1 = 0;i1 < manager->NMdtStatType; ++i1 ){
    for( int i2 = 0;i2 < manager->NMdtStatEta; ++i2 ){
      for( int i3 = 0;i3 < manager->NMdtStatPhi; ++i3 ){
  	for( int i4 = 0;i4 < manager->NMdtMultilayer; ++i4 ){
  	  const MuonGM::MdtReadoutElement* detEl = manager->getMdtReadoutElement(i1,i2,i3,i4);
  	  if( !detEl ) continue;
  	  const std::vector<const Trk::Surface*>& Nsurf = detEl->surfaces();
  	  (*fout) << " New " << m_idHelperSvc->mdtIdHelper().print_to_string(detEl->identify())
  		  << " nlayers " << detEl->getNLayers() << " ntubes " << detEl->getNtubesperlayer() << " Nsurf " << Nsurf.size() << std::endl
  		  << Amg::toString( detEl->transform(),6 ) << std::endl;
	  
  	  for( int tl = 0; tl<detEl->getNLayers();++tl ){
  	    for( int t = 0; t<detEl->getNtubesperlayer();++t ){
  	      const Trk::Surface& surf = detEl->surface(tl+1,t+1);
  	      (*fout) << " New tube: layer " << tl << " tube " << t << std::endl
  		      << " X, Y, Z:       " << detEl->tubePos(i4+1, tl+1, t+1).x() << ", " << detEl->tubePos(i4+1, tl+1, t+1).y() << ", " << detEl->tubePos(i4+1, tl+1, t+1).z() << std::endl
  		      << " Local X, Y, Z: " << detEl->localTubePos(i4+1, tl+1, t+1).x() << ", " << detEl->localTubePos(i4+1, tl+1, t+1).y() << ", " << detEl->localTubePos(i4+1, tl+1, t+1).z() << std::endl
  		      << " Cent X, Y, Z: " << detEl->center(tl+1, t+1).x() << ", " << detEl->center(tl+1, t+1).y() << ", " << detEl->center(tl+1, t+1).z() << std::endl
  		      << " Norm X, Y, Z: " << detEl->normal(tl+1, t+1).x() << ", " << detEl->normal(tl+1, t+1).y() << ", " << detEl->normal(tl+1, t+1).z() << std::endl
  		      << " TNor X, Y, Z: " << detEl->tubeNormal(tl+1, t+1).x() << ", " << detEl->tubeNormal(tl+1, t+1).y() << ", " << detEl->tubeNormal(tl+1, t+1).z() << std::endl
		      << Amg::toString( surf.transform(),6 ) << std::endl << detEl->bounds(tl+1, t+1) << std::endl;
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
        int stationName = manager->rpcStationName(i1);
        bool isValid=false;
        Identifier rpcId = m_idHelperSvc->rpcIdHelper().channelID(stationName, i2, i3, i4, i5, 1, 1, 1, 1, true, &isValid); // last 5 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid
        if (!isValid) continue;
  	    const MuonGM::RpcReadoutElement* detEl = manager->getRpcReadoutElement(rpcId);
  	    if( !detEl ) continue;
	    const std::vector<const Trk::Surface*>& Nsurf = detEl->surfaces();
   
	    Identifier id = detEl->identify();
	    const Amg::Vector3D stripPos = detEl->stripPos(id);
	    const Amg::Vector3D localStripPos = detEl->localStripPos(id);
  	    (*fout) << " New " << m_idHelperSvc->rpcIdHelper().print_to_string(detEl->identify())
  		    << " NphiStripPanels " << detEl->NphiStripPanels() << " Nsurf " << Nsurf.size() << std::endl
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
	const std::vector<const Trk::Surface*>& Nsurf = detEl->surfaces();

	Identifier id = detEl->identify();
	const Amg::Vector3D stripPos = detEl->stripPos(id);
	const Amg::Vector3D localStripPos = detEl->localStripPos(id);
	(*fout) << " New " << m_idHelperSvc->tgcIdHelper().print_to_string(detEl->identify()) << " Nsurf " << Nsurf.size() << std::endl 
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
	const std::vector<const Trk::Surface*>& Nsurf = detEl->surfaces();
	      
	Identifier id = detEl->identify();
	const Amg::Vector3D stripPos = detEl->stripPos(id);
	const Amg::Vector3D localStripPos = detEl->localStripPos(id);
	(*fout) << " New " << m_idHelperSvc->tgcIdHelper().print_to_string(detEl->identify()) << " Nsurf " << Nsurf.size() << std::endl 
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

