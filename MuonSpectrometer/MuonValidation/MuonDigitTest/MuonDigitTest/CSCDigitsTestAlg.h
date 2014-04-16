/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCDigitsTestAlg_H
#define CSCDigitsTestAlg_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/MsgStream.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "TTree.h" 

#include "MuonDigitTest/TestAlgInterface.h"

#include "MuonIdHelpers/MuonIdHelper.h"
    
#include <string> 

class Identifier;
class TTree;
class TFile;
class CscIdHelper;
class ActiveStoreSvc;

// pre-declarations
namespace MuonGM {
  class CscReadoutElement;
}
namespace MuonGM{
  class MuonDetectorManager;
}
 
template <class ConcreteAlgorithm> class AlgFactory;

class CSCDigitsTestAlg: public Algorithm, private TestAlgInterface {

  friend class AlgFactory<CSCDigitsTestAlg>;

 public:
  CSCDigitsTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~CSCDigitsTestAlg();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
  StatusCode clear();
 
	
 private:
  HepGeom::Point3D<double> truth_info(bool & is_muon) const;
  StatusCode CSCvalidateDigits();
  double getDistance(HepGeom::Point3D<double> maxPos, HepGeom::Point3D<double> vertex, int measuresPhi, Identifier id);
  HepGeom::Point3D<double> findThePoint(HepGeom::Point3D<double> vertex, HepGeom::Point3D<double> maxPos, HepGeom::Point3D<double> norm);

  StatusCode do_validation();
  void	clear_variables();
 
  TTree* csctree; //!< Pointer to the NTuple tree
  // The tree that will contain our "ntuple" with one entry per event

  // Simple method for accessing root histo service for booking
  ITHistSvc*     ToolRootHistSvc();
  

  double  m_nPar;
  double mu_eta;
  double mu_phi;
  double mu_p;
  double mu_pT;
 

  long	m_Val_CSC_RunNumber;
  long	m_Val_CSC_EventNumber;
  long	m_Val_CSC_NumberOfDigits;
  int	m_Validation_CSC_StationName;
  int	m_Validation_CSC_StationEta;
  int	m_Validation_CSC_StationPhi;
  int	m_Validation_CSC_ChamberLayer;
  int	m_Validation_CSC_WireLayer;
  int	m_Validation_CSC_MeasuresPhi;
  int	m_Validation_CSC_Strip;
  float	m_Validation_CSC_PosX;
  float	m_Validation_CSC_PosY;
  float	m_Validation_CSC_PosZ;
  float	m_Validation_CSC_Charge;

  bool m_DoCSCDigits;
  bool m_writeNtuple;                 //!< jobOption: write data to ntuple?
  bool m_dumpTrackRecord;

 
  std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name
  std::string m_ntupleFileName;       //!< jobOption: Ntuple file name    
  std::string m_ntupleDirName;        //!< jobOption: Ntuple directory name
          

  // Setable Properties:-
  std::string m_key; 
  
  // Pointer On MuonDetectorManager
  const MuonGM::CscReadoutElement* descriptor;
  const MuonGM::MuonDetectorManager*    m_pMuonMgr;

  // Pointers On Helpers
  const CscIdHelper*      m_pCscIdHelper;

  int m_doValidation;
  double m_threshold;
  
  MsgStream log;
  StoreGateSvc*              m_sgSvc;
  bool isVerbose() {return (outputLevel() <= MSG::VERBOSE);}

  /// Athena-Aware Ntuple Variables
 
  //sequential event number
  int m_iEvent ;


	 
 protected:
  ActiveStoreSvc* m_activeStore;
  
  ITHistSvc*     m_rootsvc;
   
  std::map<int, HepGeom::Point3D<double> > m_theMuons;

  unsigned int m_event;  

};

#endif
