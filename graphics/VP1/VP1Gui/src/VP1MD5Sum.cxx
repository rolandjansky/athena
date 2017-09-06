/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1MD5Sum                         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1MD5Sum.h"
#include <QFileInfo>
#include <QCryptographicHash>
#include <QRegExp>

//____________________________________________________________________
class VP1MD5Sum::Imp {
public:
  static bool fileok(const QString& filename);
  static QByteArray actualMd5Sum( const QString& filename, bool&ok );
};

//____________________________________________________________________
QByteArray VP1MD5Sum::sum(const QString& filename)
{
  if (!Imp::fileok(filename))
    return QByteArray();
  bool ok;
  QByteArray arr = Imp::actualMd5Sum(filename,ok);
  return ok ? arr : QByteArray();
}

//____________________________________________________________________
bool VP1MD5Sum::sumMatches( const QString& filename, const QByteArray& targetmd5sum )
{
  if (targetmd5sum.isEmpty()||!Imp::fileok(filename))
    return false;
  bool ok;
  return targetmd5sum==Imp::actualMd5Sum(filename,ok) && ok;
}

//____________________________________________________________________
bool VP1MD5Sum::sumMatches( const QString& filename, const QString& md5sum )
{
  return sumMatches( filename, sumToByteArray(md5sum) );
}

//____________________________________________________________________
bool VP1MD5Sum::Imp::fileok(const QString& filename)
{
  QFileInfo fi(filename);
  return fi.exists() && fi.isReadable();
}

//____________________________________________________________________
QByteArray VP1MD5Sum::Imp::actualMd5Sum( const QString& filename, bool&ok )
{
  ok = false;
  QFile file(filename);
  QCryptographicHash md5Hash(QCryptographicHash::Md5);

  if (file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
    ok = true;
    while (!file.atEnd())
      md5Hash.addData(file.read(10485760));
  }
  file.close();
  return md5Hash.result();
}

//____________________________________________________________________
QString VP1MD5Sum::sumToString(const QByteArray& ba )
{
  return ba.toHex();
}

//____________________________________________________________________
QByteArray VP1MD5Sum::sumToByteArray(const QString& str )
{
  return QByteArray::fromHex(QByteArray().append(str));
}

//____________________________________________________________________
bool VP1MD5Sum::validMD5Sum(const QString& sum)
{
  if (sum.count()!=32)
    return false;
  return QRegExp("[0-9a-z]*").exactMatch(sum.toLower());
}

//____________________________________________________________________
QString VP1MD5Sum::sumToStandardFormat(const QString& sum)
{
  return sum.simplified().toLower();
}
