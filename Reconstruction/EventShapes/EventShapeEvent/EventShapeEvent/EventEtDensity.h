// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEEVENT_EVENTETDENSITY_H
#define EVENTSHAPEEVENT_EVENTETDENSITY_H

#include "fastjet/RangeDefinition.hh"

#include <cmath>

class EventEtDensity
{
public:

  /*! @brief Constructor
   * 
   *  Generates empty @c EventEtDensity structure - generally not very
   *  useful.
   */
  EventEtDensity();
  /*! @brief Constructor
   * 
   *  Generates loaded @c EventEtDensity structure.
   *
   *  @param[in] rho event transverse energy density
   *  @param[in] sigma event transverse energy fluctuation (default 0. - 
   *                    optional argument from e.g. @c fastjet )
   *  @param[in] area total area in range occupied by probe jets with 
   *                  @f$ p_T > 0 @f$ (default no jets, i.e. 
   *                  @f$ \Delta\eta \times 2\pi$ @f$ - optional argument from 
   *                  e.g. @c fastjet )  
   *  @param[in] etaLow lower limit @f$ \eta_L @f$ of @f$ \eta @f$ range in 
   *                    which density is evaluated (default: ATLAS coverage
   *                    @f$ \eta_L = -5. @f$ - optional argument ) 
   *  @param[in] etaHigh upper limit @f$ \eta_H @f$ of @f$ \eta @f$ range in 
   *                     which density is evaluated (default: ATLAS coverage
   *                     @f$ \eta_H = 5. @f$ - optional argument ) 
   */
  EventEtDensity(double rho,double sigma=0.,double area=40*asin(1.),
		 double etaLow=-5.,double etaHigh=5.);
  /*! @brief Constructor
   * 
   *  Generates loaded @c EventEtDensity structure.
   *
   *  @param[in] rho   event transverse energy density
   *  @param[in] sigma event transverse energy fluctuation (typically from 
   *                   @c fastjet )
   *  @param[in] area  total area in range occupied by probe jets with 
   *                   @f$ p_T > 0 @f$ (typically from @c fastjet )
   *  @param[in] range range in which density is to be evaluated, presented
   *                   as @c fastjet::Rangedefinition
   */
  EventEtDensity(double rho,double sigma,double area,
		 const fastjet::RangeDefinition& range);
  // Use default copy constructor.
  /*! @brief Destructor */
  ~EventEtDensity();

  /*! @brief Data indices in external vectors
   *
   *  Deines the order in an external vector, as e.g. used in the @c copyData
   *  function. 
   */
  enum DataIndex { RHOVALUE     = 2,
		   RHOSIGMA     = 3,
		   AREAVALUE    = 4,
		   ETARANGELOW  = 0,
		   ETARANGEHIGH = 1,
		   MAXINDEX     = 5,
		   INVALIDINDEX = -1 };

  /*! @brief Access to event transverse energy density @f$ \rho @f$ */
  double rhoValue;
  /*! @brief Access to spread of transverse energy density @f$ \sigma @f$ */
  double rhoSigma;
  /*! @brief Access to total area occupied by @f$ p_T > 0. @f$ probe jets */
  double areaValue;
  /*! @brief Access to lower @f$ \eta @f$ range limit */
  double etaRangeLow;
  /*! @brief Access to upper @f$ \eta @f$ range limit */
  double etaRangeHigh;

  /**/
  static size_t capacity();

