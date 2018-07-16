// Dear emacs, this is -*-c++-*-

#ifndef __IASGFORWARDELECTRONLIKELIHOODSELECTOR__
#define __IASGFORWARDELECTRONLIKELIHOODSELECTOR__

/**
   @class IAsgForwardElectronLikelihoodTool
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
#include "xAODEgamma/EgammaFwd.h"

class IAsgForwardElectronLikelihoodTool : virtual public IAsgSelectionTool
{

  ASG_TOOL_INTERFACE(IAsgForwardElectronLikelihoodTool)

  public:

  /**Virtual Destructor*/
  virtual ~IAsgForwardElectronLikelihoodTool() {};


  /** The main accept method: using the generic interface , with pointers*/
  virtual const Root::TAccept& accept( const xAOD::IParticle* part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron* part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Egamma* part ) const = 0;

  /** The main accept method: in case mu not in EventInfo online */
  virtual const Root::TAccept& accept( const xAOD::Electron* part, double mu ) const = 0;

  /** The main accept method: in case mu not in EventInfo online */
  virtual const Root::TAccept& accept( const xAOD::Egamma* part, double mu ) const = 0;


  /** The main accept method: using the generic interface, with reference */
  virtual const Root::TAccept& accept( const xAOD::IParticle& part) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Egamma& part ) const = 0;

  /** The main accept method: in case mu not in EventInfo online */
  virtual const Root::TAccept& accept( const xAOD::Electron& part, double mu ) const = 0;

  /** The main accept method: in case mu not in EventInfo online */
  virtual const Root::TAccept& accept( const xAOD::Egamma& part, double mu ) const = 0;


  /** The main result method: the actual likelihood is calculated here */
  virtual const Root::TResult& calculate( const xAOD::IParticle* part , int set  ) const = 0;

  /** The main result method: the actual likelihood is calculated here */
  virtual const Root::TResult& calculate( const xAOD::Electron* eg , int set  ) const = 0;

  /** The main result method: the actual likelihood is calculated here */
  virtual const Root::TResult& calculate( const xAOD::Egamma* eg , int set  ) const = 0;

  /** The main result method: in case mu not in EventInfo online */
  virtual const Root::TResult& calculate( const xAOD::Electron* eg, double mu , int set  ) const = 0;

  /** The main result method: in case mu not in EventInfo online */
  virtual const Root::TResult& calculate( const xAOD::Egamma* eg, double mu , int set  ) const = 0;


  /** Method to get the plain TResult */
  virtual const Root::TResult& getTResult( ) const=0;

  /** Method to get the operating point */
  virtual std::string getOperatingPointName( ) const =0;

  //virtual const Root::TAccept& getTAccept( ) const =0; // in base

}; // End: class definition


#endif

