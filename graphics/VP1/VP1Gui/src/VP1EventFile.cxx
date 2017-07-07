/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1EventFile                      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1EventFile.h"
#include <QStringList>

//____________________________________________________________________
class VP1EventFile::Imp {
public:
  Imp() : runNumber(0), eventNumber(0), rawTime(0), valid(false) {}
  Imp(const QString& fn,const QString& sum,
      int r,int e,unsigned t,bool isvalid)
    : fileName(fn), md5Sum(sum),
      runNumber(r), eventNumber(e), rawTime(t), valid(isvalid)
  {}
  QString fileName;
  QString md5Sum;
  int runNumber;
  unsigned long long eventNumber;
  unsigned rawTime;
  bool valid;

  static Imp * initFromFilename(const QString& filename, const QString& md5sum);
};

//____________________________________________________________________
VP1EventFile::VP1EventFile()
 : d(new Imp)
{
}

//____________________________________________________________________
VP1EventFile::Imp * VP1EventFile::Imp::initFromFilename(const QString& filename, const QString& md5sum)
{
  QStringList filenameparts = filename.split('.', QString::SkipEmptyParts );
  if (filenameparts.isEmpty())
    return new Imp;

  //We find the relevant part of the filename the following way:
  //Starting from the back, we find the first part which contains at
  //least two underscores)
  int ieventinfo=-1;
  for (int i=filenameparts.count()-1;i>=0;--i) {
    if (filenameparts.at(i).count('_')>=2) {
      ieventinfo = i;
      break;
    }
  }
  if (ieventinfo==-1)
    return new Imp;

  QStringList eventinfoparts = filenameparts.at(ieventinfo).split('_', QString::SkipEmptyParts );
  if (eventinfoparts.count()<3)
    return new Imp;
  bool ok;
  int time = eventinfoparts.at(eventinfoparts.count()-1).toInt(&ok);
  //  if (!ok||time<1175378400/*april 1. 2007*/||time>2058991200/*april 1 2035*/)
  if (!ok||time>2058991200/*april 1 2035*/)//No lower bound sanity check, since the events sometimes have faulty ~1970 timestamps!
    return new Imp;

  unsigned long long eventnumber = eventinfoparts.at(eventinfoparts.count()-2).toInt(&ok);
  if (!ok)
    return new Imp;
  int runnumber = eventinfoparts.at(eventinfoparts.count()-3).toInt(&ok);
  if (!ok)
    return new Imp;
  return new Imp(filename,md5sum,runnumber,eventnumber,time,true);
}

//____________________________________________________________________
VP1EventFile::VP1EventFile(const QString& filename, const QString& md5sum)
  : d(Imp::initFromFilename(filename,md5sum))
{
}

//____________________________________________________________________
VP1EventFile::VP1EventFile(const QString& filename,const QString& md5sum,
		int runnumber, unsigned long long eventnumber,unsigned time,bool isvalid)
  : d(new Imp(filename,md5sum,runnumber,eventnumber,time,isvalid))
{
}

//____________________________________________________________________
VP1EventFile::~VP1EventFile()
{
  delete d;
}

//____________________________________________________________________
bool VP1EventFile::operator<( const VP1EventFile & other ) const
{
  //newer (larger time, run and evt numbers) means "smaller".
  if (d->rawTime!=other.d->rawTime) return d->rawTime>other.d->rawTime;
  if (d->runNumber!=other.d->runNumber) return d->runNumber>other.d->runNumber;
  if (d->eventNumber!=other.d->eventNumber) return d->eventNumber>other.d->eventNumber;
  if (d->fileName!=other.d->fileName) return d->fileName<other.d->fileName;
  if (d->valid!=other.d->valid) return d->valid;
  return d->md5Sum<other.d->md5Sum;
}

//____________________________________________________________________
bool VP1EventFile::operator==(const VP1EventFile & other ) const
{
  return d->rawTime==other.d->rawTime
    && d->eventNumber==other.d->eventNumber
    && d->md5Sum==other.d->md5Sum
    && d->fileName==other.d->fileName
    && d->runNumber==other.d->runNumber
    && d->valid==other.d->valid;
}

//____________________________________________________________________
bool VP1EventFile::operator!=(const VP1EventFile & other ) const
{
  return !(*this==other);
}

//____________________________________________________________________
VP1EventFile::VP1EventFile( const VP1EventFile & other )
 : d(new Imp)
{
  *this = other;
}

//____________________________________________________________________
VP1EventFile & VP1EventFile::operator= ( const VP1EventFile & other )
{
  d->fileName = other.d->fileName;
  d->md5Sum = other.d->md5Sum;
  d->runNumber = other.d->runNumber;
  d->eventNumber = other.d->eventNumber;
  d->rawTime = other.d->rawTime;
  d->valid = other.d->valid;
  return *this;
}

//____________________________________________________________________
bool VP1EventFile::isValid() const
{
  return d->valid;
}

//____________________________________________________________________
QString VP1EventFile::fileName() const
{
  return d->fileName;
}

//____________________________________________________________________
QString VP1EventFile::md5Sum() const
{
  return d->md5Sum;
}

//____________________________________________________________________
int VP1EventFile::runNumber() const
{
  return d->runNumber;
}

//____________________________________________________________________
unsigned long long VP1EventFile::eventNumber() const
{
  return d->eventNumber;
}

//____________________________________________________________________
unsigned VP1EventFile::rawTime() const
{
  return d->rawTime;
}

//____________________________________________________________________
QDateTime VP1EventFile::time() const
{
  return QDateTime::fromTime_t(d->rawTime);
}

//____________________________________________________________________
QString VP1EventFile::print() const
{
  if (!isValid())
    return "[invalid]";
  return fileName()
    +", run="+QString::number(runNumber())
    +", evt="+QString::number(eventNumber())
    +", time="+time().toString()
    +", md5="+md5Sum();
}
