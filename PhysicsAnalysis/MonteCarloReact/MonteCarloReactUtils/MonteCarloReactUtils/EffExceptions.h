/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MONTECARLOREACTUTILS_EFFEXCEPTIONS_H
#define MONTECARLOREACTUTILS_EFFEXCEPTIONS_H
#include <string>

namespace MonteCarloReact {

  class  EffException {

  public:
    EffException(const std::string mess) : m_message(mess) {;}

    std::string printErrorMessage() {return m_message;}
    
  protected:
    std::string m_message;
  };
  
  class EffConfigException : public EffException {
  public:
    EffConfigException(const std::string & mess) : EffException("MonteCarloReactUtils: ConfigException : " + mess) {;}
  };
  
  class EffNotImplemented : public EffException {
  public:
    EffNotImplemented( const std::string & mess="") : EffException("MonteCarloReactUtils: Feature not implemented yet : " +mess) {;}
  };

  class EffRequestFileNotFound : public EffException {
  public:
    EffRequestFileNotFound(const std::string & file_name) : EffException("MonteCarloReactUtils : No request file \"" + 
									 file_name + "\" was found.") {
      m_message += "Verify that all following fields are correctly specified: ";
      m_message += "EffName, ObjType, ObjQuality, EffVarNames ,EffType" ;
	}
  };


  class EffRequestNotFoundInFile : public EffException {
  public:
    EffRequestNotFoundInFile() : 
      EffException("MonteCarloReactUtils : The specified request is not found in file.") {;}
  };


  class EffDuplicateMatchesToRequest : public EffException {
  public:
    EffDuplicateMatchesToRequest() :
      EffException("MonteCarloReactUtils : Ambiguous request - more than one match found in input stream") {;}
  };

  class BinnedEffUnderflow : public EffException {
  public:
    BinnedEffUnderflow(const std::string & mess = "") : EffException( "BinnedEfficiency : underflow in bin lookup" + mess) {;}
  };
  
  class BinnedEffOverflow : public EffException {
  public:
    BinnedEffOverflow( const std::string & mess = "") : EffException( "BinnedEfficiency : overflow in bin lookup" + mess) {;}
  };
  
  class BinnedMalformed : public EffException {
  public:
    BinnedMalformed( const std::string & mess = "" ) : EffException("Malformed BinInfo line :" + mess ) {;}
  };

  class BinnedBadDimension : public EffException {
  public:
    BinnedBadDimension( const std::string & mess = "") : EffException("BinnedEfficiency : Dimension of Eff call does not match dimension of Efficiency : " +mess) {;}
  };

  class ResDuplicateMatchesToRequest : public EffException {
  public:
    ResDuplicateMatchesToRequest() :
      EffException("MonteCarloReact : Ambiguous request - more than one match found in input stream") {;}
  };
  
  
}
#endif // MONTECARLOREACTUTILS_EFFEXCEPTIONS_H
