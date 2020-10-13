/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGMNtupleWriter.h"
#include "GaudiKernel/ITHistSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h" 
#include "MuonReadoutGeometry/MdtReadoutElement.h" 
#include "MuonReadoutGeometry/RpcReadoutElement.h" 
#include "MuonReadoutGeometry/TgcReadoutElement.h" 
#include "MuonReadoutGeometry/CscReadoutElement.h" 
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include <TTree.h>
#include <fstream>
#include <sstream>

namespace MuonGM {
   
  MuonGMNtupleWriter::MuonGMNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_tree(0),
    m_nevents(0),
    m_detMgr(NULL)
  {
    declareProperty("NtupleFileName",       m_ntupleFileName = "MuonGMNtuple");
    declareProperty("NtupleDirectoryName",  m_ntupleDirName  = "MuonGM");
    declareProperty("NtupleTreeName",       m_ntupleTreeName = "data");
    declareProperty("OutputTextFile",       m_outputToTextFile = false );

  }

  MuonGMNtupleWriter::~MuonGMNtupleWriter() {

  }

  StatusCode MuonGMNtupleWriter::initialize() {

    if (detStore()->retrieve( m_detMgr ).isFailure()){
      ATH_MSG_ERROR("Cannot retrieve MuonDetectorManager");
      return StatusCode::FAILURE;
    }

    std::string streamName = "/"+m_ntupleFileName+"/"+m_ntupleDirName;
    std::string treeName = m_ntupleTreeName;

    std::string treePath = streamName+"/"+treeName;
    
    m_tree = new TTree(treeName.c_str(), "MdtTubeSurface");
    
    ITHistSvc* rootsvc = 0;  
    if( service("THistSvc",rootsvc, true).isFailure() ) {
      ATH_MSG_ERROR("Unable to locate the MDTPRDValAlg Histogram service");
      delete m_tree;
      return StatusCode::FAILURE;   
    }
    if(rootsvc->regTree(treePath, m_tree).isFailure()) {
      ATH_MSG_ERROR("MuonGMNtupleWriter:: Unable to register TTreeTuple : " << treePath );
      delete m_tree;
      return StatusCode::FAILURE;   
    }

    // if(m_idHelper.retrieve().isFailure()) {
    //   ATH_MSG_ERROR("Failed to retrieve: " << m_idHelper );
    //   return StatusCode::FAILURE;   
    // }
    
    m_mdtSurfaceBranch.initForWrite(*m_tree,"mdt_");

    return AthAlgorithm::initialize();
  }

  StatusCode MuonGMNtupleWriter::execute() {
    ++m_nevents;
    if( m_nevents == 1 ) fillNtuple();

    return StatusCode::SUCCESS;
  }
       
