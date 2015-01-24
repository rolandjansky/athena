/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIFlowData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HIFLOWDATA_H
#define HIFLOWDATA_H

#include "HIGlobal/HICaloUtil.h"

class AtlasDetectorID;
class Identifier;

/** @class HIFLOWDATA

    This is for the Doxygen-Documentation.  
    Please delete these lines and fill in information about
    the Algorithm!
    Please precede every member function declaration with a
    short Doxygen comment stating the purpose of this function.
    
    @author  Adam Trzupek <adam.Trzupek@ifj.edu.pl>
*/  


class HIFlowData 
  {
  public:
    
// v2 Pixel Blayer0  and RP : EMB1
    void v2EMB1sil(float v2sil){ m_v2EMB1sil = v2sil; }
    float v2EMB1sil() const { return m_v2EMB1sil; }
// v2 Pixel Blayer0  and RP : FCAL0
    void v2FCAL0sil(float v2sil){ m_v2FCAL0sil = v2sil; }
    float v2FCAL0sil() const { return m_v2FCAL0sil; }
// RP Trk reconstructed for eta>0.2 
    void PsiPtrack(float PsiPtrack){ m_PsiPtrack = PsiPtrack; }
    float PsiPtrack() const { return m_PsiPtrack; }
// RP Trk reconstructed for eta<-0.2
    void PsiNtrack(float PsiNtrack){ m_PsiNtrack = PsiNtrack; }
    float PsiNtrack() const { return m_PsiNtrack; }
// v2  Trk reconstructed 
    void v2track(float v2track){ m_v2track = v2track; }
    float v2track() const { return m_v2track; }
    
// EP positive side FCAL0 after flattening procedure  
    void PsiPFCAL0_w(float PsiPFCAL0_w){ m_PsiPFCAL0_w = PsiPFCAL0_w; }
    float PsiPFCAL0_w() const { return m_PsiPFCAL0_w; }
// EP negative side FCAL0 after flattening procedure  
    void PsiNFCAL0_w(float PsiNFCAL0_w){ m_PsiNFCAL0_w = PsiNFCAL0_w; }
    float PsiNFCAL0_w() const { return m_PsiNFCAL0_w; }
// EP full FCAL0 after flattening procedure  
    void PsiFCAL0_w(float PsiFCAL0_w){ m_PsiFCAL0_w = PsiFCAL0_w; }
    float PsiFCAL0_w() const { return m_PsiFCAL0_w; }

// psi for e()
// mean value of sin and cos 
// allow to calculate Psi, psiN and PsiP
    void Ssin(float *ssin);
    float const * Ssin() const { return m_Ssin; }
    void Scos(float *scos);
    float const * Scos() const { return m_Scos; }
    void Psi(float *Psi);
    float const * Psi() const { return m_Psi; }

// psi for et()
    void SsinEt(float *ssin);
    float const * SsinEt() const { return m_SsinEt; }
    void ScosEt(float *scos);
    float const * ScosEt() const { return m_ScosEt; }
    void PsiEt(float *Psi);
    float const * PsiEt() const { return m_PsiEt; }
   
// v2(eta) for RP from EMB1 for e()
    void v2EtaEMB1(std::vector<float> *v2);
    std::vector<float> const * v2EtaEMB1() const{return m_v2EtaEMB1;};
// v2(eta) for RP from FCAL0
    void v2EtaFCAL0(std::vector<float> *v2);
    std::vector<float> const * v2EtaFCAL0() const{return m_v2EtaFCAL0;};
// total energy vs etat and det
    void SumE(std::vector<float> *s);
    std::vector<float> const * SumE() const {return m_SumE;};


// v2(eta) for RP from EMB1 for et()
    void v2EtaEtEMB1(std::vector<float> *v2);
    std::vector<float> const * v2EtaEtEMB1() const {return m_v2EtaEtEMB1;};
// v2(eta) for RP from FCAL0
    void v2EtaEtFCAL0(std::vector<float> *v2);
    std::vector<float> const * v2EtaEtFCAL0() const {return m_v2EtaEtFCAL0;};
// total energy vs etat and det
    void SumEt(std::vector<float> *s);
    std::vector<float> const * SumEt() const {return m_SumEt;};

    ~HIFlowData() {};
    HIFlowData();
    
  private:
    
    float m_Psi[HICaloUtil::Unknown] ;
    float m_Ssin[HICaloUtil::Unknown] ;
    float m_Scos[HICaloUtil::Unknown] ;

    float m_PsiEt[HICaloUtil::Unknown] ;
    float m_SsinEt[HICaloUtil::Unknown] ;
    float m_ScosEt[HICaloUtil::Unknown] ;
    
    std::vector<float> m_v2EtaEMB1[HICaloUtil::Unknown];
    std::vector<float> m_v2EtaFCAL0[HICaloUtil::Unknown];
    std::vector<float> m_SumE[HICaloUtil::Unknown];
    
    std::vector<float> m_v2EtaEtEMB1[HICaloUtil::Unknown];
    std::vector<float> m_v2EtaEtFCAL0[HICaloUtil::Unknown];
    std::vector<float> m_SumEt[HICaloUtil::Unknown];

    float m_v2EMB1sil; 
    float m_v2FCAL0sil;  
    float m_PsiPtrack;
    float m_PsiNtrack; 
    float m_v2track; 
    
    float m_PsiPFCAL0_w; 
    float m_PsiNFCAL0_w;
    float m_PsiFCAL0_w;
    
  }; 


#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(HIFlowData, 8008, 0)

#endif 
