/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerRateSlices_TrigL2JetObject
#define TriggerRateSlices_TrigL2JetObject

#include "TObject.h"
#include <vector>
#include <string>

//#include "TrigCaloEvent/Trig3Momentum.h"
//#include "FourMom/P4EEtaPhiM.h"

//class TrigL2JetObject: public P4EEtaPhiMBase, public TObject {
class TrigL2JetObject: public TObject {

public:
    TrigL2JetObject();
    TrigL2JetObject(const TrigL2JetObject &other);
    
    virtual ~TrigL2JetObject();

    //----------- public data members
    // basic information: global quantities
    
    double m_e;
    double m_ehad0;
    double m_eem0;
    double m_eta;
    double m_phi;

    //std::vector<Trig3Momentum>* m_grid;

    unsigned int m_roiWord;

private:    
    ClassDef(TrigL2JetObject,1);
};

#endif
