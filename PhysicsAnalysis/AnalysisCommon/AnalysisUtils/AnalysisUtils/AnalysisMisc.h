/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISUTILS_ANALYSISMISC_H
#define ANALYSISUTILS_ANALYSISMISC_H

/**
   Tools collection

   @author Tadashi Maeno
*/

#include <math.h>
#include <vector>
#include <algorithm>

#include "CxxUtils/fpcompare.h"
#include "AthContainers/DataVector.h"
#include "boost/type_traits/remove_pointer.hpp"
#include "boost/type_traits/is_convertible.hpp"

#ifndef XAOD_STANDALONE
#include "CLHEP/Vector/LorentzVector.h"
#include "EventKernel/INavigable4Momentum.h"
#include "McParticleEvent/TruthParticleContainer.h"
#endif

namespace AnalysisUtils {

#ifndef XAOD_STANDALONE
  
  /** compute @f$ \Delta @f$
   */
  namespace Delta {

    /** @f$ \Delta\phi @f$
     */
    inline double phi (const INavigable4Momentum *p1, const INavigable4Momentum *p2) {
      double phi1 = (p1->phi()>M_PI) ? p1->phi()-2*M_PI : p1->phi();
      double phi2 = (p2->phi()>M_PI) ? p2->phi()-2*M_PI : p2->phi();
      double dphi = fabs(phi1-phi2);
      if(dphi>M_PI) dphi = 2*M_PI-dphi;
      return dphi;
    }
    
    /** @f$ \Delta{R} @f$
     */
    inline double R (const INavigable4Momentum *p1, const double v_eta, const double v_phi) {
      double phi1 = (p1->phi()>M_PI) ? p1->phi()-2*M_PI : p1->phi();
      double phi2 = (v_phi>M_PI) ? v_phi-2*M_PI : v_phi;
      double dphi = fabs(phi1-phi2);
      if(dphi>M_PI) dphi = 2*M_PI-dphi;
      double deta = p1->eta() - v_eta;
      return sqrt(dphi*dphi+deta*deta);
    }

    /** @f$ \Delta{R} @f$
     */
    inline double R (const INavigable4Momentum *p1, const INavigable4Momentum *p2) {
      return R (p1, p2->eta(), p2->phi());
    }
  } // end of Delta namespace

  ///////////////////////////////////////////////////////////////////////////

  /** compute Invariant mass
   */
  namespace Imass {

    inline double two (const INavigable4Momentum *p1, const INavigable4Momentum *p2) {
      return (p1->hlv()+p2->hlv()).m();
    }
    
    inline double four (const INavigable4Momentum *p1, const INavigable4Momentum *p2,
		 const INavigable4Momentum *p3, const INavigable4Momentum *p4) {
      return (p1->hlv()+p2->hlv()+p3->hlv()+p4->hlv()).m();
    }
  } // end of Imass namespace

  ///////////////////////////////////////////////////////////////////////////

  /** find the closest element in a collection to an INavigable4Momentum
   */
  namespace Match {

    /** find the closest element in R
	@param index [out] index of the closest element
	@param deltaR [out] @f$ \Delta{R} @f$
	@return true if found
    */
    template <class COLL>
    inline bool R (const double eta, const double phi, COLL *coll, int &index, double &deltaR, const int pdg)
    {
      deltaR = 10000.; // big value
      bool l_return = false;
      int l_idx = 0;
      typename COLL::const_iterator it  = coll->begin();
      typename COLL::const_iterator itE = coll->end();
      for (; it != itE; ++it)
	{
	  if (((*it)->pdgId()==pdg) || pdg==0 )
	    {
	      double rtu = Delta::R(*it,eta,phi);
	      if ( rtu < deltaR )
		{
		  index  = l_idx;
		  deltaR = rtu;
		  l_return = true;
		}
	    }
	  ++l_idx;
      }
      return l_return;
    }

