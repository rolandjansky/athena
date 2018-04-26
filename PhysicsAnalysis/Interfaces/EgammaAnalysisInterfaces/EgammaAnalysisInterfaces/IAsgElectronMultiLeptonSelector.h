/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAANALYSISINTERFACES__IASGELECTRONMULTILEPTONSELECTOR__
#define EGAMMAANALYSISINTERFACES__IASGELECTRONMULTILEPTONSELECTOR__

/**
   @class IAsgElectronMultiLeptonSelector
   @brief Interface to tool to select electrons

   12-MAR-2014, convert to ASG tool

*/
// Include the interfaces
#include "PATCore/IAsgSelectionTool.h"
// Forward declarations
namespace Root{
  class TAccept;
}
#include "xAODEgamma/ElectronFwd.h"

class IAsgElectronMultiLeptonSelector : virtual public IAsgSelectionTool
{

  ASG_TOOL_INTERFACE(IAsgElectronMultiLeptonSelector)

public:

  /**Virtual Destructor*/
  virtual ~IAsgElectronMultiLeptonSelector() {};

  /** The main accept method: using the generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle* part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron* part ) const = 0;
  /** The main accept method: using the generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle& part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const = 0;

  /** Method to get the operating point */
  virtual std::string getOperatingPointName( ) const=0;

}; // End: class definition

#endif

