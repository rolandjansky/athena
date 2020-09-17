/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file   BPhysHypoHelper.cxx
 *  @author Daniel Scheirich <daniel.scheirich@cern.ch>
 */

#include "xAODBPhys/BPhysHypoHelper.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

/*****************************************************************************/

/** @{
 *  Some useful macros
 */

/*****************************************************************************/
#define GET_FLOAT(name)                                    \
{                                                          \
  SG::AuxElement::Accessor<float> floatAcc(name);   \
  if(!floatAcc.isAvailable(*m_b)) return -9999999.;        \
  return floatAcc(*m_b);                                   \
}
/*****************************************************************************/
#define SET_FLOAT( name, val)                              \
{                                                          \
  SG::AuxElement::Decorator<float> floatDec(name);  \
  floatDec(*m_b) = val;                                    \
  return true;                                             \
}
/*****************************************************************************/

/** @} */

/*****************************************************************************/
float xAOD::BPhysHypoHelper::mass() const
{
  GET_FLOAT( m_hypo+"_mass" );
}
/*****************************************************************************/
float xAOD::BPhysHypoHelper::massErr() const
{
  GET_FLOAT( m_hypo+"_massErr" );  
}
/*****************************************************************************/
bool xAOD::BPhysHypoHelper::setMass(const float val)
{
  SET_FLOAT( m_hypo+"_mass", val );
}
/*****************************************************************************/
bool xAOD::BPhysHypoHelper::setMassErr(const float val)
{
  SET_FLOAT( m_hypo+"_massErr", val );  
}
/*****************************************************************************/
bool xAOD::BPhysHypoHelper::setMass(const std::vector<double> &trkMasses)
{
  // get 4-vector
  TLorentzVector P = totalP(trkMasses);
  if( P == TLorentzVector(0,0,0,0) )
    return false;
  
  // set mass:
  return setMass(P.M());  
}
/*****************************************************************************/
float xAOD::BPhysHypoHelper::tau(const pv_type vertexType, const tau_type tauType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_TauConstMassPVMaxSumPt2");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_TauInvMassPVMaxSumPt2");
        default: return -9999999.;
      }
    }
    case PV_MIN_A0: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_TauConstMassPVMinA0");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_TauInvMassPVMinA0");
        default: return -9999999.;
      }
    }
    case PV_MIN_Z0: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_TauConstMassPVMinZ0");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_TauInvMassPVMinZ0");
        default: return -9999999.;
      }
    }
    case PV_MIN_Z0_BA: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_TauConstMassPVMinZ0BA");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_TauInvMassPVMinZ0BA");
        default: return -9999999.;
      }
    }
    default: return -9999999.;
  }  
}
/*****************************************************************************/              
float xAOD::BPhysHypoHelper::tauErr(const pv_type vertexType, const tau_type tauType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_TauErrConstMassPVMaxSumPt2");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_TauErrInvMassPVMaxSumPt2");
        default: return -9999999.;
      }
    }
    case PV_MIN_A0: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_TauErrConstMassPVMinA0");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_TauErrInvMassPVMinA0");
        default: return -9999999.;
      }
    }
    case PV_MIN_Z0: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_TauErrConstMassPVMinZ0");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_TauErrInvMassPVMinZ0");
        default: return -9999999.;
      }
    }
    case PV_MIN_Z0_BA: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_TauErrConstMassPVMinZ0BA");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_TauErrInvMassPVMinZ0BA");
        default: return -9999999.;
      }
    }
    default: return -9999999.;
  }    
}
/*****************************************************************************/
bool xAOD::BPhysHypoHelper::setTau(const float val, const pv_type vertexType, const tau_type tauType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_TauConstMassPVMaxSumPt2", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_TauInvMassPVMaxSumPt2", val);
        default: return false;
      }
    }
    case PV_MIN_A0: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_TauConstMassPVMinA0", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_TauInvMassPVMinA0", val);
        default: return false;
      }
    }
    case PV_MIN_Z0: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_TauConstMassPVMinZ0", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_TauInvMassPVMinZ0", val);
        default: return false;
      }
    }
    case PV_MIN_Z0_BA: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_TauConstMassPVMinZ0BA", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_TauInvMassPVMinZ0BA", val);
        default: return false;
      }
    }
    default: return false;
  }  
  
}
/*****************************************************************************/
bool xAOD::BPhysHypoHelper::setTauErr(const float val, const pv_type vertexType, const tau_type tauType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_TauErrConstMassPVMaxSumPt2", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_TauErrInvMassPVMaxSumPt2", val);
        default: return false;
      }
    }
    case PV_MIN_A0: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_TauErrConstMassPVMinA0", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_TauErrInvMassPVMinA0", val);
        default: return false;
      }
    }
    case PV_MIN_Z0: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_TauErrConstMassPVMinZ0", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_TauErrInvMassPVMinZ0", val);
        default: return false;
      }
    }
    case PV_MIN_Z0_BA: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_TauErrConstMassPVMinZ0BA", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_TauErrInvMassPVMinZ0BA", val);
        default: return false;
      }
    }
    default: return false;
  }    
}

