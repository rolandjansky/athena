/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tile Calorimeter detector description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: TileDetDescriptor.h,v 1.15 2007-11-06 15:20:19 baranov Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef TILEDETDESCR_TILEDETDESCRIPTOR_H
#define TILEDETDESCR_TILEDETDESCRIPTOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "Identifier/Identifier.h"

#include <assert.h>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>

#define MAX_N_SAMP_TILEDD 20

//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
    @section s1 class TileDetDescriptor
  
    This class contains the detector description information for the
    Tile calorimeters: Barrel, Ext.Barrel, Gap detector.
  
    There are 3 detectors (bar,ext.bar,gap) * 2 sides = 6 regions in TileCal
    Granularity in phi is one and the same for all cells in the region
    and all phi variables are scalars (min_phi, max_phi, dphi, nphi)
    Granularity in eta can be different for different samplings or towers
    and all eta variables are arrays (min_eta, max_eta, deta, neta)
    TileCal has 3 samplings and index in "eta" arrays corresponds to sampling
    number: eta[0] - samling A, eta[1] - sampling BC, eta[2] - sampling D
    In Gap detector (detector=3) apart from normal samplings there are
    4 so-called gap scintillators. For all these scintillators sampling=3
    and tower number is used instead of sampling number to distinguish them.
    In current geometry gap scintillators are positioned in towers 10,11,13,15
    hence information about them is saved in eta[10], eta[11], eta[13], eta[15]
    variable "n_samp" is the length of "eta" arrays, n_samp=3 for barrel and
    ext. barrel and n_samp=16 for gap detector.
    Rcenter[] is the radius of the center of cells in the given sampling
    It can be used to re-calculate shift in eta due to zshift 
    etashift = zshift * tanh(eta) * tan(2 * atan(exp(-eta))) / Rcenter
    dr[] is the thickness of the samplings in R
*/

class TileDetDescriptor
{
public:
    
    //
    //	Typedefs:
    //

    TileDetDescriptor();
    TileDetDescriptor(const float               eta_min[],
                      const float               eta_max[],
                      const float               deta[],
                      const float               phi_min,
                      const float               phi_max,
                      const float               dphi,
                      const float               zshift,
                      const float               rcenter[],
                      const float               dr[],
                      const float               zcenter[],
                      const float               dz[],
                      const unsigned int        n_samp,
                      const int                 sign_eta);
    ~TileDetDescriptor();
    
    // Allow setting of some internals
    void	set			(const Identifier& id);

    // Identifier - indicates which calorimeter
    Identifier	identify(void) const;

    // Region parameters for eta/phi counting
    int		sign_eta() const;	// is -1 or +1    
    float	eta_min (unsigned int samp) const;	// is min abs(eta)
    float	eta_max	(unsigned int samp) const;	// is max abs(eta)
    float	deta	(unsigned int samp) const;
    int		n_eta	(unsigned int samp) const;
    float	phi_min	() const;	// phi goes from 0 to 2pi
    float	phi_max	() const;	// phi goes from 0 to 2pi
    float	dphi	() const;	// phi goes from 0 to 2pi
    int		n_phi	() const;
    float	zshift	() const;
    float	rcenter	(unsigned int samp) const;
    float	dr	(unsigned int samp) const;
    float	zcenter	(unsigned int samp) const;
    float	dz	(unsigned int samp) const;
    int		n_samp	() const;
    
    // Decoding methods
    float	eta	(unsigned int eta_channel, unsigned int samp) const;
    float	phi	(unsigned int phi_channel) const;

    // Print
    void	print	() const;

    void        set ( const float               eta_min[],
                      const float               eta_max[],
                      const float               deta[],
                      const float               phi_min,
                      const float               phi_max,
                      const float               dphi,
                      const float               zshift,
                      const float               rcenter[],
                      const float               dr[],
                      const float               zcenter[],
                      const float               dz[],
                      const unsigned int        n_samp,
                      const unsigned int        n_eta[],
                      const unsigned int        n_phi,
                      const int                 sign_eta);

private:
  
