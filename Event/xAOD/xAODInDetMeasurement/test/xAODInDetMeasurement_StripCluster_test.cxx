/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODInDetMeasurement/StripClusterContainer.h"
#include "xAODInDetMeasurement/StripClusterAuxContainer.h"

#include "GeoPrimitives/GeoPrimitives.h"

template< typename T >
std::ostream& operator<< ( std::ostream& out,
                           const std::vector< T >& vec ) {

   out << "[";
   for( size_t i = 0; i < vec.size(); ++i ) {
      out << vec[ i ];
      if( i < vec.size() - 1 ) {
         out << ", ";
      }
   }
   out << "]";
   return out;
}

/// Function fill one Strip cluster with information

void fill( xAOD::StripCluster& stripCluster) {

    Identifier::value_type idval(0x20024480ea00000);
    Identifier id(idval);

    Eigen::Matrix<float,1,1> localPosition(0.15);

    Eigen::Matrix<float,1,1> localCovariance;
    localCovariance.setZero();
    localCovariance(0, 0) = 0.012;

    stripCluster.setMeasurement<1>(id, localPosition, localCovariance);

    Eigen::Matrix<float, 3, 1> globalPosition(10, 10, 10);

    std::vector < Identifier > rdoList = { Identifier(Identifier::value_type(0x200921680c00000)),
                                           Identifier(Identifier::value_type(0x298094737200000)),
                                           Identifier(Identifier::value_type(0x24e105292800000)) };

    stripCluster.setRDOlist(rdoList);

    stripCluster.globalPosition() = globalPosition;
    stripCluster.setChannelsInPhi(3);

    uint16_t hitsInThirdTimeBin = 0;
    std::vector < std::bitset<3> > timePatterns = { std::bitset<3>("010"),
                                                    std::bitset<3>("011"),
                                                    std::bitset<3>("010") };
    for (unsigned int iStrip=0; iStrip<timePatterns.size(); iStrip++) {
        if (iStrip < 16) hitsInThirdTimeBin |= (timePatterns.at(iStrip).test(0) << iStrip);
    }
    stripCluster.setHitsInThirdTimeBin(hitsInThirdTimeBin);

    return;
}

void print ( const xAOD::StripCluster& stripCluster) {
    std::cout << " --------- MEASUREMENT BASE ------------ " << std::endl;
    std::cout << "Identifier = " << stripCluster.identifier() << std::endl;
    std::cout << "Local Position = " << stripCluster.localPosition<1>() << std::endl;
    std::cout << "Local Covariance = " << stripCluster.localCovariance<1>() << std::endl;
    std::cout << " ----------STRIP CLUSTER INFO ----------- " << std::endl;
    std::cout << "Global Position = " << stripCluster.globalPosition() << std::endl;
    std::cout << "RDOs = " << stripCluster.rdoList() << std::endl;
    std::cout << "Number of strips = " << stripCluster.channelsInPhi() << std::endl;
    std::cout << "hits In Third Time Bin = " << stripCluster.hitsInThirdTimeBin() << std::endl;

    return;
}

int main() {

    // create the main containers to test:
    xAOD::StripClusterAuxContainer aux;
    xAOD::StripClusterContainer tpc;

    tpc.setStore(&aux);

    // add one strip cluster to the container
    xAOD::StripCluster * str = new xAOD::StripCluster();
    tpc.push_back(str);

    // fill information
    fill(*str);

    //print information
    print(*str);

    return 0;
}
