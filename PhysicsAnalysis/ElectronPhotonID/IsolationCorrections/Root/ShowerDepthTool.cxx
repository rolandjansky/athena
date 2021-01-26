/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IsolationCorrections/ShowerDepthTool.h"

#include <TFile.h>
#include <TH1.h>
#include <TSystem.h>

#include <string>
#include <cmath>
#include <stdlib.h>


#include <map>
#include <memory>

#include "PathResolver/PathResolver.h"

using std::make_pair;

namespace CP{

  ShowerDepthTool::ShowerDepthTool() : 
    m_hData(nullptr),
    m_hMC(nullptr),
    m_configFileName("ElectronIsolationSelection/v1/CaloDeltaRZ.root"),
    m_dataHistoName("hData"),
    m_mcHistoName("hMC")
  {          
  }

  ShowerDepthTool::~ShowerDepthTool() {
    delete m_hData;
    delete m_hMC;
  }
      
  bool ShowerDepthTool::initialize() 
  {   
    std::string filename = PathResolverFindCalibFile( m_configFileName );
       
    TString Tfilename( filename );    

    m_hData = getHistoFromFile( Tfilename , m_dataHistoName );
    m_hMC = getHistoFromFile( Tfilename , m_mcHistoName );   
    
    if( m_hData == nullptr || m_hMC == nullptr ){
      return false;
    }
    return true;
  }      
   
  /** Shower depth (in mm) on EM1 vs. eta, considering misalignments **/
  float ShowerDepthTool::getCorrectedShowerDepthEM1(const float& etas1,const float& phi,const bool& isData) const
  {
    return getShowerDepthEM1(etas1) - getRZCorrection(etas1, phi, isData); 
  }

  /** Shower depth (in mm) on EM2 vs. eta, considering misalignments **/
  float ShowerDepthTool::getCorrectedShowerDepthEM2(const float& etas2,const float& phi,const bool& isData) const
  {
    return getShowerDepthEM2(etas2) - getRZCorrection(etas2, phi, isData); 
  }

  /** Return the shower depth on sampling 1 given etas1. From:
  https://svnweb.cern.ch/trac/atlasoff/browser/Calorimeter/CaloDetDescr/trunk/src/CaloDepthTool.cxx#L347 **/
  float ShowerDepthTool::getShowerDepthEM1(const float& etas1) const
  {
    float radius, aetas1 = fabs(etas1);
    if (aetas1 < 0.8)
      radius = (1558.859292 - 4.990838*aetas1 - 21.144279*aetas1*aetas1);
    else if (aetas1<1.5)
      radius = (1522.775373 + 27.970192*aetas1 - 21.104108*aetas1*aetas1);
    else
      radius = 3790.671754;
    if (etas1 < 0. and aetas1 > 1.5)
      return -radius;
    return radius;
  }

  /** Return the shower depth on sampling 2 given etas2. From:
  https://svnweb.cern.ch/trac/atlasoff/browser/Calorimeter/CaloDetDescr/trunk/src/CaloDepthTool.cxx#L347 **/
  float ShowerDepthTool::getShowerDepthEM2(const float& etas2) const
  {
    float radius, aetas2 = fabs(etas2);
    if (aetas2 < 1.425) // Barrel, my definition
      radius = (1698.990944 - 49.431767*aetas2 - 24.504976*aetas2*aetas2);
    else if (aetas2 < 1.5) // EME2 in tool
      radius = (8027.574119 - 2717.653528*aetas2);
    else
      radius = (3473.473909 + 453.941515*aetas2 - 119.101945*aetas2*aetas2);
    if (etas2 < 0. and aetas2 > 1.5)
      return -radius;
    return radius;
  }


  float ShowerDepthTool::getCorrectedEtaDirection(const float& zvertex, 
                                                  const float& eta,
                                                  const float& phi, 
                                                  const bool& isData,
                                                  const int& sampling) const 
  {
    std::pair<float, float> RZ = getCorrectedRZ(eta, phi, isData, sampling);
    return getEtaDirection(zvertex, RZ.first, RZ.second);
  }

  std::pair<float,float> ShowerDepthTool::getRZ(const float& eta,const int& sampling) const
  {
    if ((sampling != 1 && sampling != 2) || (fabs(eta)>10))
    {
//       ATH_MSG_INFO( "Invalid sampling: " << sampling );
      return std::make_pair(0., 0.);
    }
    float depth = (sampling == 1 ? getShowerDepthEM1(eta) : getShowerDepthEM2(eta) );
    if (fabs(eta) <  1.5)
      return std::make_pair( depth, depth*sinh(eta) );
    return std::make_pair( depth/sinh(eta), depth );
  }


  std::optional<float> ShowerDepthTool::getCaloPointingEta(const float& etas1,const float& etas2,const float& phi,const bool& isData) const
  {
    std::pair<float, float> RZ1 = getCorrectedRZ(etas1, phi, isData, 1);
    std::pair<float, float> RZ2 = getCorrectedRZ(etas2, phi, isData, 2);

    //Sanity check
    constexpr float epsilon=1e-6;
    if (std::fabs(RZ2.first - RZ1.first) < epsilon) return std::nullopt;

    return std::optional<float>(asinh( (RZ2.second - RZ1.second) / (RZ2.first - RZ1.first)));
  }



  std::pair<float, float> ShowerDepthTool::getCorrectedRZ(const float& eta, 
                                                          const float& phi,
                                                          const bool& isData,
                                                          const int& sampling) const 
  {
    if ((sampling != 1 && sampling != 2) || (fabs(eta)>10))
    {
//       ATH_MSG_INFO( "Invalid sampling: " << sampling );
      return std::make_pair(0., 0.);
    }
    float depth = (sampling == 1 ? getCorrectedShowerDepthEM1(eta, phi, isData) :
      getCorrectedShowerDepthEM2(eta, phi, isData) );
    if (fabs(eta) <  1.5)
      return std::make_pair( depth, depth*sinh(eta) );
    return std::make_pair( depth/sinh(eta), depth );
  }


  /** Return the calorimeter displacement in R(Z) for barrel (endcap) **/
  float ShowerDepthTool::getRZCorrection(const float& eta,const float& phi,const bool& isData) const
  {
    TH1* histo = (isData ? m_hData : m_hMC);
    if (!histo)
      return 0;
    return histo->Interpolate(eta, phi);
  }
  
  float ShowerDepthTool::getEtaDirection(const float& zvertex,const float& R,const float& z) const
  {
    return asinh( (z- zvertex)/R );
  }  


  TH1* ShowerDepthTool::getHistoFromFile(const TString& fileName,const TString& histoName)
  {
    std::unique_ptr<TFile> f(TFile::Open(fileName));
    if (!f.get()){
      return 0;
    }
    TH1 *h = dynamic_cast<TH1*>( f->Get(histoName) );
    if (!h){
      f.get()->Close();
      return 0;
    }
    //The file we be deleted so use SetDirectory
    h->SetDirectory(0);
    f.get()->Close();
    return h;
  }

} // namespace CP
