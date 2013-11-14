/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Fakes_h
#define Fakes_h
#include "Analysis.h"
#include "AANTMisc.h"

class Fakes : public Analysis  {
public: 
  Fakes();
  virtual ~Fakes(); 

public:
///////////////////////////////////
    virtual void initialize(char* option);
    virtual void execute();
    virtual void finalize();

private:
///////////////////////////////////
// Methods
    void bookHistos(int i);
    void checkAssoc(int i);
    void studyFakes(int i);
    void closeHistos(int i);
    int  getEtaZone(float eta);

// Data
    int  m_nevt ;
    int  m_Nassoc;
    int  m_sing[10];
    Assoc* p_assoc[10];

// Histos
    TH1F *  p_pullPhi[10];
    TH1F *  p_pullTheta[10];
    TH1F *  p_pullP[10];
    TH1F *  p_histNfake[10];
    TH1F *  p_histNaddi[10];
    TH1F *  p_histNdupli[10];
    TH1F *  p_histNmuon[10];
    TH2F *  p_histFake[10];
    TH1F *  p_simuVSeta[10];
    TH1F *  p_fakeVSeta[10];

};
#endif // 
