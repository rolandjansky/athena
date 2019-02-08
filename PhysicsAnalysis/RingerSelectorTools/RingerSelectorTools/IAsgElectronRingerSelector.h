/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IAsgElectronRingerSelector.h 704615 2015-10-29 18:50:12Z wsfreund $
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
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"

// Forward declarations:
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronFwd.h"

class IAsgElectronIsEMSelector;

class IAsgElectronRingerSelector : virtual public IAsgSelectionTool
{

  ASG_TOOL_INTERFACE(IAsgElectronRingerSelector)

  public:

  /** Virtual Destructor */
  virtual ~IAsgElectronRingerSelector() {};

  /** The main accept method: using the generic interface */
  virtual asg::AcceptData accept( const xAOD::IParticle* part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual asg::AcceptData accept( const xAOD::Electron* part ) const = 0;

  /** The main accept method: using the generic interface */
  virtual asg::AcceptData accept( const xAOD::IParticle& part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual asg::AcceptData accept( const xAOD::Electron& part ) const = 0;

  /** The basic Ringer selector tool execution */
  virtual StatusCode execute(const xAOD::Electron* eg,
                             asg::AcceptData& acceptData) const = 0;

  /** The Ringer selector tool execution for HLT */
  virtual StatusCode execute(const xAOD::Egamma* eg,
                             asg::AcceptData& acceptData) const = 0;

  /** Get last executed TAccept answer */
  virtual const asg::AcceptInfo& getAcceptInfo() const = 0;

  /** Get last executed TResult value */
  virtual const std::vector<float>& getOutputSpace() const = 0;

  /**
   * @brief Set the discrimination configuration file
   **/
  virtual void setDiscrFile( const std::string path ) = 0;

  /**
   * @brief Set the threshold configuration file
   **/
  virtual void setThresFile( const std::string path ) = 0;

  /**
   * @brief Set the threshold configuration file
   **/
  virtual void setCutMask( const unsigned int cutMask ) = 0;

  /**
   * @brief Set the CutIDSelector to be used
   **/
  virtual void setCutIDSelector( IAsgElectronIsEMSelector *cutID ) = 0;

  /**
   * @brief Set the RingSetConfContainer (MetaData) key
   **/
  virtual void setRSMetaName( const std::string name ) = 0;

};


#endif // RINGERSELECTORTOOLS_IASGELECTRONRINGERSELECTOR_H
