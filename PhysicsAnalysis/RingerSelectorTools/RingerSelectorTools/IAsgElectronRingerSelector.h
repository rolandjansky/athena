/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IAsgElectronRingerSelector.h 668868 2015-05-20 20:26:18Z wsfreund $
#ifndef RINGERSELECTORTOOLS_IASGELECTRONRINGERSELECTOR_H
#define RINGERSELECTORTOOLS_IASGELECTRONRINGERSELECTOR_H

/**
 * @class IAsgElectronRingerSelector
 * @brief Interface to select electrons using Ringer information
 *
 * @author Werner Freund <wsfreund@cern.ch>
 * @date   Nov 2014
 *
 **/

// STL includes
#include<vector>

// Include the interfaces
#include "PATCore/IAsgSelectionTool.h"

// Include the return object and the underlying ROOT tool
#include "PATCore/TAccept.h"

// Forward declarations:
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronFwd.h"

class IAsgElectronRingerSelector : virtual public IAsgSelectionTool
{

  ASG_TOOL_INTERFACE(IAsgElectronRingerSelector)
  
  public:

  /** Virtual Destructor */
  virtual ~IAsgElectronRingerSelector() {};

  /** The main accept method: using the generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle* part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron* part ) const = 0;

  /** The main accept method: using the generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle& part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const = 0;
  
  /** The basic Ringer selector tool execution */
  virtual StatusCode execute(const xAOD::Electron* eg) const = 0;

  /** The Ringer selector tool execution for HLT */
  virtual StatusCode execute(const xAOD::Egamma* eg) const = 0;

  /** Get last executed TAccept answer */
  virtual const Root::TAccept& getTAccept() const = 0;
  
  /** Get last executed TResult value */
  virtual const std::vector<float>& getOutputSpace() const = 0;

};


#endif // RINGERSELECTORTOOLS_IASGELECTRONRINGERSELECTOR_H