    Identifier	m_id; // in CaloDetDescriptor now
    float		m_eta_min[MAX_N_SAMP_TILEDD];
    float		m_eta_max[MAX_N_SAMP_TILEDD];
    float		m_deta[MAX_N_SAMP_TILEDD];
    float		m_phi_min;
    float		m_phi_max;
    float		m_dphi;
    float               m_zshift;
    float               m_rcenter[MAX_N_SAMP_TILEDD];
    float               m_dr[MAX_N_SAMP_TILEDD];
    float               m_zcenter[MAX_N_SAMP_TILEDD];
    float               m_dz[MAX_N_SAMP_TILEDD];
    unsigned int        m_n_samp;
    unsigned int	m_n_eta[MAX_N_SAMP_TILEDD];
    unsigned int	m_n_phi;
    int			m_sign_eta;
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline
TileDetDescriptor::~TileDetDescriptor()
{}

inline float	
TileDetDescriptor::eta_min	(unsigned int samp) const
{
  assert (samp < m_n_samp);
    
  return m_eta_min[samp];
}

inline float	
TileDetDescriptor::eta_max	(unsigned int samp) const
{
  assert (samp < m_n_samp);

  return m_eta_max[samp];
}

inline float	
TileDetDescriptor::deta	        (unsigned int samp) const
{
  assert (samp < m_n_samp);

  return m_deta[samp];
}

inline int	
TileDetDescriptor::n_eta	(unsigned int samp) const
{
  assert (samp < m_n_samp);

  return m_n_eta[samp];
}

inline float	
TileDetDescriptor::phi_min      () const
{ 
  return m_phi_min;
}	

inline float	
TileDetDescriptor::phi_max      () const
{ 
  return m_phi_max;
}	

inline float	
TileDetDescriptor::dphi	        () const
{ 
  return m_dphi;
}	

inline int	
TileDetDescriptor::n_phi	() const
{ return m_n_phi; }

inline float	
TileDetDescriptor::zshift        () const
{ 
  return m_zshift;
}	

inline float	
TileDetDescriptor::rcenter       (unsigned int samp) const
{ 
  assert (samp < m_n_samp);

  return m_rcenter[samp]; 
}	

inline float	
TileDetDescriptor::dr            (unsigned int samp) const
{ 
  assert (samp < m_n_samp);

  return m_dr[samp]; 
}	

inline float	
TileDetDescriptor::zcenter       (unsigned int samp) const
{ 
  assert (samp < m_n_samp);

  return m_zcenter[samp]; 
}	

inline float	
TileDetDescriptor::dz            (unsigned int samp) const
{ 
  assert (samp < m_n_samp);

  return m_dz[samp]; 
}	

inline int	
TileDetDescriptor::n_samp	() const
{ 
  return m_n_samp;
}

inline int	
TileDetDescriptor::sign_eta	() const
{ 
  return m_sign_eta;
}

inline float	
TileDetDescriptor::eta	(unsigned int eta_channel, unsigned int samp) const
{ 
  assert (samp < m_n_samp);
  assert (eta_channel < m_n_eta[samp]);
    
  return (m_sign_eta*(((float)eta_channel + 0.5)*m_deta[samp] + m_eta_min[samp]));
}

inline float	
TileDetDescriptor::phi	(unsigned int phi_channel) const
{ 
  assert (phi_channel < m_n_phi);
    
  return (((float)phi_channel + 0.5)*m_dphi + m_phi_min);
}

inline void	
TileDetDescriptor::set (const Identifier& id)
{
  m_id = id;
}

inline Identifier	
TileDetDescriptor::identify(void) const
{
  return m_id;
}

#endif // TILEDETDESCR_TILEDETDESCRIPTOR_H
