/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __IASGELECTRONLIKELIHOODSELECTOR__
#define __IASGELECTRONLIKELIHOODSELECTOR__

/**
   @class IAsgElectronLikelihoodTool
   @brief Interface to tool to select electrons

   12-MAR-2014, convert to ASG tool

*/

// CONSIDER MERGING THIS WITH IASGELECTRONMULTILEPTONSELECTOR, SINCE THE INTERFACE
// IS EXACTLY THE SAME. LEAVING THEM SEPERATE FOR NOW IN CASE THEY DIVERGE

// Include the interfaces
#include "PATCore/IAsgSelectionTool.h"

// Include the return object and the underlying ROOT tool
#include "PATCore/TAccept.h"
#include "PATCore/TResult.h"

// Forward declarations
#include "xAODEgamma/ElectronFwd.h"

class IAsgElectronLikelihoodTool : virtual public IAsgSelectionTool
{

  ASG_TOOL_INTERFACE(IAsgElectronLikelihoodTool)

  public:

  /**Virtual Destructor*/
  virtual ~IAsgElectronLikelihoodTool() {};

  /** The main accept method: using the generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle* part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron* part ) const = 0;

  /** The main result method: the actual likelihood is calculated here */
  virtual const Root::TResult& calculate( const xAOD::IParticle* part ) const = 0;

  /** The main result method: the actual likelihood is calculated here */
  virtual const Root::TResult& calculate( const xAOD::Electron* eg ) const = 0;

  /** Method to get the plain TResult */
  virtual const Root::TResult& getTResult( ) const=0;

  //virtual const Root::TAccept& getTAccept( ) const =0; // in base

}; // End: class definition


#endif

