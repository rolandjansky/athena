/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSISPACEPOINT_H
#define TRIGSISPACEPOINT_H
#include<utility>
#include "Identifier/Identifier.h"
#include <utility>
#include "InDetPrepRawData/SiCluster.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"

class TrigSiSpacePoint : public TrigSiSpacePointBase {

public:

  // Constructor from single cluster using cylindrical co-ordinates, no. errors
  TrigSiSpacePoint ( const InDet::SiCluster* cluster1,
		     const Identifier& elementId, long layer,
		     double r,  double phi,  double z,
		     double dr=0.0, double dphi=0.0, double dz=0.0 ) : 
    TrigSiSpacePointBase(layer, r, phi, z, dr, dphi, dz), 
    m_clusters(cluster1, 0),
    m_elementId(elementId) {};

  // Constructor from two clusters using cylindrical co-ordinates, no. errors  
  TrigSiSpacePoint ( const InDet::SiCluster* cluster1,
		     const InDet::SiCluster* cluster2,
		     const Identifier& elementId, long layer,
		     double r,  double phi,  double z,
		     double dr=0.0, double dphi=0.0, double dz=0.0) : 
    TrigSiSpacePointBase(layer, r, phi, z, dr, dphi, dz), 
    m_clusters(cluster1, cluster2),
    m_elementId(elementId) {};
    
 // Destructor
 virtual ~TrigSiSpacePoint() {};
      
 // Methods to return values of data members
 const Identifier& identify() const {return m_elementId;}

 // Methods to retrieve data members 
 std::pair < const InDet::SiCluster*, const InDet::SiCluster* > clusters() const {
    return m_clusters;
 }

private:

  std::pair<const InDet::SiCluster*, const InDet::SiCluster*> m_clusters;
  const Identifier m_elementId;
};

#endif 




