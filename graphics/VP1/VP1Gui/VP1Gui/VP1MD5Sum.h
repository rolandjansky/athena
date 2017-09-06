/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1MD5Sum                           //
//                                                            //
//  Description: Calculates md5sum of file without reading    //
//               entire file into memory at once.             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1MD5SUM_H
#define VP1MD5SUM_H

#include <QString>
#include <QByteArray>

class VP1MD5Sum {
public:

  static QByteArray sum( const QString& filename );
  //Returns empty array in case of errors

  static bool sumMatches( const QString& filename, const QByteArray& md5sum );
  static bool sumMatches( const QString& filename, const QString& md5sum );
  //Returns false in case of errors (missing or unreadable file/empty md5sum) or mismatch.

  //We always assume sums in byte arrays to be raw bits, and sums in
  //strings to be in hex format. Here are methods for
  //conversion/sanity checking:
  static QString sumToString(const QByteArray& );
  static QByteArray sumToByteArray(const QString& );
  static bool validMD5Sum(const QString&);//Does string have correct length and contain no invalid characters?
  static QString sumToStandardFormat(const QString&);//Removes spaces converts to lower case.
private:
  class Imp;
  VP1MD5Sum();
  ~VP1MD5Sum();
};

#endif
