/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1SGContentsHelper                 //
//                                                            //
//  Description: Helper class for retrieving the available    //
//               keys in storegate for a given type.          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1SGCONTENTSHELPER_H
#define VP1SGCONTENTSHELPER_H

#include "VP1Base/VP1HelperClassBase.h"
#include "CLIDSvc/tools/ClassID_traits.h"
#include <QStringList>
class StoreGateSvc;

class VP1SGContentsHelper : public VP1HelperClassBase {
public:

  VP1SGContentsHelper( IVP1System *, const bool detStore = false );//For messages in gui (detStore=true takes detector rather than event store)
  VP1SGContentsHelper( StoreGateSvc * );
  virtual ~VP1SGContentsHelper() {}

  template <typename T>
  QStringList getKeys() const;
  QStringList getKeys( const CLID& id ) const;

  template <typename T>
  bool contains( const QString& key ) const;
  bool contains( const CLID& id, const QString& key ) const;

  QList<CLID> getPossibleCLIDs() const;

private:

  StoreGateSvc * m_sg;

};


///////////////
//  Inlines  //
///////////////

template <typename T>
inline QStringList VP1SGContentsHelper::getKeys() const
{
  return getKeys(ClassID_traits<T>::ID());
}

template <typename T>
inline bool VP1SGContentsHelper::contains( const QString& key ) const
{
  return contains(ClassID_traits<T>::ID(),key);
}


#endif
