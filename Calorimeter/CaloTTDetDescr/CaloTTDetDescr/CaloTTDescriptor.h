/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calorimeter LVL1 Trigger Towers description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CaloTTDescriptor.h,v 1.5 2007-06-19 11:12:58 fledroit Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef CALODETDESCR_CALOTTDESCRIPTOR_H
#define CALODETDESCR_CALOTTDESCRIPTOR_H

#include <Identifier/Identifier.h>

#include <math.h>



/**  @brief This class holds the Calorimeter TriggerTower geometrical description  

     There are four regions of different granularities in the Trigger Tower system: <br>
     0.1x0.1, 0.2x0.2, 0.1x0.2, 0.4x0.4 <br>
     There is 1 Descriptor per such region.

*/
class CaloTTDescriptor
{
public:
    
    /** Default constructor */
    CaloTTDescriptor();
    
    /** Constructor : set geom parameters of each descriptor of equal granularity TT regions  */
    CaloTTDescriptor(float eta_min, float eta_max, float deta, 
		     float phi_min, float phi_max, float dphi,
		     int   sign_eta,short n_lay);

    ~CaloTTDescriptor();

    /** set internal data member m_id (which is unused. should be removed */
    void	set			(const Identifier& id);

    /** Identifier - indicates which calorimeter */
    Identifier	identify(void) const;

    /** descriptor parameter: sign of eta (+-1) */
    int		sign_eta() const;          
    /** descriptor parameter: min value of abs(eta) */
    float	eta_min () const;
    /** descriptor parameter: max value of abs(eta) */
    float	eta_max	() const;
    /** descriptor parameter: min value of phi */
    float	phiMin	() const;	
    /** descriptor parameter: max value of phi */
    float	phiMax	() const;	
    /** descriptor parameter: eta granularity */
    float	deta	() const;
    /** descriptor parameter: phi granularity */
    float	dphi	() const;
    /** descriptor parameter: number of eta bins */
    short       nEta    () const;
    /** descriptor parameter: number of phi bins */
    short       nPhi    () const;
    /** descriptor parameter: number of layers */
    short       nLay    () const;

    /** Print */
    void	print	() const;

    /** set descriptor parameters */
    void        set ( float               eta_min,
                      float               eta_max,
                      float               deta,
                      float               phi_min,
                      float               phi_max,
                      float               dphi,
                      int                 sign_eta,
		      short               n_lay);

 
private:
  
    /** an identifier associated to the descriptor. actually useless. should be cleaned away */
    Identifier		m_id;
    /** descriptor parameter: sign of eta (+-1) */
    int			m_sign_eta;
    /** descriptor parameter: min value of abs(eta) */
    float		m_eta_min;
    /** descriptor parameter: max value of abs(eta) */
    float		m_eta_max;
    /** descriptor parameter: eta granularity */
    float		m_deta;
    /** descriptor parameter: min value of phi */
    float		m_phi_min;
    /** descriptor parameter: max value of phi */
    float		m_phi_max;
    /** descriptor parameter: phi granularity */
    float		m_dphi;
    /** descriptor parameter: number of eta bins */
    short               m_nEta;
    /** descriptor parameter: number of phi bins */
    short               m_nPhi;
    /** descriptor parameter: number of layers */
    short               m_nLay;


};


inline
CaloTTDescriptor::CaloTTDescriptor()
    :
    m_sign_eta(0),
    m_eta_min(0), 
    m_eta_max(0),
    m_deta(0),
    m_phi_min(0),
    m_phi_max(0),
    m_dphi(0),
    m_nEta(0),
    m_nPhi(0),
    m_nLay(0)

{
}

inline void	
CaloTTDescriptor::set (const Identifier& id)
{
    m_id = id;
}


inline Identifier	
CaloTTDescriptor::identify(void) const
{
    return m_id;
}

inline int
CaloTTDescriptor::sign_eta	() const
{ return m_sign_eta; }

inline float	
CaloTTDescriptor::eta_min	() const
{ return m_eta_min; }

inline float	
CaloTTDescriptor::eta_max	() const
{ return m_eta_max; }

inline float	
CaloTTDescriptor::phiMin	() const
{ return m_phi_min; }

inline float	
CaloTTDescriptor::phiMax	() const
{ return m_phi_max; }

inline float	
CaloTTDescriptor::deta	() const
{ return m_deta; }

inline float	
CaloTTDescriptor::dphi	() const
{ return m_dphi; }	

inline short
CaloTTDescriptor::nEta	() const
{ return m_nEta; }	

inline short
CaloTTDescriptor::nPhi	() const
{ return m_nPhi; }	

inline short
CaloTTDescriptor::nLay	() const
{ return m_nLay; }	


#endif // CALODETDESCR_CALOTTDESCRIPTOR_H

