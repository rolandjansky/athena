/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAlignEvent/AlignPar.h"

namespace Trk {
 
  //________________________________________________________________________
  MsgStream& operator << (MsgStream& sl, const AlignPar& ap) 
  {
    sl << "alignPar type : " << std::setw(10) << ap.dumpType()
       << "(" << ap.paramType() << " ) : " << &ap << endmsg;
    sl << "   init par: " << std::setw(6) << std::showpos << std::fixed 
       << ap.initPar()<<" +/- "<<std::noshowpos << ap.initErr() 
       <<   resetiosflags(std::ios::floatfield) << endmsg;
    sl << "        par: "<< std::showpos << ap.par() << " +/- "
       << std::noshowpos << ap.err() << endmsg;
    sl << "  final par: "<< std::showpos << ap.finalPar() << " +/- "
       << std::noshowpos << ap.finalErr() << endmsg;
    sl << "      sigma: "<< ap.sigma() << endmsg; 
    sl << "   soft-cut: "<< ap.softCut() << endmsg; 
    sl << "  first drv: "<< std::showpos << ap.firstDeriv()
       << std::noshowpos << endmsg;
    sl << "  secnd drv: "<< std::showpos << ap.secndDeriv()
       << std::noshowpos << endmsg;
    return sl; 
  } 
  
  //________________________________________________________________________
  std::string AlignPar::dumpType() const
  {
    switch(paramType()) {
      case AlignModule::TransX :  return "TransX";
      case AlignModule::TransY :  return "TransY";
      case AlignModule::TransZ :  return "TransZ";
      case AlignModule::RotX   :  return "RotX";
      case AlignModule::RotY   :  return "RotY";
      case AlignModule::RotZ   :  return "RotZ";
      case AlignModule::BowX   :  return "BowX";
      case AlignModule::BowY   :  return "BowY";
      case AlignModule::BowZ   :  return "BowZ";
      default:
        std::ostringstream stm;
        stm << "ParType" << paramType();
        return stm.str();
    }
  }

}