    inline bool R (const double eta, const double phi, const TruthParticleContainer *coll, int &index, 
		   double &deltaR, const int pdg, const bool genOnly=true)
    {
      deltaR = 10000.; // big value
      bool l_return = false;
      int l_idx = 0;
      TruthParticleContainer::const_iterator it  = coll->begin();
      TruthParticleContainer::const_iterator itE = coll->end();
      if ( genOnly )
	{
	  for (; it != itE; ++it)
	    {
	      if ( (*it)->barcode() < 100000 ) // only generator particles
		{
		  if (((*it)->pdgId()==pdg) || pdg==0 )
		    {
		      double rtu = Delta::R(*it,eta,phi);
		      if ( rtu < deltaR )
			{
			  index  = l_idx;
			  deltaR = rtu;
			  l_return = true;
			}
		    }
		}
	      ++l_idx;
	    }
	  return l_return;
	}
      else return R( eta, phi, coll, index, deltaR, pdg);
    }

    /** find the closest element in R - with a condition on E
	@param index [out] index of the closest element
	@param deltaR [out] @f$ \Delta{R} @f$
	@return true if found
    */
    template <class COLL>
    inline bool R (const double eta, const double phi, const double e, COLL *coll, 
                   int &index, double &deltaR, const int pdg, double &deltaE)
    {
      deltaR = 1.0e+20; // big value
      deltaE = 1.0e+20;
      bool l_return = false;
      int l_idx = 0;
      typename COLL::const_iterator it  = coll->begin();
      typename COLL::const_iterator itE = coll->end();
      for (; it != itE; ++it)
	{
	  if (((*it)->pdgId()==pdg) || pdg==0 )
	    {
	      double rtu = Delta::R(*it,eta,phi);
              double dE  = fabs( e-(*it)->e() );
	      if ( rtu < deltaR && dE < deltaE)
		{
		  index  = l_idx;
		  deltaR = rtu;
                  deltaE = dE;
		  l_return = true;
		}
	    }
	  ++l_idx;
      }
      return l_return;
    }

    inline bool R (const double eta, const double phi, const double e, const TruthParticleContainer *coll, 
                   int &index, double &deltaR, const int pdg, double &deltaE, const bool genOnly=true)
    {
      deltaR = 1.0e+20; // big value
      deltaE = 1.0e+20;
      bool l_return = false;
      int l_idx = 0;
      TruthParticleContainer::const_iterator it  = coll->begin();
      TruthParticleContainer::const_iterator itE = coll->end();
      if ( genOnly ) 
	{
	  for (; it != itE; ++it)
	    {
	      if ( (*it)->barcode() < 100000 ) // only generator particles
		{	      
		  if (((*it)->pdgId()==pdg) || pdg==0 )
		    {
		      double rtu = Delta::R(*it,eta,phi);
		      double dE  = fabs( e-(*it)->e() );
		      if ( rtu < deltaR && dE < deltaE)
			{
			  index  = l_idx;
			  deltaR = rtu;
			  deltaE = dE;
			  l_return = true;
			}
		    }
		}
	      ++l_idx;
	    }
	  return l_return;
	}
      else return R(eta, phi, e, coll, index, deltaR, pdg, deltaE);
    }

    /** find the closest element in R
	@param index [out] index of the closest element
	@param deltaR [out] @f$ \Delta{R} @f$
	@return true if found
    */
    template <class COLL>
    inline bool R (const INavigable4Momentum *t, COLL *coll, int &index, double &deltaR, const int pdg)
    {
      return R (t->eta(), t->phi(), coll, index, deltaR, pdg);
    }

    inline bool R (const INavigable4Momentum *t, const TruthParticleContainer *coll, int &index, double &deltaR,
		   const int pdg, const bool genOnly=true)
    {
      return R (t->eta(), t->phi(), coll, index, deltaR, pdg, genOnly);
    }

    /** find the closest element in R - with a condition on E
	@param index [out] index of the closest element
	@param deltaR [out] @f$ \Delta{R} @f$
	@return true if found
    */
    template <class COLL>
    inline bool R (const INavigable4Momentum *t, COLL *coll, int &index, double &deltaR, 
                   const int pdg, double &deltaE)
    {
      return R (t->eta(), t->phi(), t->e(), coll, index, deltaR, pdg, deltaE);
    }

    inline bool R (const INavigable4Momentum *t, const TruthParticleContainer *coll, int &index, 
		   double &deltaR, const int pdg, double &deltaE, const bool genOnly=true)
    {
      return R (t->eta(), t->phi(), t->e(), coll, index, deltaR, pdg, deltaE, genOnly);
    }

