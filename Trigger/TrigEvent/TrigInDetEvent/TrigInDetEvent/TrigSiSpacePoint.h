/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSISPACEPOINT_H
#define TRIGSISPACEPOINT_H
#include<utility>
#include "Identifier/Identifier.h"
#include "InDetPrepRawData/SiCluster.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"

class TrigSiSpacePoint : public TrigSiSpacePointBase {

public:

  // Constructor from single cluster using cylindrical co-ordinates, no. errors
  TrigSiSpacePoint ( const InDet::SiCluster* cluster1,
		     const Identifier& elementId, long layer,
		     double r,  double phi,  double z,
		     double dr=0.0, double dphi=0.0, double dz=0.0 ) : 
    TrigSiSpacePointBase(layer, r, phi, z, dr, dz), 
    m_clusters(cluster1, 0),
    m_dphi(dphi),
    m_locT(0.), m_locL(0.),
    m_original_r(0.),
    m_original_phi(0.),
    m_original_x(0.),
    m_original_y(0.),
    m_elementId(elementId) {};

  // Constructor from two clusters using cylindrical co-ordinates, no. errors  
  TrigSiSpacePoint ( const InDet::SiCluster* cluster1,
		     const InDet::SiCluster* cluster2,
		     const Identifier& elementId, long layer,
		     double r,  double phi,  double z,
		     double dr=0.0, double dphi=0.0, double dz=0.0) : 
    TrigSiSpacePointBase(layer, r, phi, z, dr, dz), 
    m_clusters(cluster1, cluster2),
    m_dphi(dphi),
    m_original_r(0.),
    m_original_phi(0.),
    m_original_x(0.),
    m_original_y(0.),
    m_elementId(elementId) {};
    
 // Destructor
 virtual ~TrigSiSpacePoint() {};

 //Setter methods
 void locT(const double locT) {m_locT = locT;}
 void locL(  const double locL  ) {m_locL   = locL;  }
      
 // Methods to return values of data members
 const Identifier& identify() const {return m_elementId;}
 double dphi() const {return m_dphi;}
 double locT() const {return m_locT;}
 double locL() const {return m_locL;}
 double original_r() const {return  m_original_r;}
 double original_phi() const {return  m_original_phi;}
 double original_x() const {return  m_original_x;}
 double original_y() const {return  m_original_y;}


 // Methods to retrieve data members 
 std::pair < const InDet::SiCluster*, const InDet::SiCluster* > clusters() const {
    return m_clusters;
 }

private:

  std::pair<const InDet::SiCluster*, const InDet::SiCluster*> m_clusters;
  double	m_dphi;
  double	m_locT;
  double	m_locL;
  double  m_original_r; 
  double  m_original_phi; 
  double  m_original_x;
  double  m_original_y;

  const Identifier m_elementId;
};

#endif 




