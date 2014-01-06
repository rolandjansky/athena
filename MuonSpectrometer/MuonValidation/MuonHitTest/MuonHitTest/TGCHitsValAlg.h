/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCHitsValAlg_H
#define TGCHitsValAlg_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/MsgStream.h"

#include "TTree.h"

#include "MuonHitTest/ValAlgInterface.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"
#include <sstream>
#include <string>
#include <vector>
#include <map>

class ActiveStoreSvc;
class TGCSimHit; 
class Identifier;
class TTree; 
class TFile;
class TgcIdHelper;
 
// pre-declarations
//namespace MuonGM {
//class TgcReadoutElement;
//}
//namespace MuonGM{
//class MuonDetectorManager;
//}

template <class ConcreteAlgorithm> class AlgFactory;

class TGCHitsValAlg: public Algorithm, private ValAlgInterface {

friend class AlgFactory<TGCHitsValAlg>;

public:
	TGCHitsValAlg(const std::string& name, ISvcLocator* pSvcLocator);
	~TGCHitsValAlg();

	StatusCode initialize();
        StatusCode finalize();
        StatusCode execute();

private:
 
TTree* tgctree; /** Pointer to the NTuple tree
The tree that will contain our "AANtuple" with one entry per hit*/ 

/**Simple method for accessing root histo service for booking*/
   ITHistSvc*     ToolRootHistSvc();
 int event_counter;
 bool m_DoTGCTest;

 bool m_writeNtuple;               /**jobOption: write data to AANtuple*/  
 bool m_writeChambers;
 int m_LastEvent;

 std::string m_ntupleTreeName;    /** jobOption: Ntuple tree name*/  
 std::string m_ntupleFileName;    /** jobOption: Ntuple file name*/      
 std::string m_ntupleDirName;     /** jobOption: Ntuple directory name*/   
 

/**The MC truth key */
   std::string m_key; 	
   std::vector<std::string> hardware_name_list;

   bool histo_flag;
   std::map<std::string,int> hitsperchamber_map;

   ActiveStoreSvc* m_activeStore;
   
   
/**Pointer On MuonDetectorManager */
const MuonGM::TgcReadoutElement* descriptor;
const MuonGM::MuonDetectorManager*    m_pMuonMgr;

/**Pointers On Helpers */
const TgcIdHelper*      m_pTgcIdHelper; 
	
	MsgStream log;
	StoreGateSvc*              m_sgSvc;
	bool isVerbose() {
     return (outputLevel() <= MSG::VERBOSE);
   }

/**Athena-Aware Ntuple variable declaration, explanation in RPCHitsValAlg.cxx*/
    int counter_ValHitNumber ; 
    int m_Validation_TGC_Type;
    int m_Validation_TGC_NumberOfHits;
    int m_Validation_TGC_EventNumber;
    int m_Validation_TGC_RunNumber;
    double m_Validation_TGC_HitX;
    double m_Validation_TGC_HitY;
    double m_Validation_TGC_HitZ;
    double m_Validation_TGC_HitR;
    //char m_Validation_TGC_StName[3]; // string in as char array
    int m_Validation_TGC_StName;
    int m_Validation_TGC_StationEta;
    int m_Validation_TGC_StationPhi;
    int m_Validation_TGC_GasGap;
    int m_Validation_TGC_IsStrip;
    int m_Validation_TGC_Channel;
    double m_Validation_TGC_LocalX;
    double m_Validation_TGC_LocalY;
    double m_Validation_TGC_LocalZ;
    double m_Validation_TGC_LocalR;


	 
protected:
 
ITHistSvc*     m_rootsvc;


};

#endif