    /** find the closest element in R (not check PDG ID)
	@return index of the element; -1 if not found
    */
    template <class COLL>
    inline bool R (const double eta, const double phi, COLL *coll, int &index, double &deltaR)
    {
      deltaR = 10000.; // big value
      bool l_return = false;
      int l_idx = 0;
      typename COLL::const_iterator it  = coll->begin();
      typename COLL::const_iterator itE = coll->end();
      for (; it != itE; ++it)
	{
	  double rtu = Delta::R(*it,eta,phi);
	  if ( rtu < deltaR )
	    {
	      index  = l_idx;
	      deltaR = rtu;
	      l_return = true;
	    }
	  ++l_idx;
	}
      return l_return;
    }

    inline bool R (const double eta, const double phi, const TruthParticleContainer *coll, 
		   int &index, double &deltaR, const bool genOnly=true)
    {
      deltaR = 10000.; // big value
      bool l_return = false;
      int l_idx = 0;
      TruthParticleContainer::const_iterator it  = coll->begin();
      TruthParticleContainer::const_iterator itE = coll->end();
      if ( genOnly ) 
	{
	  for (; it != itE; ++it)
	    {
	      if ( (*it)->barcode() < 100000 ) // only generator particles
		{
		  double rtu = Delta::R(*it,eta,phi);
		  if ( rtu < deltaR )
		    {
		      index  = l_idx;
		      deltaR = rtu;
		      l_return = true;
		    }
		}
	      ++l_idx;
	    }
	  return l_return;
	}
      else return R(eta,phi,coll, index, deltaR);
    }

    /** find the closest element in R (not check PDG ID) - with a condition on E
	@return index of the element; -1 if not found
    */
    template <class COLL>
    inline bool R (const double eta, const double phi, const double e, COLL *coll, int &index, 
                   double &deltaR, double &deltaE)
    {
      deltaR = 1.0e+20; // big value
      deltaE = 1.0e+20;
      bool l_return = false;
      int l_idx = 0;
      typename COLL::const_iterator it  = coll->begin();
      typename COLL::const_iterator itE = coll->end();
      for (; it != itE; ++it)
	{
	  double rtu = Delta::R(*it,eta,phi);
	  double dE  = fabs( e-(*it)->e() );
	  if ( rtu < deltaR && dE < deltaE)
	    {
	      index  = l_idx;
	      deltaR = rtu;
	      deltaE = dE;
	      l_return = true;
	    }
	  ++l_idx;
	}
      return l_return;
    }

    inline bool R (const double eta, const double phi, const double e, const TruthParticleContainer *coll, int &index, 
                   double &deltaR, double &deltaE, const bool genOnly=true)
    {
      deltaR = 1.0e+20; // big value
      deltaE = 1.0e+20;
      bool l_return = false;
      int l_idx = 0;
      TruthParticleContainer::const_iterator it  = coll->begin();
      TruthParticleContainer::const_iterator itE = coll->end();
      if ( genOnly ) 
	{
	  for (; it != itE; ++it)
	    {
	      if ( (*it)->barcode() < 100000 ) // only generator particles
		{
		  double rtu = Delta::R(*it,eta,phi);
		  double dE  = fabs( e-(*it)->e() );
		  if ( rtu < deltaR && dE < deltaE)
		    {
		      index  = l_idx;
		      deltaR = rtu;
		      deltaE = dE;
		      l_return = true;
		    }
		}
	      ++l_idx;
	    }
	  return l_return;
	}
      else return R(eta, phi, e, coll, index, deltaR, deltaE);
    }

    /** find the closest element in R (not check PDG ID)
	@return index of the element; -1 if not found
    */
    template <class COLL>
    inline bool R (const INavigable4Momentum *t, COLL *coll, int &index, double &deltaR)
    {
      return R (t->eta(), t->phi(), coll, index, deltaR);
    }

    inline bool R (const INavigable4Momentum *t, const TruthParticleContainer *coll, int &index, double &deltaR,
		   const bool genOnly=true)
    {
      return R (t->eta(), t->phi(), coll, index, deltaR, genOnly);
    }

