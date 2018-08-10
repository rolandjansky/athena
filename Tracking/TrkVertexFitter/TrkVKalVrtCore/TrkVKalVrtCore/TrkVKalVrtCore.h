/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TrkVKalVrtCore_VKalVrtCore_H
#define _TrkVKalVrtCore_VKalVrtCore_H

#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include "TrkVKalVrtCore/Propagator.h"
#include "TrkVKalVrtCore/ForCFT.h"
#include <vector>
#include <memory>

namespace Trk {
//
//   Main class to control TrkVKalVrtCore package
//  If references to external Propagator/Mag.Field are present - the package uses them,
//  otherwise default internal fixed_field/simple_propagator are used. 
//--------------------------------------------------------------------

   class VKalVrtControlBase
   {
     public:
       VKalVrtControlBase(const baseMagFld*, const addrMagHandler, const basePropagator*, const addrPropagator);
       VKalVrtControlBase(const VKalVrtControlBase & src);              //copy
      ~VKalVrtControlBase();

       const baseMagFld*      m_objMagFld;
       const addrMagHandler   m_funcMagFld;
       const basePropagator*  m_objProp;
       const addrPropagator   m_funcProp; 
   };

   class VKalVrtControl : public VKalVrtControlBase
   {
     public:
       VKalVrtControl(const VKalVrtControlBase &);
       VKalVrtControl(const VKalVrtControl & src);              //copy
      ~VKalVrtControl();

     public:

       void setIterationNum(int Iter);
       void setIterationPrec(double Prec);
       void setRobustScale(double Scale);
       void setRobustness(int Rob);
       void setMassCnstData(int Ntrk, double Mass);
       void setMassCnstData(int Ntrk, std::vector<int> Index, double Mass);

       void setUseMassCnst();
       void setUsePhiCnst();
       void setUsePlaneCnst(double a, double b, double c, double d);
       void setUseThetaCnst();
       void setUseAprioriVrt();
       void setUsePointingCnst(int );
       void setUsePassNear(int);

     public:

       double * m_fullCovariance;   // On vertex fit exit contains full covariance matrix 
                                    // (x,y,z,px_0,py_0,pz_0,....,px_n,py_n,pz_n)
                                    // in symmetric form
       ForCFT m_forcft;
       double m_vrtMassTot;
       double m_vrtMassError;
  };

} // end of namespace bracket

#endif
