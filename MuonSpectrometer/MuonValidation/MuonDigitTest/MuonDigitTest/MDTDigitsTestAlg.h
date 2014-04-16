/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTDigitsTestAlg_H
#define MDTDigitsTestAlg_H

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

class MDTSimHit; 
class Identifier;
class TTree;
class TFile;
class MdtIdHelper;
class MuonSimDataCollection;
class MuonSimData;
class MdtCalibrationSvc;
class ActiveStoreSvc;

//const int maxDigits = 500;

// pre-declarations
namespace MuonGM
{
  class MdtReadoutElement;
}
namespace MuonGM
{
  class MuonDetectorManager;
}
 
template <class ConcreteAlgorithm> class AlgFactory;

class DistSegment
{
 public:
  DistSegment(HepGeom::Point3D<double> p_tube,HepGeom::Point3D<double> p_track) :
    pos_tube(p_tube), pos_track(p_track) {}
  HepGeom::Point3D<double> pos_tube;
  HepGeom::Point3D<double> pos_track;
  double distance() { return (pos_tube-pos_track).mag() ; }
};


class MDTDigitsTestAlg: public Algorithm, private TestAlgInterface
{

  friend class AlgFactory<MDTDigitsTestAlg>;

 public:
  MDTDigitsTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~MDTDigitsTestAlg();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
  StatusCode clear();

 private:
  StatusCode 	do_AANtuple_conversion();
  double	getDistance(HepGeom::Point3D<double> maxPos, HepGeom::Point3D<double> vertex, int measuresPhi) const;
  const	MuonSimData* getMuonSimData(const Identifier& id, const MuonSimDataCollection& sdoContainer);
  void	findMuons();
  void	findMsel();
  void	clear_variables();

  TTree* 		mdttree; //!< Pointer to the NTuple tree
  ITHistSvc*	ToolRootHistSvc();


  double m_nPar;
  double mu_eta;
  double mu_phi;
  double mu_p;
  double mu_pT;
 // int m_nDigits;

  bool m_DoMDTTest;
  bool m_writeNtuple;                 //!< jobOption: write data to ntuple?
  bool m_writeChambers;
  int m_BarrelEtaCut;
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
  const MuonGM::MdtReadoutElement*	descriptor;
  const MuonGM::MuonDetectorManager*	m_pMuonMgr;

  // Pointers On Helpers
  const MdtIdHelper*      m_pMdtIdHelper;

  MsgStream log;
  StoreGateSvc*              m_sgSvc;
  bool isVerbose() {return (outputLevel() <= MSG::VERBOSE);}

  /// Athena-Aware Ntuple Variables
  //int nDigits;

  //sequential event number
  int m_nEventNumber;

  long	m_Val_MDT_RunNumber;
  long	m_Val_MDT_EventNumber;
  long	m_Val_MDT_NumberOfDigits;
  int	m_Validation_MDT_StationName;
  int	m_Validation_MDT_StationEta;
  int	m_Validation_MDT_StationPhi;
  double	m_Validation_MDT_WireX;
  double  m_Validation_MDT_WireY;
  double  m_Validation_MDT_WireZ;
  double  m_Validation_MDT_Driftradius;
  double  m_Validation_MDT_DriftradiusHit;
  double  m_Validation_MDT_Residuals;
  int	m_Validation_MDT_IDTube;
  int	m_Validation_MDT_IDMultilayer;
  int	m_Validation_MDT_IDLayer;

 protected:
  ActiveStoreSvc* m_activeStore;
  MdtCalibrationSvc* m_calibSvc;

  ITHistSvc*     m_rootsvc;
  DistSegment getDistance( const HepGeom::Point3D<double>& lpos,const HepGeom::Vector3D<double>& ldir,
			   const HepGeom::Point3D<double>& tpos,const HepGeom::Vector3D<double>& tdir );

  std::map<int, HepGeom::Point3D<double> > m_theMuons;
  unsigned int m_event;

};

#endif
