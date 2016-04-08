/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MTTHist_H__
#define __MTTHist_H__

/*
 * Example algorithm usong THistSvc in Trigger
 * questions? mail to Tomasz.Bold@cern.ch
 */


#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include <vector>

class TH1F;
class TH2D;
class TTree;

class MTTHist: public AthAlgorithm {
public:
    MTTHist (const std::string& name, ISvcLocator* pSvcLocator);
    virtual StatusCode initialize();
    virtual StatusCode beginRun();
    virtual StatusCode execute();
    virtual StatusCode finalize();
private:
    TH1F *m_h1fTest;
    TH2D *m_h2muMon;
    std::vector<TH1F*> m_h1binTest;
    TTree *m_timing;
    TTree *m_evdata;
    TH1F *m_hGlobal;  
    TH1F *m_hCustom;  

    int m_previousEvent;
    int m_event;
    int m_eventDistance;
    double m_testDouble;
    float m_testFloat;
    float m_dataTime;
    float m_processTime;
    uint m_eventCounter;
    uint m_addBin;
    uint m_nHistsBinTest;
    bool m_useMutex;
};

#endif //  __MTTHist_H__
