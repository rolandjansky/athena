/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//implementation
////////////////////////////////////////////////////////////////////////////////
// FTK_UncertaintyTool tool
// -------------------------------
// ATLAS Collaboration
//
// Store and return the parametrized uncertianties for the FTK tracks   
//
// 17.07.2013 Package created
//
// Author: John Alison, UChicago
// e-mail: John.Alison@cern.ch
//
////////////////////////////////////////////////////////////////////////////////


#ifndef __TRIG_FTK_UNCERTIANTY_TOOL_H__
#define __TRIG_FTK_UNCERTIANTY_TOOL_H__

#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"

#include "TrigFTK_RawData/FTK_RawTrack.h"

class FTK_UncertaintyTool : public AthAlgTool, virtual public IFTK_UncertaintyTool
{

 private :

  enum hasBL {
    noBL      = 0,
    BL        = 1
  };

  //
  // Paramatire the uncertianties linearly or vs sqrt(TP)
  //
  enum FitMode {
    sqroot      = 0,
    linear      = 1
  };
  
 public:

  FTK_UncertaintyTool( const std::string&, const std::string&, const IInterface* );
  virtual ~FTK_UncertaintyTool(){};

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  //
  //  Get the element of the covariance matrix for id0th and id1th track parameter 
  //
  virtual double getParamCovMtx    (const FTK_RawTrack& trk, bool hasIBL, int id0, int id1);

  //
  //  Initialize the constants
  //
  void LoadConstants      ();
  void LoadConstants_NoIBL();

 private:

  //
  //  Helper functions with the uncerianties
  //
  double getSigmaTheta     (double eta,   double sigmaEta);
  double getSigmaQoverP    (double invpt, double sigmaTwoInvPt, double eta,  double sigmaEta);
  double getSigmaPt        (double invpt, double sigmaTwoInvPt);

  //
  // Structs to hold fit constansts
  //
  struct vsPt {
    
    vsPt() {}
    
    FitMode  m_mode;
    double   m_par0;
    double   m_par1;
    
  };

  struct vsEtaAndPt {
    
    vsEtaAndPt() {
      m_nEtaBins = 5; 
      m_etaLimits = new float[m_nEtaBins];
      m_etaLimits[0] = 0.5;
      m_etaLimits[1] = 1.0;
      m_etaLimits[2] = 1.5;
      m_etaLimits[3] = 2.0;
      m_etaLimits[4] = 2.5;

      m_etaBin = new vsPt[m_nEtaBins];
    }

    ~vsEtaAndPt(){
      delete[] m_etaLimits;
      delete[] m_etaBin;
    }

    FitMode mode(float eta){
      return m_etaBin[getEtaBin(eta)].m_mode;
    }


    double par0(float eta){
      return m_etaBin[getEtaBin(eta)].m_par0;
    }

    double par1(float eta){
      return m_etaBin[getEtaBin(eta)].m_par1;
    }

    unsigned int getEtaBin(float eta){

      for(unsigned int i = 0; i < m_nEtaBins; ++i){
	if(fabs(eta) < m_etaLimits[i] ){
	  return i;
	}
      }
      return (m_nEtaBins-1);
    }

    
    vsPt*        m_etaBin;
    float*       m_etaLimits;
    unsigned int m_nEtaBins;
  };


  struct TPConsts {
    
    TPConsts() {
      m_TP = new vsEtaAndPt[5];
    }

    ~TPConsts(){
      delete[] m_TP;
    }

    void set(FTKTrackParam::TP tp, unsigned int etaBin, FitMode mode, double par0, double par1){
      m_TP[tp].m_etaBin[etaBin].m_mode = mode;
      m_TP[tp].m_etaBin[etaBin].m_par0 = par0;
      m_TP[tp].m_etaBin[etaBin].m_par1 = par1;
    }

    FitMode mode(unsigned int tp, float eta){
      return m_TP[tp].mode(eta);
    }

    double par0(unsigned int tp, float eta){
      return m_TP[tp].par0(eta);
    }

    double par1(unsigned int tp, float eta){
      return m_TP[tp].par1(eta);
    }
    
    vsEtaAndPt*  m_TP;
  };

  //
  // Array of constants
  //   - 1 entry for tracks with a  BL
  //   - 1 entry for tracks with no BL
  //
  TPConsts  m_allConsts[2];

  //
  //  Flag to turn off IBL
  //
  bool m_noIBL;

};


#endif
