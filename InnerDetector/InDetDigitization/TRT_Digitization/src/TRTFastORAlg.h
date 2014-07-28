/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTFASTORALG_H
#define TRTFASTORALG_H

#include "Identifier/Identifier.h"
#include "InDetIdentifier/TRT_ID.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"

#include "EventInfo/EventInfo.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "InDetIdentifier/TRT_ID.h"

// ROOT includes
#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TNtuple.h"
#include "TROOT.h"
#include "TList.h"
class TTree;
                 

class TRT_ID;
//=========================================
//  FastOR Class
//========================================
class TRTFastORAlg {
public:

  TRTFastORAlg();

  virtual StatusCode initialize();
  int TRTFastORAlg_getStrawId(std::vector<std::vector<int> >& , std::vector<int>&, std::vector<int>&, std::vector<int>&, 
                              std::vector<unsigned short>&, 
                              int m_qval=0, int m_preproc=0);

  int straw_isHIPcandidate(std::vector<int>);

  ~TRTFastORAlg();

private:
  int TrigPass( const int TrigPat[], const int Thresh[], int FEboard[][3] );
  int TrigPass_ec( const int TrigPat[], int Thresh, int FEboard[][3] );
  ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc;
  int m_strawID;
  int m_qval;
  int m_preproc;

  // /////////// GP  
  ServiceHandle<ITHistSvc> tHistSvc;
  
  std::string m_ntupleFileName;       //!< jobOption: Ntuple file name
  std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name
  TTree* m_nt;
  int m_eventNumber; //!< event number
  UChar_t m_nChips[2][32][29][24];
  UChar_t m_isHIP[2][32][29];
  UChar_t m_isEle[2][32][29];
  UChar_t m_isMuo[2][32][29];
  
  // /////////// ~GP 

};

#endif
