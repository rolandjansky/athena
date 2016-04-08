/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOOLS_IANALYSISTOOLS_H
#define ANALYSISTOOLS_IANALYSISTOOLS_H

/**
   This class provides a common interface to Analysis Tools

   @author Tadashi Maeno
*/

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"

#include "NavFourMom/INavigable4MomentumCollection.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "AnalysisUtils/AnalysisMisc.h"

//class INavigable4Momentum;

//class INavigable4MomentumCollection; //this causes a problem in client packages (eg. AnalysisExamples) because it is now a typedef

class StoreGateSvc;

class IAnalysisTools : public AthAlgTool
{
public:

  /** constructor
   */
  IAnalysisTools ( const std::string& type, const std::string& name,
		   const IInterface* parent )
    : AthAlgTool(type,name,parent)
   {}

  /** destructor
   */
  virtual ~IAnalysisTools () {}

  /** \Delta\phi
   */
  double deltaPhi (const INavigable4Momentum *p1, const INavigable4Momentum *p2) const;
  
  /** \Delta{R}
   */
  double deltaR (const INavigable4Momentum *p1, const INavigable4Momentum *p2) const;

  /** Invariant mass : 2 body
   */
  double imass2 (const INavigable4Momentum *p1, const INavigable4Momentum *p2) const;

  /** Invariant mass : 4 body
   */
  double imass4 (const INavigable4Momentum *p1, const INavigable4Momentum *p2,
		 const INavigable4Momentum *p3, const INavigable4Momentum *p4) const;

  /** find the closest (in R) element in a collection to an INavigable4Momentum
      not check PDG ID
      @param index [out] index of the closest element
      @param deltaR [out] \Delta{R}
      @return true if found
  */
  template <class COLL> bool matchR
    (const INavigable4Momentum *t, COLL *coll, int &index, double &deltaR) const;

  template <class COLL> bool matchR
    (const double eta, const double phi, COLL *coll, int &index, double &deltaR) const;

   bool matchR (const INavigable4Momentum *t, const TruthParticleContainer *coll, 
		int &index, double &deltaR, const bool genOnly=true) const
     {
       return AnalysisUtils::Match::R(t->eta(), t->phi(), coll, index, deltaR, genOnly);
     }

   bool matchR (const double eta, const double phi, const TruthParticleContainer *coll, 
		int &index, double &deltaR, const bool genOnly=true) const
     {
       return AnalysisUtils::Match::R(eta, phi, coll, index, deltaR, genOnly);
     }

  /** find the closest (in R) element in a collection to an INavigable4Momentum
      not check PDG ID but a condition on E
      @param index [out] index of the closest element
      @param deltaR [out] \Delta{R}
      @return true if found
  */
  template <class COLL> bool matchR
    (const INavigable4Momentum *t, COLL *coll, int &index, double &deltaR, double &deltaE) const;

  template <class COLL> bool matchR
    (const double eta, const double phi, const double e, COLL *coll, int &index, 
     double &deltaR, double &deltaE) const;

  bool matchR (const double eta, const double phi, const double e, const TruthParticleContainer *coll, int &index, 
	       double &deltaR, double &deltaE, const bool genOnly=true) const 
    {
      return AnalysisUtils::Match::R(eta, phi, e, coll, index, deltaR, deltaE, genOnly);
    }

  bool matchR (const INavigable4Momentum *t, const TruthParticleContainer *coll, int &index, 
	       double &deltaR, double &deltaE, const bool genOnly=true) const 
    {
      return AnalysisUtils::Match::R(t->eta(), t->phi(), t->e(), coll, index, deltaR, deltaE, genOnly);
    }

  /** find the closest (in R) element in a collection to an INavigable4Momentum
      not check PDG ID
      @param element [out] pointer to the closest element
      @param deltaR  [out] \Delta{R}
      @return true if found
  */
  template <class COLL, class ELEMENT> bool matchR
    (const INavigable4Momentum *t, COLL *coll, ELEMENT *&element, double &deltaR) const;

  template <class COLL, class ELEMENT> bool matchR
    (const double eta, const double phi, COLL *coll, ELEMENT *&element, double &deltaR) const;

  bool matchR (const INavigable4Momentum *t, const TruthParticleContainer *coll, const TruthParticle *&element, 
	       double &deltaR, const bool genOnly=true) const
    {
      int index = 0;
      bool ret = AnalysisUtils::Match::R(t,coll,index,deltaR, genOnly);
      if (!ret) return false;
      element = (*coll)[index];
      return true;
    }

