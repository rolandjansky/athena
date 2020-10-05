/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
//-------------------------------------------------
#include<iostream>


namespace Trk{
//
// Option setting for VKalVrt core via state.m_vkalFitControl object
//

  void TrkVKalVrtFitter::VKalVrtConfigureFitterCore(int NTRK, State& state) const
  {
    state.m_FitStatus = 0;     // Drop all previous fit results
    state.m_vkalFitControl.vk_forcft = ForCFT();
  
    //Set input particle masses
    for(int it=0; it<NTRK; it++){
      if( it<(int)state.m_MassInputParticles.size() ) {
        state.m_vkalFitControl.vk_forcft.wm[it]  = (double)(state.m_MassInputParticles[it]);
      }
      else { state.m_vkalFitControl.vk_forcft.wm[it]=(double)(139.5702); }
    }
    // Set reference vertex for different pointing constraints
    if(state.m_VertexForConstraint.size() >= 3){
      state.m_vkalFitControl.vk_forcft.vrt[0]    =state.m_VertexForConstraint[0] - state.m_refFrameX;
      state.m_vkalFitControl.vk_forcft.vrt[1]    =state.m_VertexForConstraint[1] - state.m_refFrameY;
      state.m_vkalFitControl.vk_forcft.vrt[2]    =state.m_VertexForConstraint[2] - state.m_refFrameZ;
    }else {for( int i=0; i<3; i++)  state.m_vkalFitControl.vk_forcft.vrt[i] = 0.; }
    // Set covariance matrix for reference vertex
    if(state.m_CovVrtForConstraint.size() >= 6){
           for( int i=0; i<6; i++) { state.m_vkalFitControl.vk_forcft.covvrt[i] = (double)(state.m_CovVrtForConstraint[i]); }
    }else{ for( int i=0; i<6; i++) { state.m_vkalFitControl.vk_forcft.covvrt[i] = 0.; } }

    // Add global mass constraint if present
    if(state.m_massForConstraint >= 0.) state.m_vkalFitControl.setMassCnstData(NTRK,state.m_massForConstraint);
    // Add partial mass constraints if present
    if(!state.m_partMassCnst.empty()) {
      for(int ic=0; ic<(int)state.m_partMassCnst.size(); ic++){
        state.m_vkalFitControl.setMassCnstData(NTRK, state.m_partMassCnstTrk[ic],state.m_partMassCnst[ic]);
      }
    }
    // Set general configuration parameters
    state.m_vkalFitControl.setRobustness(state.m_Robustness);
    state.m_vkalFitControl.setRobustScale(state.m_RobustScale);
    state.m_vkalFitControl.setUsePlaneCnst(0.,0.,0.,0.);
    if(state.m_useAprioriVertex) state.m_vkalFitControl.setUseAprioriVrt();
    if(state.m_useThetaCnst)     state.m_vkalFitControl.setUseThetaCnst();
    if(state.m_usePhiCnst)       state.m_vkalFitControl.setUsePhiCnst();
    if(state.m_usePointingCnst)  state.m_vkalFitControl.setUsePointingCnst(1);
    if(state.m_useZPointingCnst) state.m_vkalFitControl.setUsePointingCnst(2);
    if(state.m_usePassNear)      state.m_vkalFitControl.setUsePassNear(1);
    if(state.m_usePassWithTrkErr)state.m_vkalFitControl.setUsePassNear(2);

    if(m_IterationPrecision>0.) state.m_vkalFitControl.setIterationPrec(m_IterationPrecision);
    if(m_IterationNumber)  state.m_vkalFitControl.setIterationNum(m_IterationNumber);
 }

/* 
  void TrkVKalVrtFitter::initCnstList()
  {
//--- Mass constraint is restored here
    if( m_massForConstraint>0. || m_PartMassCnst.size()>0 ) vksetUseMassCnst();
    if( m_useAprioriVertex ) vksetUseAprioriVrt();
    if( m_useThetaCnst )     vksetUseThetaCnst();
    if( m_usePhiCnst   )     vksetUsePhiCnst();
    if( m_usePointingCnst )  vksetUsePointingCnst(1);
    if( m_useZPointingCnst)  vksetUsePointingCnst(2);
    if( m_usePassNear)       vksetUsePassNear(1);
    if( m_usePassWithTrkErr) vksetUsePassNear(2);
  }
*/ 
//Old logics. Left here for reference to compare with previous releases of VKalVrt
//
  void TrkVKalVrtFitter::setCnstType(int TYPE, IVKalState& istate) const
  {
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    if(TYPE>0)msg(MSG::DEBUG)<< "ConstraintType is changed at execution stage. New type="<<TYPE<< endmsg;
    if(TYPE<0)TYPE=0;
    if(TYPE>14)TYPE=0;
    if( TYPE == 2)  state.m_usePointingCnst   = true;    
    if( TYPE == 3)  state.m_useZPointingCnst  = true;    
    if( TYPE == 4)  state.m_usePointingCnst   = true;    
    if( TYPE == 5)  state.m_useZPointingCnst  = true;    
    if( TYPE == 6)  state.m_useAprioriVertex  = true;    
    if( TYPE == 7)  state.m_usePassWithTrkErr = true;    
    if( TYPE == 8)  state.m_usePassWithTrkErr = true;    
    if( TYPE == 9)  state.m_usePassNear       = true;    
    if( TYPE == 10) state.m_usePassNear       = true;    
    if( TYPE == 11) state.m_usePhiCnst = true;    
    if( TYPE == 12) { state.m_usePhiCnst = true; state.m_useThetaCnst = true;}
    if( TYPE == 13) { state.m_usePhiCnst = true; state.m_usePassNear  = true;}
    if( TYPE == 14) { state.m_usePhiCnst = true; state.m_useThetaCnst = true; state.m_usePassNear  = true;}
  }

//
// Define finctions for on-the-fly fitter configuration 
//
//
  void TrkVKalVrtFitter::setApproximateVertex(double X,double Y,double Z,
                                              IVKalState& istate) const 
  {
     assert(dynamic_cast<State*> (&istate)!=nullptr);
     State& state = static_cast<State&> (istate);
     state.m_ApproximateVertex.assign ({X, Y, Z});
  }
  
