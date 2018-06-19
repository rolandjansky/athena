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
  for (unsigned int i=0; i<m_outputFileManager.size(); i++) {
    m_outputDrivers[i]->write(m_outputFileManager[i]);
    m_outputDrivers[i]->close(m_outputFileManager[i]);
    delete m_outputFileManager[i];
  }
  for (unsigned int i=0; i<m_inputFileManager.size(); i++) {
    m_inputDrivers[i]->close(m_inputFileManager[i]);
    delete m_inputFileManager[i];
  }
} 

int MultiHistFileManager::getNumInputManagers() {
  return m_inputFileManager.size();
}

const HistogramManager *MultiHistFileManager::getInputFileManager(int i) const {
  return m_inputFileManager[i];
}

std::string   MultiHistFileManager::getInputFileName(int i) const {
  return m_inputFileName[i];
}

MultiHistFileManager::FileFormat  MultiHistFileManager::getInputFileFormat(int i) const {
  return m_inputFileFormat[i];
}

int MultiHistFileManager::getNumOutputManagers() {
  return m_outputFileManager.size();
}

HistogramManager *MultiHistFileManager::getOutputFileManager(int i) const {
  return m_outputFileManager[i];
}

std::string   MultiHistFileManager::getOutputFileName(int i) const {
  return m_outputFileName[i];
}

MultiHistFileManager::FileFormat  MultiHistFileManager::getOutputFileFormat(int i) const {
  return m_outputFileFormat[i];
}

MultiHistFileManager::Status MultiHistFileManager::addInputFileManager (std::string name) {

  const IODriver *pvsdriver = m_loader.ioDriver("PVSDriver");
  if (pvsdriver) {
    const HistogramManager *manager = pvsdriver->openManager(name);
    if (manager) {
      m_inputFileManager.push_back(manager);
      m_inputFileFormat.push_back(NATIVE);
      m_inputFileName.push_back(name);
      m_inputDrivers.push_back(pvsdriver);
      return NORMAL;
    }
  }
  const IODriver *rootdriver = m_loader.ioDriver("RootDriver");
  if (rootdriver) {
    const HistogramManager *manager = rootdriver->openManager(name);
    if (manager) {
      m_inputFileManager.push_back(manager);
      m_inputFileFormat.push_back(ROOT);
      m_inputFileName.push_back(name);
      m_inputDrivers.push_back(rootdriver);
      return NORMAL;
    }
  }
  return ERROR;
}
  
 

MultiHistFileManager::Status MultiHistFileManager::addOutputFileManager (std::string  name) {

   if (name.find(".pvs")!= name.npos) {
    const IODriver *pvsdriver = m_loader.ioDriver("PVSDriver");
    if (pvsdriver) {
      HistogramManager *manager = pvsdriver->newManager(name);
      if (manager) {
	m_outputFileManager.push_back(manager);
	m_outputFileFormat.push_back(NATIVE);
	m_outputFileName.push_back(name);
	m_outputDrivers.push_back(pvsdriver);
	return NORMAL;
      }
    }
  }
  else {
    const IODriver *rootdriver = m_loader.ioDriver("RootDriver");
    if (rootdriver) {
      HistogramManager *manager = rootdriver->newManager(name);
      if (manager) {
	m_outputFileManager.push_back(manager);
	m_outputFileFormat.push_back(ROOT);
	m_outputFileName.push_back(name);
	m_outputDrivers.push_back(rootdriver);
	return NORMAL;
      }
    }
  }
  return ERROR;
}
  