  /*! @brief Convert @f$ \eta @f$ range into @c fastjet::RangeDefinition */
  fastjet::RangeDefinition range() const;
  /*! @brief Convert @f$ \eta @f$ range into @c fastjet::RangeDefinition */
  static fastjet::RangeDefinition range(const EventEtDensity& data);
  /*! @brief Fill @c EventEtDensity from a vector 
   *
   *  @param[in] lparm @c const reference to non-modifiable list of density 
   *                   variables (needs to contain at least @c MAXINDEX
   *                   variables)
   *  @param[out] data reference to modifiable @c EventEtDensity object
   *  @param[in] lindx starting index for parameter copy (optional, default 
   *                   <code>lindx = 0</code>)
   *
   *  @return Number of values filled - typically the minimum of the vector 
   *  size and the number of storable parameters (5). 
   *
   *  This function will make the following assignments (not the actual 
   *  implementation, just to illustrate the behaviour):
   *
   *  <code>
   *    size_t offset(lindx*(size_t)MAXINDEX);
   *    data.rhoValue     = lparm.at(offset+(size_t)RHOVALUE);
   *    data.rhoSigma     = lparm.at(offset+(size_t)RHOSIGMA);
   *    data.areaValue    = lparm.at(offset+(size_t)AREAVALUE);
   *    data.etaRangeLow  = lparm.at(offset+(size_t)ETARANGELOW);
   *    data.etaRangeHigh = lparm.at(offset+(size_t)ETARANGEHIGH);
   *  </code>
   *
   *  Only full blocks of @c MAXINDEX data words are copied. This means that 
   *  only data up to the last full block of @c MAXINDEX numbers is converted.
   */
  static bool copyData(const std::vector<double>& lparm,
		       EventEtDensity&            data,
		       size_t                     lindx=0);
  /*! @brief Fill @c this @c EventEtDensity from a vector
   *
   *  @param[in] lparm @c const reference to non-modifiable list of density 
   *                   variables
   *  @param[in] lindx starting index for parameter copy (optional, default 
   *                   <code>lindx = 0</code>)
   *
   *  @return Number of values filled - typically the minimum of the vector 
   *  size and the number of storable parameters (5). 
   *
   *  This function will make the following assignments (not the actual 
   *  implementation, just to illustrate the behaviour):
   *
   *  <code>
   *    size_t offset(lindx*(size_t)MAXINDEX);
   *    data.rhoValue     = lparm.at(offset+(size_t)RHOVALUE);
   *    data.rhoSigma     = lparm.at(offset+(size_t)RHOSIGMA);
   *    data.areaValue    = lparm.at(offset+(size_t)AREAVALUE);
   *    data.etaRangeLow  = lparm.at(offset+(size_t)ETARANGELOW);
   *    data.etaRangeHigh = lparm.at(offset+(size_t)ETARANGEHIGH);
   *  </code>
   *
   *  Only full blocks of @c MAXINDEX data words are copied. This means that 
   *  only data up to the last full block of @c MAXINDEX numbers is converted.
   */
  bool copyData(const std::vector<double>& lparm,size_t lindx=0);

  /**/
  static bool extractData(const EventEtDensity& data,
			  std::vector<double>& lparm,size_t lindx=0);
  /**/
  bool extractData(std::vector<double>& lparm,size_t lindx=0) const;

  /*! @brief Create @c EventEtDensity store
   *
   */
  static EventEtDensity createStore(const std::vector<double>& lparm,
				    size_t lindx=0);
  /*! @brief Clear @c EventEtDensity store 
   * 
   *  Sets all data to @c EventFeatureToolBase::invalidValue() .
   */
  static void clear(EventEtDensity& data);
  /*! @brief Clear @c this @c EventEtDensity store 
   *
   *  Sets all data to @c EventFeatureToolBase::invalidValue() .
   */
  void clear();


  static void reset(EventEtDensity& data);
  void reset();

  bool inRange(double eta) const;

// private:

//   /*! @brief Cached @f$ \rho @f$ */
//   double m_rho;
//   /*! @brief Cached @f$ \sigma @f$ */
//   double m_sigma;
//   /*! @brief Cached integrated jet catchment area */
//   double m_area;
//   /*! @brief Cached lower @f$ \eta @f$ range limit */
//   double m_etaLow;
//   /*! @brief Cached upper @f$ \eta @f$ range limit */
//   double m_etaHigh;
};

