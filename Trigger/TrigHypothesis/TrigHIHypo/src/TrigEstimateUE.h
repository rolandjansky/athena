/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGESTIMATEUE_H
#define TRIGESTIMATEUE_H
#include "TrigInterfaces/AllTEAlgo.h"

/**
 * This is test algorithm required as a stopgap while the proper Jet reco and proper UE is available
 *
 **/

class TrigEstimateUE : public HLT::AllTEAlgo{
 public:
  TrigEstimateUE(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEstimateUE();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  typedef std::vector<std::vector<HLT::TriggerElement*> > VecVecTE;
  HLT::ErrorCode hltExecute(VecVecTE& tes_in, unsigned int type_out);
  HLT::ErrorCode hltRest();
 private:

  HLT::ErrorCode verifyInput(VecVecTE&);
  const CaloCellContainer* getCells(VecVecTE& );
  
  bool m_hasRun;

  const CaloCellContainer* m_cells;
  double m_EtaMin;
  double m_EtaMax;
  size_t m_nEtaSlices;

  struct etaBin {
    double etaMin;
    double etaMax;
  };
  
  struct Averages {
    void setSize(size_t);
    void setEtaBins(double etamin, double etamax, size_t s);
    void fillSlice(double energy, double eta);
    void getAverages(std::vector<double>& avg) const;
    void reset();
    std::vector<double> m_etSum;
    std::vector<double> m_nCells;
    std::vector<etaBin> m_dEta;
  };
  
  Averages m_avg;

};

 
#endif
