//Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "CaloTowerGeometryCondAlg.h" 
#include "AthenaKernel/IOVInfiniteRange.h"
#include <memory>
#include <cstdarg>

namespace {
  std::string fmtMsg(const char* fmt,...) {
    char buffer[1024];
    va_list args;
    va_start(args,fmt);
    vsprintf(buffer,fmt,args);
    va_end(args);
    return std::string(buffer);
  }
}


StatusCode CaloTowerGeometryCondAlg::initialize() {
 
  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_caloMgrKey.initialize());
  
  ATH_CHECK(m_outputKey.initialize());
  // Register write handle
  if (m_condSvc->regHandle(this, m_outputKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_outputKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode CaloTowerGeometryCondAlg::execute(const EventContext& ctx) const {

  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,ctx};
  const CaloDetDescrManager* caloDDM = *caloMgrHandle;

  //Set up write handle
  SG::WriteCondHandle<CaloTowerGeometry> writeHandle{m_outputKey,ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }

  writeHandle.addDependency(caloMgrHandle);


  std::unique_ptr<CaloTowerGeometry> towerGeo=std::make_unique<CaloTowerGeometry>(caloDDM);

  //Assign properties to CaloTowerGeometry CDO:
  towerGeo->m_towerEtaBins   =m_towerEtaBins;   
  towerGeo->m_towerEtaMin    =m_towerEtaMin;    
  towerGeo->m_towerEtaMax    =m_towerEtaMax;    
  towerGeo->m_adjustEta	     =m_adjustEta;	     
  towerGeo->m_towerPhiBins   =m_towerPhiBins;   
  towerGeo->m_towerPhiMin    =m_towerPhiMin;    
  towerGeo->m_towerPhiMax    =m_towerPhiMax;    
  towerGeo->m_fcal1Xslice    =m_fcal1Xslice;    
  towerGeo->m_fcal1Yslice    =m_fcal1Yslice;    
  towerGeo->m_fcal2Xslice    =m_fcal2Xslice;    
  towerGeo->m_fcal2Yslice    =m_fcal2Yslice;    
  towerGeo->m_fcal3Xslice    =m_fcal3Xslice;    
  towerGeo->m_fcal3Yslice    =m_fcal3Yslice;

  ATH_CHECK(towerGeo->initialize( msg() ));


  dump(towerGeo.get()); //does nothing execept jO "TowerGridFile" or "TowerMapFile" given
  
  
  ATH_CHECK(writeHandle.record(std::move(towerGeo)));
  ATH_MSG_INFO("recorded new CaloTowerGeometry object with key " << writeHandle.key() << " and range " << writeHandle.getRange());

  return StatusCode::SUCCESS;
}


void CaloTowerGeometryCondAlg::dump(const CaloTowerGeometry* towerGeo) const {
  
  if (!m_towerGridFile.empty()) {
    std::vector<std::string> logger; logger.reserve(towerGeo->m_towerBins+10);
    logger.push_back(fmtMsg("[%s] +-------- Tower Index Mapping ---------+------------+------------+",this->name().c_str()));
    logger.push_back(fmtMsg("[%s] | %10.10s | %10.10s | %10.10s | %10.10s | %10.10s |",this->name().c_str(),"TowerIndex", " EtaIndex ", " PhiIndex ","     Eta     ","     Phi    "));
    logger.push_back(fmtMsg("[%s] +------------+------------+------------+------------+------------+",this->name().c_str()));
    for ( size_t i(0); i<towerGeo->m_towerBins; ++i ) { 
      size_t etaIndex(towerGeo->etaIndexFromTowerIndex(i));
      size_t phiIndex(towerGeo->phiIndexFromTowerIndex(i));
      double eta(towerGeo->towerEta(i)); double phi(towerGeo->towerPhi(i));
      logger.push_back(fmtMsg("[%s] |    %5zu   |    %5zu   |    %5zu   |   %6.3f   |   %6.3f   |",this->name().c_str(),i,etaIndex,phiIndex,eta,phi));
    }
    logger.push_back(fmtMsg("[%s] +------------+------------+------------+------------+------------+",this->name().c_str()));
  // 
    std::ofstream logfile;
    logfile.open(m_towerGridFile); 
    for ( const auto& mlog : logger ) { 
      logfile << mlog << std::endl; 
    } 
    logfile.close();
  } //end if tower grid file name given


  if (!m_towerMapFile.empty()>0) {
    std::ofstream towerfile;
    towerfile.open("towermap.dat");
    auto it=towerGeo->begin();
    unsigned cellCounter=0;
    for (;it!=towerGeo->end();++it,++cellCounter) { //Loop over  cells in cell-to-tower lookup
      towerfile << cellCounter << ": ";
      for (auto& item : *it) { //
	auto idx=std::get<0>(item);
	double w=std::get<1>(item);
	towerfile << " (" << idx << "," << w <<");";
      }//end loop over towers per cell
      towerfile << std::endl;
    }//end loop over cells
    towerfile.close();
  }//end if file-name given


  return;
}


