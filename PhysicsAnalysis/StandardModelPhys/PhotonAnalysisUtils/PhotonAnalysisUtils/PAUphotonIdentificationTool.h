/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PAUprimaryVertexFinder.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////


#ifndef PHOTONANALYSISUTILS_PhotonIdentificationTOOL_H
#define PHOTONANALYSISUTILS_PhotonIdentificationTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "PhotonAnalysisUtils/IPAUphotonIdentificationTool.h"

class PAUphotonIdentificationTool : public AthAlgTool, virtual public IPAUphotonIdentificationTool {
    public:
           
      /** AlgTool like constructor */
      PAUphotonIdentificationTool(const std::string&,const std::string&,const IInterface*);

      /**Virtual destructor*/
      virtual ~PAUphotonIdentificationTool();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
                  
      int photonCuts(const Analysis::Photon *, int itune=0, int iconv=0);
      int photonCuts(const Analysis::Electron *, int itune=0, int iconv=0);

   private:
       double m_pt;
       double m_eta2;
       double m_rhad;
       double m_e277;
       double m_e237;
       double m_e233;
       double m_weta2;
       double m_f1;
       double m_emax2;
       double m_emin2;
       double m_fracm;
       double m_wtot;
       double m_w1;
       double m_emax2r;
       double m_deltae;
       int m_conv;

       int cuts(int itune=0);
       bool CutHad_tune1();
       bool CutHad_tune2();
       bool CutHad_tune3();
       bool CutS2_tune1();
       bool CutS2_tune2();
       bool CutS2_tune3();
       bool CutS1_tune1();
       bool CutS1_tune2();
       bool CutS1_tune3();

       std::string m_egDetailContainer;

       //const CaloCluster* clus; 
       //const EMShower* shower; 

  };


#endif // PHOTONANALYSISUTILS_PhotonIdentificationTOOL_H
