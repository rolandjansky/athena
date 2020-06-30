// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODCORE_XAODCORESTLDICT_H
#define XAODCORE_XAODCORESTLDICT_H

// System include(s).
#include <bitset>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODCORESTL {

      // "Single" vector type(s).
      std::vector< char > stl_v1;
      std::vector< signed char > stl_v2;
      std::vector< unsigned char > stl_v3;
      std::vector< uint32_t > stl_v4;

      // "Double" vector type(s).
      std::vector< std::vector< bool > > stl_vv1;
      std::vector< std::vector< char > > stl_vv2;
      std::vector< std::vector< signed char > > stl_vv3;
      std::vector< std::vector< unsigned char > > stl_vv4;
      std::vector< std::vector< int > > stl_vv5;
      std::vector< std::vector< size_t > > stl_vv6;
      std::vector< std::vector< uint16_t > > stl_vv7;
      std::vector< std::vector< uint32_t > > stl_vv8;
      std::vector< std::vector< unsigned long > > stl_vv9;
      std::vector< std::vector< unsigned long long > > stl_vv10;
      std::vector< std::vector< std::string > > stl_vv11;

      // "Triple" vector type(s).
      std::vector< std::vector< std::vector< int > > > stl_vvv1;
      std::vector< std::vector< std::vector< unsigned char > > > stl_vvv2;
      std::vector< std::vector< std::vector< size_t > > > stl_vvv3;
      std::vector< std::vector< std::vector< uint16_t > > > stl_vvv4;
      std::vector< std::vector< std::vector< uint32_t > > > stl_vvv5;
      std::vector< std::vector< std::vector< unsigned long > > > stl_vvv6;
      std::vector< std::vector< std::vector< unsigned long long > > > stl_vvv7;
      std::vector< std::vector< std::vector< float > > >  stl_vvv8;
      std::vector< std::vector< std::vector< std::string > > > stl_vvv9;

      // "Quadruple" vector type(s).
      std::vector< std::vector< std::vector< std::vector< std::string > > > >
         stl_vvvv1;

      // "Special" vector type(s).
      std::vector< std::set< uint32_t > > stl_sv1;
      std::vector< std::vector< std::set< uint32_t > > > stl_sv2;
      std::vector< std::pair< std::string, std::string > > stl_sv3;
      std::vector< std::vector< std::pair< std::string, std::string > > > stl_sv4;
      std::vector< std::vector< std::pair< unsigned int, double > > > stl_sv5;

      // Map type(s).
      std::map< std::string, std::vector< int > > stl_m1;
      std::map< std::string, std::vector< float > > stl_m2;
      std::map< std::string, std::vector< std::vector< int > > > stl_m3;
      std::map< std::string, std::vector< std::vector< float > > > stl_m4;

      // "Other" type(s).
      std::bitset< 3 > stl_o1; // 3 == CaloCluster::NSTATES
      std::bitset< 11 > stl_o2; // 11 == xAOD::NumberOfTrackRecoInfo
      std::set< uint32_t > stl_o3;
   };
}

#endif // XAODCORE_XAODCORESTLDICT_H
