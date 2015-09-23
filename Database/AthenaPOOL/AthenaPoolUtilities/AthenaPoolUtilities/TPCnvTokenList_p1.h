/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TPCnvTokenList_p1_h
#define TPCnvTokenList_p1_h

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

/** @author  Marcin.Nowak@cern.ch
 */

/** @class TPCnvToken_p1

    This class servers as a persistent pointer to maintain reference
    between the principal top-level object and its extentions. It
    holds a POOL token and extending TP converter ID (the same one as
    used in @c TPObjRef) 
*/

class TPCnvToken_p1 {
public:
  /** Constructor
      @param [in] cnvID ID of the TP converter for the object
      @param [in] token token referencing the extending object
  */
  TPCnvToken_p1(unsigned short cnvID=0, const std::string& token="")
	: m_converterID(cnvID), m_token(token) {}

  /// Returns the topl level TP converter ID
  unsigned short	converterID() const { return m_converterID; }

  /// Returns token
  const	std::string&	token() const { return m_token; }
  
protected:
  unsigned short	m_converterID;	//!< ID of the top-level TP converter
  std::string		m_token;	//!< token to the extending object
};



/** @class TPCnvTokenList_p1

    vector of TPCnvToken_p1
    Each top-level persistent object that has extentions needs a data
    member of this type.
*/

class TPCnvTokenList_p1
   : public std::vector<TPCnvToken_p1>
{
public: 
  /** Stores a new reference to an extending object
      @param [in] cnvID ID of the TP converter for the object
      @param [in] token token referencing the extending object
  */
  void	addToken( unsigned short cnvID, const std::string& token) {
     push_back( TPCnvToken_p1(cnvID, token) );
  }

  /** Retrieves token of the extending object given the converter ID
      @param [in] id the ID of the extending converter
      @return token to the extending persistent object
  */
  const std::string&	findToken(unsigned short id) {
     for( const_iterator it = begin();  it != end(); it++ ) {
	if( it->converterID() == id )
	   return it->token();
     }
     std::ostringstream error;
     error << "TPCnvTokenList_p1::findToken(): could not find a token for converter ID=" << id << std::ends;
     throw std::runtime_error( error.str() );
  }
};



#endif