  void MuonGMNtupleWriter::fillNtuple() {
    
    std::ofstream* fout = 0;
    if( m_outputToTextFile ){
      std::string gVersion = m_detMgr->geometryVersion();
      std::string fileName = "muon_current_"+gVersion;
      fout = new std::ofstream(fileName.c_str());
    }

    unsigned int nmdt = 0;
    unsigned int nrpc = 0;
    unsigned int ntgc = 0;
    unsigned int ncsc = 0;
    for( int i1 = 0;i1<MuonDetectorManager::NMdtStatType; ++i1 ){
      for( int i2 = 0;i2<MuonDetectorManager::NMdtStatEta; ++i2 ){
	for( int i3 = 0;i3<MuonDetectorManager::NMdtStatPhi; ++i3 ){
	  for( int i4 = 0;i4<MuonDetectorManager::NMdtMultilayer; ++i4 ){
	    const MdtReadoutElement* detEl = m_detMgr->getMdtReadoutElement(i1,i2,i3,i4);
	    if( !detEl ) continue;
	    ++nmdt;
	    if( fout ) {
	      (*fout) << " New MDT ReadoutElement " << detEl->identify().get_compact() 
		      << " " << m_detMgr->mdtIdHelper()->print_to_string(detEl->identify()) // m_idHelper->toStringDetEl(detEl->identify() ) 
		      << " nlayers " << detEl->getNLayers() << " ntubes " << detEl->getNtubesperlayer() << std::endl
		      << Amg::toString( detEl->transform(),6 ) << std::endl;
	    }

	    for( int tl = 0; tl<detEl->getNLayers();++tl ){
	      for( int t = 0; t<detEl->getNtubesperlayer();++t ){
		const Trk::Surface& surf = detEl->surface(tl+1,t+1);
		m_mdtSurfaceBranch.fill(surf);
		m_tree->Fill();
		if( fout ) {
		  (*fout) << " New tube: layer " << tl << " tube " << t << std::endl
			  << Amg::toString( surf.transform(),6 ) << std::endl;
		}
	      }
	    }
	  }
	}
      }
    }
    
    for( int i1 = 0;i1<MuonDetectorManager::NRpcStatType; ++i1 ){
      for( int i2 = 0;i2<MuonDetectorManager::NRpcStatEta; ++i2 ){
	for( int i3 = 0;i3<MuonDetectorManager::NRpcStatPhi; ++i3 ){
	  for( int i4 = 0;i4<MuonDetectorManager::NDoubletR; ++i4 ){
	    for( int i5 = 0;i5<MuonDetectorManager::NDoubletZ; ++i5 ){

          int stationName = m_detMgr->rpcStationName(i1);
          bool isValid=false;
          Identifier id = m_detMgr->rpcIdHelper()->channelID(stationName, i2, i3+1, i4+1, i5+1, 1, 1, 1, 1, true, &isValid); // last 6 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid
          if (!isValid) continue;
          const RpcReadoutElement* detEl = m_detMgr->getRpcReadoutElement(id);

	      if( !detEl ) continue;
	      ++nrpc;

	      if( fout ) {
		(*fout) << " New RPC ReadoutElement " << detEl->identify().get_compact() 
			<< " " << m_detMgr->rpcIdHelper()->print_to_string(detEl->identify()) // m_idHelper->toStringDetEl(detEl->identify() ) 
			<< "  NphiStripPanels " << detEl->NphiStripPanels() << std::endl
			<< Amg::toString( detEl->transform(),6 ) << std::endl;
	      }

	      for( int dbPhi=1;dbPhi<=detEl->NphiStripPanels();++dbPhi ){
		for( int gp = 1; gp<=2;++gp ){
		  for( int mp = 0; mp<2;++mp ){
		    const Trk::Surface& surf = detEl->surface( detEl->surfaceHash(dbPhi,gp,mp) );
		    if( fout ) {
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
    }

    for( int i1 = 0;i1<MuonDetectorManager::NTgcStatType; ++i1 ){
      for( int i2 = 0;i2<MuonDetectorManager::NTgcStatEta; ++i2 ){
	for( int i3 = 0;i3<MuonDetectorManager::NTgcStatPhi; ++i3 ){

	  const TgcReadoutElement* detEl = m_detMgr->getTgcReadoutElement(i1,i2,i3);
	  if( !detEl ) continue;
	  ++ntgc;
	      
	  if( fout ) {
	    (*fout) << " New TGC ReadoutElement " << detEl->identify().get_compact() 
		    << " " << m_detMgr->tgcIdHelper()->print_to_string(detEl->identify()) << std::endl 
		    << Amg::toString( detEl->transform(),6 ) << std::endl;
	  }

	  for( int gp = 0; gp<2;++gp ){
	    for( int mp = 0; mp<2;++mp ){
	      const Trk::Surface& surf = detEl->surface(2*gp+mp);
	      if( fout ) {
		(*fout) << " New layer: gp " << gp << " measPhi " << mp << std::endl
			<< Amg::toString( surf.transform(),6 ) << std::endl;
	      }
	    }
	  }
	}
      }
    }

    for( int i1 = 0;i1<MuonDetectorManager::NCscStatType; ++i1 ){
      for( int i2 = 0;i2<MuonDetectorManager::NCscStatEta; ++i2 ){
	for( int i3 = 0;i3<MuonDetectorManager::NCscStatPhi; ++i3 ){
	  const CscReadoutElement* detEl = m_detMgr->getCscReadoutElement(i1,i2,i3,1);
	  if( !detEl ) continue;
	  ++ncsc;
	      
	  if( fout ) {
	    (*fout) << " New CSC ReadoutElement " << detEl->identify().get_compact() 
		    << " " << m_detMgr->cscIdHelper()->print_to_string(detEl->identify()) << std::endl 
		    << Amg::toString( detEl->transform(),6 ) << std::endl;
	  }

	  for( int gp = 0; gp<4;++gp ){
	    for( int mp = 0; mp<2;++mp ){
	      const Trk::Surface& surf = detEl->surface(2*gp+mp);
	      if( fout ) {
		(*fout) << " New layer: gp " << gp << " measPhi " << mp << std::endl
			<< Amg::toString( surf.transform(),6 ) << std::endl;
	      }
	    }
	  }
	}
      }
    }
    
    if( fout ){
      (*fout) << " NMDT " << nmdt << " NRPC " << nrpc << " NTGC " << ntgc << " NCSC " << ncsc << std::endl;
      fout->close();
      delete fout;
    }
  }  

  StatusCode MuonGMNtupleWriter::finalize() {
    return AthAlgorithm::finalize();
  }

}

