// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FPEControlSvc.h,v 1.2 2008-09-23 22:00:47 binet Exp $

/**
 * @file  AthenaServices/src/FPEControlSvc.h
 * @author scott snyder
 * @date Nov 2007
 * @brief Service to enable or disable floating-point exceptions.
 */

#ifndef ATHENASERVICES_FPECONTROLSVC_H
#define ATHENASERVICES_FPECONTROLSVC_H


#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/StatusCode.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <fenv.h>


/**
 * @brief Service to enable or disable floating-point exceptions.
 *
 * When this service initializes, it will set the floating-point
 * exception mask based on the value of the @c Exceptions property.
 * This is a string array, containing any of the following strings
 * to enable the corresponding exceptions:
 *
 *   inexact
 *   divbyzero
 *   underflow
 *   overflow
 *   invalid
 *
 * In addition, these words can appear with a ! in front to disable them.
 * Later words take precendence over earlier ones.
 *
 * The default is divbyzero overflow invalid.
 *
 * The property can be reassigned at any time to change the current
 * exception mask.
 */
class FPEControlSvc
  : public AthService, public IToolSvc::Observer
{
public:
  /// Standard initialize method.
  virtual StatusCode initialize();

  /// Standard finalize method.
  virtual StatusCode finalize();

  /// Called after each tool has been created.
  virtual void onCreate(const IAlgTool*);


  /**
   * @brief Constructor.
   * @param name The service name.
   * @param svcloc The service locator.
   */
  FPEControlSvc( const std::string& name, ISvcLocator* svcloc );


private:
  /// Set the FPU exception masks from m_enabled and m_disabled.
  void setFPU();

  /// Property specifying the desired exception mask.
  StringArrayProperty m_exceptions;

  /// Tool service.
  ServiceHandle<IToolSvc> m_toolSvc;

  /// The FP environment before we initialize.
  fenv_t m_env;

  /// Flag that we've retrieved the environment.
  bool m_haveEnv;
  
  /// Mask of enabled exceptions.
  int m_enabled;

  /// Mask of disabled exceptions.
  int m_disabled;

  /// boolean to decide to (not) remove the observer in finalize
  bool m_removeInFinalize;
  
  /// flag to decide on rounding mode (for stability tests)
  std::string m_feSetRounding;
  
  /// Property change handler.
  void prophand (Gaudi::Details::PropertyBase& prop);
};

#endif // not ATHENASERVICES_FPECONTROLSVC_H
