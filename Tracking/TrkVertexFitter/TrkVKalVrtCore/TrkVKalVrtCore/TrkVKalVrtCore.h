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

   class CascadeEvent;

   class VKalVrtControlBase
   {
     public:
       VKalVrtControlBase(const baseMagFld*, const addrMagHandler, const basePropagator*, const addrPropagator);
       VKalVrtControlBase(const VKalVrtControlBase & src);              //copy
      ~VKalVrtControlBase();

       const baseMagFld*      vk_objMagFld;
       const addrMagHandler   vk_funcMagFld;
       const basePropagator*  vk_objProp;
       const addrPropagator   vk_funcProp;
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
       void setMassCnstData(int Ntrk, std::vector<int> &Index, double Mass);

       void setUseMassCnst();
       void setUsePhiCnst();
       void setUsePlaneCnst(double a, double b, double c, double d);
       void setUseThetaCnst();
       void setUseAprioriVrt();
       void setUsePointingCnst(int );
       void setUsePassNear(int);

       void renewCascadeEvent(CascadeEvent *);
       CascadeEvent * getCascadeEvent() const;
       void renewFullCovariance(double *);
       double * getFullCovariance() const;

       void setVertexMass(double mass) { m_vrtMassTot=mass;}
       void setVrtMassError(double error) { m_vrtMassError=error;}
       double getVertexMass() { return m_vrtMassTot;}
       double getVrtMassError() {return m_vrtMassError;}

       ForCFT vk_forcft;

     private:

       double * m_fullCovariance;   // On vertex fit exit contains full covariance matrix 
                                    // (x,y,z,px_0,py_0,pz_0,....,px_n,py_n,pz_n)
                                    // in symmetric form
       double m_vrtMassTot;
       double m_vrtMassError;
       CascadeEvent * m_cascadeEvent=nullptr;       

  };

} // end of namespace bracket

#endif
