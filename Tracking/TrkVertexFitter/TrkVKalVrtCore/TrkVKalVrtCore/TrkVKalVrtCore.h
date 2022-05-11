/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVKALVRTCORE_VKALVRTCORE_H
#define TRKVKALVRTCORE_VKALVRTCORE_H

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
   class IVKalState;

   class VKalVrtControlBase
   {
     public:
       VKalVrtControlBase(baseMagFld*,
                          const addrMagHandler,
                          const basePropagator*,
                          const addrPropagator,
                          IVKalState* istate = nullptr);
      VKalVrtControlBase(const VKalVrtControlBase & src) = default;
      ~VKalVrtControlBase() = default;

             baseMagFld*      vk_objMagFld;
       const addrMagHandler   vk_funcMagFld;
       const basePropagator*  vk_objProp;
       const addrPropagator   vk_funcProp;
             IVKalState*      vk_istate;
   };

   class VKalVrtControl : public VKalVrtControlBase
   {
     public:
       VKalVrtControl(const VKalVrtControlBase &);
       VKalVrtControl(const VKalVrtControl & src);              //copy
      ~VKalVrtControl() = default;

     public:

       void setIterationNum(int Iter);
       void setIterationPrec(double Prec);
       void setRobustScale(double Scale);
       void setRobustness(int Rob);
       void setMassCnstData(int Ntrk, double Mass);
       void setMassCnstData(int Ntrk, const std::vector<int> &Index, double Mass);

       void setUseMassCnst();
       void setUsePhiCnst();
       void setUsePlaneCnst(double a, double b, double c, double d);
       void setUseThetaCnst();
       void setUseAprioriVrt();
       void setUsePointingCnst(int );
       void setUsePassNear(int);

       void renewCascadeEvent(CascadeEvent *);
       //Only edit the CascadeEvent in the same thread it was created in
       const CascadeEvent * getCascadeEvent() const{ return m_cascadeEvent;};
       CascadeEvent * getCascadeEvent() { return m_cascadeEvent;};
       void renewFullCovariance(double *);
       //Only edit the covariance in the same thread it was created.
       const double * getFullCovariance () const { return m_fullCovariance.get(); }
       double * getFullCovariance () { return m_fullCovariance.get(); }
       void setVertexMass(double mass) { m_vrtMassTot=mass;}
       void setVrtMassError(double error) { m_vrtMassError=error;}
       double getVertexMass() const { return m_vrtMassTot;}
       double getVrtMassError() const {return m_vrtMassError;}



     private:

       std::unique_ptr<double[]> m_fullCovariance;   // On vertex fit exit contains full covariance matrix 
                                    // (x,y,z,px_0,py_0,pz_0,....,px_n,py_n,pz_n)
                                    // in symmetric form
       double m_vrtMassTot;
       double m_vrtMassError;
       CascadeEvent * m_cascadeEvent=nullptr;       
     public:
       ForCFT vk_forcft;
  };

} // end of namespace bracket

#endif
