/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/// @file   AFPSiDBasicKalmanTool.h
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-03-01
/// 
/// @brief  Header file for AFPSiDBasicKalmanTool used in tracks reconstruction.
#ifndef AFP_LOCRECO_AFPSIDBASICKALMANTOOL_H
#define AFP_LOCRECO_AFPSIDBASICKALMANTOOL_H 1

// STL includes
#include <string>
#include <list>
#include <vector>
#include <map>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include <TFile.h>
#include <TH2.h>
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>

// AFP_LocReco includes
#include "AFP_LocRecoInterfaces/IAFPSiDLocRecoTool.h"
#include "AFP_LocReco/AFP_UserObjects.h"
#include "AFP_Geometry/AFP_constants.h"
#include "AFP_Geometry/AFP_Geometry.h"
#include "AFP_Geometry/AFP_ConfigParams.h"


#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPTrackContainer.h"

// Forward declaration


#define SIDSTATIONID 4
#define MAXCHI2HIT 3.0
#define MAXCHI2TRK 3.0
#define MAXSHAREDHITS 2


class AFPSiDBasicKalmanTool
  : virtual public ::IAFPSiDLocRecoTool,
    public ::AthAlgTool
{
public:
  AFPSiDBasicKalmanTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual ~AFPSiDBasicKalmanTool();

  // Athena algtool's Hooks
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode reconstructTracks();

  void FillSIDHITPOS(const SIDHIT &SIDHit, std::map<Int_t, SIDHITPOS> &MapSIDHitPos);
  void GetTrkSeeds();
  bool FillTrkPropagators(const SIDHITSEED &SIDHitSeed, Int_t plateF);
  bool FillTrkPropagators(Int_t stID, Int_t plateF);
  void Smooth();
  void FilterTrkCollection();
  Int_t GetSharedHits(const std::vector<Int_t> &HID1, const std::vector<Int_t>  &HID2);
  void ClearMatrix();
		
  void GetData();

private:
  float m_AmpThresh;
  int m_iDataType;


  std::list<SIDRESULT> m_listResults;
  std::list<SIDHIT>    m_ListSIDHits;
  std::map<Int_t, SIDHITPOS> m_MapSIDHitPos;

  Float_t fxMapSID[SIDSTATIONID][SIDCNT][336][80];
  Float_t fyMapSID[SIDSTATIONID][SIDCNT][336][80];
  Float_t fzMapSID[SIDSTATIONID][SIDCNT][336][80];

  std::string m_hitsContainerName;
  std::string m_tracksContainerName;
		
  const xAOD::AFPSiHitContainer* m_siHitContainer;

private:
  std::vector<SIDHITSEED> pTrkSeeds;
  CLHEP::HepMatrix Hk, Qk, Vk, C0;
		
  CLHEP::HepMatrix m0, x0;
  Float_t z0;
		
  std::vector< CLHEP::HepMatrix > Fk;
  std::vector< CLHEP::HepMatrix > Ck;
  std::vector< CLHEP::HepMatrix > CkP;
  std::vector< CLHEP::HepMatrix > Rk;

  std::vector< CLHEP::HepVector > xk;
  std::vector< CLHEP::HepVector > xkP;
  std::vector< CLHEP::HepVector > rk;
  std::vector< CLHEP::HepVector > rkP;
  std::vector< CLHEP::HepVector > mk;
  std::vector< Float_t > chik;
  std::vector< Float_t > zk;
  std::vector< Int_t >   HID;
		
  std::vector< CLHEP::HepVector > xkS;
  std::vector< CLHEP::HepMatrix > CkS;
  std::vector< Float_t > chikS;

  Float_t m_fsSID[SIDSTATIONID][SIDCNT];
  Float_t m_fxSID[SIDSTATIONID][SIDCNT];
  Float_t m_fySID[SIDSTATIONID][SIDCNT];
  Float_t m_fzSID[SIDSTATIONID][SIDCNT];


  AFP_Geometry* m_pGeometry;

  AFPSiDBasicKalmanTool();
  StatusCode AFPCollectionReading();
  StatusCode saveToXAOD ();
  bool ReadGeometryDetCS();
  // Containers
};


#endif // > !AFP_LOCRECO_AFPSIDBASICKALMANTOOL_H
