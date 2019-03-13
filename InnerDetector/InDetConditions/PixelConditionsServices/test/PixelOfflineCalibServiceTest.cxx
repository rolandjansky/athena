/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// PixelOfflineCalibServiceTest.cxx
// Algorithm to create Pixel Offline Calib objects and place them in 
// Condition DB
// Author Weiming Yao <wmyao@lbl.gov>
/////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "PixelConditionsData/PixelCalibDataColl.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelConditionsServices/IPixelOfflineCalibSvc.h" 
#include "PixelConditionsData/PixelOfflineCalibData.h"
#include "PixelOfflineCalibServiceTest.h"
#include "GaudiKernel/IToolSvc.h"

PixelOfflineCalibServiceTest::PixelOfflineCalibServiceTest(const std::string& name, ISvcLocator* pSvcLocator)
    :AthAlgorithm   (name, pSvcLocator),
     // m_log       (msgSvc(), name),
     // m_sgSvc (0),
     //m_detStore  (0), 
     m_calibsvc("PixelOfflineCalibSvc",name),
     m_pat(0),
     m_setup(0),
     m_par_rfile1(""),
     m_par_rfile2(""),
     m_par_rfile3("")     
{
  // declare algorithm parameters
  declareProperty("OutputTextFilePixelClusterError",m_par_rfile1);
  declareProperty("OutputTextFilePixelClusterOnTrackError",m_par_rfile2);
  declareProperty("OutputTextFilePixelChargeInterpolation",m_par_rfile3);

}


PixelOfflineCalibServiceTest::~PixelOfflineCalibServiceTest(void)
{}

