/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigEvent/TrigTopoEvent/ElectronMuonTopoInfo.h
 **
 **   Description: -Class for description of combined electron-muon object for 
 ** 		algorithms in TrigEgammaMuonCombHypo package
 **
 **   Author: Pavel Jez <pavel.jez@cern.ch>
 **
 **   Created:   Apr 9, 2011
 **
 **
 **************************************************************************/ 

#ifndef _ELECTRONMUONTOPOINFO_H_ 
#define _ELECTRONMUONTOPOINFO_H_

// standard stuff
#include <string>
#include <map>
#include <cmath> 
// general athena stuff
#include "GaudiKernel/MsgStream.h"

#include "TrigParticle/TrigElectron.h"
#include "TrigParticle/TrigTau.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "egammaEvent/egammaContainer.h"

#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauCommonExtraDetails.h"
#include "tauEvent/TauPID.h"



/**
 * \class ElectronMuonTopoInfo
 * \brief ElectronMuonTopoInfo is a class for storing information about combuned electron-muon object. It stores angular distance, 
 * invariant mass, common vertex information, and opposite charge bit
 *
 *
 */
 
class ElectronMuonTopoInfo  {
  
  public:
    ElectronMuonTopoInfo();
    ElectronMuonTopoInfo(int roiWord, float deltaPhi=-1., float deltaR = -1., float invMass = -1., bool el_valid = false, bool oppositeCharge = 0, unsigned short vertexState =0);
    ElectronMuonTopoInfo(const ElectronMuonTopoInfo& emuTopoInfo);
    
    ~ElectronMuonTopoInfo();
    
    
     /** accessor methods */

  int   RoiWord()  const { return m_roiWord;    }
  float DeltaPhi() const { return m_DeltaPhi;   }
  float DeltaR()   const { return m_DeltaR;		}
  float InvMass()  const { return m_InvMass; 	}
  bool  ElecValid() const{ return m_electronValid; }
  bool  OppositeCharge() const { return m_oppositeCharge; }
  unsigned short VertexState() const { return m_vertexState;}
  
  
  /** set methods */
  void 	SetRoiWord(int RoiWord);
  void 	SetDeltaPhi(float DeltaPhi);
  void 	SetDeltaR(float DeltaR);
  void 	SetInvMass(float InvMass);
  void  SetElecValid(bool ElecValid);
  void  SetOppositeCharge(bool OppositeCharge); 
  void 	SetVertexState(unsigned short vextexState);
  
  /** other methods */
  
  
  
  enum Vertex {  Common, NotCommon, Unapplicable }; //!< Vertex states
  Vertex commonVertex(const TrigElectron* electron1, const CombinedMuonFeature* muon1); //!< checking Vertex compatibility @ L2
  bool opositeCharge(const TrigElectron* electron1, const CombinedMuonFeature* muon1); //!< Opposite charge @ L2

  double invariantMass(const TrigElectron* electron1, const CombinedMuonFeature* muon1); //!< Invariant mass calculation @ L2
  double deltaPhi(const TrigElectron* electron1, const CombinedMuonFeature* muon1); //!< Delta phi @ L2
  double deltaR(const TrigElectron* electron1, const CombinedMuonFeature* muon1); //!< Delta R @ L2

  //overloading for L2 tau tau combinations
  double invariantMass(const TrigTau* tau1, const TrigTau* tau2); //!< Invariant mass calculation @ L2
  double deltaPhi(const TrigTau* tau1, const TrigTau* tau2); //!< Delta phi @ L2
  double deltaR(const TrigTau* tau1, const TrigTau* tau2); //!< Delta R @ L2
  //overloading for L2 tau e combinations
  double invariantMass(const TrigTau* tau1, const TrigElectron* electron1); //!< Invariant mass calculation @ L2
  double deltaPhi(const TrigTau* tau1, const TrigElectron* electron1); //!< Delta phi @ L2
  double deltaR(const TrigTau* tau1, const TrigElectron* electron1); //!< Delta R @ L2
  //overloading for L2 tau mu combinations
  double invariantMass(const TrigTau* tau1, const CombinedMuonFeature* muon1); //!< Invariant mass calculation @ L2
  double deltaPhi(const TrigTau* tau1, const CombinedMuonFeature* muon1); //!< Delta phi @ L2
  double deltaR(const TrigTau* tau1, const CombinedMuonFeature* muon1); //!< Delta R @ L2


  // overloading for EF
  Vertex commonVertex(const Trk::Perigee* perigeeEL, const Trk::Perigee* perigeeMU, double& pull, bool debug = false);

  bool opositeCharge(const egamma* electron1, const Trk::Perigee* muon1);
  double invariantMass(const egamma* electron1, const Trk::Perigee* muon1);
  double deltaR(const egamma* electron1, const Trk::Perigee* muon1);
  double deltaPhi(const egamma* electron1, const Trk::Perigee* muon1);

  double invariantMass(double Pt1, double  eta1, double phi1,  double m1,
		       double Pt2, double  eta2, double phi2,  double m2) const ;

  // overloading for EF tau tau combinations
  double deltaR(const Analysis::TauJet* tau1, const Analysis::TauJet* tau2);
  double deltaPhi(const Analysis::TauJet* tau1, const Analysis::TauJet* tau2);
  double invariantMass(const Analysis::TauJet* tau1, const Analysis::TauJet* tau2);
  // overloading for EF tau e combinations
  double deltaR(const Analysis::TauJet* tau1, const egamma* electron1);
  double deltaPhi(const Analysis::TauJet* tau1, const egamma* electron1);
  double invariantMass(const Analysis::TauJet* tau1, const egamma* electron1);
  // overloading for EF tau mu combinations
  double deltaR(const Analysis::TauJet* tau1, const Trk::Perigee* muon1);
  double deltaPhi(const Analysis::TauJet* tau1, const Trk::Perigee* muon1);
  double invariantMass(const Analysis::TauJet* tau1, const Trk::Perigee* muon1);
  

  private:
  /** Identifier of the RoI. */
  int  m_roiWord;
  /** Delta Phi between electron and muon */
  float m_DeltaPhi;
  /** Delta R between electron and muon */
  float m_DeltaR;
  /** Invariant  mass of electron and muon */
  float m_InvMass;
  /** boolean flag showing the validity of electron */
  bool  m_electronValid;
  /** True if electron and muon have opposite charged*/
  bool  m_oppositeCharge;
  /** 3 bit description of vertex: 0=common, 1=not common, 3= not applicable */
  unsigned short m_vertexState;
  
};

/// Helper function for printing the object
std::string str( const ElectronMuonTopoInfo& d );

MsgStream& operator<< ( MsgStream& m, const ElectronMuonTopoInfo& d );
	
// comparison operators
bool operator==( const ElectronMuonTopoInfo& d1,  const ElectronMuonTopoInfo& d2 );
inline bool operator!=( const ElectronMuonTopoInfo& d1,  const ElectronMuonTopoInfo& d2 ) {
  return !( d1 == d2 ); 
    }


CLASS_DEF( ElectronMuonTopoInfo , 129722551 , 1 )
  
  
#endif //ELECTRONMUONTOPOINFO_H_
  
