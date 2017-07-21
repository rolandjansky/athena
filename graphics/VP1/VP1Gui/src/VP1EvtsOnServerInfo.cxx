/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1EvtsOnServerInfo               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1EvtsOnServerInfo.h"
#include "VP1Gui/VP1MD5Sum.h"
#include <QFileInfo>
#include <QTextStream>
#include <map>


//____________________________________________________________________
class VP1EvtsOnServerInfo::Imp {
public:
  Imp(const QString& infofile)
  {
    error = init(infofile);
  }

  QString error;
  QString init(const QString& infofile);

  QList<VP1EventFile> events;//if invalid, we make a list with one dummy
                             //event (to avoid crashes if user forget to
                             //check isValid())
};


//____________________________________________________________________
VP1EvtsOnServerInfo::VP1EvtsOnServerInfo(const QString& infofile)
 : d(new Imp(infofile))
{
}

//____________________________________________________________________
VP1EvtsOnServerInfo::~VP1EvtsOnServerInfo()
{
  delete d;
}

//____________________________________________________________________
bool VP1EvtsOnServerInfo::isValid() const
{
  return d->error.isEmpty();
}

//____________________________________________________________________
QString VP1EvtsOnServerInfo::error() const
{
  return d->error;
}

//____________________________________________________________________
QStringList VP1EvtsOnServerInfo::print() const
{
  QStringList l;
  if (!isValid()) {
    l << "Invalid. Reason: " + error();
    return l;
  }
  l << "Valid with "+QString::number(numberOfEvents())+" files:";
  foreach(VP1EventFile evt, events())
    l << "  "+evt.print();
  l << "Events within 10 minutes of latest:";
  foreach(VP1EventFile evt, events(10*60))
    l << "  "+evt.print();
  l << "Events with same run number as latest:";
  foreach(VP1EventFile evt, events(-1,true))
    l << "  "+evt.print();
  l << "Events with same run number as latest and within 1 minute:";
  foreach(VP1EventFile evt, events(1*60,true))
    l << "  "+evt.print();

  return l;
}

