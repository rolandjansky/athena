/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ANALYSISUTILS_KINEMATICSELECTORCORE_H 
#define ANALYSISUTILS_KINEMATICSELECTORCORE_H 

/**
   @Project: AnalysisUtils
 
   @class KinematicSelectorCore
   
   @author Amir Farbin <amir.farbin@cernSPAMNOT.ch>
   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
   
   @date June 2009
   
   @brief Dual use tool (athena/ARA) for kinematic cuts.
*/


// STL includes
#include <string>
#include <cfloat>

// Atlas includes
#include "AraTool/AraToolBase.h"
#include "AnalysisUtils/ISelectorCore.h"
#include "EventKernel/IParticle.h"
#include "EventKernel/INavigable4Momentum.h"

static const InterfaceID IID_KinematicSelectorCore("KinematicSelectorCore", 1, 0);


class KinematicSelectorCore : public ISelectorCore, virtual public AraToolBase
{
public:
  /** Default contructor */
  KinematicSelectorCore(PropertyMgr *pmgr=0);
  
  /** Default destructor */
  virtual ~KinematicSelectorCore() {}

public:
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_KinematicSelectorCore; };

public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();



public:
  /** Main method for IParticle, all cuts are applied (const method) */
  bool accept( const IParticle* part ) const;


  /** Main method, all cuts are applied (const method) */
  bool accept( const INavigable4Momentum* part ) const;


public:  

  /** Only transverse momentum cut is applied */
  inline bool passPt( const INavigable4Momentum* part ) const
  {
    double pt = part->pt();
    if ( (std::isinf(pt) || std::isnan(pt)) ) return true;
    return ( pt > m_minPt  &&  pt < m_maxPt );
  };


  /** Only momentum cut is applied */
  inline bool passP( const INavigable4Momentum* part ) const
  {
    double p = part->p();
    if ( (std::isinf(p) || std::isnan(p)) ) return true;
    return ( p > m_minP  &&  p < m_maxP );
  };


  /** Only transverse energy cut is applied */
  inline bool passEt( const INavigable4Momentum* part ) const
  {
    double et = part->et();
    if ( (std::isinf(et) || std::isnan(et)) ) return true;
    return ( et > m_minEt  &&  et < m_maxEt );
  };


  /** Only energy cut is applied */
  inline bool passE( const INavigable4Momentum* part ) const
  {
    double e = part->e();
    if ( (std::isinf(e) || std::isnan(e)) ) return true;
    return ( e > m_minE  &&  e < m_maxE );
  };


  /** Only eta cut is applied */
  inline bool passEta( const INavigable4Momentum* part ) const
  {
    double eta = part->eta();
    if ( (std::isinf(eta) || std::isnan(eta)) ) return true;
    return ( eta > m_minEta  &&  eta < m_maxEta );
  };


  /** Only |eta| cut is applied */
  inline bool passAbsEta( const INavigable4Momentum* part ) const
  {
    double eta = part->eta();
    if ( (std::isinf(eta) || std::isnan(eta)) ) return true;
    return ( fabs(eta) > m_minAbsEta  &&  fabs(eta) < m_maxAbsEta );
  };


  /** Only the eta veto ranges cut is applied */
  bool passEtaVetoRanges( const INavigable4Momentum* part ) const;


  /** Only phi cut is applied */
  inline bool passPhi( const INavigable4Momentum* part ) const
  {
    double phi = part->phi();
    if ( (std::isinf(phi) || std::isnan(phi)) ) return true;
    return ( phi > m_minPhi  &&  phi < m_maxPhi );
  };


  /** Only the phi veto ranges cut is applied */
  bool passPhiVetoRanges( const INavigable4Momentum* part ) const;


  /** Only mass cut is applied */
  inline bool passMass( const INavigable4Momentum* part ) const
  {
    double mass = part->m();
    if ( (std::isinf(mass) || std::isnan(mass)) ) return true;
    return ( mass > m_minMass  &&  mass < m_maxMass );
  };



