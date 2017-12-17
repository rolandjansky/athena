/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBTrack_H
#define TBEVENT_TBTrack_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Reconstructed Track from hit position in 
// Test Beam detectors                                                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <vector>

class TBTrack 
{
 public:
  
  typedef double signal_type;
  
  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  TBTrack() : m_hitNumberU(0), m_hitNumberV(0),
    m_chi2(0),
    m_chi2u(0),
    m_chi2v(0),
    m_angle(0),
    m_uslope(0),
    m_vslope(0),
    m_uintercept(0),
    m_vintercept(0),
    m_cryou(0),
    m_cryov(0),
    m_cryow(0)
    {};
    
  TBTrack(int hitNumberX,int hitNumberY);

  ~TBTrack();
  
  ///////////////////////
  // Data Manipulation //
  ///////////////////////
  
  // Getter --------------------------------  
  
  int getHitNumberU() const { return m_hitNumberU; }
  int getHitNumberV() const { return m_hitNumberV; }

  double getChi2_global() const { return m_chi2; }
  double getChi2_u() const { return m_chi2u; }
  double getChi2_v() const { return m_chi2v; }
  double getAngle() const { return m_angle; }
  double getUslope() const { return m_uslope; }
  double getVslope() const { return m_vslope; }
  double getUintercept() const {  return m_uintercept; }
  double getVintercept() const {  return m_vintercept; }

  double getResidualu(int ind) const 
  { return ind<m_hitNumberU ? m_residualu[ind]: 0; }

  double getResidualv(int ind) const 
  { return ind<m_hitNumberV ? m_residualv[ind]: 0; }

  const std::vector<double>& getResidualu() const { return m_residualu; }
  const std::vector<double>& getResidualv() const { return m_residualv; }

  double getCryoHitu() const {return m_cryou;}
  double getCryoHitv() const {return m_cryov;}
  double getCryoHitw() const {return m_cryow;}

  // Setter --------------------------------  

  void setChi2(double chi2) { m_chi2=chi2; }
  void setChi2_u(double chi2u) { m_chi2u = chi2u; }
  void setChi2_v(double chi2v) { m_chi2v = chi2v; }
  void setAngle(double angle) { m_angle=angle; }
  void setUslope(double uslope) { m_uslope=uslope; }
  void setVslope(double vslope) { m_vslope=vslope; }  
  void setUintercept(double uintercept) { m_uintercept = uintercept; }
  void setVintercept(double vintercept) { m_vintercept = vintercept; }

  void setResidualu(int ind, double residualu ) { m_residualu[ind] = residualu;}
  void setResidualv(int ind, double residualv ) { m_residualv[ind] = residualv;}
 
  void setCryoHitu(float cryou){ m_cryou=cryou;}
  void setCryoHitv(float cryov){ m_cryov=cryov;}
  void setCryoHitw(float cryow){ m_cryow=cryow;}

 private:

  //////////////////
  // Private Data //
  //////////////////

  // number of hits used for reconstruction
  int m_hitNumberU, m_hitNumberV;

  // Residuals between detector hit and fitted hit (in detector local coordonates)
  std::vector<double> m_residualu, m_residualv;


  // track parameters 
  double m_chi2, m_chi2u, m_chi2v;
  double m_angle;             // angle between track and Z TestBeam axe
  double m_uslope,m_vslope;   // slopes with respect to Z TestBeam axe
  double m_uintercept, m_vintercept;

  // extrapoled hit position at cryostat (reconstruction coordonates);
  double m_cryou,m_cryov,m_cryow;

};
CLASS_DEF( TBTrack , 129507460 , 1 )

#endif