//____________________________________________________________________
QString VP1EvtsOnServerInfo::Imp::init(const QString& infofile)
{
  //Example of file we are trying to parse:
  //
  //begin_copyresult
  //source dir /client/VP1_events
  //dest dir /www/web_files/html/vp1events
  //vp1_69629_5487_1211316972.pool.root copy success
  //end_copyresult
  //begin_checksums
  //vp1_69629_5487_1211316972.pool.root	30fa3c9a05251a4d741fce52c304a341
  //vp1_69629_5270_1211316960.pool.root	7a38564cdd2f2951e76ec37c565dbbf9
  //vp1_69629_3968_1211316900.pool.root	6c716952a7fdb0371b1541a45d0029f8
  //end_checksums
  //
  //The format of the event file names are: vp1_[run#]_[evt#]_[unixtime].pool.root
  //
  //In principle we are only interested in the checksums section, but
  //it might be a good sanity check to see that the last copied file
  //(from copyresult) is also the one with the highest time.

  ///////////////////////////////////////////////////////
  //  Basic check that file exists, is readable, etc.  //
  ///////////////////////////////////////////////////////

  if (infofile.isEmpty())
    return "Given empty path to info file";
  QFileInfo fi(infofile);
  if (!fi.exists())
    return "infofile "+infofile+" does not exist";
  if (!fi.isReadable())
    return "infofile "+infofile+" is not readable";
  if (fi.size()==0)
    return "infofile "+infofile+" is not empty";

  /////////////////////////////////////////////////////////////////////////////
  //  Read file into stringlist, ignoring empty lines and excess whitespace  //
  /////////////////////////////////////////////////////////////////////////////

  int i_begin_copyresult(-1), i_end_copyresult(-1);
  int i_begin_checksums(-1), i_end_checksums(-1);


  QStringList lines;
  QFile file(infofile);
  if (!file.open(QFile::ReadOnly))
    return "Could not open file in readonly mode";

  //Just to avoid any potential performance problems if pointed to a
  //wrong input file:
  const qint64 max_linelength(1000);
  const qint64 max_numberoflines(2000);
  qint64 ilines(0);

  QTextStream stream(&file);
  while (!stream.atEnd()) {

    if (ilines++>max_numberoflines)
      return "Too many lines in file";

    //Read and prune next line:
    QString rawline = stream.readLine(max_linelength).simplified();
    if (rawline.isEmpty())
      continue;

    //Get section boundaries:
    if (rawline=="begin_copyresult") {
      if (i_begin_copyresult!=-1)
	return "Saw two lines with begin_copyresult";
      i_begin_copyresult = lines.count();
    } else if (rawline=="end_copyresult") {
      if (i_end_copyresult!=-1)
	return "Saw two lines with end_copyresult";
      i_end_copyresult = lines.count();
    } else if (rawline=="begin_checksums") {
      if (i_begin_checksums!=-1)
	return "Saw two lines with begin_checksums";
      i_begin_checksums = lines.count();
    } else if (rawline=="end_checksums") {
      if (i_end_checksums!=-1)
	return "Saw two lines with end_checksums";
      i_end_checksums = lines.count();
    }

    //Store line:
    lines << rawline;
  }
  if (lines.isEmpty())
    return "Did not read any lines from file";

  //copy result is only used for a sanity check, so we keep it optional
  const bool hascopyresult(i_begin_copyresult!=-1||i_end_copyresult!=-1);

  if (i_begin_checksums==-1) return "File did not have begin_checksums line";
  if (i_end_checksums==-1) return "File did not have end_checksums line";
  if (i_end_checksums<=i_begin_checksums) return "checksum section delimiters out of order";

  if (hascopyresult) {
    if (i_begin_copyresult==-1) return "File had end_copyresult but no begin_copyresult line";
    if (i_end_copyresult==-1) return "File had begin_copyresult but no end_copyresult line";
    if (i_end_copyresult<=i_begin_copyresult) return "copyresult section delimiters out of order";
    if (i_begin_copyresult>i_begin_checksums&&i_begin_copyresult<i_end_checksums)
      return "copyresult and checksum sections mixed";
    if (i_end_copyresult>i_begin_checksums&&i_end_copyresult<i_end_checksums)
      return "copyresult and checksum sections mixed";
  }

  ///////////////////////////////
  //  Let the parsing begin!!  //
  ///////////////////////////////

  for (int i = i_begin_checksums+1;i < i_end_checksums; ++i) {
    QStringList parts = lines.at(i).split ( ' ', QString::SkipEmptyParts );
    if (parts.count()!=2)
      return "Invalid line in checksums section";
    QString filename(parts.at(0));
    QString md5sum(VP1MD5Sum::sumToStandardFormat(parts.at(1)));
    if (!VP1MD5Sum::validMD5Sum(md5sum))
      return "Invalid md5sum in checksums section: "+md5sum;
    VP1EventFile evt(filename,md5sum);
    if (!evt.isValid())
      return "Failed to decode event file information from filename: "+filename;
    events << evt;
  }

  qSort(events);

  if (hascopyresult) {
    //Todo: Use the copy result lines for a sanity check of that the
    //newest file is also the one copied last (if copy succesful).
  }

  return "";
}

//____________________________________________________________________
unsigned long long VP1EvtsOnServerInfo::numberOfEvents() const
{
  return d->events.count();
}

//____________________________________________________________________
VP1EventFile VP1EvtsOnServerInfo::newestEvent() const
{
  return d->events.isEmpty() ? VP1EventFile() : d->events.at(0);
}

//____________________________________________________________________
QList<VP1EventFile> VP1EvtsOnServerInfo::events(int timecut, bool requireNewestRunNumber ) const
{
  //Notice: Logic here must be similar to logic in VP1AvailEvents::isConsideredFresh

  if ( d->events.isEmpty() )
    return d->events;

  if (timecut==0) {
    QList<VP1EventFile> l;
    l << d->events.at(0);
    return l;
  }
  const unsigned newestRawTime = d->events.at(0).rawTime();
  const int newestRunNumber = d->events.at(0).runNumber();

  const unsigned minTime = (timecut<0&&unsigned(timecut)>newestRawTime) ? 0 : newestRawTime-timecut;

  QList<VP1EventFile> out;
  foreach (VP1EventFile evt, d->events) {
    if (minTime>0&&evt.rawTime()<minTime)
      continue;
    if (requireNewestRunNumber&&evt.runNumber()!=newestRunNumber)
      continue;
    out << evt;
  }
  return out;
}
