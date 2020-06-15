/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDPERFMON_FOURMUONEVENT_H
#define IDPERFMON_FOURMUONEVENT_H

//==============================================================================
// Include files...
//==============================================================================
#include "CLHEP/Vector/LorentzVector.h"
#include "InDetPerformanceMonitoring/MuonSelector.h"
#include "InDetPerformanceMonitoring/ElectronSelector.h"

#include "InDetPerformanceMonitoring/EventAnalysis.h"
#include "InDetPerformanceMonitoring/PerfMonServices.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
//==============================================================================
// Forward class declarations...
//==============================================================================
class TrackParticle;

//==============================================================================
// Class declaration...
//==============================================================================
class FourMuonEvent : public EventAnalysis
{
 public:
  FourMuonEvent();
  virtual ~FourMuonEvent();

  enum
  {
    MUON1,
    MUON2,
    MUON3,
    MUON4,
    NUM_MUONS
  };

  enum
  {
    CENTRAL,
    FORWARD,
    BACKWARD,
    UNDEF
  };

  enum ZTYPE
  {
    MS,     // Just the muon system ( uncorrected )
    ME,     // The adjusted muon system properties ( corr. for cal. )
    ID,     // Using the ID system.
    CB,     // Using both the muon & ID system.
    NUM_TYPES
  };

  void Init(); 
  bool Reco();

  // Public access methods
  inline void                        doIsoSelection(bool doIso) { m_xMuonID.doIsoSelection(doIso); }
  inline void                        doIPSelection(bool doIPsel) { m_xMuonID.doIPSelection(doIPsel);}
  inline void                        doMCPSelection(bool doMCP) { m_xMuonID.doMCPSelection(doMCP);}
  inline bool                        EventPassed () { return m_passedSelectionCuts; }
  inline const float&                get4MuInvMass (ZTYPE eType)          { return m_fInvariantMass[eType];  } 
  inline int                         getAcceptedEvents ()                 { return m_acceptedEventCount; }      
  inline const xAOD::Muon*           getCombMuon ( unsigned int uPart )   { return (uPart < NUM_MUONS) ? m_pxRecMuon[uPart] : nullptr;  }
  inline const xAOD::TrackParticle*  getELTrack  ( unsigned int uPart )   { return (uPart < NUM_MUONS) ? m_pxELTrack[uPart] : nullptr;  }
  inline const xAOD::TrackParticle*  getIDTrack  ( unsigned int uPart )   { return (uPart < NUM_MUONS) ? m_pxIDTrack[uPart] : nullptr;  }
  inline double                      GetInvMass() { return m_FourMuonInvMass; }
  inline const xAOD::TrackParticle*  getMSTrack  ( unsigned int uPart )   { return (uPart < NUM_MUONS) ? m_pxMSTrack[uPart] : nullptr;  }
  inline const xAOD::TrackParticle*  getLooseIDTk( unsigned int uPart );
  inline const float&                getLeptonOpeningAngle( ZTYPE eType ) { return m_fMuonDispersion[eType]; }
  unsigned int                       getNegMuon( int eType );
  inline unsigned int                getNumberOfTaggedMuons() { return m_numberOfFullPassMuons; }
  inline int                         GetNVertex () { return m_nVertex; }
  float                              getPtImbalance( ZTYPE eType );
  const std::string                  getRegion() const ;
  unsigned int                       getPosMuon( int eType );
  inline int                         GetVertexElec ( unsigned int uPart ) { return (uPart < NUM_MUONS) ? m_elec_vtx[uPart] : 0;}
  inline int                         GetVertexElNeg1 () { return m_elec_vtx[0];}
  inline int                         GetVertexElNeg2 () { return m_elec_vtx[1];}
  inline int                         GetVertexElPos1 () { return m_elec_vtx[2];}
  inline int                         GetVertexElPos2 () { return m_elec_vtx[3];}
  inline int                         GetVertexMuNeg1 () { return m_muon_vtx[0];} //{ return m_muonneg1_vtx; }
  inline int                         GetVertexMuNeg2 () { return m_muon_vtx[1];} //{ return m_muonneg2_vtx; }
  inline int                         GetVertexMuPos1 () { return m_muon_vtx[2];} //{ return m_muonpos1_vtx; }
  inline int                         GetVertexMuPos2 () { return m_muon_vtx[3];} //{ return m_muonpos2_vtx; }
  int                                getZCharge( ZTYPE eType );
  inline const float&                getZEta   ( ZTYPE eType ) { return m_fZEtaDir[eType];      }
  inline const float&                getZMass  ( ZTYPE eType ) { return m_fInvariantMass[eType];}
  inline const float&                getZPhi   ( ZTYPE eType ) { return m_fZPhiDir[eType];      }
  inline const float&                getZPt    ( ZTYPE eType ) { return m_fZPt[eType];          }
  void                               OrderMuonList ();
  void                               setContainer (PerfMonServices::CONTAINERS container) { m_container = container; };
  inline void                        setDebugMode (bool debug) { m_doDebug=debug; }
  void                               SetLeadingMuonPtCut (double newvalue); 
  inline void                        SetMassWindowLow (double newvalue) {m_MassWindowLow = newvalue;}
  inline void                        SetMassWindowHigh (double newvalue) {m_MassWindowHigh = newvalue;}
  inline void                        SetMuonPtCut (double newvalue) { m_xMuonID.SetPtCut(newvalue);}
  void                               SetSecondMuonPtCut (double newvalue); 
  inline void                        SetOpeningAngleCut (double newvalue) {m_OpeningAngleCut = newvalue;}
  inline void                        SetZ0GapCut (double newvalue) {m_Z0GapCut = newvalue;}


