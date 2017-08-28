/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: BinDump.cxx,v 1.2 2009/02/07 09:37:17 Remi Lafaye
// **********************************************************************

#include "dqm_algorithms/BinDump.h"

#include <cmath>
#include <iostream>
#include <map>
#include <algorithm>

#include <TClass.h>
#include <TH1.h>
#include <TAxis.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/Result.h"
#include "ers/ers.h"


static dqm_algorithms::BinDump staticInstance;


namespace dqm_algorithms {

// *********************************************************************
// Public Methods
// *********************************************************************

BinDump::
BinDump()
  : m_name("BinDump")
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
}


BinDump::
~BinDump()
{
}


dqm_core::Algorithm*
BinDump::
clone()
{
  return new BinDump(*this);
}


dqm_core::Result*
BinDump::
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
  int method;
  double gmin;
  double rmin;
  try {
    binStart  = (int) dqm_algorithms::tools::GetFirstFromMap("BinStart", config.getParameters() );
    binEnd    = (int) dqm_algorithms::tools::GetFirstFromMap("BinEnd", config.getParameters() );
    method    = (int) dqm_algorithms::tools::GetFirstFromMap("Method", config.getParameters() );
    rmin      = dqm_algorithms::tools::GetFromMap( "NEntries", config.getRedThresholds());
    gmin      = dqm_algorithms::tools::GetFromMap( "NEntries", config.getGreenThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }

  if (rmin < gmin) {
    std::swap(rmin, gmin);
  }

  double count = 0;
  int nbinx = histogram -> GetNbinsX(); 
  int nbiny = histogram -> GetNbinsY(); 
  if(binStart>binEnd) {
    binStart= 1;
    binEnd  = histogram -> GetNbinsX();
  }
  int binSize = binEnd-binStart+1;

  std::vector<double> contents(binSize);
  //contents.resize(binSize);
  for(int i=0;i<binSize;i++) {
    int binx = binStart+i;
    contents[i]=0;
    for ( int j = 1; j <= nbiny; ++j ) {
      double content= histogram -> GetBinContent(binx,j);
      contents[i]+=content;
      count+=content;
    }
  }

  ERS_DEBUG(1,"Number of entries for bins is "
<< count );
  ERS_DEBUG(1,"Green: "<< gmin << " entries;   Red: " << rmin << " entries ");


  TAxis *axis = histogram->GetXaxis();
  dqm_core::Result* result = new dqm_core::Result();
  if(axis) {
    for(int i=0;i<binSize;i++) {
      char tag[256];
      const char *label;
      if(binStart+i==0) sprintf(tag,"Underflows");
      else if(binStart+i==nbinx+1) sprintf(tag,"Overflows");
      else {
	label=axis->GetBinLabel(binStart+i);
	// remove space in names to help handy.py
	if(label) {
	  int l=strlen(label);
	  int j=0;
	  
	  char *mylabel=new char[l+1];
	  if(mylabel) {
	    for(int k=0;k<l;k++) 
	      if(label[k]!=' ') mylabel[j++]=label[k];
	    mylabel[j]='\0';
	    if (mylabel[0]!=0) sprintf(tag,"%s",mylabel);
	    else               sprintf(tag,"Bin%d",binStart+i);
	    delete[] mylabel;
	  }
	  else                 sprintf(tag,"Bin%d",binStart+i);
	}
	else                   sprintf(tag,"Bin%d",binStart+i);
      }
      result->tags_[tag] = contents[i];
    }
  } else {
    for(int i=0;i<binSize;i++) {
      char tag[256];
      if(binStart+i==0) sprintf(tag,"Underflows");
      else if(binStart+i==nbinx+1) sprintf(tag,"Overflows");
      else sprintf(tag,"Bin%d",binStart+i);
      result->tags_[tag] = contents[i];
    }
  }

  if(method==0) { // Check on bin sums
    if ( count <= gmin ) {
      result->status_ = dqm_core::Result::Green;
    } else if ( count > rmin ) {
      result->status_ = dqm_core::Result::Red;
    } else {
      result->status_ = dqm_core::Result::Yellow;
    }
    result->tags_["Total"] = count;
  }
  else if(method>0) { // Check on maximum bin
    count = 0;
    for(int i=0;i<binSize;i++) {
      if(contents[i]>count) count = contents[i];
    }
    if ( count <= gmin ) {
      result->status_ = dqm_core::Result::Green;
    } else if ( count > rmin ) {
      result->status_ = dqm_core::Result::Red;
    } else {
      result->status_ = dqm_core::Result::Yellow;
    }
    result->tags_["Highest"] = count;
  }
  else if(method<0) { // Check on minimum bin
    count = 1e30;
    for(int i=0;i<binSize;i++) {
      if(contents[i]<count) count = contents[i];
    }
    if ( count <= gmin ) {
      result->status_ = dqm_core::Result::Green;
    } else if ( count > rmin ) {
      result->status_ = dqm_core::Result::Red;
    } else {
      result->status_ = dqm_core::Result::Yellow;
    }
    result->tags_["Lowest"] = count;
  }

  return result;
}


void
BinDump::
printDescription(std::ostream& out)
{
  std::string message;
  message += "\n";
  message += "Algorithm: \"" + m_name + "\"\n";
  message += "Description: Check the number of entries in listed bins\n";
  message += "Parameters:  BinStart first bin to be checked (0=underflows)\n";
  message += "             BinEnd   last bin to be checked (nbin+1=overflows)\n";
  message += "             Method=0  check is performed on bin sum\n";
  message += "             Method<0  check is performed on lowest bin\n";
  message += "             Method>0  check is performed on highest bin\n";
  message += "             Nentries  Green/Red minimums";
  message += "\n";
  
  out << message;
}

} // namespace dqm_algorithms