  bool matchR (const double eta, const double phi, const TruthParticleContainer *coll, const TruthParticle *&element, 
	       double &deltaR, const bool genOnly=true) const
    {
      int index;
      bool ret = AnalysisUtils::Match::R(eta,phi,coll,index,deltaR,genOnly);
      if (!ret) return false;
      element = (*coll)[index];
      return true;
    }

  /** find the closest (in R) element in a collection to an INavigable4Momentum
      not check PDG ID - with a condition on E
      @param element [out] pointer to the closest element
      @param deltaR  [out] \Delta{R}
      @return true if found
  */
  template <class COLL, class ELEMENT> bool matchR
    (const INavigable4Momentum *t, COLL *coll, ELEMENT *&element, double &deltaR, double &deltaE) const;

  template <class COLL, class ELEMENT> bool matchR
    (const double eta, const double phi, const double e, COLL *coll, ELEMENT *&element, 
     double &deltaR, double &deltaE) const;

  bool matchR (const INavigable4Momentum *t, const TruthParticleContainer *coll, const TruthParticle *&element, 
	       double &deltaR, double &deltaE, const bool genOnly=true) const 
    {
      int index = -1;
      bool ret = AnalysisUtils::Match::R(t,coll,index,deltaR, deltaE, genOnly);
      if (!ret || index == -1) return false;
      element = (*coll)[index];
      return true;
    }

  bool matchR (const double eta, const double phi, const double e, const TruthParticleContainer *coll, 
	       const TruthParticle *&element, double &deltaR, double &deltaE, const bool genOnly=true) const
    {
      int index = -1;
      bool ret = AnalysisUtils::Match::R(eta,phi,e,coll,index,deltaR,deltaE,genOnly);
      if (!ret || -1 == index) return false;
      element = (*coll)[index];
      return true;
    }

  /** find the closest (in R) element in a collection to an INavigable4Momentum
      @param index [out] index of the closest element
      @param deltaR [out] \Delta{R}
      @return true if found
  */
  template <class COLL> bool matchR
    (const INavigable4Momentum *t, COLL *coll, int &index, double &deltaR, const int pdg) const;

  template <class COLL> bool matchR
    (const double eta, const double phi, COLL *coll, int &index, double &deltaR, const int pdg) const;

  bool matchR (const double eta, const double phi, const TruthParticleContainer *coll, int &index, 
	       double &deltaR, const int pdg, const bool genOnly=true) const
    {
      return AnalysisUtils::Match::R(eta, phi, coll, index, deltaR, pdg, genOnly);
    }

  bool matchR (const INavigable4Momentum *t, const TruthParticleContainer *coll, int &index, 
	       double &deltaR, const int pdg, const bool genOnly=true) const
    {
      return AnalysisUtils::Match::R(t->eta(), t->phi(), coll, index, deltaR, pdg, genOnly);
    }

  /** find the closest (in R) element in a collection to an INavigable4Momentum
      @param index [out] index of the closest element
      @param deltaR [out] \Delta{R}
      @return true if found
  */
  template <class COLL> bool matchR
    (const INavigable4Momentum *t, COLL *coll, int &index, double &deltaR, const int pdg, 
     double &deltaE) const;

  template <class COLL> bool matchR
    (const double eta, const double phi, const double e, COLL *coll, int &index, double &deltaR, 
     const int pdg, double &deltaE) const;

  bool matchR (const double eta, const double phi, const double e, const TruthParticleContainer *coll, 
	       int &index, double &deltaR, const int pdg, double &deltaE, const bool genOnly=true) const
    {
      return AnalysisUtils::Match::R(eta, phi, e, coll, index, deltaR, pdg, deltaE, genOnly);
    }

  bool matchR (const INavigable4Momentum *t, const TruthParticleContainer *coll, 
	       int &index, double &deltaR, const int pdg, double &deltaE, const bool genOnly=true) const
    {
      return AnalysisUtils::Match::R(t->eta(), t->phi(), t->e(), coll, index, deltaR, pdg, deltaE, genOnly);
    }

