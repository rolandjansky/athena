// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODDATASOURCE_MAKEDATAFRAME_H
#define XAODDATASOURCE_MAKEDATAFRAME_H

// ROOT include(s).

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RStringView.hxx>

// System include(s).
#include <string>
#include <vector>

namespace xAOD {

   /// Helper function for creating an xAOD reading data frame
   ROOT::RDataFrame
   MakeDataFrame( std::string_view fileNameGlob,
                  std::string_view treeName = "CollectionTree",
                  bool verboseOutput = false );
   /// Helper function for creating an xAOD reading data frame
   ROOT::RDataFrame
   MakeDataFrame( const std::vector< std::string >& fileNames,
                  std::string_view treeName = "CollectionTree",
                  bool verboseOutput = false );

} // namespace xAOD

#endif // XAODDATASOURCE_MAKEDATAFRAME_H
