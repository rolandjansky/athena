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

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QFileInfoList>
#include <QThread>

#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <thread>         // std::this_thread::sleep_for // C++11
#include <chrono>         // std::chrono::seconds // C++11

VP1FileUtilities::VP1FileUtilities(const std::string& inputDirectory,
				   unsigned int fileLimit, const std::string& outputDir, bool forceMakeOutputDir, bool removeInputFile):
  m_inputDirectory(inputDirectory),
  m_fileLimit(fileLimit),
  m_outputDirectory(outputDir),
  m_forceMakeOutputDir(forceMakeOutputDir),
  m_removeInputFile(removeInputFile)
{
  // Check if the input directory exists and is writable
  QFileInfo inpDir(m_inputDirectory.c_str());
  if(!inpDir.exists()||!inpDir.isDir()||!inpDir.isReadable()||!inpDir.isWritable()) {
    std::string errMessage = std::string("VP1FileUtilities: ERROR!! The directory ") + inputDirectory + std::string(" either does not exist or is not writable");
    throw std::runtime_error(errMessage.c_str());
  }

  // Check if the output directory exists and is writable
  if (m_outputDirectory != "") {
	  QFileInfo inpDir(m_outputDirectory.c_str());
	  if(!inpDir.exists()||!inpDir.isDir()||!inpDir.isReadable()||!inpDir.isWritable()) {
		  std::string errMessage = std::string("VP1FileUtilities: ERROR!! The directory ") + m_outputDirectory + std::string(" does not exist.");
		  if (m_forceMakeOutputDir) {
			  errMessage += "\nforceMakeOutputDir=True --> Creating the output folder now...";
			  QDir().mkdir(m_outputDirectory.c_str());
		  } else {
			  throw std::runtime_error(errMessage.c_str());
		  }
	  }
  }

}

VP1FileUtilities::~VP1FileUtilities()
{
}

void VP1FileUtilities::produceNewFile(const std::string& sourceFile,
				      int runNumber,
				      int eventNumber,
				      int timeStamp,
                      std::string textLabel)
{
  // Check if the sourceFile exists
  QString srcName(sourceFile.c_str());
  QFile srcFile(srcName);
  if(!srcFile.exists())
    throw std::runtime_error("Source file does not exist!");

  QString inpDirName(m_inputDirectory.c_str());
  QString outDirName(m_outputDirectory.c_str());

  // Construct new filename
  QString newFileName = inpDirName;
  std::ostringstream newFileStr;

  // if input directory name is empty don't add / to the file name
  // also take into account a possibility of trailing slash in directory name
  if(m_inputDirectory!="" && m_inputDirectory.rfind("/")!=m_inputDirectory.size()-1)
    newFileStr << "/";

  QString latestEventFileName = inpDirName + QString(newFileStr.str().c_str()) + QString("latest_vp1event");

  if (textLabel != "" ) {
    newFileStr << "vp1_r" << runNumber << "_ev" << eventNumber << "_u" << timeStamp << "_t" << textLabel << ".pool.root";
  } else {
    newFileStr << "vp1_r" << runNumber << "_ev" << eventNumber << "_u" << timeStamp << ".pool.root";
  }

  newFileName += QString(newFileStr.str().c_str());


  // adding the output folder, if not empty
  if (outDirName != "")
	  newFileName = outDirName + QDir::separator() + newFileName;

  // do copy
  std::cout << "copying '" <<  (srcFile.fileName()).toStdString() << "' to: '" << newFileName.toStdString() << "'..." << std::endl;


    		  // get size of the file to be copied
  qint64 inputSize = srcFile.size();
    		  std::cout << "Size of the input file to be copied: " << inputSize << std::endl;


  if(!srcFile.copy(newFileName))
    throw std::runtime_error("Unable to produce new vp1 event file");



      // the operation to time (for elapsed time)
      char ch; std::cout << '?' && std::cin >> ch ;



      // remove the input file (if not disabled by user)
      if (m_removeInputFile) {
    	  bool copyDone = false;
    	  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now() ;
    	  typedef std::chrono::duration<int,std::milli> millisecs_t ;
    	  while (!copyDone) {

    		  // get handle on new file
    		  QFileInfo checkFile(newFileName);


    		  // check if file exists (and it is a file, and not a directory)
    		  if (checkFile.exists() && checkFile.isFile() && (checkFile.size() == inputSize) )
    		  {
    			  std::cout << "Size of the copied file: " << checkFile.size() << std::endl;
    			  copyDone = true;
    			  if(!srcFile.remove())
    				  std::cerr << "VP1FileUtilities WARNING! Unable to delete " << sourceFile << std::endl;
    		  }
    		  else
    		  {
    			  std::cout << "I could not find the output file, so probably the copy action is not finished yet. I'll wait for a short while and I will retry..." << std::endl;
    			  std::this_thread::sleep_for(std::chrono::milliseconds(500)); //make the program waiting for 0.5 seconds
    			  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now() ;
    			  millisecs_t duration( std::chrono::duration_cast<millisecs_t>(end-start) ) ;
    			  if (duration.count() > 2000.0 )
    			  {
    				  std::cout << "WARNING!!! " << duration.count() << " milliseconds passed and still I cannot find the output file. Probably there was a problem. Giving up with the removal of the source file...\n" ;
    				  copyDone = true;
    			  }
    		  }
    	  }
      }

  // create/update the latest event file
  QFile latestEvent(latestEventFileName);
  if(latestEvent.exists() && !latestEvent.remove())
    throw std::runtime_error("Unable to overwrite the existing latest event file");

  if(!latestEvent.open(QIODevice::WriteOnly | QIODevice::Text))
    throw std::runtime_error("Unable to create new latest event file");
  latestEvent.write(newFileName.toStdString().c_str());
  latestEvent.close();

  // do cleanup if requested. '-1' means 'KEEP ALL FILES'
  if (m_fileLimit != -1) {
	  std::cout << "VP1FileUtilities - cleaning up..." << std::endl;
	  cleanUp();
  }
}