    /** find the closest element in R (not check PDG ID) - with a condition on E
	@return index of the element; -1 if not found
    */
    template <class COLL>
    inline bool R (const INavigable4Momentum *t, COLL *coll, int &index, double &deltaR, double &deltaE)
    {
      return R (t->eta(), t->phi(), t->e(), coll, index, deltaR, deltaE);
    }

    inline bool R (const INavigable4Momentum *t, const TruthParticleContainer *coll, int &index, 
		   double &deltaR, double &deltaE, const bool genOnly=true)
    {
      return R (t->eta(), t->phi(), t->e(), coll, index, deltaR, deltaE, genOnly);
    }

  } // end of Math namespace

  ///////////////////////////////////////////////////////////////////////////

#endif
  
  /** sort 
   */
  namespace Sort {

    namespace Private {

      using CxxUtils::fpcompare::greater;

      // function object for pT sorting
      template <class T> inline bool compPt (T a, T b)
      {
	return greater (a->pt(), b->pt());
      }

      // function object for e sorting
      template <class T> inline bool compE (T a, T b)
      {
	return greater (a->e(), b->e());
      }

      // function object for eta sorting
      template <class T> inline bool compEta (T a, T b)
      {
	return greater (a->eta(), b->eta());
      }

      // function object for phi sorting
      template <class T> inline bool compPhi (T a, T b)
      {
	return greater (a->phi(), b->phi());
      }

      template <bool b>
      struct dosort_imp
      {
        template <class COLL, class COMP>
        static void dosort (COLL& coll, COMP comp)
        { std::sort (coll.begin(), coll.end(), comp); }
      };

      template <>
      struct dosort_imp<true>
      {
        template <class COLL, class COMP>
        static void dosort (COLL& coll, COMP comp)
        { coll.sort(comp); }
      };

      template <class COLL, class COMP>
      inline void dosort (COLL& coll, COMP comp)
      {
        typedef typename boost::remove_pointer<typename COLL::value_type>::type
                valtype;
        typedef DataVector<valtype> dvtype;
        const bool is_dv = boost::is_convertible<COLL&, dvtype&>::value;
        dosort_imp<is_dv>::dosort (coll, comp);
      }

    } // end of Private namespace

    /** sort by pT
	@param COLL [in] type of collection
	@param coll [i/o] pointer to collection
    */
    template <class COLL> inline void pT (COLL *coll)
    {
      // sort
      Private::dosort (*coll, Private::compPt<typename COLL::value_type>);
    }

    /** sort by e
	@param COLL [in] type of collection
	@param coll [i/o] pointer to collection
    */
    template <class COLL> inline void e (COLL *coll)
    {
      // sort
      Private::dosort (*coll, Private::compE<typename COLL::value_type>);
    }

    /** sort by eta
	@param COLL [in] type of collection
	@param coll [i/o] pointer to collection
    */
    template <class COLL> inline void eta (COLL *coll)
    {
      // sort
      Private::dosort (*coll, Private::compEta<typename COLL::value_type>);
    }

    /** sort by phi
	@param COLL [in] type of collection
	@param coll [i/o] pointer to collection
    */
    template <class COLL> inline void phi (COLL *coll)
    {
      // sort
      Private::dosort (*coll, Private::compPhi<typename COLL::value_type>);
    }
  } // end of Sort namespace

#ifndef XAOD_STANDALONE
  ///////////////////////////////////////////////////////////////////////////

  /** classify
   */
  namespace Classify {

    /** classify by charge
	@param COLL [in] type of collection
	@param coll [in] pointer to collection
	@param pos [out] collection of positives
	@param neg [out] collection of nagatives
    */
    template <class COLL> inline void charge (COLL *coll,
					      std::vector<typename COLL::value_type> &pos,
					      std::vector<typename COLL::value_type> &neg)
    {
      pos.clear();
      neg.clear();

      // classify
      typename COLL::const_iterator it  = coll->begin();
      typename COLL::const_iterator itE = coll->end();
      for (; it != itE; ++it)
	{
	  if ((*it)->pdgId() > 0)
	    pos.push_back(*it);
	  else if ((*it)->pdgId() < 0)
	    neg.push_back(*it);
	}
    }
  } // end of Classify namespace

#endif

} // end of AnalysisUtils namespace

#endif
