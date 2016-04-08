/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignmentData/MdtAsBuiltPar.h"
#include <sstream>

 
MdtAsBuiltPar::MdtAsBuiltPar():
  MuonAlignmentPar()
{ }

MdtAsBuiltPar::~MdtAsBuiltPar()
{ }

MdtAsBuiltPar::AlignmentParameters::AlignmentParameters():
  iML(multilayer_t::ML1),
  iTubeSide(tubeSide_t::POS),
  y0(0.),
  z0(0.),
  alpha(0.),
  ypitch(0.),
  zpitch(0.),
  stagg(1)
{ }

void MdtAsBuiltPar::setAlignmentParameters(
    multilayer_t iML,
    tubeSide_t  iTubeSide,
    float       y0,
    float       z0,
    float       alpha,
    float       ypitch,
    float       zpitch,
    int         stagg
    ) {
  AlignmentParameters& params = meas(iML, iTubeSide);
  params.iML    = iML;
  params.iTubeSide = iTubeSide;
  params.y0     = y0;
  params.z0     = z0;
  params.alpha  = alpha;
  params.ypitch = ypitch;
  params.zpitch = zpitch;
  params.stagg  = stagg;
}

bool MdtAsBuiltPar::setFromAscii(const std::string& line) {
  std::istringstream in(line);

  std::string tok;
  if (!((in >> tok) && (tok=="Corr:")))
    return false;

  std::string typ;
  int jff, jzz;
  if (!(in >> typ >> jff >> jzz))
    return false;
  setAmdbId(typ, jff, jzz, 0);

  multilayer_t ml[NMLTYPES] = { ML1, ML2 };
  tubeSide_t sid[NTUBESIDETYPES] = { POS, NEG };
  int stagg[NMLTYPES];
  if (!(in >> stagg[ML1] >> stagg[ML2]))
    return false;
  for (int iML=0; iML<NMLTYPES; ++iML) {
    for (int iTubeSide=0; iTubeSide<NTUBESIDETYPES; ++iTubeSide) {
      float y0, z0, alpha, ypitch, zpitch;
      if (!(in >> y0 >> z0 >> alpha >> ypitch >> zpitch))
	  return false;
      setAlignmentParameters(ml[iML], sid[iTubeSide], y0, z0, alpha, ypitch, zpitch, stagg[iML]);
    }
  }

  return true;
}

