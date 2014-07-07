/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronIsolationSelection/ShowerDepthTool.h"

#include "TFile.h"
#include "TH1.h"
#include "TSystem.h"

#include <map>
using std::make_pair;

namespace CP{

  ShowerDepthTool::ShowerDepthTool( const std::string& name )
   : asg::AsgTool( name ),
      m_hData(0),
      m_hMC(0),
      m_inputFile("CaloDeltaRZ.root"),
      m_dataHistoName("hData"),
      m_mcHistoName("hMC"){
        
       declareProperty( "inputFile", m_inputFile = "CaloDeltaRZ.root" );        
      }
      
   StatusCode ShowerDepthTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising..." );
       
      TString inputFile = this->findInputFile_ExpectBetterSolutionFromASG();
      ATH_MSG_INFO( "Using input File " << inputFile );

      m_hData = getHistoFromFile( inputFile , m_dataHistoName );
      m_hMC = getHistoFromFile( inputFile , m_mcHistoName );
      
      // Return gracefully:
      return StatusCode::SUCCESS;
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
    if (sampling != 1 && sampling != 2)
    {
      ATH_MSG_INFO( "Invalid sampling: " << sampling );
      return std::make_pair(0., 0.);
    }
    float depth = (sampling == 1 ? getShowerDepthEM1(eta) : getShowerDepthEM2(eta) );
    if (fabs(eta) <  1.5)
      return std::make_pair( depth, depth*sinh(eta) );
    return std::make_pair( depth/sinh(eta), depth );
  }


  float ShowerDepthTool::getCaloPointingEta(const float& etas1,const float& etas2,const float& phi,const bool& isData) const
  {
    std::pair<float, float> RZ1 = getCorrectedRZ(etas1, phi, isData, 1);
    std::pair<float, float> RZ2 = getCorrectedRZ(etas2, phi, isData, 2);

    if (RZ1.first == 0. || RZ2.first == 0.) return -9999.;
    return asinh( (RZ2.second - RZ1.second) / (RZ2.first - RZ1.first) );
  }



  std::pair<float, float> ShowerDepthTool::getCorrectedRZ(const float& eta, 
                                                          const float& phi,
                                                          const bool& isData,
                                                          const int& sampling) const 
  {
    if (sampling != 1 && sampling != 2)
    {
      ATH_MSG_INFO( "Invalid sampling: " << sampling );
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
    TFile *f = TFile::Open(fileName);
    if (!f)
    {
      ATH_MSG_INFO( "Invalid file: " << fileName );
      return 0;
    }
    TH1 *h = dynamic_cast<TH1*>( f->Get(histoName) );
    if (!h)
    {
      ATH_MSG_INFO( "Invalid histogram name (" << histoName << ") or type" );
      return 0;
    }
    h->SetDirectory(0);
    return h;
  }
  
  
  TString ShowerDepthTool::findInputFile_ExpectBetterSolutionFromASG()
  {
      std::string s_path;
      //now setting the correct path for the file. Borrowed from Fede (MuonMomentumCorrections)
      char *rootCoreArea = getenv("ROOTCOREBIN");
      char *testArea = getenv("TestArea");

      // ROOTCore: Data folder
      if(rootCoreArea != NULL){
        s_path = std::string(rootCoreArea) + "/data/egammaAnalysisUtils/";
      }
      // Athena: InstallArea
      else if ( testArea != NULL ){
        s_path = std::string(testArea) + "/InstallArea/share/";
      } 
      
      TString fileName = s_path + m_inputFile;
      return fileName;     
  }

} // namespace CP
