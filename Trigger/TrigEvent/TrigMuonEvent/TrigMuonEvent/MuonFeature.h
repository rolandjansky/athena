// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_MUON_FEATURE_H
#define TRIGMUONEVENT_MUON_FEATURE_H

// STL include(s):
#include <string>
#include <map>

// Gaudi/Athena include(s):
#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"

// Forward declaration(s):
class MsgStream;

class MuonFeature {

public:
   enum AlgoId{GEV900ID=0, MUONID=1, HALOID=2, NULLID=99999};

   MuonFeature();
   // keep this for backward compatibility
   MuonFeature(int saddress, float pt, float radius, float eta, 
               float phi, float dir_phi, float zeta, float dir_zeta, float beta);
   // new constructor
   MuonFeature(int saddress, float pt, float radius, float eta, 
               float phi, float dir_phi, float zeta, float dir_zeta, float beta,
	       float sp1_r, float sp1_z, float sp1_slope, 
	       float sp2_r, float sp2_z, float sp2_slope, 
	       float sp3_r, float sp3_z, float sp3_slope, 
	       float br_radius, float br_sagitta,
	       float ec_alpha, float ec_beta,
	       double dq_var1, double dq_var2); 
  
   MuonFeature(const MuonFeature* muon_feature);
   MuonFeature(const MuonFeature& muon_feature);

   MuonFeature& operator=(const MuonFeature& muon_feature);

   AlgoId algoId(void)     const {return m_algoId;}
   int    roiId(void)      const {return m_RoIId;}
   int    saddress(void)   const {return m_saddress;}
   float  pt(void)         const {return m_pt;}
   float  radius(void)     const {return m_radius;}
   float  eta(void)        const {return m_eta;}
   float  phi(void)        const {return m_phi;}
   float  dir_phi(void)    const {return m_dir_phi;}
   float  zeta(void)       const {return m_zeta;}
   float  dir_zeta(void)   const {return m_dir_zeta;}
   float  beta(void)       const {return m_beta;}
   float  sp1_r(void)      const {return m_sp1_r; }
   float  sp1_z(void)      const {return m_sp1_z; }
   float  sp1_slope(void)  const {return m_sp1_slope; }
   float  sp2_r(void)      const {return m_sp2_r; }
   float  sp2_z(void)      const {return m_sp2_z; }
   float  sp2_slope(void)  const {return m_sp2_slope; }
   float  sp3_r(void)      const {return m_sp3_r; }
   float  sp3_z(void)      const {return m_sp3_z; }
   float  sp3_slope(void)  const {return m_sp3_slope; }
   float  br_radius(void)  const {return m_br_radius; }
   float  br_sagitta(void) const {return m_br_sagitta; }
   float  ec_alpha(void)   const {return m_ec_alpha; }
   float  ec_beta(void)    const {return m_ec_beta; }
   double dq_var1(void)    const {return m_dq_var1; }
   double dq_var2(void)    const {return m_dq_var2; }
  
   void set_algoId(AlgoId algoId) {m_algoId = algoId;}
   void set_roiId(int roiId)      {m_RoIId = roiId;}
   void set_sp1(float r, float z, float slope) {m_sp1_r=r; m_sp1_z=z; m_sp1_slope=slope;}
   void set_sp2(float r, float z, float slope) {m_sp2_r=r; m_sp2_z=z; m_sp2_slope=slope;}
   void set_sp3(float r, float z, float slope) {m_sp3_r=r; m_sp3_z=z; m_sp3_slope=slope;}
   void set_br(float radius, float sagitta)    {m_br_radius=radius; m_br_sagitta=sagitta;}
   void set_ec(float alpha,  float beta)       {m_ec_alpha =alpha ; m_ec_beta   =beta;}
   void set_dq(double var1, double var2)       {m_dq_var1  =var1  ; m_dq_var2   =var2;}

private:
   AlgoId m_algoId;
   int    m_RoIId;
   int    m_saddress;
   float  m_pt;
   float  m_radius;
   float  m_eta;
   float  m_phi;
   float  m_dir_phi;
   float  m_zeta;
   float  m_dir_zeta;
   float  m_beta;
   float  m_sp1_r;
   float  m_sp1_z;
   float  m_sp1_slope;
   float  m_sp2_r;
   float  m_sp2_z;
   float  m_sp2_slope;
   float  m_sp3_r;
   float  m_sp3_z;
   float  m_sp3_slope;
   float  m_br_radius;
   float  m_br_sagitta;
   float  m_ec_alpha;
   float  m_ec_beta;
   double m_dq_var1;
   double m_dq_var2;

};

/// Helper function for printing the object
std::string str( const MuonFeature& muon );
/// Helper operator for printing the object
MsgStream& operator<< ( MsgStream& m, const MuonFeature& muon );

/// Operator comparing two MuonFeature objects for equality
bool operator== ( const MuonFeature& left, const MuonFeature& right );
/// Operator comparing two MuonFeature objects for inequality
inline bool operator!= ( const MuonFeature& left, const MuonFeature& right ) {
   return !( left == right );
}

/// Comparison with feedback
void diff( const MuonFeature& left, const MuonFeature& right,
           std::map< std::string, double >& varChange );

CLASS_DEF(MuonFeature, 1500, 1)
CLASS_DEF(DataVector<MuonFeature>, 227312873, 1)

#include "TrigMuonEvent/MuonFeatureContainer.h"

#endif // TRIGMUONEVENT_MUON_FEATURE_H
