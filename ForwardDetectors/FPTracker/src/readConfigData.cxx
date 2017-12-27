/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/readConfigData.h"
#include "FPTracker/ConfigData.h"
#include "FPTracker/SimpleLogger.h"
#include "boost/lexical_cast.hpp"
#include "boost/tokenizer.hpp"
#include <fstream>
#include <string>
#include <sstream>


namespace FPTracker{
    

  class ValueSetter{
  public:
    ValueSetter():m_errors(false){}
    
    template<class T>
    bool setValue(T& val, const std::string& s)
    {
      bool ok = true;
      try
	{
	  val = boost::lexical_cast<T>(s);
	}
      catch (boost::bad_lexical_cast& )
	{
	  m_errors = true;
	  ok      = false;

	  std::ostringstream ost;
	  ost<<"error converting "<< s <<'\n';
	  SimpleLogger::getLogger()->info(ost);

	  m_notConverted.push_back(s);
	}
      
      return ok;
    }
    
    bool errors() const {return m_errors;}
    
    std::vector<std::string>::const_iterator begin() const
    {
      return m_notConverted.begin();
    }

    
    std::vector<std::string>::const_iterator end() const
    {
      return m_notConverted.end();
    }

  private:
    bool m_errors;
    std::vector< std::string > m_notConverted;
  };
      

  bool readConfigData(boost::shared_ptr< std::ifstream> p_mydata, ConfigData& configData){
    // ConfigData readConfigData(std::string& dir){
    // Read data cards from Data.txt file 

    
    std::ifstream& in = *p_mydata;
    std::string    line;
    ValueSetter    vs;

    boost::char_separator<char> sep(" \f\t\v");
    typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;

    
    std::ostringstream ost;
    while( std::getline(in, line) )
      {
	Tokenizer tok(line, sep);
	std::vector< std::string > tokens(tok.begin(), tok.end());

	if ( tokens.size() > 1 )
	  {
	    ost<<tokens[0]<<" "<<tokens[1]<<'\n';
	    std::string& dataname = tokens[0];
	    std::string& svalue   = tokens[1];
	    if(dataname == "IP"  ) vs.setValue(configData.IP,         svalue);
	    if(dataname == "APER") vs.setValue(configData.useaper,    svalue);
	    if(dataname == "MBAP") vs.setValue(configData.apermb,     svalue);
	    if(dataname == "COL1") vs.setValue(configData.xcol1,      svalue);
	    if(dataname == "COL2") vs.setValue(configData.xcol2,      svalue);
	    if(dataname == "ZMAX") vs.setValue(configData.absZMagMax, svalue);
	    if(dataname == "ENDM") vs.setValue(configData.endMarker,  svalue);

	    if(dataname == "PBEA") {
	      float pbeam;
	      if ( vs.setValue(pbeam, svalue) ){configData.setpbeam(pbeam);}
	    }
	    

	  }
	
      }

    SimpleLogger::getLogger()->info( ost );
    
    return !vs.errors();
  }
}
