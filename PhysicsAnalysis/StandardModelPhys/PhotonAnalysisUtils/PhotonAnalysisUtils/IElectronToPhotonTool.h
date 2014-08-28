//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IELECTRONTOPHOTONTOOL_H
#define IELECTRONTOPHOTONTOOL_H

#include <string>

// Gaudi
#include "GaudiKernel/IAlgTool.h"

class egamma;
namespace Analysis{
  class Photon;
}

static const InterfaceID IID_IElectronToPhotonTool("IElectronToPhotonTool", 1, 0);

class IElectronToPhotonTool : virtual public IAlgTool {
  
public:

  /** Virtual destructor */
  virtual ~IElectronToPhotonTool(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_IElectronToPhotonTool; }
 
  // public methods:
  virtual Analysis::Photon* ConvertElectronToPhoton(const egamma* eg, bool conv, const std::string& egDetailContainerName) = 0;
};

#endif // HIGGSANALYSYSUTILS_IHGGPHOTONSELECTOR_H