public:
  /** set minimum transverse momentum cut value */
  void setminPt(double val) { m_minPt=val; }

  /** get minimum transverse momentum cut value */
  double getminPt(void) { return m_minPt; }

  /** set maximum transverse momentum cut value */
  void setmaxPt(double val) { m_maxPt=val; }

  /** get maximum transverse momentum cut value */
  double getmaxPt(void) { return m_maxPt; }
  

  /** set minimum momentum cut value */
  void setminP(double val) { m_minP=val; }

  /** get minimum momentum cut value */
  double getminP(void) { return m_minP; }

  /** set maximum momentum cut value */
  void setmaxP(double val) { m_maxP=val; }

  /** get maximum momentum cut value */
  double getmaxP(void) { return m_maxP; }
  

  /** set minimum transverse energy cut value */
  void setminEt(double val) { m_minEt=val; }

  /** get minimum transverse energy cut value */
  double getminEt(void) { return m_minEt; }

  /** set maximum transverse energy cut value */
  void setmaxEt(double val) { m_maxEt=val; }

  /** get maximum transverse energy cut value */
  double getmaxEt(void) { return m_maxEt; }
  

  /** set minimum energy cut value */
  void setminE(double val) { m_minE=val; }

  /** get minimum energy cut value */
  double getminE(void) { return m_minE; }

  /** set maximum energy cut value */
  void setmaxE(double val) { m_maxE=val; }

  /** get maximum energy cut value */
  double getmaxE(void) { return m_maxE; }
  

  /** set minimum eta cut value */
  void setminEta(double val) { m_minEta=val; }

  /** get minimum eta cut value */
  double getminEta(void) { return m_minEta; }

  /** set maximum eta cut value */
  void setmaxEta(double val) { m_maxEta=val; }

  /** get maximum eta cut value */
  double getmaxEta(void) { return m_maxEta; }
  

  /** set minimum |eta| cut value */
  void setminAbsEta(double val) { m_minAbsEta=val; }

  /** get minimum |eta| cut value */
  double getminAbsEta(void) { return m_minAbsEta; }

  /** set maximum |eta| cut value */
  void setmaxAbsEta(double val) { m_maxAbsEta=val; }

  /** get maximum |eta| cut value */
  double getmaxAbsEta(void) { return m_maxAbsEta; }


  /** set eta veto ranges */
  void setEtaVetoRanges(std::string &range) ;

  /** get eta veto ranges */
  std::string getEtaVetoRanges(void) { return m_etaVetoRanges; }
  

  /** set minimum phi cut value */
  void setminPhi(double val) { m_minPhi=val; }

  /** get minimum phi cut value */
  double getminPhi(void) { return m_minPhi; }

  /** set maximum phi cut value */
  void setmaxPhi(double val) { m_maxPhi=val; }

  /** get maximum phi cut value */
  double getmaxPhi(void) { return m_maxPhi; }
  

  /** set phi veto ranges */
  void setPhiVetoRanges(std::string &range) ;

  /** get phi veto ranges */
  std::string getPhiVetoRanges(void) { return m_phiVetoRanges; }
  

  /** set minimum mass cut value */
  void setminMass(double val) { m_minMass=val; }

  /** get minimum mass cut value */
  double getminMass(void) { return m_minMass; }

  /** set maximum mass cut value */
  void setmaxMass(double val) { m_maxMass=val; }

  /** get maximum mass cut value */
  double getmaxMass(void) { return m_maxMass; }


  // Private member functions
private:
  /** Helper function */
  void convertStringRange( std::string &range,
                           std::vector<double> &minVals, 
                           std::vector<double> &maxVals );


  // Private class variables  
private:
  /** minimum transverse momentum cut value */
  double m_minPt;

  /** maximum transverse momentum cut value */
  double m_maxPt;
  

  /** minimum momentum cut value */
  double m_minP;

  /** maximum momentum cut value */
  double m_maxP;
  

  /** minimum energy cut value */
  double m_minE;

  /** maximum energy cut value */
  double m_maxE;


  /** minimum transverse energy cut value */
  double m_minEt;

  /** maximum transverse energy cut value */
  double m_maxEt;


  /** minimum eta cut value */
  double m_minEta;

  /** maximum eta cut value */
  double m_maxEta;


  /** minimum |eta| cut value */
  double m_minAbsEta;

  /** maximum |eta| cut value */
  double m_maxAbsEta;


  /** eta veto ranges string */
  std::string m_etaVetoRanges;

  /** eta veto ranges lower range boundaries */
  std::vector<double> m_minEtaVeto;

  /** eta veto ranges upper range boundaries */
  std::vector<double> m_maxEtaVeto;


  /** minimum phi cut value */
  double m_minPhi;

  /** maximum phi cut value */
  double m_maxPhi;


  /** phi veto ranges string */
  std::string m_phiVetoRanges;

  /** phi veto ranges lower range boundaries */
  std::vector<double> m_minPhiVeto;

  /** phi veto ranges upper range boundaries */
  std::vector<double> m_maxPhiVeto;


  /** minimum mass cut value */
  double m_minMass;

  /** maximum mass cut value */
  double m_maxMass;

};

#endif // ANALYSISUTILS_KINEMATICSELECTORCORE_H 
