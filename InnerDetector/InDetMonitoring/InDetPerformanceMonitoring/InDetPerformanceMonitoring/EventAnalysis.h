/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

// Bit of an abstract class ( not in the C++ sense )
// designed to apply a bunch of cuts & chop up the data

#ifndef IDPERFMON_EVENTANALYSIS_H
#define IDPERFMON_EVENTANALYSIS_H

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

// Crap that may stay or go
#include "CLHEP/Vector/LorentzVector.h"

#include <map>
#include <vector>

namespace EAna
{
  const float g_fMuonMass  =  0.1056f;
  const float g_fElecMass  =  0.0005f;
  const float CGeV         =  1.0e-3;
}

class EventAnalysis
{
 public:
  EventAnalysis();
  virtual ~EventAnalysis();

  // Overridden functions.
  virtual void Init();
  virtual bool Reco();

  // Static Util. function declarations. Defined below class. Can use if no inheritance struct.
  template<class T> static float EvalInvMass( const T* pxP1, const T* pxP2,
					      float fMass1, float fMass2 = -999.9 );
  template<class T> static float EvalDiMuInvMass( const T* pxP1, const T* pxP2 );
  template<class T> static float EvaluateAngle( const T* pxP1, const T* pxP2 );
  template<class T> static float EvalPtDiff( const T* pxP1, const T* pxP2 );
  template<class T> static float EvalPhiDiff( const T* pxP1, const T* pxP2 );
  template<class T> static float EvalEtaDiff( const T* pxP1, const T* pxP2 );

  template<class T> static float EvalPt( const T* pxP1, const T* pxP2 );
  template<class T> static float EvalPhi( const T* pxP1, const T* pxP2 );
  template<class T> static float EvalEta( const T* pxP1, const T* pxP2 );
  template<class T> static float EvalCharge( const T* pxP1, const T* pxP2 );

  template<class T> static float EvalTransverseMass( const T* pxP1,float fMETx, float fMETy,
						     float fMass1, float fMass2 = -999.9);
  template<class T> static float EvalTransverseMass( const T* pxP1, float fMETx, float fMETy );

  template<class T> static float EvalTransverseMass( const T* pxP1, const T* pxP2,
						      float fMETx, float fMETy,
						      float fMass1, float fMass2 = -999.9);
  template<class T> static float EvalTransverseMass( const T* pxP1, const T* pxP2,
						      float fMETx, float fMETy );


 protected:
  virtual void BookHistograms();

  unsigned int m_uPassedEvents;
  std::map<unsigned int, TH1F*>         m_x1DHistograms;
  std::map<unsigned int, TH2F*>         m_x2DHistograms;
  std::map<unsigned int, TProfile*>     m_x1DProfHistograms;
  std::map<unsigned int, TProfile2D*>   m_x2DProfHistograms;

  std::string  m_xSampleName;

 private:
  void Register();                 // Register the histograms.

  void Register_1DHistos();
  void Register_2DHistos();
  void Register_1DProfHistos();
  void Register_2DProfHistos();

};


//=============================================================================
// Useful static functions defined here
//=============================================================================
// 2 Particle Invariant Mass
template<class T> float EventAnalysis::EvalDiMuInvMass( const T* pxP1, const T* pxP2 )
{
  // Check integrity of inputs.
  if ( !pxP1 || !pxP2 ) return -999.9f;

  // Evaluate Di-mu invariant mass.
  return EvalInvMass( pxP1, pxP2, EAna::g_fMuonMass );
}

template<class T> float EventAnalysis::EvalInvMass( const T* pxP1, const T* pxP2,
						    float fMass1, float fMass2 /* = -999.9 */ )
{
  // Check integrity of inputs.No tachyons.
  if ( fMass1 < 0.0f )  return -999.9f;
  if ( !pxP1 || !pxP2 ) return -999.9f;

  // Set masses equal if required by user
  fMass2 = ( fMass2 < 0.0f ) ? fMass1 : fMass2;

  // Evaluate invariant mass.
  CLHEP::Hep3Vector xTmp1 = CLHEP::Hep3Vector( pxP1->p4().Px() * EAna::CGeV, pxP1->p4().Py() * EAna::CGeV, pxP1->p4().Pz() * EAna::CGeV  );
  CLHEP::Hep3Vector xTmp2 = CLHEP::Hep3Vector( pxP2->p4().Px() * EAna::CGeV, pxP2->p4().Py() * EAna::CGeV, pxP2->p4().Pz() * EAna::CGeV  );

  CLHEP::HepLorentzVector xLVec1; xLVec1.setVectM( xTmp1, fMass1 );
  CLHEP::HepLorentzVector xLVec2; xLVec2.setVectM( xTmp2, fMass2 );

  return static_cast<float>( xLVec1.invariantMass( xLVec2 ) );
}

