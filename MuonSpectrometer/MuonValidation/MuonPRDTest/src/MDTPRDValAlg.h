/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTPRDValAlg_H
#define MDTPRDValAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <sstream>   
#include <string>
#include <vector>
#include <map>

class MDTSimHit; 
class MuonSimData;
class TTree;
class ITHistSvc;

// pre-declarations
namespace MuonGM {
  class MdtReadoutElement;
}

class MDTPRDValAlg: public AthAlgorithm {
 public:
  struct ParticleParameters {
    ParticleParameters() : charge(-9999.), pars(0) {}
    ~ParticleParameters(){
      delete pars;
    }
    Amg::Vector3D momentum;
    Amg::Vector3D   position;
    double      charge;
    const Trk::TrackParameters* pars;

    private:
    ParticleParameters& operator=( const ParticleParameters& ) { return *this; }
    ParticleParameters( const ParticleParameters& ) {}
  };

  struct TruthInfo {
    TruthInfo() : vertex(0),muonEntry(0),muonExit(0),pdg(0),barcode(-1) {}
    ~TruthInfo() {
      delete vertex;
      delete muonEntry;
      delete muonExit;
    }
    ParticleParameters* vertex;
    ParticleParameters* muonEntry;
    ParticleParameters* muonExit;
    int         pdg;
    int         barcode;
    
    private:
    TruthInfo( const TruthInfo& ) {}
    TruthInfo& operator=( const TruthInfo& ) { return *this; }
  };

  typedef std::map<int,TruthInfo*> TruthMap;
  typedef TruthMap::iterator       TruthIt;

  struct MdtHitData {
    MdtHitData() : barcode(-1),simHit(0),sdo(0),prd(0) {}
    int   barcode;
    const MDTSimHit*         simHit;
    const MuonSimData*       sdo;
    const Muon::MdtPrepData* prd;
  };
  
  typedef std::map<Identifier,MdtHitData*> MdtHitMap;
  typedef MdtHitMap::iterator              MdtHitIt;

  typedef std::map<int,MdtHitMap>          MuonMdtHitMap;
  typedef MuonMdtHitMap::iterator          MuonMdtHitIt;

 public:
  MDTPRDValAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~MDTPRDValAlg()=default;

  StatusCode initialize();
  StatusCode execute();

 private:
 

  TTree* m_mdttree; /** Pointer to the NTuple tree
		      The tree that will contain our "AANtuple" with one entry per hit*/

  /**Simple method for accessing root histo service for booking*/
  ITHistSvc*     ToolRootHistSvc();

  
  int m_event_counter;
  bool m_DoMDTTest;		  
 
  bool m_writeNtuple;                 /**jobOption: write data to AANtuple*/

  std::string m_ntupleTreeName;       /** jobOption: Ntuple tree name*/
  std::string m_ntupleFileName;       /** jobOption: Ntuple file name*/    
  std::string m_ntupleDirName;        /** jobOption: Ntuple directory name*/
          
     
  /**The MC truth key */
  std::string m_key; 
  
  /**MDT barrel eta cut, applicable to the MDT 2D cross section plot */
  double m_BarrelEtaCut;

  std::string m_chamberName;
  std::string m_StationSize;
  int m_StationEta;
  int m_StationPhi;
  int m_LastEvent;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  bool isVerbose() {return (msgLevel() <= MSG::VERBOSE);}

  void addMcEventCollection( TruthMap& truthMap ) const;
  void addMuonRecord( TruthMap& truthMap, bool exit ) const; // add MuonEntry or MuonExit Record/Layer
  void addSimHits( MuonMdtHitMap& muonMdtHitMap, TruthMap& truthMap ) const;
  void addSimData( MuonMdtHitMap& muonMdtHitMap, TruthMap& truthMap ) const;
  void addPrepData( MuonMdtHitMap& muonMdtHitMap ) const;
  void analyseHits( MuonMdtHitMap& muonMdtHitMap, TruthMap& truthMap );

  MdtHitData* findMdtHitData( const Identifier& id, MuonMdtHitMap& muonMdtHitMap ) const;

  void cleanUp( MuonMdtHitMap& muonMdtHitMap ) const;
  void cleanUp( TruthMap& truthMap ) const;

  /**Athena-Aware Ntuple variable declaration, explanation in MDTPRDValAlg.cxx*/
  int m_counter_ValHitNumber ;
  int m_Validation_MDT_Type;
  int m_Validation_MDT_NumberOfHits;
  int m_Validation_MDT_EventNumber;
  int m_Validation_MDT_RunNumber;
  int m_Validation_MDT_pdg;
  double m_Validation_MDT_phi;
  double m_Validation_MDT_theta;
  double m_Validation_MDT_eta;
  double m_Validation_MDT_qp;
  double m_Validation_MDT_MEX;
  double m_Validation_MDT_MEY;
  double m_Validation_MDT_MEZ;
  double m_Validation_MDT_Exit_qp;
  double m_Validation_MDT_ExitX;
  double m_Validation_MDT_ExitY;
  double m_Validation_MDT_ExitZ;
  double m_Validation_MDT_HitX;
  double m_Validation_MDT_HitY;
  double m_Validation_MDT_HitZ;
  double m_Validation_MDT_HitR;
  double m_Validation_MDT_ExX;
  double m_Validation_MDT_ExY;
  double m_Validation_MDT_ExZ;
  double m_Validation_MDT_ExR;
  char m_Validation_MDT_StationName[4]; // string in as char array
  int m_Validation_MDT_StationEta;
  int m_Validation_MDT_StationPhi;
  int m_Validation_MDT_IDTube;
  int m_Validation_MDT_IDMultiLayer;
  int m_Validation_MDT_IDLayer;
  int m_Validation_MDT_GeoSign;
  int m_Validation_MDT_BESL;

  double m_Validation_MDT_LocalX;
  double m_Validation_MDT_LocalY;
  double m_Validation_MDT_LocalZ;
  double m_Validation_MDT_LocalR;
  double m_Validation_MDT_Res_LocalX;
  double m_Validation_MDT_Res_LocalY;
  double m_Validation_MDT_ResEloss_LocalX;
  double m_Validation_MDT_ResEloss_LocalY;
  double m_Validation_MDT_dtheta_dLocX;
  double m_Validation_MDT_dtheta_dLocY;
  double m_Validation_MDT_dphi_dLocX;
  double m_Validation_MDT_dphi_dLocY;
  double m_Validation_MDT_dtheta;
  double m_Validation_MDT_3DdistME;
  double m_Validation_MDT_ploss;
  double m_Validation_MDT_3DdistExit;
  int m_Validation_MDT_last;

  double m_Validation_MDT_ExNomWireR;
  double m_Validation_MDT_ExNomWireZ;
  double m_Validation_MDT_ExSagWireR;
  double m_Validation_MDT_ExSagWireZ;
  double m_Validation_MDT_ExSagRotR;
  double m_Validation_MDT_ExSagRotZ;
  double m_Validation_MDT_SimRadius;
  double m_Validation_MDT_SdoRadius;
  double m_Validation_MDT_RotRadius;
  double m_Validation_MDT_WireLen;


  bool m_useEntryLayer;

 protected:

  ITHistSvc*     m_rootsvc;

  ToolHandle<Trk::IExtrapolator>    m_extrapolator;
  ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>    m_rotCreator;

};

#endif
