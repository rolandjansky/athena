/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCHitsValAlg_H
#define CSCHitsValAlg_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/MsgStream.h"

#include "TTree.h"

#include "MuonHitTest/ValAlgInterface.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"

//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"
#include <sstream> 
#include <string>

class ActiveStoreSvc;
class CSCSimHit; 
class Identifier;
class TTree; 
class TFile;
class CscIdHelper;
 
// pre-declarations
//namespace MuonGM {
//class CscReadoutElement;
//}
//namespace MuonGM{
//class MuonDetectorManager;
//} 

template <class ConcreteAlgorithm> class AlgFactory;

class CSCHitsValAlg: public Algorithm, private ValAlgInterface {

friend class AlgFactory<CSCHitsValAlg>;

public:
	CSCHitsValAlg(const std::string& name, ISvcLocator* pSvcLocator);
	~CSCHitsValAlg();

	StatusCode initialize();
        StatusCode finalize();
        StatusCode execute();

private:


TTree* csctree;/** Pointer to the NTuple tree
The tree that will contain our "AANtuple" with one entry per hit*/ 

/**Simple method for accessing root histo service for booking*/ 
   ITHistSvc*     ToolRootHistSvc();

 int event_counter;
 bool m_DoCSCTest;

 bool m_writeNtuple;                 /**jobOption: write data to AANtuple*/
 bool m_writeChambers;

 std::string m_ntupleTreeName;       /** jobOption: Ntuple tree name*/
 std::string m_ntupleFileName;       /** jobOption: Ntuple file name*/     
 std::string m_ntupleDirName;        /** jobOption: Ntuple directory name*/
 int m_LastEvent;

/**The MC truth key */
   std::string m_key; 	

 std::vector<std::string> hardware_name_list;
 bool histo_flag;
 std::map<std::string,int> hitsperchamber_map;

 ActiveStoreSvc* m_activeStore;

/**Pointer On MuonDetectorManager */
 const MuonGM::CscReadoutElement* descriptor;
 const MuonGM::MuonDetectorManager*    m_pMuonMgr;

/**Pointers On Helpers */
 const CscIdHelper*      m_pCscIdHelper;
  	
	MsgStream log;
	StoreGateSvc*              m_sgSvc;
	bool isVerbose() {return (outputLevel() <= MSG::VERBOSE);}

/**Athena-Aware Ntuple variable declaration, explanation in CSCHitsValAlg.cxx*/
    int counter_ValHitNumber ; 
    int m_Validation_CSC_Type;
    int m_Validation_CSC_NumberOfHits;
    int m_Validation_CSC_EventNumber;
    int m_Validation_CSC_RunNumber;
    double m_Validation_CSC_HitX;
    double m_Validation_CSC_HitY;
    double m_Validation_CSC_HitZ;
    double m_Validation_CSC_HitR;
    //char m_Validation_CSC_StationName[3]; // string in as char array
    int m_Validation_CSC_StationName;
    int m_Validation_CSC_StationEta;
    int m_Validation_CSC_StationPhi;
    int m_Validation_CSC_ChamberLayer;
    int m_Validation_CSC_WireLayer;
    int m_Validation_CSC_MeasuresPhi;
    int m_Validation_CSC_Strip;


//sequential event number
        int m_iEvent ;


	 
protected:
 
ITHistSvc*     m_rootsvc;






};//  

#endif
 
