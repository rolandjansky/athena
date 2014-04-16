/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDigitsTestAlg_H
#define TGCDigitsTestAlg_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/MsgStream.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "TTree.h" 
#include "TFile.h" 
#include "MuonDigitTest/TestAlgInterface.h"

#include "MuonIdHelpers/MuonIdHelper.h"

//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"

#include <string> 

class TGCSimHit; 
class Identifier;
class TTree;
class TFile;
class TgcIdHelper;
class MuonSimDataCollection;
class MuonSimData;
class ActiveStoreSvc;

// pre-declarations
namespace MuonGM
{
  class TgcReadoutElement;
}
namespace MuonGM
{
  class MuonDetectorManager;
}
 
template <class ConcreteAlgorithm> class AlgFactory;

class TGCDigitsTestAlg: public Algorithm, private TestAlgInterface
{

  friend class AlgFactory<TGCDigitsTestAlg>;

 public:
  TGCDigitsTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~TGCDigitsTestAlg();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
  StatusCode clear();


 private:
  StatusCode do_validation();
  const MuonSimData* getMuonSimData(const Identifier& id, const MuonSimDataCollection& sdoContainer);
  void findMuons();
  void clear_variables();


  TTree* tgctree; //!< Pointer to the NTuple tree
  ITHistSvc*     ToolRootHistSvc();


  double  m_nPar;
  double mu_eta;
  double mu_phi;
  double mu_p;
  double mu_pT;

  int m_nHits;


  bool m_DoTGCTest;

  bool m_writeNtuple;                 //!< jobOption: write data to ntuple?

  std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name
  std::string m_ntupleFileName;       //!< jobOption: Ntuple file name
  std::string m_ntupleDirName;        //!< jobOption: Ntuple directory name


  // Setable Properties:-
  std::string m_key;

  // Pointer On MuonDetectorManager
  const MuonGM::TgcReadoutElement* descriptor;
  const MuonGM::MuonDetectorManager*    m_pMuonMgr;

  // Pointers On Helpers
  const TgcIdHelper*      m_pTgcIdHelper;

  MsgStream log;
  StoreGateSvc*              m_sgSvc;
  bool isVerbose() {return (outputLevel() <= MSG::VERBOSE);}

  /// Athena-Aware Ntuple Variables
  int nHits;

  //sequential event number
  long	m_Val_TGC_RunNumber;
  long	m_Val_TGC_EventNumber;
  long	m_Val_TGC_NumberOfDigits;
  int	m_Validation_TGC_StationName;
  int	m_Validation_TGC_StationEta;
  int	m_Validation_TGC_StationPhi;
  int	m_Validation_TGC_GasGap;
  int	m_Validation_TGC_IsStrip;
  int	m_Validation_TGC_Channel;
  float	m_Validation_TGC_ChannelPosX;
  float	m_Validation_TGC_ChannelPosY;
  float	m_Validation_TGC_ChannelPosZ;

 protected:
  ActiveStoreSvc* m_activeStore;
  ITHistSvc*     m_rootsvc;
  std::map<int, HepGeom::Point3D<double> > m_theMuons;
  unsigned int m_event;

};

#endif