  /** find the closest (in R) element in a collection to an INavigable4Momentum
      @param element [out] pointer to the closest element
      @param deltaR [out] \Delta{R}
      @return true if found
  */
  template <class COLL, class ELEMENT> bool matchR
    (const INavigable4Momentum *t, COLL *coll, ELEMENT *&element, double &deltaR, const int pdg) const;

  template <class COLL, class ELEMENT> bool matchR
    (const double eta, const double phi, COLL *coll, ELEMENT *&element, double &deltaR, const int pdg) const;

  bool matchR (const INavigable4Momentum *t, const TruthParticleContainer *coll, const TruthParticle *&element, 
	       double &deltaR, const int pdg, const bool genOnly=true) const
    {
      int index;
      bool ret = AnalysisUtils::Match::R(t,coll,index,deltaR,pdg,genOnly);
      if (!ret) return false;
      element = (*coll)[index];
      return true;
    }

  bool matchR (const double eta, const double phi, TruthParticleContainer *coll, const TruthParticle *&element, 
	       double &deltaR, const int pdg, const bool genOnly=true) const
    {
      int index;
      bool ret = AnalysisUtils::Match::R(eta,phi,coll,index,deltaR,pdg,genOnly);
      if (!ret) return false;
      element = (*coll)[index];
      return true;
    }

  /** find the closest (in R) element in a collection to an INavigable4Momentum
      @param element [out] pointer to the closest element and a condition on E
      @param deltaR [out] \Delta{R}
      @return true if found
  */
  template <class COLL, class ELEMENT> bool matchR
    (const INavigable4Momentum *t, COLL *coll, ELEMENT *&element, double &deltaR, const int pdg,
     double &deltaE) const;

  template <class COLL, class ELEMENT> bool matchR
    (const double eta, const double phi, const double e, COLL *coll, ELEMENT *&element, 
     double &deltaR, const int pdg, double &deltaE) const;

  bool matchR(const INavigable4Momentum *t, const TruthParticleContainer *coll, const TruthParticle *&element, double &deltaR, 
	      const int pdg,double &deltaE, const bool genOnly=true) const
    {
      int index;
      bool ret = AnalysisUtils::Match::R(t,coll,index,deltaR,pdg,deltaE,genOnly);
      if (!ret) return false;
      element = (*coll)[index];
      return true;
    }

  bool matchR (const double eta, const double phi, const double e, const TruthParticleContainer *coll, 
	       const TruthParticle *&element, double &deltaR, const int pdg, double &deltaE, const bool genOnly=true) const
    {
      int index;
      bool ret = AnalysisUtils::Match::R(eta,phi,e,coll,index,deltaR,pdg, deltaE,genOnly);
      if (!ret) return false;
      element = (*coll)[index];
      return true;
    }

  /** sort by pT
      @param coll [in] collection
      @param out [out] sorted collection
   */
  template <class COLL> void sortPT (COLL *coll) const;

  /** sort by energy
      @param coll [in] collection
      @param out [out] sorted collection
   */
  template <class COLL> void sortE (COLL *coll) const;

  /** sort by eta
      @param coll [in] collection
      @param out [out] sorted collection
   */
  template <class COLL> void sortEta (COLL *coll) const;

  /** sort by phi
      @param coll [in] collection
      @param out [out] sorted collection
   */
  template <class COLL> void sortPhi (COLL *coll) const;

  /** classify by charge
      @param COLL [in] type of collection
      @param coll [in] pointer to collection
      @param pos [out] collection of positives
      @param neg [out] collection of nagatives
  */
  template <class COLL> void classifyCharge (const COLL *coll,
					     std::vector<typename COLL::value_type> &pos,
					     std::vector<typename COLL::value_type> &neg) const;

  /** selection
      @param CALLER   [in] type of caller. e.g., class XYZ
      @param CRITERIA [in] function pointer. must be void XYZ::abc(COLL::value_type, XYZ*) 
      @param COLL     [in] type of 
      @param caller   [in] pointer to caller
      @param criteria [in] pointer to criteria
      @param coll     [in] pointer to collection
      @param key      [in] key for selected collection in SG
      @param lock     [in] lock the selected collection in SG. default=false
  */
  template <class CALLER, class CRITERIA, class COLL> StatusCode select
     (CALLER *caller, CRITERIA criteria, COLL *coll, const std::string &key, const bool=false) const;
};

// implementation for templates
#include "AnalysisTools/IAnalysisTools.icc"

#endif
