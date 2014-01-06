/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTHitsValAlg_H
#define MDTHitsValAlg_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/MsgStream.h"

#include "TTree.h"

#include "MuonHitTest/ValAlgInterface.h"

#include "MuonIdHelpers/MuonIdHelper.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"

#include <sstream>   
#include <string>
#include <vector>
#include <map>

class ActiveStoreSvc;
class MDTSimHit; 
class Identifier;
class TTree;
class TFile;
class MdtIdHelper;


template <class ConcreteAlgorithm> class AlgFactory;

class MDTHitsValAlg: public Algorithm, private ValAlgInterface {

  friend class AlgFactory<MDTHitsValAlg>;

 public:
  MDTHitsValAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~MDTHitsValAlg();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 
	
 private:
 

  TTree* mdttree; /** Pointer to the NTuple tree
		      The tree that will contain our "AANtuple" with one entry per hit*/

  /**Simple method for accessing root histo service for booking*/
  ITHistSvc*     ToolRootHistSvc();

  
  int event_counter;
  bool m_DoMDTTest;		  
  bool etaregion;
  double mdt_eta_cutmin;
  double mdt_eta_cutmax;
  bool m_writeNtuple;                 /**jobOption: write data to AANtuple*/
  bool m_GnaMode;
  bool m_writeChambers;

  std::string m_ntupleTreeName;       /** jobOption: Ntuple tree name*/
  std::string m_ntupleFileName;       /** jobOption: Ntuple file name*/    
  std::string m_ntupleDirName;        /** jobOption: Ntuple directory name*/
          
     
  /**The MC truth key */
  std::string m_key; 
  
  /**MDT barrel eta cut, applicable to the MDT 2D cross section plot */
  double m_BarrelEtaCut;

  std::vector<std::string> hardware_name_list;
  bool histo_flag;
  std::string m_chamberName;
  std::string m_StationSize;
  int m_StationEta;
  int m_StationPhi;
  int m_SpecRegion;
  int m_LastEvent;
  std::map<std::string,int> hitsperchamber_map;

  ActiveStoreSvc* m_activeStore;


  /**Pointer On MuonDetectorManager */
  const MuonGM::MdtReadoutElement* descriptor;
  const MuonGM::MuonDetectorManager* m_pMuonMgr;

  /**Pointers On Helpers */
  const MdtIdHelper*      m_pMdtIdHelper;
  
  MsgStream log;
  StoreGateSvc*              m_sgSvc;
  bool isVerbose() {return (outputLevel() <= MSG::VERBOSE);}

  /**Athena-Aware Ntuple variable declaration, explanation in MDTHitsValAlg.cxx*/
  int counter_ValHitNumber ;
  int m_Validation_MDT_Type;
  int m_Validation_MDT_NumberOfHits;
  int m_Validation_MDT_EventNumber;
  int m_Validation_MDT_RunNumber;
  double m_Validation_MDT_HitX;
  double m_Validation_MDT_HitY;
  double m_Validation_MDT_HitZ;
  double m_Validation_MDT_HitR;
  //  char m_Validation_MDT_StationName[3]; // string in as char array
  int m_Validation_MDT_StationName;
  int m_Validation_MDT_StationEta;
  int m_Validation_MDT_StationPhi;
  int m_Validation_MDT_IDTube;
  int m_Validation_MDT_IDMultiLayer;
  int m_Validation_MDT_IDLayer;
  double m_Validation_MDT_LocalX;
  double m_Validation_MDT_LocalY;
  double m_Validation_MDT_LocalZ;
  double m_Validation_MDT_LocalR;


	 
 protected:

  ITHistSvc*     m_rootsvc;


};

#endif
