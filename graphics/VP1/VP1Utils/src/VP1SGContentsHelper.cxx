/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1SGContentsHelper               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Base/IVP1System.h"
#include <stdexcept>
#include <QtCore/QStringList>


/////////////////////////////////////////////////////////////////////////
// In order to be able to provide list of possible CLID's, we need to  //
// use the following hack when including the StoreGateSvc header:      //
#define private public
#include "StoreGate/StoreGateSvc.h"                                    //
#undef private
//                                                                     //
/////////////////////////////////////////////////////////////////////////

//____________________________________________________________________
VP1SGContentsHelper::VP1SGContentsHelper( IVP1System * sys, const bool detStore )
  : VP1HelperClassBase(sys,"VP1SGContentsHelper"), m_sg(sys?(detStore?sys->detectorStore():sys->storeGate()):0)
{
  if (!sys)
    message("ERROR: Received null system pointer (won't be able to get storeGate pointer either)");
  else if (!m_sg)
    message("ERROR: Could not get "+QString(detStore?"storeGate":"detectorStore")+" pointer from system");
}

//____________________________________________________________________
VP1SGContentsHelper::VP1SGContentsHelper( StoreGateSvc * sg )
  : VP1HelperClassBase(0,"VP1SGContentsHelper"), m_sg(sg)
{
  if (!sg)
    message("ERROR: Received null storegate pointer");
}

//____________________________________________________________________
bool VP1SGContentsHelper::contains( const CLID& id, const QString& key ) const
{
  if (verbose())
    messageVerbose("contains(..) called for key = "+key);
  if (!m_sg) {
    message("ERROR: Does not have StoreGate pointer - returning false.");
    return false;
  }
  if (key.isEmpty()) {
    message("ERROR: Passed key is empty. Returning false.");
    return false;
  }
  bool contains = false;
  bool exception = true;
  try {
    contains = m_sg->contains(id,key.toStdString());
    exception = false;
  } catch (std::runtime_error e) {
    exception = true;
  }
  if (exception) {
    message("ERROR: Exception thrown during call to StoreGateSvc::contains(..). Returning false.");
    return false;
  }
  if (verbose())
    messageVerbose("contains(..) returning "+QString(contains?"true":"false"));
  return contains;
}

//____________________________________________________________________
QStringList VP1SGContentsHelper::getKeys(const CLID& id) const
{
  messageVerbose("getKeys(..) called");
  if (!m_sg) {
    message("ERROR: Does not have StoreGate pointer - returning empty key list");
    return QStringList();
  }
  QStringList l;

  std::vector<std::string> keys;

  bool exception = true;
  try {
    keys = m_sg->keys(id);
    exception = false;
  } catch (std::runtime_error e) {
    exception = true;
  }
  if (exception) {
    message("ERROR: Exception thrown during call to StoreGateSvc::keys(..)");
    return QStringList();
  }
  std::vector<std::string>::const_iterator it(keys.begin()), itE(keys.end());
  for (;it!=itE;++it) {
    if (it->empty())
      continue;
    if (contains(id,it->c_str()))
      l << it->c_str();
    else if (verbose())
      messageVerbose("NB: StoreGateSvc::keys() included '"+QString(it->c_str())
		     +"' but StoreGateSvc::contains("+QString(it->c_str())+") returns false." );
  }
  if (verbose()) {
    messageVerbose("returning "+QString::number(l.count())+" keys:");
    messageVerbose("      => ",l);
  }
  return l;
}

//____________________________________________________________________
QList<CLID> VP1SGContentsHelper::getPossibleCLIDs() const
{
  messageVerbose("getPossibleCLIDs(..) called");
  QList<CLID> l;
  if (!m_sg) {
    message("ERROR: Does not have StoreGate pointer - returning empty key list");
    return l;
  }

  SG::DataStore *store = m_sg->store();//<-- This is where we use the private->public a hack.
  if (!store) {
    message("ERROR: Found null DataStore pointer - returning empty key list");
    return l;
  }

  SG::DataStore::ConstStoreIterator s_iter, s_end;
  if (!store->tRange(s_iter, s_end).isFailure())
    for (; s_iter != s_end; s_iter++)
      l << s_iter->first;
  return l;
}
