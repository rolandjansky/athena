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


#include "QatDataAnalysis/MultiHistFileManager.h"
#include "QatDataAnalysis/HistogramManager.h"
#include "QatDataAnalysis/IODriver.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <climits>

MultiHistFileManager::MultiHistFileManager()
{
}

MultiHistFileManager::~MultiHistFileManager(){
  for (unsigned int i=0; i<_outputFileManager.size(); i++) {
    _outputDrivers[i]->write(_outputFileManager[i]);
    _outputDrivers[i]->close(_outputFileManager[i]);
    delete _outputFileManager[i];
  }
  for (unsigned int i=0; i<_inputFileManager.size(); i++) {
    _inputDrivers[i]->close(_inputFileManager[i]);
    delete _inputFileManager[i];
  }
} 

int MultiHistFileManager::getNumInputManagers() {
  return _inputFileManager.size();
}

const HistogramManager *MultiHistFileManager::getInputFileManager(int i) const {
  return _inputFileManager[i];
}

std::string   MultiHistFileManager::getInputFileName(int i) const {
  return _inputFileName[i];
}

MultiHistFileManager::FileFormat  MultiHistFileManager::getInputFileFormat(int i) const {
  return _inputFileFormat[i];
}

int MultiHistFileManager::getNumOutputManagers() {
  return _outputFileManager.size();
}

HistogramManager *MultiHistFileManager::getOutputFileManager(int i) const {
  return _outputFileManager[i];
}

std::string   MultiHistFileManager::getOutputFileName(int i) const {
  return _outputFileName[i];
}

MultiHistFileManager::FileFormat  MultiHistFileManager::getOutputFileFormat(int i) const {
  return _outputFileFormat[i];
}

MultiHistFileManager::Status MultiHistFileManager::addInputFileManager (std::string name) {

  const IODriver *pvsdriver = _loader.ioDriver("PVSDriver");
  if (pvsdriver) {
    const HistogramManager *manager = pvsdriver->openManager(name);
    if (manager) {
      _inputFileManager.push_back(manager);
      _inputFileFormat.push_back(NATIVE);
      _inputFileName.push_back(name);
      _inputDrivers.push_back(pvsdriver);
      return NORMAL;
    }
  }
  const IODriver *rootdriver = _loader.ioDriver("RootDriver");
  if (rootdriver) {
    const HistogramManager *manager = rootdriver->openManager(name);
    if (manager) {
      _inputFileManager.push_back(manager);
      _inputFileFormat.push_back(ROOT);
      _inputFileName.push_back(name);
      _inputDrivers.push_back(rootdriver);
      return NORMAL;
    }
  }
  return ERROR;
}
  
 

MultiHistFileManager::Status MultiHistFileManager::addOutputFileManager (std::string  name) {

   if (name.find(".pvs")!= name.npos) {
    const IODriver *pvsdriver = _loader.ioDriver("PVSDriver");
    if (pvsdriver) {
      HistogramManager *manager = pvsdriver->newManager(name);
      if (manager) {
	_outputFileManager.push_back(manager);
	_outputFileFormat.push_back(NATIVE);
	_outputFileName.push_back(name);
	_outputDrivers.push_back(pvsdriver);
	return NORMAL;
      }
    }
  }
  else {
    const IODriver *rootdriver = _loader.ioDriver("RootDriver");
    if (rootdriver) {
      HistogramManager *manager = rootdriver->newManager(name);
      if (manager) {
	_outputFileManager.push_back(manager);
	_outputFileFormat.push_back(ROOT);
	_outputFileName.push_back(name);
	_outputDrivers.push_back(rootdriver);
	return NORMAL;
      }
    }
  }
  return ERROR;
}
  
