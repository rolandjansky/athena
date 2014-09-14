/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : ScalerSvc, IScalerFactory, ScalerFactory
 *
 * @brief : The ScalerSvc is responsible for all scaler engines.
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: ScalerSvc.h,v 1.5 2008-10-21 06:44:24 tbold Exp $
 **********************************************************************************/

#ifndef TrigSteering_ScalerSvc_h
#define TrigSteering_ScalerSvc_h

#include "AthenaBaseComps/AthService.h"
#include "TrigSteering/IScalerSvc.h"

#include "TrigSteering/Scaler.h"

class StatusCode;

namespace HLT {

  class IScalerFactory;
  template<class S> class ScalerFactory;

  /**
     @class ScalerSvc
     The ScalerSvc is responsible for all scaler engines.

     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
  */

  class ScalerSvc : virtual public IScalerSvc, public AthService
  {

  public:

    ScalerSvc( const std::string& name, ISvcLocator* sl ); //!< Gaudi constructor

    virtual ~ScalerSvc() { } //!< virtual destructor

    StatusCode initialize(); //!< Gaudi initialize
    StatusCode start();      //!< Gaudi start (called online for new run start, offline once)
    StatusCode finalize();   //!< Gaudi finalize

    /** @brief Retrieve a new Scaler of the requested type - we keep ownership of the object
	@param type specifying the engine to retrieve
	@return pointer to engine
     */
    IScaler* get( const std::string& type = "" ) const;

    /** @brief register a new typestring/factory pair for non-reusable scaler engine types.
               takes ownership of the factory pointer
    */
    template<class S> static bool registerType( const std::string& type,
						ScalerFactory<S>* factory )
      { return registerType( factory, type ); }


    static bool registerType( const std::string& type ) { return registerType( 0, type ); } //!< register a reusable scaler type

    StatusCode queryInterface( const InterfaceID& riid, void** ppvIF ); //!< Gaudi interface

  private:

    static bool registerType( IScalerFactory* factory, const std::string& type );
    static std::map< std::string, IScalerFactory* > m_factories;
    
    std::string m_defaultType; //!<  default Type for create()
    mutable std::vector< IScaler* > m_scalers;  //!< Vector of scalers to delete at the end

    std::map< std::string, IReusableScaler* > m_reusables;
    ServiceHandle<IToolSvc> m_toolSvc;
  };


  /** @class IScalerFactory
      Abstract untyped factory class, to interface with the manager
  */
  class IScalerFactory {

  public:
    virtual Scaler* create() = 0; //!< abstract create method for factory
    virtual ~IScalerFactory(){}
  };

  /** @class ScalerFactory
      Templated implementation that know how to create new scaler objects
  */
  template<class S> class ScalerFactory : virtual public IScalerFactory {

  public:
    ScalerFactory() { }
    virtual ~ScalerFactory() { }

    Scaler* create() { return new S(); }
  };
}

#endif
