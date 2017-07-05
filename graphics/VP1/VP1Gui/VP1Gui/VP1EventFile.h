/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1EventFile                        //
//                                                            //
//  Description: Class describing single-event files          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1EVENTFILE_H
#define VP1EVENTFILE_H

#include <QString>
#include <QDateTime>

class VP1EventFile {
public:
  //The interesting bits:
  //Event identity:
  int runNumber() const;
  unsigned long long eventNumber() const;
  unsigned rawTime() const;
  QDateTime time() const;

  //Where it is:
  QString fileName() const;//base file name only. No '/' allowed!
  QString md5Sum() const;

  bool isValid() const;//Internally, invalid instances are those with rawTime()==0.
  QString print() const;

  //The boring bits:
  VP1EventFile();//constructs invalid instance.
  ~VP1EventFile();
  bool operator<(const VP1EventFile &) const;//Newer means "smaller".
  bool operator==(const VP1EventFile &) const;
  bool operator!=(const VP1EventFile &) const;
  VP1EventFile(const VP1EventFile & );
  VP1EventFile & operator= (const VP1EventFile & );
  VP1EventFile(const QString& filename,const QString& md5sum,
	       int runnumber, unsigned long long eventnumber,unsigned time, bool isvalid = true );
  VP1EventFile(const QString&, const QString& md5sum = 0);//will attempt to decode from file name (e.g. vp1_234_123_1321334.pool.root). Instance becomes invalid if not succesful.
private:
  class Imp;
  Imp * d;

};

#endif
