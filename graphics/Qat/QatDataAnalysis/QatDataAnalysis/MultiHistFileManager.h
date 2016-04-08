//---------------------------------------------------------------------------//
//                                                                           //
// This file is part of the Pittsburgh Visualization System (PVS)            //
//                                                                           //
// Copyright (C) 2004 Joe Boudreau, University of Pittsburgh                 //
//                                                                           //
//  This program is free software; you can redistribute it and/or modify     //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation; either version 2 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  This program is distributed in the hope that it will be useful,          //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with this program; if not, write to the Free Software              //
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307     //
// --------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------//
//                                                                                              //
//   MultiHistFileManager...This beast autodectects several formats of HistogramManager files   //
//   and connects them to multiple Histogram Managers.                                          //
//                                                                                              //
//----------------------------------------------------------------------------------------------//


#ifndef _MultiHistFileManager_h_
#define _MultiHistFileManager_h_
#include "QatDataAnalysis/IOLoader.h"
#include <vector>
#include <string>
class HistogramManager;
class IODriver;
class MultiHistFileManager {

 public:

  enum FileFormat {HBOOK,  ROOT, NATIVE};
  enum Status {NORMAL, ERROR};

  MultiHistFileManager();
  virtual ~MultiHistFileManager();

  int getNumInputManagers();
  int getNumOutputManagers();
  
  const HistogramManager *getInputFileManager(int i) const;
  std::string             getInputFileName(int i) const;
  FileFormat              getInputFileFormat(int i) const;

  HistogramManager       *getOutputFileManager(int i) const;
  std::string             getOutputFileName(int i) const;
  FileFormat              getOutputFileFormat(int i) const;

  Status addInputFileManager  (std::string name);
  Status addOutputFileManager (std::string name);
  
 private:

  // Illegal operations:
  MultiHistFileManager & operator=(const MultiHistFileManager &);
  MultiHistFileManager(const MultiHistFileManager &);

  // Member data:
  std::vector<const HistogramManager *> _inputFileManager;
  std::vector<HistogramManager       *> _outputFileManager;
  std::vector<std::string>              _inputFileName,    _outputFileName;
  std::vector<FileFormat>               _inputFileFormat,  _outputFileFormat;
  std::vector<const IODriver *>         _inputDrivers,     _outputDrivers;

  IOLoader                              _loader;
  
  
};

#endif