/*! @class EventEtDensity
 *
 *  @brief Data container for event Et densities 
 *
 *  Allows compact distribution and access to transverse energy event densities
 *  as provided by @c fastjet
 */

inline EventEtDensity::EventEtDensity()
{
  rhoValue     = 0.;
  rhoSigma     = 0.;
  etaRangeLow  = -5.;
  etaRangeHigh = 5.;
  areaValue    = (etaRangeHigh-etaRangeLow)*4.*asin(1.);
}
inline EventEtDensity::EventEtDensity(double rho,double sigma,double area,
				      double etaLow,double etaHigh)
{
  rhoValue     = rho;
  rhoSigma     = sigma;
  areaValue    = area;
  etaRangeLow  = etaLow;
  etaRangeHigh = etaHigh;
}
inline EventEtDensity::EventEtDensity(double rho,double sigma,double area,
				      const fastjet::RangeDefinition& range)
{ 
  rhoValue  = rho;
  rhoSigma  = sigma;
  areaValue = area;
  range.get_rap_limits(etaRangeLow,etaRangeHigh); 
}

inline EventEtDensity::~EventEtDensity()
{ }

inline fastjet::RangeDefinition EventEtDensity::range() const
{ return fastjet::RangeDefinition(etaRangeLow,etaRangeHigh); }

inline fastjet::RangeDefinition EventEtDensity::range(const EventEtDensity&
						      data)
{ return data.range(); }

inline bool
EventEtDensity::copyData(const std::vector<double>& lparm,EventEtDensity& data,
			 size_t lindx)
{ return data.copyData(lparm,lindx); }				     

inline 
EventEtDensity EventEtDensity::createStore(const std::vector<double>& lparm,
					   size_t                     lindx)
{ EventEtDensity data; copyData(lparm,data,lindx); return data; }

inline 
bool EventEtDensity::extractData(const EventEtDensity& data,
				 std::vector<double>& lparm,size_t lindx)
{ return data.extractData(lparm,lindx); }

inline void EventEtDensity::clear(EventEtDensity& data)
{ data.clear(); }

inline size_t EventEtDensity::capacity()
{ return (size_t)MAXINDEX; }

inline void EventEtDensity::reset(EventEtDensity& data) 
{ data.reset(); }

inline bool EventEtDensity::inRange(double eta) const 
{ return eta >= etaRangeLow && eta <= etaRangeHigh; }

// inline EventEtDensity::EventEtDensity()
//   : m_rho(0.), m_sigma(0.), m_area(40.*asin(1.)),
//   , m_etaLow(-5.), m_etaHigh(5.)
// { }

// inline EventEtDensity::EventEtDensity(double rho,double sigma,double area,
// 				      double etaLow,double etaHigh)
//   : m_rho(rho), m_sigma(sigma), m_area(area),
//   , m_etaLow(etaLow), m_etaHigh(etaHigh)
// { }
// inline EventEtDensity::EventEtDensity(double rho,double sigma,double area,
// 				      const fastjet::RangeDefinition range)
//   : m_rho(rho), m_sigma(sigma), m_area(area)
// { range.get_rap_limits(m_etaLow,m_etaHigh); }

// inline EventEtDensity::EventEtDensity(const EventEtDensity& data)
//   : m_rho(data.m_rho), m_sigma(data.m_sigma), m_area(data.m_area)
//   , m_etaLow(data.m_etaLow), m_etaHigh(data.m_etaHigh)
// { }

// inline double EventEtDensity::rho() const
// { return m_rho; } 

// inline double EventEtDensity::sigma() const
// { return m_sigma; }

// inline double EventEtDensity::area() const
// { return m_area; }

// inline double EventEtDensity::etaLow() const
// { return m_etaLow; }

// inline double EventEtDensity::etaHigh() const
// { return m_etaHigh; }
#endif