  void TrkVKalVrtFitter::setRobustness(int IROB, IVKalState& istate) const
  { if(IROB>0)msg(MSG::DEBUG)<< "Robustness is changed at execution stage "<<m_Robustness<<"=>"<<IROB<< endmsg;
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    state.m_Robustness = IROB;
    if(state.m_Robustness<0)state.m_Robustness=0;
    if(state.m_Robustness>7)state.m_Robustness=0;
  }

  void TrkVKalVrtFitter::setRobustScale(double Scale, IVKalState& istate) const
  { if(Scale!=m_RobustScale)msg(MSG::DEBUG)<< "Robust Scale is changed at execution stage "<<m_RobustScale<<"=>"<<Scale<< endmsg;
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    state.m_RobustScale = Scale;
    if(state.m_RobustScale<0.01) state.m_RobustScale=1.;
    if(state.m_RobustScale>100.) state.m_RobustScale=1.;
  }

  void TrkVKalVrtFitter::setMassForConstraint(double MASS,
                                              IVKalState& istate) const
  {
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    state.m_massForConstraint = MASS;
  }

  void TrkVKalVrtFitter::setMassForConstraint(double MASS,
                                              const std::vector<int>& TrkIndex,
                                              IVKalState& istate) const
  {
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    state.m_partMassCnst.push_back(MASS);
    state.m_partMassCnstTrk.push_back(TrkIndex);
  }

  void TrkVKalVrtFitter::setVertexForConstraint(const xAOD::Vertex & Vrt,
                                                IVKalState& istate) const
  {
     assert(dynamic_cast<State*> (&istate)!=nullptr);
     State& state = static_cast<State&> (istate);
     state.m_VertexForConstraint.assign ({Vrt.position().x(),
                                          Vrt.position().y(),
                                          Vrt.position().z()});

     state.m_CovVrtForConstraint.assign ({
         Vrt.covariancePosition()(Trk::x,Trk::x),
         Vrt.covariancePosition()(Trk::x,Trk::y),
         Vrt.covariancePosition()(Trk::y,Trk::y),
         Vrt.covariancePosition()(Trk::x,Trk::z),
         Vrt.covariancePosition()(Trk::y,Trk::z),
         Vrt.covariancePosition()(Trk::z,Trk::z)});
  }

  void TrkVKalVrtFitter::setVertexForConstraint(double X,double Y,double Z,
                                                IVKalState& istate) const
  {
     assert(dynamic_cast<State*> (&istate)!=nullptr);
     State& state = static_cast<State&> (istate);
     state.m_VertexForConstraint.assign ({X, Y, Z});
  }

  void TrkVKalVrtFitter::setCovVrtForConstraint(double XX,double XY,double YY,
                                                double XZ,double YZ,double ZZ,
                                                IVKalState& istate) const
  {
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    state.m_CovVrtForConstraint.assign ({XX, XY, YY, XZ, YZ, ZZ});
  }			  

  void TrkVKalVrtFitter::setMassInputParticles( const std::vector<double>& mass,
                                                IVKalState& istate) const
  {
    assert(dynamic_cast<State*> (&istate)!=nullptr);
    State& state = static_cast<State&> (istate);
    state.m_MassInputParticles = mass;
    for (double& m : state.m_MassInputParticles) {
      m = std::abs(m);
    }
  }

} //end of namespace
