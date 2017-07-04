/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "dqm_algorithms/CountsBinsGreaterThan.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TClass.h>
#include <TH1.h>
#include <TAxis.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"
#include "ers/ers.h"


static dqm_algorithms::CountsBinsGreaterThan staticInstance;


namespace dqm_algorithms {

  // *********************************************************************
  // Public Methods
  // *********************************************************************

  CountsBinsGreaterThan::
  CountsBinsGreaterThan()
    : m_name("AlgCountsBinsGreaterThan")
  {
    dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
  }


  CountsBinsGreaterThan::
  ~CountsBinsGreaterThan()
  {
  }


  dqm_core::Algorithm*
  CountsBinsGreaterThan::
  clone()
  {
    return new CountsBinsGreaterThan(*this);
  }


  dqm_core::Result*
  CountsBinsGreaterThan::
  execute( const std::string& name, const TObject& object, const dqm_core::AlgorithmConfig& config)
  {
    TH1 * histogram;

    if( object.IsA()->InheritsFrom( "TH1" ) ) {
      histogram = (TH1*)&object;
      if (histogram->GetDimension() > 2 ){
	throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
      }
    } else {
      throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
    }

    int binStart, binEnd;
    int CountsTh;
    double gmin;
    double rmin;
    try {
      binStart  = (int) dqm_algorithms::tools::GetFirstFromMap("BinStart", config.getParameters() );
      binEnd    = (int) dqm_algorithms::tools::GetFirstFromMap("BinEnd"  , config.getParameters() );
      CountsTh  = (int) dqm_algorithms::tools::GetFirstFromMap("CountsTh", config.getParameters() );
      rmin      = dqm_algorithms::tools::GetFromMap( "NEntries", config.getRedThresholds());
      gmin      = dqm_algorithms::tools::GetFromMap( "NEntries", config.getGreenThresholds() );
    }
    catch ( dqm_core::Exception & ex ) {
      throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
    }


    double count = 0;
  
    int nbiny = histogram -> GetNbinsY(); 
    if(binStart>binEnd) {
      binStart= 1;
      binEnd  = histogram -> GetNbinsX();
    }
    int binSize = binEnd-binStart+1;

    std::vector<double> contents;
    contents.resize(binSize);
    for(int i=0;i<binSize;i++) {
      int binx = binStart+i;
      contents[i]=0;
      for ( int j = 1; j <= nbiny; ++j ) {
	double content= histogram -> GetBinContent(binx,j);
	contents[i]+=content;
	if(content>CountsTh)count++;
      }
    }
  

    ERS_DEBUG(1,"Number of entries for bins is "<< count );
    ERS_DEBUG(1,"Green: "<< gmin << " entries;   Red: " << rmin << " entries ");

    dqm_core::Result* result = new dqm_core::Result();
  

  
    if ( count >= gmin ) {
      result->status_ = dqm_core::Result::Green;
    } else if ( count > rmin ) {
      result->status_ = dqm_core::Result::Yellow;
    } else {
      result->status_ = dqm_core::Result::Red;
    }
    result->tags_["BinsAboveTh"] = count;
   

    return result;
  }


  void
  CountsBinsGreaterThan::
  printDescription(std::ostream& out)
  {
    std::string message;
    message += "\n";
    message += "Algorithm: \"" + m_name + "\"\n";
    message += "Description: Counts the number of listed bins with entries above CountsTh\n";
    message += "Parameters:  BinStart first bin to be checked (1=first bin)\n";
    message += "             BinEnd   last bin to be checked (nbin=last bin)\n";
    message += "             CountsTh thresholds on bin counts \n";
    message += "             Nentries Red/Green";
    message += "\n";
  
    out << message;
  }

} // namespace dqm_algorithms
