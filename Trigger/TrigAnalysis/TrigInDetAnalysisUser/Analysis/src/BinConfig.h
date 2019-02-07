/* emacs: this is -*- c++ -*- */
/**
 **     @file    BinConfig.h
 **
 **     @brief   scales to modify the binning for histograms 
 **
 **     @author  mark sutton
 **     @date    Thu 27 Feb 2014 07:57:21 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  BINCONFIG_H
#define  BINCONFIG_H

#include <iostream>
#include <string>

#include "ReadCards.h"

struct BinConfig {

public:

  BinConfig(const std::string& n="" ) :
    _name(n),
    /// scale factors
    pt_NScale(1),
    eta_NScale(1),
    phi_NScale(1),
    d0_NScale(1),
    a0_NScale(1),
    z0_NScale(1),
    /// scale factors for the residual binning
    ptres_NScale(1),
    etares_NScale(1),
    phires_NScale(1),
    d0res_NScale(1),
    a0res_NScale(1),
    z0res_NScale(1)
  {   } 

  virtual ~BinConfig() { } 


  void set( ReadCards& r, const std::string& n ) { 
    r.Set( n+"ptNScale",  pt_NScale );
    r.Set( n+"etaNScale", eta_NScale );
    r.Set( n+"phiNScale", phi_NScale );
    r.Set( n+"d0NScale",  d0_NScale );
    r.Set( n+"a0NScale",  a0_NScale );
    r.Set( n+"z0NScale",  z0_NScale );
  }

  void set( ReadCards& r ) { 
    if ( _name!="" ) set ( r, _name+"_" );
    else             set ( r, "" );
  }

  std::string _name;

  double pt_NScale;
  double eta_NScale;
  double phi_NScale;
  double d0_NScale;
  double a0_NScale;
  double z0_NScale;

  /// scales for the residuals
  double ptres_NScale;
  double etares_NScale;
  double phires_NScale;
  double d0res_NScale;
  double a0res_NScale;
  double z0res_NScale;

};

inline std::ostream& operator<<( std::ostream& s, const BinConfig& _b ) { 
  return s << "\n\t[ config: " << _b._name << " Nbins scale factors : " 
	   <<   "\t  pt: " << _b.pt_NScale
	   <<    "  eta: " << _b.eta_NScale
	   <<    "  phi: " << _b.phi_NScale
	   <<    "  d0: "  << _b.d0_NScale
	   <<    "  a0: "  << _b.a0_NScale
	   <<    "  z0: "  << _b.z0_NScale 
	   << "\n\t residuals bins:\n"
	   <<   "\t  pt: "  << _b.ptres_NScale
	   <<    "  eta: " << _b.etares_NScale
	   <<    "  phi: " << _b.phires_NScale
	   <<    "  d0: "  << _b.d0res_NScale
	   <<    "  a0: "  << _b.a0res_NScale
	   <<    "  z0: "  << _b.z0res_NScale 
	   << " ]"; 
}


#endif  // BINCONFIG_H 