StatusCode PixelOfflineCalibServiceTest::initialize() {
  
  msg(MSG::INFO) << "PixelOfflineCalibServiceTest::initialize() called" << endmsg;
  
  //get storegate pointers (not need for AthAlgorithm classes)
  //if ((StatusCode::SUCCESS!=service("StoreGateSvc",m_sgSvc)) ||
  //   (StatusCode::SUCCESS!=service("DetectorStore",m_detStore))) {
  //  msg(MSG::INFO) << "StoreGate services not found" << endmsg;
  //  return StatusCode::FAILURE;
  // }

  //get Database manager tool
  if (StatusCode::SUCCESS!=m_calibsvc.retrieve() ) {
    msg(MSG::FATAL) << "PixelOfflineCalibSvc not found" << endmsg;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << " PixelOfflineCalibSvc found " << endmsg;
  
  if ( m_dbTool.retrieve().isFailure() )
    {
      msg(MSG::ERROR)
        << " Failed to retrieve tool " << m_dbTool.type() << endmsg;
      return StatusCode::RECOVERABLE;
    }
  else{
    msg(MSG::INFO)
      << "Retrieved tool " <<  m_dbTool.type() << endmsg;
  }

  //print the options
  msg(MSG::INFO) << " Read from Pixel Conditions database into a text file: " << m_par_rfile1<<" "<<m_par_rfile2<<" "<<m_par_rfile3<<endmsg;
  if(m_par_rfile1 ==""||m_par_rfile2 ==""||m_par_rfile3 ==""){
    msg(MSG::ERROR) << " It's reading, Output text file is required "<<endmsg; 
    return StatusCode::FAILURE; 
  }  
  //
  return StatusCode::SUCCESS;
}


StatusCode PixelOfflineCalibServiceTest::execute() {
  
  //StatusCode sc;
  //
  // at first event:
  // create Conditions objects in the detectorstore

  if(!m_setup){ 
    m_setup = true; 
    std::ofstream* outfile1=0;
    std::ofstream* outfile2=0;
    std::ofstream* outfile3=0;

    if(msgLvl(MSG::INFO)) msg() << " read PixelOfflineCalibData to text file: "
				<< m_par_rfile1 <<" "<<m_par_rfile2 <<" "
				<<m_par_rfile3 <<endmsg;
    outfile1 = new std::ofstream(m_par_rfile1.c_str());
    outfile2 = new std::ofstream(m_par_rfile2.c_str());
    outfile3 = new std::ofstream(m_par_rfile3.c_str());

    m_pat = m_dbTool->getCalibPtr(); 
    if(!m_pat){
      if(msgLvl(MSG::ERROR)) msg() << "PixelOfflineCalib pointer undefined!"
				   << endmsg;
      return StatusCode::FAILURE;
    }

    // PixelClusterErrorData
    int v1 = m_pat->getPixelClusterErrorData()->getVersion();
    int n1 = m_pat->getPixelClusterErrorData()->getNumberOfBarrelBins();
    int n2 = m_pat->getPixelClusterErrorData()->getNumberOfEndcapBins();
    *outfile1 <<v1<<std::endl;
    *outfile1 <<n1<<std::endl;
    *outfile1 <<n2<<std::endl;
    int mcol = 4; 
    int mrow = 3; 
    std::vector<float> metaref; 
    metaref.reserve(6);
    metaref.push_back(0.0);
    metaref.push_back(0.55);
    metaref.push_back(0.87);
    metaref.push_back(1.32);
    metaref.push_back(2.00);
    metaref.push_back(2.95);
    for(unsigned int i = 0; i<metaref.size()-1; ++i){ 
      for(int j = 0; j<mrow; ++j){ 
	for(int k = 0; k<mcol; ++k){ 
	  *outfile1 <<m_calibsvc->getBarrelErrorPhi(metaref[i]+0.01, k+1,j+1)<<" "<<m_calibsvc->getBarrelErrorEta(metaref[i]+0.01, k+1,j+1)<<std::endl; 
	}
      }
    }
    for(int j = 0; j<mrow; ++j){ 
      for(int k = 0; k<mcol; ++k){ 
	*outfile1 <<m_calibsvc->getEndCapErrorPhi(k+1,j+1)<<" "<<m_calibsvc->getEndCapErrorEta(k+1,j+1)<<std::endl; 
      }
    }
    if(v1<-1){ 
      for(unsigned int i = 0; i<metaref.size()-1; ++i){ 
	for(int j = 0; j<mrow; ++j){ 
	  for(int k = 0; k<mcol; ++k){ 
	    *outfile1 <<m_calibsvc->getIBLErrorPhi(metaref[i]+0.01, k+1,j+1)<<" "<<m_calibsvc->getIBLErrorEta(metaref[i]+0.01, k+1,j+1)<<std::endl; 
	  }
	}
      }
    }

    // PixelClusterOnTrackErrorData
    int v2 = m_pat->getPixelClusterOnTrackErrorData()->getVersion();
    const std::vector<float> &csx =m_pat->getPixelClusterOnTrackErrorData()->getClusterSizeBinsX();
    const std::vector<float> &csy =m_pat->getPixelClusterOnTrackErrorData()->getClusterSizeBinsY();
    const std::vector<float> &eta =m_pat->getPixelClusterOnTrackErrorData()->getEtaBins();
    const std::vector<float> &alpha = m_pat->getPixelClusterOnTrackErrorData()->getIncidenceAngleBins();
    const std::vector<float> &etaibl =  m_pat->getPixelClusterOnTrackErrorData()->getEtaIBLBins();
    const std::vector<float> &alphaibl =  m_pat->getPixelClusterOnTrackErrorData()->getIncidenceAngleIBLBins();

    int ncsx = csx.size();
    int ncsy = csy.size();
    int neta = eta.size();
    int nalpha = alpha.size();
    int ncsx_ibl = m_pat->getPixelClusterOnTrackErrorData()->getIBLcsxbins();
    int ncsy_ibl = m_pat->getPixelClusterOnTrackErrorData()->getIBLcsybins();
    int netaibl = m_pat->getPixelClusterOnTrackErrorData()->getIBLetabins();
    int nalphaibl = m_pat->getPixelClusterOnTrackErrorData()->getIBLphibins(); 
    *outfile2 <<v2<<std::endl;

    if(v2<-1){ 
      *outfile2 <<ncsx<<" "<<ncsy<<" "<<neta<<" "<<nalpha<<" "<<ncsx_ibl<<" "<<ncsy_ibl<<" "<<netaibl<<" "<<nalphaibl<<std::endl;
    }
    else{
      *outfile2 <<ncsx<<" "<<ncsy<<" "<<neta<<" "<<nalpha<<std::endl;
    }

    for(unsigned int i = 0; i<csx.size();++i)*outfile2<<csx[i]<<" ";
    *outfile2 <<std::endl;
    for(unsigned int i = 0; i<csy.size();++i)*outfile2<<csy[i]<<" ";
    *outfile2 <<std::endl;

    if(v2<-1){
      for(unsigned int i = 0; i<etaibl.size();++i)*outfile2<<etaibl[i]<<" ";
      *outfile2 <<std::endl;
      for(unsigned int i = 0; i<alphaibl.size();++i)*outfile2<<alphaibl[i]<<" ";
      *outfile2 <<std::endl;
    }

    for(unsigned int i = 0; i<eta.size();++i)*outfile2<<eta[i]<<" ";
    *outfile2 <<std::endl;
    for(unsigned int i = 0; i<alpha.size();++i)*outfile2<<alpha[i]<<" ";
    *outfile2 <<std::endl;
    if(v2<-1){ 
      for(unsigned int i = 0; i<alphaibl.size()-1; ++i){ 
	for(int j = 0; j<ncsx_ibl; ++j){ 
	  *outfile2 <<m_calibsvc->getIBLNewErrorPhi(alphaibl[i]+0.01,j+1)<<std::endl;
	}
      }
    }
    for(unsigned int i = 0; i<alpha.size(); ++i){ 
      for(int j = 0; j<ncsx; ++j){ 
	*outfile2 <<m_calibsvc->getBarrelNewErrorPhi(alpha[i]+0.01,j+1)<<std::endl;
      }
    }
    if(v2<-1){ 
      for(unsigned int i = 0; i<etaibl.size()-1; ++i){
	for(int j = 0; j<ncsy_ibl; ++j){
	  *outfile2 <<m_calibsvc->getIBLNewErrorEta(etaibl[i]+0.01,j+1)<<std::endl;
	}
      }
    }

    for(unsigned int i = 0; i<eta.size(); ++i){
      for(int j = 0; j<ncsx; ++j){
	for(int k = 0; k<ncsy; ++k){
	  *outfile2 <<m_calibsvc->getBarrelNewErrorEta(eta[i]+0.01,j+1, k+1)<<std::endl;
	}
      }
    }

    // PixelChargeInterpolationData
    int v3 = m_pat->getPixelChargeInterpolationParameters()->getVersion();
    const std::vector<float> &csx2 = m_pat->getPixelChargeInterpolationParameters()->getClusterSizeXBins();
    const std::vector<float> &csy2 = m_pat->getPixelChargeInterpolationParameters()->getClusterSizeYBins();
    const std::vector<float> &eta2 = m_pat->getPixelChargeInterpolationParameters()->getEtaBins();
    const std::vector<float> &alpha2 = m_pat->getPixelChargeInterpolationParameters()->getAngleBins();
    // IBL
    const std::vector<float> &etaibl2 = m_pat->getPixelChargeInterpolationParameters()->getIBLEtaBins() ;

    const std::vector<float> &alphaibl2 = m_pat->getPixelChargeInterpolationParameters()->getIBLAngleBins() ;
    //
    // number of bins
    // The upper limit is not implicit, we must subtract one!
    int ncsx2 = csx2.size()-1;
    int ncsy2 = csy2.size()-1;
    int neta2 = eta2.size()-1;
    int nalpha2 = alpha2.size()-1;
    int ncsx2_ibl =  m_pat->getPixelChargeInterpolationParameters()->getIBLcsxbins();
    int ncsy2_ibl =  m_pat->getPixelChargeInterpolationParameters()->getIBLcsybins();
    int netaibl2 =  m_pat->getPixelChargeInterpolationParameters()->getIBLetabins(); 
    int nalphaibl2 =   m_pat->getPixelChargeInterpolationParameters()->getIBLphibins();

    *outfile3<<v3<<std::endl;
    if(v3<-1){ 
      *outfile3 <<ncsx2<<" "<<ncsy2<<" "<<neta2<<" "<<nalpha2<<" "<<ncsx2_ibl<<" "<<ncsy2_ibl<<" "<<netaibl2<<" "<<nalphaibl2<<std::endl;
    }
    else{
      *outfile3 <<ncsx2<<" "<<ncsy2<<" "<<neta2<<" "<<nalpha2<<std::endl;
    }

    for(unsigned int i = 0; i<csx2.size();++i)*outfile3<<csx2[i]<<" ";
    *outfile3 <<std::endl;
    for(unsigned int i = 0; i<csy2.size();++i)*outfile3<<csy2[i]<<" ";
    *outfile3 <<std::endl;

    if(v3<-1){
      for(unsigned int i = 0; i<etaibl2.size();++i)*outfile3<<etaibl2[i]<<" ";
      *outfile3 <<std::endl;
      for(unsigned int i = 0; i<alphaibl2.size();++i)*outfile3<<alphaibl2[i]<<" ";
      *outfile3 <<std::endl;
    }
    for(unsigned int i = 0; i<eta2.size();++i)*outfile3<<eta2[i]<<" ";
    *outfile3 <<std::endl;
    for(unsigned int i = 0; i<alpha2.size();++i)*outfile3<<alpha2[i]<<" ";
    *outfile3 <<std::endl;
    if(v3<-1){ 
      for(unsigned int i = 0; i<alphaibl2.size()-1; ++i){ 
	for(int j = 0; j<ncsx2_ibl; ++j){ 
	  *outfile3 << m_calibsvc->getIBLDeltaX(j+2, alphaibl2[i]+0.01)<<" "<<0<<std::endl;
	}
      }
    }
    for(unsigned int i = 0; i<alpha2.size()-1; ++i){ 
      for(int j = 0; j<ncsx2; ++j){ 
	*outfile3 << m_calibsvc->getBarrelDeltaX(j+2, alpha2[i]+0.01)<<" "<<0<<std::endl;
      }
    }
    if(v3<-1){ 
      for(unsigned int i = 0; i<etaibl2.size()-1; ++i){
	for(int j = 0; j<ncsy2_ibl; ++j){
	  *outfile3 << m_calibsvc->getIBLDeltaY(j+i, etaibl2[i]+0.01)<<" "<<0<<std::endl;
	}
      }
    }
    for(unsigned int i = 0; i<eta2.size()-1; ++i){
      for(int j = 0; j<ncsy2; ++j){
	*outfile3 << m_calibsvc->getBarrelDeltaY(j+1,eta2[i]+0.01)<<std::endl;
      }
    }

    outfile1->close();
    outfile2->close();
    outfile3->close();

    delete outfile1;
    delete outfile2;
    delete outfile3;
  }
 //
  return StatusCode::SUCCESS;
}
 
StatusCode PixelOfflineCalibServiceTest::finalize() 
{  
  msg(MSG::INFO)<<" PixelOfflineCalibServiceTest: finishing "<<endmsg; 
  return StatusCode::SUCCESS;
}