 protected:
  void BookHistograms(); // virtual?

 private:
  typedef EventAnalysis PARENT;

  // Private methods
  bool     CheckMuonVertices ();
  void     Clear ();
  bool     EventSelection (ZTYPE eType);
  bool     EventSelectionNew (ZTYPE eType);
  bool     ReconstructKinematicsNew();
  bool     ReconstructKinematics();
  bool     ReconstructKinematics4Elec();
  void     RecordMuon( const xAOD::Muon* pxMuon );

  // Active mu-cuts for the analysis
  MuonSelector            m_xMuonID;
  ElectronSelector        m_xElecID;
  PerfMonServices::CONTAINERS m_container;

  // Tag Setup variables
  unsigned int m_uMuonTags;
  unsigned int m_uTrackMatch;
  bool m_bLooseMatch;
  double m_etaCut;
  double m_FourMuonInvMass;

  double m_LeadingMuonPtCut;
  double m_SecondMuonPtCut;
  double m_MassWindowLow;
  double m_MassWindowHigh;
  double m_OpeningAngleCut;
  double m_deltaXYcut;
  double m_Z0GapCut;

  bool m_doDebug;
  bool m_workAsFourMuons;
  bool m_workAsFourElectrons;
  bool m_workAsFourLeptons;
  // Member variables : Mostly to store relevant muon data for quick access.
  unsigned int     m_numberOfFullPassMuons;
  unsigned int     m_numberOfFullPassElectrons;
  bool             m_passedSelectionCuts;
  bool             m_passedFourMuonSelection;
  bool             m_passedFourElectronSelection;
  bool             m_passedFourLeptonSelection;

  const            xAOD::Muon*      m_pxRecMuon[NUM_MUONS];
  const            xAOD::TrackParticle*  m_pxMETrack[NUM_MUONS];  // Pointer to muon spectro ( corr. )
  const            xAOD::TrackParticle*  m_pxMSTrack[NUM_MUONS];      // Pointer to muon spectro
  const            xAOD::TrackParticle*  m_pxIDTrack[NUM_MUONS];       // Pointer to ID track

  const            xAOD::TrackParticle*  m_pxELTrack[NUM_MUONS];  // pointer to Track particle of the electrons
  const            xAOD::TrackParticle*  m_pxMUTrack[NUM_MUONS];  // pointer to Track particle of the muons

  // Keep kinematic information on the Z
  float m_fZPt[NUM_TYPES];
  float m_fZEtaDir[NUM_TYPES];
  float m_fZPhiDir[NUM_TYPES];
  float m_fInvariantMass[NUM_TYPES];
  float m_fMuonDispersion[NUM_TYPES];

  // Graphs
  enum HISTOS_1D
  {
    ZMASS_MUON, ZMASS_MUONADJ, ZMASS_TRACK, ZMASS_COMB,
    NUM_1HISTOS
  };

  // muon selector configuration
  bool m_SelectMuonByIso;
  bool m_SelectMuonByIP;

  // event count
  int m_eventCount;
  int m_acceptedEventCount;

  // selected muon identifiers
  // to be removed
  int m_muon1;
  int m_muon2;
  // selected muon identifiers
  int m_muonpos1;
  int m_muonpos2;
  int m_muonneg1;
  int m_muonneg2;

  // 
  int m_nVertex; // number of vertex to which the muons are associated
  int m_muonneg1_vtx; // tell us wich of the vertex
  int m_muonneg2_vtx; // tell us wich of the vertex
  int m_muonpos1_vtx; // tell us wich of the vertex
  int m_muonpos2_vtx; // tell us wich of the vertex

  int m_muon_vtx[NUM_MUONS];
  int m_elec_vtx[NUM_MUONS];

};
//==============================================================================
#endif
