/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1ParticleData                     //
//                                                            //
//  Description: Convenient and efficient access to particle  //
//               data.                                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1PARTICLEDATA_H
#define VP1PARTICLEDATA_H

#include <QString>
namespace HepPDT { class ParticleData; }

class VP1ParticleData {
public:

  //Fast and convenient access methods for often accessed data:

  static double particleMass( const int& pdgcode, bool& ok );//ok == false indicates problems
  static double particleCharge( const int& pdgcode, bool& ok );//ok == false indicates problems
  static QString particleName( const int& pdgcode, bool& ok );//ok == false indicates problems

  //Full information:
  static const HepPDT::ParticleData * particleData( const int& pdgcode );//Returns 0 in case of problems

private:
  VP1ParticleData(){}
  ~VP1ParticleData(){}
  class Imp;
};

#endif
