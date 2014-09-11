/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// emacs this is -*-C++-*-
#ifndef JETUTILS_JETKINEMATICS_H
#define JETUTILS_JETKINEMATICS_H

// #include "EventKernel/I4Momentum.h"

// //#include "FourMom/P4Help.h"
// #include "FourMomUtils/P4Helpers.h"

// #include "JetEvent/Jet.h"
// #include "JetUtils/JetDistances.h"

// #include <cmath>

// struct JetKinematics
// {

//   typedef CLHEP::Hep3Vector vector_t;
  
//   static double dotProduct(const I4Momentum* pJet1, const I4Momentum* pJet2)
//   {
//     return 
//       vector_t(pJet1->px(),pJet1->py(),pJet1->pz()).dot(vector_t(pJet2->px(),
// 								 pJet2->py(),
// 								 pJet2->pz()));
//   };

//   static double dotProductTrans(const I4Momentum* pJet1, 
// 				const I4Momentum* pJet2)
//   {
//     return
//       vector_t(pJet1->px(),pJet1->py(),double(0.)).dot(vector_t(pJet2->px(),
// 							pJet2->py(),
// 							double(0.)));
//   };

//   static vector_t crossProduct(const I4Momentum* pJet1, 
// 				 const I4Momentum* pJet2)
//   {
//     return 
//       vector_t(pJet1->px(),pJet1->py(),pJet1->pz()).cross(vector_t(pJet2->px(),
// 								   pJet2->py(),
// 								   pJet2->
// 								   pz()));
//   };

//   static CLHEP::Hep3Vector crossProductTrans(const I4Momentum* pJet1,
// 				      const I4Momentum* pJet2)
//   {
//     return 
//       vector_t(pJet1->px(),pJet1->py(),double(0.)).cross(vector_t(pJet2->px(),
// 								  pJet2->py(),
// 								  double(0.)));

//   };

//   static double projPtContrib(const I4Momentum* pJet1,
// 			      const I4Momentum* pJet2)
//   {
//     if ( pJet1->pt() == 0 || pJet2->pt() == 0. ) return 0.;
//     // calculate projection (absolute)
//     return dotProductTrans(pJet1,pJet2)/pJet1->pt();
//   };

//   static double perpPtContrib(const I4Momentum* pJet1,
// 			      const I4Momentum* pJet2)
//   {
//     if ( pJet1->pt() == 0 || pJet2->pt() == 0. ) return 0.;
//     // calculate perpendicular component (absolute)
//     return (crossProductTrans(pJet1,pJet2).mag())/pJet1->pt();
//   };

//   static double integratedProjPt(const Jet* pJet,
// 				 const double& radius)
//   {
//     Jet::constituent_iterator fCon(pJet->begin());
//     Jet::constituent_iterator lCon(pJet->end());
//     double intPtProj(double(0.0));
//     for ( ; fCon != lCon; fCon++ )
//       {
// 	if ( P4Helpers::deltaR(pJet,(*fCon)) < radius )
// 	  {
// 	    intPtProj += projPtContrib(pJet,(*fCon));
// 	  }
//       }
//     return intPtProj;
//   };




//   template<class CONT>
//   static double ptWeightedWidth(CONT & c, const I4Momentum* axis){
//     typename CONT::const_iterator it = c.begin();
//     typename CONT::const_iterator itE = c.end();
//     return ptWeightedWidth(it, itE, axis);
//   }

//   template<class ITERATOR>
//   static double ptWeightedWidth(ITERATOR & it, ITERATOR & itE, const I4Momentum* axis){
//     double jetPtSumConst = 0;
//     double jetWidth = 0;
    
//     for( ; it!=itE; ++it)
//       {
//         double dR = JetDistances::deltaR(axis,(*it));
//         double pt=(*it)->pt();
//         jetWidth += dR * pt;
//         jetPtSumConst += pt;
//       }
    
//     if(jetPtSumConst <= 0)
//       return -1;
    
//     jetWidth = jetWidth / jetPtSumConst;
//     return jetWidth;
//   }


//   /// Functions to compute various kinematic quantities related to a collection of 
//   /// 4Momentum in a single call.
//   /// As a compromise for a stable yet flexible interface, we simply use a vector<double> as 
//   ///  the returned object with an enum helper to flag each value in this vector.
//   struct CollectionQuantities {
//     enum {
//       N, SumPt, WeightedWidth,
//       Px,Py,Pz,E,
//       MAX
//     };
//     Jet::hlv_t toHepLotentzVect(std::vector<double> & v){ return Jet::hlv_t(v[Px], v[Py], v[Pz], v[E]); }
//   };
  
//   template<class CONT> 
//   static std::vector<double> computeCollectionQuantities(CONT &c,const I4Momentum* parent){
//     typename CONT::const_iterator it = c.begin();
//     typename CONT::const_iterator itE = c.end();
//     return computeCollectionQuantities(it, itE, parent);
//   }
//   template<class ITERATOR>
//   static std::vector<double> computeCollectionQuantities(ITERATOR & it, ITERATOR & itE, const I4Momentum* parent){
//     std::vector<double> outV(CollectionQuantities::MAX,0.0);
//     typedef CollectionQuantities CQ;
//     for( ; it!=itE; ++it)
//       {
//         double dR = JetDistances::deltaR(parent,(*it));
//         double pt=(*it)->pt();
//         outV[CQ::WeightedWidth] += dR * pt;
//         outV[CQ::SumPt] += pt;
//         outV[CQ::N]+=1;
//         outV[CQ::Px] += (*it)->px();outV[CQ::Py] += (*it)->py();outV[CQ::Pz] += (*it)->pz();outV[CQ::E] += (*it)->e();
//       }

//     if(outV[CQ::SumPt] > 0){
//       outV[CQ::WeightedWidth] = outV[CQ::WeightedWidth] / outV[CQ::SumPt] ;
//     } else outV[CQ::WeightedWidth] = -1;

//     return outV;
//   }


};
#endif