// Angle Between two particles
template<class T> float EventAnalysis::EvaluateAngle( const T* pxP1, const T* pxP2 )
{
  // Check integrity of inputs.
  if ( !pxP1 || !pxP2 ) return -999.9f;

  // Evaluate the angle.
  CLHEP::Hep3Vector xTmp1 = CLHEP::Hep3Vector( pxP1->p4().Px() * EAna::CGeV, pxP1->p4().Py() * EAna::CGeV, pxP1->p4().Pz() * EAna::CGeV  );
  CLHEP::Hep3Vector xTmp2 = CLHEP::Hep3Vector( pxP2->p4().Px() * EAna::CGeV, pxP2->p4().Py() * EAna::CGeV, pxP2->p4().Pz() * EAna::CGeV  );


  return static_cast<float>( xTmp1.angle(xTmp2) );
}

template<class T> float EventAnalysis::EvalPtDiff( const T* pxP1, const T* pxP2 )
{
  // Check integrity of inputs.
  if ( !pxP1 || !pxP2 ) return -999.9f;

  // Evaluate the difference between the momenta. Signed using positive - negative if appropriate.
  if ( pxP1->charge() > 0.5f )
  {
    return static_cast<float>( pxP1->pt() * EAna::CGeV - pxP2->pt() * EAna::CGeV );
  }
  else
  {
    return static_cast<float>( pxP2->pt() * EAna::CGeV - pxP1->pt() * EAna::CGeV );
  }
}



template<class T> float EventAnalysis::EvalPhiDiff( const T* pxP1, const T* pxP2 )
{
  // Check integrity of inputs.
  if ( !pxP1 || !pxP2 ) return -999.9f;

  // Evaluate the angle.
  CLHEP::Hep3Vector xTmp1 = CLHEP::Hep3Vector( pxP1->p4().Px() * EAna::CGeV, pxP1->p4().Py() * EAna::CGeV, pxP1->p4().Pz() * EAna::CGeV  );
  CLHEP::Hep3Vector xTmp2 = CLHEP::Hep3Vector( pxP2->p4().Px() * EAna::CGeV, pxP2->p4().Py() * EAna::CGeV, pxP2->p4().Pz() * EAna::CGeV  );

  return static_cast<float>( xTmp1.deltaPhi(xTmp2) );
}


template<class T> float EventAnalysis::EvalEtaDiff( const T* pxP1, const T* pxP2 )
{
  // Check integrity of inputs.
  if ( !pxP1 || !pxP2 ) return -999.9f;

  // Evaluate the angle.
  CLHEP::Hep3Vector xTmp1 = CLHEP::Hep3Vector( pxP1->p4().Px() * EAna::CGeV, pxP1->p4().Py() * EAna::CGeV, pxP1->p4().Pz() * EAna::CGeV  );
  CLHEP::Hep3Vector xTmp2 = CLHEP::Hep3Vector( pxP2->p4().Px() * EAna::CGeV, pxP2->p4().Py() * EAna::CGeV, pxP2->p4().Pz() * EAna::CGeV  );

  return static_cast<float>( xTmp1.polarAngle(xTmp2) );
}

template<class T> float EventAnalysis::EvalPt( const T* pxP1, const T* pxP2 )
{
  // Check integrity of inputs.
  if ( !pxP1 || !pxP2 ) return -999.9f;

  CLHEP::Hep3Vector xTmp1 = CLHEP::Hep3Vector( pxP1->p4().Px() * EAna::CGeV, pxP1->p4().Py() * EAna::CGeV, pxP1->p4().Pz() * EAna::CGeV  );
  CLHEP::Hep3Vector xTmp2 = CLHEP::Hep3Vector( pxP2->p4().Px() * EAna::CGeV, pxP2->p4().Py() * EAna::CGeV, pxP2->p4().Pz() * EAna::CGeV  );

  return static_cast<float>( (xTmp1 + xTmp2).perp() );
}

template<class T> float EventAnalysis::EvalPhi( const T* pxP1, const T* pxP2 )
{
  // Check integrity of inputs.
  if ( !pxP1 || !pxP2 ) return -999.9f;

  CLHEP::Hep3Vector xTmp1 = CLHEP::Hep3Vector( pxP1->p4().Px() * EAna::CGeV, pxP1->p4().Py() * EAna::CGeV, pxP1->p4().Pz() * EAna::CGeV  );
  CLHEP::Hep3Vector xTmp2 = CLHEP::Hep3Vector( pxP2->p4().Px() * EAna::CGeV, pxP2->p4().Py() * EAna::CGeV, pxP2->p4().Pz() * EAna::CGeV  );

  return static_cast<float>( (xTmp1 + xTmp2).phi() );
}

