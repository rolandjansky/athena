/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1AvailEvtsLocalDir              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1AvailEvtsLocalDir.h"
#include "VP1LocalEvtRetriever.h"

#include <QStringList>

//____________________________________________________________________
class VP1AvailEvtsLocalDir::Imp {
public:
  Imp(const QString& sd) : sourcedir(sd), retriever(0) {}
  QString sourcedir;
  static unsigned ntmpdlcount;
  QStringList availablesourcedirs;
  VP1LocalEvtRetriever* retriever;
};

unsigned VP1AvailEvtsLocalDir::Imp::ntmpdlcount = 0;

//____________________________________________________________________
VP1AvailEvtsLocalDir::VP1AvailEvtsLocalDir(int timeCutForNew,
					   QString sourcedir,
					   QString tmpcopydir,
					   int maxLocalFilesToKeep,
					   QObject * parent )
  : VP1AvailEvents(timeCutForNew,tmpcopydir,maxLocalFilesToKeep,parent),
    m_d(new Imp(sourcedir+(sourcedir.endsWith("/")?"":"/")))
{
}


//____________________________________________________________________
void VP1AvailEvtsLocalDir::init()
{
  m_d->retriever = new VP1LocalEvtRetriever(this,m_d->sourcedir); // Memleak
  m_d->retriever->start();
}

//____________________________________________________________________
VP1AvailEvtsLocalDir::~VP1AvailEvtsLocalDir()
{
  delete m_d;
}

//____________________________________________________________________
QString VP1AvailEvtsLocalDir::currentSourceDir() const
{
  return m_d->sourcedir;
}

//____________________________________________________________________
void VP1AvailEvtsLocalDir::setSourceDir(QString dir)
{
  m_d->retriever->setSourceDir(dir);
  m_d->sourcedir = dir;
}

//____________________________________________________________________
void VP1AvailEvtsLocalDir::setAvailableSourceDirectories(QStringList l)
{
  m_d->availablesourcedirs = l;
}


//____________________________________________________________________
QStringList VP1AvailEvtsLocalDir::availableSourceDirectories() const
{
  return m_d->availablesourcedirs;
}
