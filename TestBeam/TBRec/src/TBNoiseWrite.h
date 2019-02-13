/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBNOISEWRITE_H
#define TBREC_TBNOISEWRITE_H

// 
// class TBNoiseWrite 
// Write ROOT Tree with noise in randomly triggered events
//
#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;
class TFile;
class TTree;
class CaloCell_ID;
class CaloDetDescrManager;
class IToolSvc;
class LArDigitContainer;

class TBNoiseWrite: public AthAlgorithm {
 public:    
  
  TBNoiseWrite(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TBNoiseWrite();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private: 
  void clear();
  /** Get Xcryo and Ytable from a text file */
  StatusCode getXcryoYtable(float &x, float &y, float &eBeam);
  
  int m_nEvent;                     // counter
  int m_nEventRandomTrigger;        // counter
  bool m_first;                     // First event flag

  // Run header
  struct NoiseHeader{
    int m_nRun;               /** Run number */
    float m_beamMom;          /** Beam momentum */
    float m_xCryo;            /** CryoX */
    float m_yTable;           /** TableY */
  };
  NoiseHeader m_header;

  // Variables to be in the TTree
  //--------------------------
  // Cell parameters
  std::vector<unsigned int>* m_cell_id;
  std::vector<float>* m_cell_energy;
  
  // Names and pointers
  std::string m_digitContainerName;
  std::string m_caloCellContainerName;    // Cell container name
  std::string m_headerTreeName;           // Header Tree name
  std::string m_noiseTreeName;            // Noise Tree name
  std::string m_rootfile_name;            // name of the ROOT file with TBTree
  TFile* m_rootfile;                      // and its pointer
  TTree* m_header_tree;                   // Header Tree pointer
  TTree* m_tree;                          // Noise Tree pointer

  //const CaloCell_ID* m_calo_id;
  //const CaloDetDescrManager* m_calo_dd_man; 

  /** Text file containing xCryo and yTable */
  std::string m_txtFileWithXY;
};

#endif
