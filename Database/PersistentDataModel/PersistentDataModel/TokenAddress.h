/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODEL_TOKENADDRESS_H
#define PERSISTENTDATAMODEL_TOKENADDRESS_H

/** @file TokenAddress.h
 *  @brief This file contains the class definition for the TokenAddress class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

// Framework include files
#include "GaudiKernel/GenericAddress.h"

#include "PersistentDataModel/Token.h"

/** @class TokenAddress
 *  @brief This class provides a Generic Transient Address for POOL tokens.
 **/
class TokenAddress : public GenericAddress {

private:
   /// No copy allowed: put prototype to disable bit-wise copy.
   TokenAddress(const TokenAddress& rhs);
   /// No assignment allowed: put prototype to disable bit-wise assignment.
   TokenAddress& operator=(const TokenAddress& rhs);

public:
   /// Dummy constructor
   TokenAddress() : GenericAddress(), m_token(0), m_par() {}

   /// Standard Constructor
   TokenAddress(long svc,
	   const CLID& clid,
	   const std::string& p1 = "",
	   const std::string& p2 = "",
	   unsigned long ip = 0,
	   Token* pt = 0) : GenericAddress(svc, clid, p1, p2, ip), m_token(pt), m_par() {
     setPar();
   }
   TokenAddress(const GenericAddress& genAddr, Token* pt = 0) : GenericAddress(genAddr), m_token(pt), m_par() {
     setPar();
   }

   virtual ~TokenAddress() { delete m_token; m_token = 0; }

   Token* getToken() { return m_token; }
   const Token* getToken() const { return m_token; }
   void setToken(Token* token) {
      delete m_token;
      m_token = token;
      setPar();
   }
   const std::string* par() const {
      return m_par;
   }

private:
   void setPar() {
      m_par[0] = this->GenericAddress::par()[0];
      m_par[1] = this->GenericAddress::par()[1];
      m_par[2] = this->GenericAddress::par()[2];
      if (m_par[0].empty() && m_token) {
         m_par[0] = m_token->toString();
      }
   }
   Token* m_token;
   std::string m_par[3];
};

#endif
