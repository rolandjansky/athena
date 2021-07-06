/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  Implementation file of a DbToken class
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//====================================================================
#include "StorageSvc/DbToken.h"
#include "CxxUtils/checker_macros.h"
#include <cstdio>
using namespace pool;

void genMD5(const std::string& s, void* code);
static const int KEY_MASK = (~0x0)&0x00;

/// Produce the token keys only on demand for export
void DbToken::makeKey(const Token* tok, int which, Guid& guid)  {
  char text[32];
  switch(which)   {
  case TOKEN_CONT_KEY:
    std::sprintf(text, "][TECH=%08X]", tok->technology()&KEY_MASK);
    genMD5("[DB="+tok->dbID().toString()+"][CNT="+tok->contID()+"][CLID="+tok->classID().toString()+text, &guid);
    break;
  case TOKEN_FULL_KEY:
    genMD5(tok->toString(), &guid);
    break;
  default:
    break;
  }
}

/// Produce the keys only on demand for THIS token
void DbToken::setKey(int which) {
  if ( 0==(type()&which) )  {
    char text[32];
    std::string s;
    switch(which)   {
    case TOKEN_CONT_KEY:
      std::sprintf(text, "][TECH=%08X]", technology()&KEY_MASK);
      s = "[DB="+dbID().toString()+"][CNT="+contID()+"][CLID="+classID().toString()+text;
      genMD5(s, &m_contKey);
      //m_type |= TOKEN_CONT_KEY;
      setType(type()|TOKEN_CONT_KEY);
      break;
    case TOKEN_FULL_KEY:
      s = toString();
      genMD5(s, &m_keyFull);
      //m_type |= TOKEN_FULL_KEY;
      setType(type()|TOKEN_FULL_KEY);
      break;
    default:
      break;
    }
  }
}

/// Copy the key 
void DbToken::setKeyEx(int which, const Guid& guid)   {
  switch(which)   {
  case TOKEN_CONT_KEY:
    m_contKey = guid;
    //m_type |= TOKEN_CONT_KEY;
    setType(type()|TOKEN_CONT_KEY);
    break;
  case TOKEN_FULL_KEY:
    m_keyFull = guid;
    //m_type |= TOKEN_FULL_KEY;
    setType(type()|TOKEN_FULL_KEY);
    break;
  default:
    break;
  }
}

/// Set a user key
void DbToken::setUserKey(int i, const Guid& guid) { 
  while ( m_ukeys.size()-1 <= size_t(i) ) {
    m_ukeys.push_back(Guid(true));
  }
  m_ukeys[i] = guid;
}

/// Fast token comparison
bool DbToken::equal(const Token& refTok) const    {
  const DbToken* dbTok = static_cast<const DbToken*>(&refTok);
  if ( this == dbTok )  {
    return true;
  }
  else if ( (type()&TOKEN_FULL_KEY) && (refTok.type()&TOKEN_FULL_KEY) )  {
    return m_keyFull == dbTok->m_keyFull;
  }
  return Token::equal(refTok);
}

/// Fast token comparison
bool DbToken::less(const Token& refTok) const    {
  const DbToken* dbTok = static_cast<const DbToken*>(&refTok);
  if ( this == dbTok )  {
    return false;
  }
  else if ( (type()&TOKEN_FULL_KEY) && (refTok.type()&TOKEN_FULL_KEY) )  {
    return m_keyFull < dbTok->m_keyFull;
  }
  return Token::less(refTok);
}
