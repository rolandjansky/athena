/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODInDetMeasurement/PixelClusterContainer.h"
#include "xAODInDetMeasurement/PixelClusterAuxContainer.h"

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

/// Function fill one Pixel cluster with information

void fill( xAOD::PixelCluster& pixelCluster) {

    Identifier::value_type idval(0x20024480ea00000);
    Identifier id(idval);

    Eigen::Matrix<float,2,1> localPosition(0.1, 0.5);

    Eigen::Matrix<float,2,2> localCovariance;
    localCovariance.setZero();
    localCovariance(0, 0) = 0.012;
    localCovariance(1, 1) = 0.012;

    pixelCluster.setMeasurement<2>(id, localPosition, localCovariance);

    Eigen::Matrix<float, 3, 1> globalPosition(10, 10, 10);

    std::vector < Identifier > rdoList = { Identifier(Identifier::value_type(0x200921680c00000)),
                                           Identifier(Identifier::value_type(0x298094737200000)),
                                           Identifier(Identifier::value_type(0x24e105292800000)),
                                           Identifier(Identifier::value_type(0x298094737200000)),
                                           Identifier(Identifier::value_type(0xaa1cdd0d200000)),
                                           Identifier(Identifier::value_type(0x200921680c00000)) };

    pixelCluster.setRDOlist(rdoList);

    std::vector < int > tots = {1, 2, 3, 4, 5, 6};

    pixelCluster.globalPosition() = globalPosition;
    pixelCluster.setToTlist(tots);
    pixelCluster.setChannelsInPhiEta(3, 2);

    return;
}

void print ( const xAOD::PixelCluster& pixelCluster) {
    std::cout << " --------- MEASUREMENT BASE ------------ " << std::endl;
    std::cout << "Identifier = " << pixelCluster.identifier() << std::endl;
    std::cout << "Local Position = " << pixelCluster.localPosition<2>() << std::endl;
    std::cout << "Local Covariance = " << pixelCluster.localCovariance<2>() << std::endl;
    std::cout << " ----------PIXEL CLUSTER INFO ----------- " << std::endl;
    std::cout << "Global Position = " << pixelCluster.globalPosition() << std::endl;
    std::cout << "RDOs = " << pixelCluster.rdoList() << std::endl;
    std::cout << "ToTs = " << pixelCluster.totList() << " with total ToT = " << pixelCluster.totalToT() << std::endl;
    std::cout << "Dimensions = " << pixelCluster.channelsInPhi() << ", " << pixelCluster.channelsInEta() << std::endl;

    return;
}

int main() {

    // create the main containers to test:
    xAOD::PixelClusterAuxContainer aux;
    xAOD::PixelClusterContainer tpc;

    tpc.setStore(&aux);

    // add one pixel cluster to the container
    xAOD::PixelCluster * pix = new xAOD::PixelCluster();
    tpc.push_back(pix);

    // fill information
    fill(*pix);

    //print information
    print(*pix);

    return 0;
}
