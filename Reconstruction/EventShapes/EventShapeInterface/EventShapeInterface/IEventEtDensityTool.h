// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEINTERFACE_IEVENTETDENSITYTOOL_H
#define EVENTSHAPEINTERFACE_IEVENTETDENSITYTOOL_H

#include "EventShapeEvent/EventFeature.h"
#include "EventShapeEvent/EventFeatureStore.h"

#include "EventShapeInterface/IEventFeatureTool.h"

class EventEtDensity;

static const InterfaceID IID_IEventDensityTool("IEventDensityTool",1,0);

class IEventEtDensityTool : virtual public IEventFeatureTool
{
public:

  /*! @brief Interface identifier access */
  static const InterfaceID& interfaceID() { return IID_IEventDensityTool; } 

  /*! @brief Virtual interface destructor */
  virtual ~IEventEtDensityTool() { }

  /*! @brief Interface to access event density */
  virtual double density(const std::string& tag) = 0;
  virtual double density(EventFeature::FeatureType type,
			 const std::string& tag) = 0;
  /*! @brief Interface to access event density as function of a variable 
   *
   *  @parm[in] variable value at which event density is evaluated
   */
  virtual double density(const std::string& tag,double variable) = 0;
  virtual double density(EventFeature::FeatureType type,
			 const std::string& tag,double variable) = 0;
  /*! @brief Interface to access event density with a variable range
   *
   *  @param[in] rangeLow lower limit of density evaluation range
   *  @param[in] rangeHigh upper limit of density evaluation range
   */
  virtual double density(const std::string& tag,
			 double rangeLow,double rangeHigh) = 0;
  virtual double density(EventFeature::FeatureType type,
			 const std::string& tag,
			 double rangeLow,double rangeHigh) = 0;

  /*! @brief Interface to access all event density related data 
   * 
   *  @param[out] data reference to modifiable @c EventEtDensity data object 
   *
   *  @return @c true if valid @c EventEtDensity object filled (data for 
   *          tag and type exists), else @c false . 
   */
  virtual bool getDensity(const std::string& tag,EventEtDensity& data) = 0;
  virtual bool getDensity(EventFeature::FeatureType type,
			  const std::string& tag,EventEtDensity& data) = 0;
  /*! @brief Interface to access all event density data as function of a 
   *         variable 
   *
   *  @parm[in] variable value at which event density is evaluated
   *  @param[out] data reference to modifiable @c EventEtDensity data object 
   *
   *  @return @c true if valid @c EventEtDensity object filled (data for 
   *          tag and type exists), else @c false . 
   */
  virtual bool getDensity(const std::string& tag,
			  double variable,EventEtDensity& data) = 0;
  virtual bool getDensity(EventFeature::FeatureType type,
			  const std::string& tag,
			  double variable,EventEtDensity& data) = 0;
  /*! @brief Interface to access all event density data within a variable range
   *
   *  @param[in] rangeLow lower limit of density evaluation range
   *  @param[in] rangeHigh upper limit of density evaluation range
   *  @param[out] data reference to modifiable @c EventEtDensity data object 
   *
   *  @return @c true if valid @c EventEtDensity object filled (data for 
   *          tag and type exists), else @c false . 
   */
  virtual bool getDensity(const std::string& tag,
			  double rangeLow,double rangeHigh,
			  EventEtDensity& data) = 0; 
  virtual bool getDensity(EventFeature::FeatureType type,
			  const std::string& tag,
			  double rangeLow,double rangeHigh,
			  EventEtDensity& data) = 0; 
};

/*! @class IEventDensityTool
 *
 *  @brief Interface for tools providing event densities.
 * 
 *  Extension of @c IEventFeatureTool interface to accommodate specific 
 *  density information.
 */
#endif
