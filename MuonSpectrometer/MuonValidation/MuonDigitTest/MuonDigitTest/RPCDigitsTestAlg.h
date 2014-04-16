/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCDigitsTestAlg_H
#define RPCDigitsTestAlg_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/MsgStream.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "TTree.h" 
#include "TFile.h"
#include "MuonDigitTest/TestAlgInterface.h"

#include "MuonIdHelpers/MuonIdHelper.h"
    
#include <string> 

class Identifier;
class TTree;
class TFile;
class RpcIdHelper;
class ActiveStoreSvc;

// pre-declarations
namespace MuonGM
{
  class RpcReadoutElement;
}
namespace MuonGM
{
  class MuonDetectorManager;
}

template <class ConcreteAlgorithm> class AlgFactory;

class RPCDigitsTestAlg: public Algorithm, private TestAlgInterface
{

  friend class AlgFactory<RPCDigitsTestAlg>;

 public:
  RPCDigitsTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~RPCDigitsTestAlg();
  void	clear_variables();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
  StatusCode clear();


 private:
  StatusCode do_AANtuple_conversion();
  double getDistance(HepGeom::Point3D<double> maxPos, HepGeom::Point3D<double> vertex, int measuresPhi) const;

  void findMuons();

  TTree* rpctree; //!< Pointer to the NTuple tree
  // The tree that will contain our "ntuple" with one entry per event

  // Simple method for accessing root histo service for booking
  ITHistSvc*     ToolRootHistSvc();

  double m_nPar;
  double mu_eta;
  double mu_phi;
  double mu_p;
  double mu_pT;

  /// Athena-Aware Ntuple Variables
  long	m_Val_RPC_RunNumber;
  long	m_Val_RPC_EventNumber;
  long	m_Val_RPC_NumberOfDigits;
  int	m_Validation_RPC_StationName;
  int	m_Validation_RPC_StationEta;
  int	m_Validation_RPC_StationPhi;
  int	m_Validation_RPC_DoubletR;
  int	m_Validation_RPC_DoubletZ;
  int	m_Validation_RPC_DoubletPhi;
  int	m_Validation_RPC_DoubletGasGap;
  int	m_Validation_RPC_MeasuresPhi;
  int	m_Validation_RPC_Strip;
  float	m_Validation_RPC_StripX;
  float	m_Validation_RPC_StripY;
  float	m_Validation_RPC_StripZ;
  float	m_Validation_RPC_Stripdistance;

  bool m_DoRPCTest;  
  bool m_writeNtuple;                 //!< jobOption: write data to ntuple?
  bool m_writeChambers;
  std::string m_chamberName;
  std::string m_StationSize;
  int m_StationEta;
  int m_StationPhi;
  int m_LastEvent;
  int event_counter;
  std::map<std::string,int> digitsperchamber_map;
  std::vector<std::string> hardware_name_list;
  bool histo_flag;


  std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name
  std::string m_ntupleFileName;       //!< jobOption: Ntuple file name
  std::string m_ntupleDirName;        //!< jobOption: Ntuple directory name

  // Setable Properties:-
  std::string m_key;

  // Pointer On MuonDetectorManager
  const MuonGM::RpcReadoutElement* descriptor;
  const MuonGM::MuonDetectorManager*    m_pMuonMgr;

  // Pointers On Helpers
  const RpcIdHelper*      m_pRpcIdHelper;

  MsgStream log;
  StoreGateSvc*              m_sgSvc;
  bool isVerbose() {return (outputLevel() <= MSG::VERBOSE);}

  //sequential event number
  int m_iEvent ;



 protected:
  ActiveStoreSvc* m_activeStore;
  ITHistSvc*     m_rootsvc;
  std::map<int, HepGeom::Point3D<double> > m_theMuons;
  unsigned int m_event;
};
#endif
