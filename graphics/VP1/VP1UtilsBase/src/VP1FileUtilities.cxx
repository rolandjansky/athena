/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1FileUtilities               //
//                                                         //
//  Author: Vakho Tsulaia <tsulaia@mail.cern.ch>           //
//                                                         //
//  Initial version: October 2007                          //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1UtilsBase/VP1FileUtilities.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QFileInfoList>
#include <QtCore/QThread>

#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

VP1FileUtilities::VP1FileUtilities(const std::string& inputDirectory,
				   unsigned int fileLimit):
  m_inputDirectory(inputDirectory),
  m_fileLimit(fileLimit)
{
  // Check if the input directory exists and is writable
  QFileInfo inpDir(m_inputDirectory.c_str());
  if(!inpDir.exists()||!inpDir.isDir()||!inpDir.isReadable()||!inpDir.isWritable()) {
    std::string errMessage = std::string("The directory ") + inputDirectory + std::string(" either does not exist or is not writable");
    throw std::runtime_error(errMessage.c_str());
  }
}

VP1FileUtilities::~VP1FileUtilities()
{
}

void VP1FileUtilities::produceNewFile(const std::string& sourceFile,
				      int runNumber,
				      int eventNumber,
				      int timeStamp)
{
  // Check if the sourceFile exists
  QString srcName(sourceFile.c_str());
  QFile srcFile(srcName);
  if(!srcFile.exists())
    throw std::runtime_error("Source file does not exist!");

  QString inpDirName(m_inputDirectory.c_str());

  // Construct new filename
  QString newFileName = inpDirName;
  std::ostringstream newFileStr;

  // if input directory name is empty don't add / to the file name
  // also take into account a possibility od trailing slash in directory name
  if(m_inputDirectory!="" && m_inputDirectory.rfind("/")!=m_inputDirectory.size()-1)
    newFileStr << "/";

  QString latestEventFileName = inpDirName + QString(newFileStr.str().c_str()) + QString("latest_vp1event");

  newFileStr << "vp1_" << runNumber << "_" << eventNumber << "_" << timeStamp << ".pool.root";
  newFileName += QString(newFileStr.str().c_str());

  // do copy
  if(!srcFile.copy(newFileName))
    throw std::runtime_error("Unable to produce new vp1 event file");

  // create/update the latest event file
  QFile latestEvent(latestEventFileName);
  if(latestEvent.exists() && !latestEvent.remove())
    throw std::runtime_error("Unable to overwrite the existing latest event file");

  if(!latestEvent.open(QIODevice::WriteOnly | QIODevice::Text))
    throw std::runtime_error("Unable to create new latest event file");
  latestEvent.write(newFileName.toStdString().c_str());
  latestEvent.close();

  // do cleanup if needed
  cleanUp();
}

void VP1FileUtilities::cleanUp()
{
  QDir _dir;
  _dir.setPath(QString(m_inputDirectory.c_str()));

  QStringList nameFilters;
  nameFilters << "vp1_*.pool.root";


  _dir.setFilter(QDir::Files);
  _dir.setNameFilters(nameFilters);
  _dir.setSorting(QDir::Time|QDir::Reversed);
  QFileInfoList list = _dir.entryInfoList();

  if(unsigned(list.size()) > m_fileLimit)
  {
    QFileInfo fileInfo = list.at(0);

    if(!_dir.remove(fileInfo.fileName()))
      throw std::runtime_error("Unable to do the clean up");
  }

  return;
}

bool VP1FileUtilities::fileExistsAndReadable(const std::string& file)
{
  return !file.empty() && QFileInfo(file.c_str()).exists();
}
