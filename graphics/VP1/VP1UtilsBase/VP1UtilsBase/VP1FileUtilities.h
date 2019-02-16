/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1FileUtilities                 //
//                                                         //
//  Author: Vakho Tsulaia <tsulaia@mail.cern.ch>           //
//                                                         //
//  Initial version: October 2007                          //
//                                                         //
//  Updates:                                               //
//      Riccardo Maria BIANCHI <rbianchi@cern.ch>          //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1FILEUTILITIES_H
#define VP1FILEUTILITIES_H

//NB: There should never be any Qt (or Athena of course) includes in this file!!!
#include <string>

class VP1FileUtilities
{
public:
  //
  // Parameters for the constructor
  //
  // 1. inputDirectory. The directory where vp1 event files are created
  //    by Writer algorithm.
  //
  // 2. fileLimit.
  //    * Writing: maximum number of files to keep in 'inputDirectory'
  //    * Reading: maximum number of files to keep in 'copyDirectory' (see below)
  VP1FileUtilities(const std::string& inputDirectory,
		   unsigned int fileLimit, const std::string& outputDir = "", bool forceMakeOutputDir = false, bool removeInputFile = true);
  virtual ~VP1FileUtilities();

  //
  //  **** Write method ****
  //
  // This method takes the 'sourceFile' (pool.root file with single event) from run directory
  // and copies it over to the 'inputDirectory' with the name:
  //
  //          vp1_runNumber_eventNumber_timeStamp.pool.root
  //
  // After that, if the total number of files in the 'inputDirectory' is > fileLimit then the oldest
  // vp1 event file is deleted.
  void produceNewFile(const std::string& sourceFile,
		      int runNumber,
		      int eventNumber,
		      int timeStamp,
              std::string textLabel = "");

  // **** Check if file exists ****
  static bool fileExistsAndReadable(const std::string&);

private:
  // Delete the oldest vp1 events file in either inputDirectory (inputDir = True) or copyDirectory
  void cleanUp();

  std::string m_inputDirectory;
  int m_fileLimit;
  std::string m_outputDirectory;
  bool m_forceMakeOutputDir;
  bool m_removeInputFile;
};

#endif
