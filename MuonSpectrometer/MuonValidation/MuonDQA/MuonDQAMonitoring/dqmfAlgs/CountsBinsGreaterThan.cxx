/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityInterfaces/CountsBinsGreaterThan.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TClass.h>
#include <TH1.h>
#include <TAxis.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
//#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"
#include "ers/ers.h"


static dqi::CountsBinsGreaterThan staticInstance;


namespace dqi {

  // *********************************************************************
  // Public Methods
  // *********************************************************************

  CountsBinsGreaterThan::
  CountsBinsGreaterThan()
    : name("CountsBinsGreaterThan")
  {
    dqm_core::AlgorithmManager::instance().registerAlgorithm( name, this );
  }


  CountsBinsGreaterThan::
  CountsBinsGreaterThan( const CountsBinsGreaterThan& other )
    : dqm_core::Algorithm(other)
    , name(other.name)
  {
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
    int method;
    double gmin;
    double rmin;
    try {
      binStart  = (int) GetFirstFromMap("BinStart", config.getParameters() );
      binEnd    = (int) GetFirstFromMap("BinEnd"  , config.getParameters() );
      CountsTh  = (int) GetFirstFromMap("CountsTh", config.getParameters() );
      rmin      = GetFromMap( "NEntries", config.getRedThresholds());
      gmin      = GetFromMap( "NEntries", config.getGreenThresholds() );
    }
    catch ( dqm_core::Exception & ex ) {
      throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
    }


    double count = 0;
  
    int nbinx = histogram -> GetNbinsX(); 
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
  printDescription()
  {
    std::string message;
    message += "\n";
    message += "Algorithm: \"" + name + "\"\n";
    message += "Description: Counts the number of listed bins with entries above CountsTh\n";
    message += "Parameters:  BinStart first bin to be checked (1=first bin)\n";
    message += "             BinEnd   last bin to be checked (nbin=last bin)\n";
    message += "             CountsTh thresholds on bin counts \n";
    message += "             Nentries Red/Green";
    message += "\n";
  
    std::cout << message;
  }


  // *********************************************************************
  // Protected Methods
  // *********************************************************************

  double CountsBinsGreaterThan::GetFirstFromMap(const std::string & pname, const std::map<std::string,std::vector<double> > & params)
  {
    std::map<std::string,std::vector<double> >::const_iterator it = params.find(pname);
    if ( it != params.end() && it->second.size() ){
      return it->second[0];
    }else {
      throw dqm_core::BadConfig( ERS_HERE, "None", pname );
    }
  }

  std::vector<double> CountsBinsGreaterThan::GetVectorFromMap(const std::string & pname, const std::map<std::string,std::vector<double> > & params)
  {
    std::map<std::string,std::vector<double> >::const_iterator it = params.find(pname);
    if ( it != params.end() && it->second.size() ){
      return it->second;
    }else {
      throw dqm_core::BadConfig( ERS_HERE, "None", pname );
    }
  }

  double CountsBinsGreaterThan::GetFromMap( const std::string & pname, const std::map<std::string,double> & params )
  {
    std::map<std::string,double>::const_iterator it = params.find( pname );
    if ( it != params.end() ){
      return it->second;
    }else {
      throw dqm_core::BadConfig( ERS_HERE, "None", pname );
    }
  }

} // namespace dqi
