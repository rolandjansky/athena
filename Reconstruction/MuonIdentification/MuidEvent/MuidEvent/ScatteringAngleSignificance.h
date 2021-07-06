/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//   lightweight data class containing analysis of inner scatterers
//////////////////////////////////////////////////////////////////

#ifndef MUIDEVENT_SCATTERINGANGLESIGNIFICANCE_H
#define MUIDEVENT_SCATTERINGANGLESIGNIFICANCE_H

namespace Rec
{
  
/**@class ScatteringAngleSignificance

lightweight return data-object for (mainly indet) scattering angle analysis by track query
     
@author Alan.Poppleton@cern.ch
*/
class ScatteringAngleSignificance
{
public:

    /** inline constructor */
    ScatteringAngleSignificance(unsigned	numberScatterers = 0,
				double		curvatureRadius = 0.,
				double		curvatureSignificance = 0.,
				double		neighbourRadius = 0.,
				double		neighbourSignificance = 0.)
	{
	    m_numberScatterers		= numberScatterers;
	    m_curvatureRadius		= curvatureRadius;
	    m_curvatureSignificance	= curvatureSignificance;
	    m_neighbourRadius		= neighbourRadius;
	    m_neighbourSignificance	= neighbourSignificance;
	}
    
    /** inline destructor  */
    ~ScatteringAngleSignificance(){}
                  
    /**ScatteringAngleSignificance inline accessor:
       number of scatterers used in analysis*/
    unsigned	numberScatterers (void) const
	{ return m_numberScatterers; }
              
    /**ScatteringAngleSignificance inline accessor:
       radius of scatterer about which curvature discontinuity is most significant*/
    double	curvatureRadius (void) const
	{ return m_curvatureRadius; }
        
    /**ScatteringAngleSignificance inline accessor:
       significance of maximum curvature discontinuity*/
    double	curvatureSignificance (void) const
	{ return m_curvatureSignificance; }

    /**ScatteringAngleSignificance inline accessor:
       mean radius of most significant neighbouring scatterers */
    double	neighbourRadius (void) const
	{ return m_neighbourRadius; }
    
    /**ScatteringAngleSignificance inline accessor:
       maximum significance of neighbouring scatterers */
    double	neighbourSignificance (void) const
	{ return m_neighbourSignificance; }
       
private:
    unsigned	m_numberScatterers;
    double	m_curvatureRadius;
    double	m_curvatureSignificance;
    double	m_neighbourRadius;
    double	m_neighbourSignificance; 
};
 
}	// end of namespace

#endif // MUIDEVENT_SCATTERINGANGLESIGNIFICANCE_H