// if the user set a "m_fileLimit" then clean the outputDirectory,
// to only keep this predefined number of output files.
// Please notice: default behaviours is: keep ALL output events.
void VP1FileUtilities::cleanUp()
{
  //std::cout << "VP1FileUtilities::cleanUp()" << std::endl;

  QDir _dir;
  _dir.setPath(QString(m_outputDirectory.c_str()));

  QStringList nameFilters;
  nameFilters << "vp1_*.pool.root";

  _dir.setFilter(QDir::Files);
  _dir.setNameFilters(nameFilters);
  _dir.setSorting(QDir::Time|QDir::Reversed);
  QFileInfoList list = _dir.entryInfoList();

  //std::cout << "m_fileLimit: " << m_fileLimit << " - list.size(): " << list.size() << " - list: " << list << std::endl;

  if(int(list.size()) > m_fileLimit)
  {
    QFileInfo fileInfo = list.at(0);

    if(!_dir.remove(fileInfo.fileName()))
      throw std::runtime_error("VP1FileUtilities::cleanup() - WARNING: Unable to do the clean up!");
  }

  QString poolCatalog("PoolFileCatalog.xml");
  std::string poolCatalogStr = poolCatalog.toStdString();
  std::cout << "looking for " << poolCatalogStr << " in " <<  QDir::currentPath().toStdString() << std::endl;
  if ( QDir::current().entryList().contains( poolCatalog ) ) {
	  std::cout << "VP1FileUtilities::cleanUp() - removing the file '" << poolCatalogStr << "' because it causes problems for subsequent Athena commands opening the copied file." << std::endl;
	  QDir cwd = QDir::currentPath();
	  if ( ! cwd.remove( poolCatalog ) )
		std::cerr << "VP1FileUtilities WARNING! Unable to delete " << poolCatalogStr << std::endl;
  }


  return;
}

bool VP1FileUtilities::fileExistsAndReadable(const std::string& file)
{
  return !file.empty() && QFileInfo(file.c_str()).exists();
}
