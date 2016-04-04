/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILS_H
#define UTILS_H
//#include <map>
#include <vector>
#include <string>

#include "AsgTools/ToolStore.h"

namespace Utils {

  double DeltaR(const double eta1, const double phi1, const double eta2, const double phi2);
  double DeltaR_Square(const double eta1, const double phi1, const double eta2, const double phi2);
  double L1_DeltaR_Square(const double eta1, const double phi1, const double eta2, const double phi2);
  double L1_DeltaPhi(const double phi1, const double phi2);
  double L1_DeltaEta(const double eta1, const double eta2);
  double DeltaPhi(const double phi1, const double phi2);
  double DeltaEta(const double eta1, const double eta2);
  std::vector<std::string> splitNames(const std::string& files, const std::string & sep = ",");
  
  inline double dR(const double eta1, const double phi1, const double eta2, const double phi2) {
      return DeltaR(eta1, phi1, eta2, phi2);
  }
  
// Athena has an old version of ASG Tools, so alias the function to something
#ifdef ASGTOOL_STANDALONE
  template< typename T>
      static bool toolStoreContains( const std::string& name ) {
          return asg::ToolStore::contains<T>(name);
      }
#else
  template< typename T >
      static bool toolStoreContains( const std::string& name ) {
          return ( dynamic_cast< T* >( asg::ToolStore::get( name ) ) != 0 );
      }
#endif

  //  std::map<std::string, int> getName(const std::string & name);
}

#endif
