/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//   lightweight data class containing various field integrals
//////////////////////////////////////////////////////////////////

#ifndef MUIDEVENT_FIELDINTEGRAL_H
#define MUIDEVENT_FIELDINTEGRAL_H

namespace Rec
{
  
/**@class FieldIntegral

lightweight return data-object for field integral track query
     
@author Alan.Poppleton@cern.ch
*/
class FieldIntegral
{
public:

    /** inline constructor */
    FieldIntegral(double betweenInDetMeasurements = 0.,
		  double betweenSpectrometerMeasurements = 0.,
		  double fromFirstMeasurement = 0.,
		  double fromPerigee = 0.)
	{
	    m_betweenInDetMeasurements		= betweenInDetMeasurements;
	    m_betweenSpectrometerMeasurements	= betweenSpectrometerMeasurements;
	    m_fromFirstMeasurement		= fromFirstMeasurement;
	    m_fromPerigee			= fromPerigee;
	}
    
    /** inline destructor  */
    ~FieldIntegral(){}
              
    /**FieldIntegral inline accessor:
       field integral from first to last measurement in inner detector*/
    double	betweenInDetMeasurements (void) const
	{ return m_betweenInDetMeasurements; }
        
    /**FieldIntegral inline accessor:
       field integral from first to last measurement in muon spectrometer*/
    double	betweenSpectrometerMeasurements (void) const
	{ return m_betweenSpectrometerMeasurements; }

    /**FieldIntegral inline accessor: field integral from first to last measurement */
    double	fromFirstMeasurement (void) const
	{ return m_fromFirstMeasurement; }
    
    /**FieldIntegral inline accessor: field integral from perigee to last measurement */
    double	fromPerigee (void) const
	{ return m_fromPerigee; }
       
private:
    double	m_betweenInDetMeasurements;
    double	m_betweenSpectrometerMeasurements;
    double	m_fromFirstMeasurement;
    double	m_fromPerigee; 
};
 
}	// end of namespace

#endif // MUIDEVENT_FIELDINTEGRAL_H


