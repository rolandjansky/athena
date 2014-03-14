/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_HIT_ANALYSIS_H
#define ISF_HIT_ANALYSIS_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArElecCalib/ILArfSampl.h"
#include "CaloDetDescr/CaloDetDescrManager.h"




#include <string>

//#include "TH1.h"

/* *************************************************************
 This is a modified copy of Simulation/Tools/CaloHitAnalysis
 Aug 27, 2013 Zdenek Hubacek (CERN)
************************************************************** */

class TileID;
class TileDetDescrManager;
class TTree;
class ITHistSvc;
class TileInfo;
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;

 
class ISF_HitAnalysis : public AthAlgorithm {

 public:

   ISF_HitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~ISF_HitAnalysis();

   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode execute();

 private:

   /** a handle on Store Gate for access to the Event Store */
   StoreGateSvc* m_storeGate;
   StoreGateSvc* m_detStore;

   const TileInfo *m_tileInfo;
   const LArEM_ID *m_larEmID;
   const LArFCAL_ID *m_larFcalID;
   const LArHEC_ID *m_larHecID;
   const TileID * m_tileID;
   const TileDetDescrManager * m_tileMgr;
   const DataHandle<ILArfSampl>   m_dd_fSampl;

   /** Simple variables by Ketevi */
   std::vector<float>* m_hit_x;
   std::vector<float>* m_hit_y;
   std::vector<float>* m_hit_z;
   std::vector<float>* m_hit_e;
   std::vector<float>* m_hit_time;
   std::vector<int>* m_detector;
   std::vector<std::string>* m_identifier;
   std::vector<std::string>* m_identifier_alt;
   std::vector<bool>* m_islarbarrel;
   std::vector<bool>* m_islarendcap;
   std::vector<bool>* m_islarhec;
   std::vector<bool>* m_islarfcal;
   std::vector<bool>* m_istile;
   std::vector<int>*  m_sampling;
   std::vector<float>* m_samplingfraction;

   std::vector<float>* m_truth_e;
   std::vector<float>* m_truth_px;
   std::vector<float>* m_truth_py;
   std::vector<float>* m_truth_pz;

   std::vector<int>* m_truth_pdg;

   std::vector<std::string>* m_cell_identifier;
   std::vector<float>* m_cell_e;
   std::vector<int>* m_cell_sampling;

   TTree * m_tree;
   std::string m_ntupleFileName; 
   std::string m_ntupleDirName; 
   std::string m_ntupleTreeName; 

   ITHistSvc * m_thistSvc;
   const CaloDetDescrManager* m_calo_dd_man;


};

#endif // ISF_HIT_ANALYSIS_H