//3dTauValues
/*****************************************************************************/
float xAOD::BPhysHypoHelper::tau3d(const pv_type vertexType, const tau_type tauType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_Tau3dConstMassPVMaxSumPt2");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_Tau3dInvMassPVMaxSumPt2");
        default: return -9999999.;
      }
    }
    case PV_MIN_A0: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_Tau3dConstMassPVMinA0");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_Tau3dInvMassPVMinA0");
        default: return -9999999.;
      }
    }
    case PV_MIN_Z0: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_Tau3dConstMassPVMinZ0");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_Tau3dInvMassPVMinZ0");
        default: return -9999999.;
      }
    }
    case PV_MIN_Z0_BA: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_Tau3dConstMassPVMinZ0BA");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_Tau3dInvMassPVMinZ0BA");
        default: return -9999999.;
      }
    }
    default: return -9999999.;
  }  
}
/*****************************************************************************/              
float xAOD::BPhysHypoHelper::tau3dErr(const pv_type vertexType, const tau_type tauType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_Tau3dErrConstMassPVMaxSumPt2");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_Tau3dErrInvMassPVMaxSumPt2");
        default: return -9999999.;
      }
    }
    case PV_MIN_A0: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_Tau3dErrConstMassPVMinA0");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_Tau3dErrInvMassPVMinA0");
        default: return -9999999.;
      }
    }
    case PV_MIN_Z0: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_Tau3dErrConstMassPVMinZ0");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_Tau3dErrInvMassPVMinZ0");
        default: return -9999999.;
      }
    }
    case PV_MIN_Z0_BA: {
      switch(tauType) {
        case TAU_CONST_MASS : GET_FLOAT(m_hypo+"_Tau3dErrConstMassPVMinZ0BA");
        case TAU_INV_MASS   : GET_FLOAT(m_hypo+"_Tau3dErrInvMassPVMinZ0BA");
        default: return -9999999.;
      }
    }
    default: return -9999999.;
  }    
}
/*****************************************************************************/
bool xAOD::BPhysHypoHelper::setTau3d(const float val, const pv_type vertexType, const tau_type tauType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_Tau3dConstMassPVMaxSumPt2", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_Tau3dInvMassPVMaxSumPt2", val);
        default: return false;
      }
    }
    case PV_MIN_A0: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_Tau3dConstMassPVMinA0", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_Tau3dInvMassPVMinA0", val);
        default: return false;
      }
    }
    case PV_MIN_Z0: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_Tau3dConstMassPVMinZ0", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_Tau3dInvMassPVMinZ0", val);
        default: return false;
      }
    }
    case PV_MIN_Z0_BA: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_Tau3dConstMassPVMinZ0BA", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_Tau3dInvMassPVMinZ0BA", val);
        default: return false;
      }
    }
    default: return false;
  }  
  
}
/*****************************************************************************/
bool xAOD::BPhysHypoHelper::setTau3dErr(const float val, const pv_type vertexType, const tau_type tauType)
{
  switch(vertexType) {
    case PV_MAX_SUM_PT2: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_Tau3dErrConstMassPVMaxSumPt2", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_Tau3dErrInvMassPVMaxSumPt2", val);
        default: return false;
      }
    }
    case PV_MIN_A0: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_Tau3dErrConstMassPVMinA0", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_Tau3dErrInvMassPVMinA0", val);
        default: return false;
      }
    }
    case PV_MIN_Z0: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_Tau3dErrConstMassPVMinZ0", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_Tau3dErrInvMassPVMinZ0", val);
        default: return false;
      }
    }
    case PV_MIN_Z0_BA: {
      switch(tauType) {
        case TAU_CONST_MASS : SET_FLOAT(m_hypo+"_Tau3dErrConstMassPVMinZ0BA", val);
        case TAU_INV_MASS   : SET_FLOAT(m_hypo+"_Tau3dErrInvMassPVMinZ0BA", val);
        default: return false;
      }
    }
    default: return false;
  }    
}
//End of 3dTau



/*****************************************************************************/
bool xAOD::BPhysHypoHelper::pass() const 
{
  SG::AuxElement::Accessor<Char_t> flagAcc("passed_"+m_hypo);
  return flagAcc.isAvailable(*m_b) && flagAcc(*m_b) != 0;
}
/*****************************************************************************/
bool xAOD::BPhysHypoHelper::setPass(bool passVal)  
{
  SG::AuxElement::Decorator<Char_t> flagDec("passed_"+m_hypo);
  flagDec(*m_b) = passVal;        
  return true;
}
/*****************************************************************************/
