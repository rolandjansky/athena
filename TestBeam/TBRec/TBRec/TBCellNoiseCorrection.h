/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBCellNoiseCorrection_H
#define TBREC_TBCellNoiseCorrection_H
/********************************************************************
NAME:     TBCellNoiseCorrection.cxx
PACKAGE:  offline/TestBeam/TBRec

PURPOSE:  Read the weighted noise events and add them

********************************************************************/
#include "CaloUtils/CaloCellCorrection.h"

#include "CLHEP/Random/RandomEngine.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

class LArCell;
class MsgStream;
class IAtRndmGenSvc;
class TFile;
class TChain;

class TBCellNoiseCorrection : public CaloCellCorrection, virtual public IIncidentListener
{

 public:

    TBCellNoiseCorrection (const std::string& type, const std::string& name, 
		const IInterface* parent);

    ~TBCellNoiseCorrection();
    virtual StatusCode initialize(); 
    virtual StatusCode finalize(); 

    void MakeCorrection(CaloCell* theCell);    
    void handle(const Incident&);

 private: 

  // Properties:  
  bool m_noise_correction;
  std::vector<std::string>  m_noise_file;

  // Data members:
  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;
  CLHEP::HepRandomEngine * m_engine;
  MsgStream *m_log;
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




