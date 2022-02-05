/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBCELLNOISECORRECTION_H
#define TBREC_TBCELLNOISECORRECTION_H
/********************************************************************
NAME:     TBCellNoiseCorrection.cxx
PACKAGE:  offline/TestBeam/TBRec

PURPOSE:  Read the weighted noise events and add them

********************************************************************/
#include "CaloUtils/CaloCellCorrection.h"
#include "AthenaKernel/IAthRNGSvc.h"

#include "CLHEP/Random/RandomEngine.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

class LArCell;
class MsgStream;
class TFile;
class TChain;

class TBCellNoiseCorrection : public CaloCellCorrection, virtual public IIncidentListener
{

 public:

    TBCellNoiseCorrection (const std::string& type, const std::string& name, 
		const IInterface* parent);

    virtual ~TBCellNoiseCorrection();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    void MakeCorrection (CaloCell* theCell,
                         const EventContext& ctx) const override;
    virtual void handle(const Incident&) override;

 private: 

  // Properties:  
  bool m_noise_correction;
  std::vector<std::string>  m_noise_file;

  // Data members:
  ServiceHandle <IAthRNGSvc> m_rndmSvc
    { this, "RndmSvc", "AthRNGSvc", "" };
  ATHRNG::RNGWrapper* m_engine;
  //TFile *m_root; // File with noise tree
  TChain *m_tree;
  float m_xcryo;  // if given, try to find run number from xcryo_ytable.txt file
  float m_ytable; // then a run number in file name is changed to this one
  float m_energy;
  // Cell parameters
  std::vector<unsigned int>* m_cell_id;
  std::vector<float>* m_cell_energy;
  unsigned int m_entries;
  // should check the event number
  //int m_myevtnum;
};

#endif     