template<class T> float EventAnalysis::EvalEta( const T* pxP1, const T* pxP2 )
{
  // Check integrity of inputs.
  if ( !pxP1 || !pxP2 ) return -999.9f;

  CLHEP::Hep3Vector xTmp1 = CLHEP::Hep3Vector( pxP1->p4().Px() * EAna::CGeV, pxP1->p4().Py() * EAna::CGeV, pxP1->p4().Pz() * EAna::CGeV  );
  CLHEP::Hep3Vector xTmp2 = CLHEP::Hep3Vector( pxP2->p4().Px() * EAna::CGeV, pxP2->p4().Py() * EAna::CGeV, pxP2->p4().Pz() * EAna::CGeV  );

  return static_cast<float>( (xTmp1 + xTmp2).pseudoRapidity() );
}

template<class T> float EventAnalysis::EvalCharge( const T* pxP1, const T* pxP2 )
{
  // Check integrity of inputs.
  if ( !pxP1 || !pxP2 ) return -999.9f;

  return static_cast<float>(  pxP1->charge() + pxP2->charge() );
}

// Transverse Mass - 1 Particle + MET
template<class T> float EventAnalysis::EvalTransverseMass( const T* pxP1, float fMETx, float fMETy )
{
  // Check integrity of inputs.
  if ( !pxP1 ) return -999.9f;

  // Evaluate Di-mu invariant mass.
  return EvalInvMass( pxP1, fMETx, fMETy, EAna::g_fMuonMass );
}

template<class T> float EventAnalysis::EvalTransverseMass( const T* pxP1, float fMETx, float fMETy,
							   float fMass1, float fMass2 /* = -999.9 */ )
{
  // Check integrity of inputs.No tachyons.
  if ( fMass1 < 0.0f )  return -999.9f;
  if ( !pxP1 ) return -999.9f;

  // Set masses equal if required by user.
  fMass2 = ( fMass2 < 0.0f ) ? fMass1 : fMass2;

  // Evaluate invariant mass.
  CLHEP::Hep3Vector xTmp1 = CLHEP::Hep3Vector( pxP1->p4().Px() * EAna::CGeV, pxP1->p4().Py() * EAna::CGeV, 0.0f  );
  CLHEP::Hep3Vector xTmp2 = CLHEP::Hep3Vector( fMETx, fMETy, 0.0f  );

  CLHEP::HepLorentzVector xLVec1; xLVec1.setVectM( xTmp1, fMass1 );
  CLHEP::HepLorentzVector xLVec2; xLVec2.setVectM( xTmp2,   0.0f );

  return static_cast<float>( xLVec1.invariantMass( xLVec2 ) );
}

// Transverse Mass - 2 Particles + MET
template<class T> float EventAnalysis::EvalTransverseMass( const T* pxP1, const T* pxP2, float fMETx, float fMETy )
{
  // Check integrity of inputs.
  if ( !pxP1 || !pxP2 ) return -999.9f;

  // Evaluate Di-mu invariant mass.
  return EvalTransverseMass( pxP1, pxP2, fMETx, fMETy, EAna::g_fMuonMass );
}

template<class T> float EventAnalysis::EvalTransverseMass( const T* pxP1, const T* pxP2, float fMETx, float fMETy,
							   float fMass1, float fMass2 /* = -999.9 */ )
{
  // Check integrity of inputs.No tachyons.
  if ( fMass1 < 0.0f )  return -999.9f;
  if ( !pxP1 || !pxP2 ) return -999.9f;

  // Set masses equal if required by user.
  fMass2 = ( fMass2 < 0.0f ) ? fMass1 : fMass2;

  // Evaluate invariant mass.
  CLHEP::Hep3Vector xTmp1  = CLHEP::Hep3Vector( pxP1->p4().Px() * EAna::CGeV, pxP1->p4().Py() * EAna::CGeV, 0.0f  );
  CLHEP::Hep3Vector xTmp2  = CLHEP::Hep3Vector( pxP2->p4().Px() * EAna::CGeV, pxP2->p4().Py() * EAna::CGeV, 0.0f  );
  CLHEP::Hep3Vector xTmp12 = xTmp1 + xTmp2;
  CLHEP::Hep3Vector xTmp3  = CLHEP::Hep3Vector( fMETx, fMETy, 0.0f  );

  CLHEP::HepLorentzVector xLVec1; xLVec1.setVectM( xTmp12, fMass1 );
  CLHEP::HepLorentzVector xLVec2; xLVec2.setVectM(  xTmp3,   0.0f );

  return static_cast<float>( xLVec1.invariantMass( xLVec2 ) );
}

//=============================================================================

#endif
