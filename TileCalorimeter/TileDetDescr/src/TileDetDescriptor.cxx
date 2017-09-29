/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tile Calorimeter detector description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: TileDetDescriptor.cxx,v 1.13 2008-12-14 02:04:58 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

//  **********************************************
//  * Auto-created from schaffer's code.
//  * ~berd
//  **********************************************

//<<<<<< INCLUDES                                                       >>>>>>

#include "TileDetDescr/TileDetDescriptor.h"

#include "AtlasDetDescr/AtlasDetectorID.h"

#include <iostream>
#include <iomanip>
#include <cstring>

using std::memset;

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

TileDetDescriptor::TileDetDescriptor()
    :
    m_phi_min(0),
    m_phi_max(0),
    m_dphi(0),
    m_zshift(0),
    m_n_samp(0),
    m_n_phi(0),
    m_sign_eta(0)
{
    AtlasDetectorID atlas;
    m_id = atlas.tile();
    memset(m_eta_min, 0, MAX_N_SAMP_TILEDD*sizeof(float));
    memset(m_eta_max, 0, MAX_N_SAMP_TILEDD*sizeof(float));
    memset(m_deta,    0, MAX_N_SAMP_TILEDD*sizeof(float));
    memset(m_rcenter, 0, MAX_N_SAMP_TILEDD*sizeof(float));
    memset(m_dr,      0, MAX_N_SAMP_TILEDD*sizeof(float));
    memset(m_zcenter, 0, MAX_N_SAMP_TILEDD*sizeof(float));
    memset(m_dz,      0, MAX_N_SAMP_TILEDD*sizeof(float));
    memset(m_n_eta,   0, MAX_N_SAMP_TILEDD*sizeof(int));
}

TileDetDescriptor::TileDetDescriptor(const float        eta_min[],
                                     const float        eta_max[],
                                     const float        deta[], 
                                     const float        phi_min,
                                     const float        phi_max,
                                     const float        dphi,
                                     const float        zshift,
                                     const float        rcenter[], 
                                     const float        dr[], 
                                     const float        zcenter[], 
                                     const float        dz[], 
                                     const unsigned int n_samp,
                                     const int          sign_eta)
    :
    m_phi_min(phi_min),
    m_phi_max(phi_max),
    m_dphi(dphi),
    m_zshift(zshift),
    m_n_samp(n_samp),
    m_n_phi((unsigned int) ((phi_max - phi_min)/dphi + 0.5)),
    m_sign_eta(sign_eta)
{
    AtlasDetectorID atlas;
    m_id = atlas.tile();
    for (unsigned int i=0; i<n_samp; ++i) {
        m_eta_min[i] = eta_min[i];
        m_eta_max[i] = eta_max[i];
        m_deta[i]    = deta[i];
        m_rcenter[i] = rcenter[i];
        m_dr[i]      = dr[i];
        m_zcenter[i] = zcenter[i];
        m_dz[i]      = dz[i];
        m_n_eta[i]   = ((unsigned int) ((eta_max[i] - eta_min[i])/deta[i] + 0.5));
    }
    for (unsigned int i=n_samp; i<MAX_N_SAMP_TILEDD; ++i) {
        m_eta_min[i] = 0;
        m_eta_max[i] = 0;
        m_deta[i]    = 0;
        m_rcenter[i] = 0;
        m_dr[i]      = 0;
        m_zcenter[i] = 0;
        m_dz[i]      = 0;
        m_n_eta[i]   = 0;
    }
}

void
TileDetDescriptor::set(const float eta_min[], 
                       const float eta_max[],
                       const float deta[], 
                       const float phi_min,
                       const float phi_max,
                       const float dphi,
                       const float zshift,
                       const float rcenter[],
                       const float dr[], 
                       const float zcenter[],
                       const float dz[], 
                       const unsigned int n_samp,
                       const unsigned int n_eta[],
                       const unsigned int n_phi,
                       const int sign_eta)
{
    m_phi_min  = phi_min;
    m_phi_max  = phi_max;
    m_dphi     = dphi;
    m_zshift   = zshift;
    m_n_samp   = n_samp;
    m_n_phi    = n_phi;
    m_sign_eta = sign_eta;

    for (unsigned int i=0; i<n_samp; ++i) {
        m_eta_min[i] = eta_min[i];
        m_eta_max[i] = eta_max[i];
        m_deta[i]    = deta[i];
        m_rcenter[i] = rcenter[i];
        m_dr[i]      = dr[i];
        m_zcenter[i] = zcenter[i];
        m_dz[i]      = dz[i];
        m_n_eta[i]   = n_eta[i];
    }
}


void	
TileDetDescriptor::print	() const
{
    std::cout << std::endl << " TileDetDescriptor print: " 
	      << std::endl << std::endl;

    std::cout << "  phi min   phi max     dphi     nphi   zshift  sign_eta  nsamp"
	      << std::endl;
    std::cout << std::setiosflags(std::ios::fixed);
    std::cout 
	      << std::setw(9) << std::setprecision(4) << m_phi_min << " " 
	      << std::setw(9) << std::setprecision(4) << m_phi_max << " " 
	      << std::setw(9) << std::setprecision(4) << m_dphi    << " "    
	      << std::setw(6) << std::setprecision(4) << m_n_phi   << " "    
	      << std::setw(8) << std::setprecision(2) << m_zshift  << " "    
	      << std::setw(7) << std::setprecision(4) << m_sign_eta<< " "    
	      << std::setw(7) << std::setprecision(4) << m_n_samp  << " "    
	      << std::endl;

    std::cout << "  eta min   eta max     deta     neta   radius     dR         Z        dZ"
	      << std::endl;

    for (unsigned int i=0; i<m_n_samp; ++i) {
        std::cout 
              << std::setw(9) << std::setprecision(4) << m_eta_min[i] << " " 
              << std::setw(9) << std::setprecision(4) << m_eta_max[i] << " " 
              << std::setw(9) << std::setprecision(4) << m_deta[i]    << " "    
              << std::setw(6) << std::setprecision(4) << m_n_eta[i]   << " "    
              << std::setw(9) << std::setprecision(2) << m_rcenter[i] << " "    
              << std::setw(9) << std::setprecision(2) << m_dr[i]      << " "    
              << std::setw(9) << std::setprecision(2) << m_zcenter[i] << " "    
              << std::setw(9) << std::setprecision(2) << m_dz[i]      << " "    
              << std::endl;
    }

    std::cout << std::resetiosflags(std::ios::fixed);
}

