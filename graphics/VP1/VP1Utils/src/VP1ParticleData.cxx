/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1ParticleData                   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Utils/VP1ParticleData.h"
#include "VP1Base/VP1Msg.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IPartPropSvc.h"

#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"

//____________________________________________________________________
class VP1ParticleData::Imp {
public:
  static bool m_badInit;
  static const HepPDT::ParticleDataTable * m_particleDataTable;
  static std::map<int,double> m_particleAbsPDGCodeToMass;
  static std::map<int,double> m_particlePDGCodeToCharge;
  static std::map<int,QString> m_particleAbsPDGCodeToName;
  static const double m_badValue;
  static const QString m_badName;
};

bool VP1ParticleData::Imp::m_badInit = false;
const HepPDT::ParticleDataTable * VP1ParticleData::Imp::m_particleDataTable = 0;
std::map<int,double> VP1ParticleData::Imp::m_particleAbsPDGCodeToMass;
std::map<int,double> VP1ParticleData::Imp::m_particlePDGCodeToCharge;
std::map<int,QString> VP1ParticleData::Imp::m_particleAbsPDGCodeToName;
const double VP1ParticleData::Imp::m_badValue = -1.0e99;
const QString VP1ParticleData::Imp::m_badName = "_Bad_Name_";

//____________________________________________________________________
const HepPDT::ParticleData * VP1ParticleData::particleData( const int& pdgcode )
{
  if (Imp::m_badInit)
    return 0;
  if (!Imp::m_particleDataTable) {
    //init
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    if (!svcLocator) {
      VP1Msg::message("VP1ParticleData ERROR: Could not get svcLocator.");
      Imp::m_badInit = true;
      return 0;
    }

    IPartPropSvc* partPropSvc;
    StatusCode status = svcLocator->service( "PartPropSvc",partPropSvc,true/*createifnotthere*/ );
    if(!status.isSuccess()||!partPropSvc) {
      VP1Msg::message("VP1ParticleData ERROR: Could not get particle property service.");
      Imp::m_badInit = true;
      return 0;
    }
    const HepPDT::ParticleDataTable* tab = partPropSvc->PDT();
    Imp::m_particleDataTable = tab;
    if (!tab) {
      VP1Msg::message("VP1ParticleData ERROR: Could not get particle data table from particle property service.");
      Imp::m_badInit = true;
      return 0;
    }
  }

  const HepPDT::ParticleData* particle =
    Imp::m_particleDataTable->particle(HepPDT::ParticleID(pdgcode));
  if (!particle&&VP1Msg::verbose())
    VP1Msg::messageVerbose("VP1ParticleData WARNING: Could not get particle data table for pdgcode="+QString::number(pdgcode));

  return particle;
}

//____________________________________________________________________
double VP1ParticleData::particleMass( const int& pdgcode, bool& ok )
{
  std::map<int,double>::const_iterator it = Imp::m_particleAbsPDGCodeToMass.find(abs(pdgcode));
  if (it!=Imp::m_particleAbsPDGCodeToMass.end()) {
    ok = it->second != Imp::m_badValue;
    return ok ? it->second : 0;
  }
  const HepPDT::ParticleData * data = particleData( abs(pdgcode) );
  double m;
  if (data)
    m = data->mass().value();
  else
    m = Imp::m_badValue;

  Imp::m_particleAbsPDGCodeToMass[abs(pdgcode)] = m;
  ok = m != Imp::m_badValue;
  return m;
}

//____________________________________________________________________
double VP1ParticleData::particleCharge( const int& pdgcode, bool& ok )
{
  if (pdgcode==22) {
    ok = true;
    return 0.0;
  }
  std::map<int,double>::const_iterator it = Imp::m_particlePDGCodeToCharge.find(pdgcode);
  if (it!=Imp::m_particlePDGCodeToCharge.end()) {
    ok = it->second != Imp::m_badValue;
    return ok ? it->second : 0;
  }
  const HepPDT::ParticleData * data = particleData( abs(pdgcode) );
  double c;
  if (data)
    c = pdgcode >= 0 ? data->charge() : - data->charge();
  else
    c = Imp::m_badValue;

  Imp::m_particlePDGCodeToCharge[pdgcode] = c;
  ok = c != Imp::m_badValue;
  return c;
}

//____________________________________________________________________
QString VP1ParticleData::particleName( const int& pdgcode, bool& ok )
{
  std::map<int,QString>::const_iterator it = Imp::m_particleAbsPDGCodeToName.find(pdgcode);
  if (it!=Imp::m_particleAbsPDGCodeToName.end()) {
    ok = (it->second != Imp::m_badName);
    return ok ? it->second : "";
  }

  QString name;
  switch (pdgcode) {
  case   21: name = "gluon"; break;
  case   22: name = "photon"; break;
  case  -11: name = "e+"; break;
  case   11: name = "e-"; break;
  case  -13: name = "mu+"; break;
  case   13: name = "mu-"; break;
  case  -15: name = "tau+"; break;
  case   15: name = "tau-"; break;
  case -211: name = "M_PI-"; break;
  case  211: name = "M_PI+"; break;
  case    1: name = "d"  ; break;
  case    2: name = "u"  ; break;
  case    3: name = "s"  ; break;
  case    4: name = "c"  ; break;
  case    5: name = "b"  ; break;
  case    6: name = "t"  ; break;
  case   -1: name = "dbar"  ; break;
  case   -2: name = "ubar"  ; break;
  case   -3: name = "sbar"  ; break;
  case   -4: name = "cbar"  ; break;
  case   -5: name = "bbar"  ; break;
  case   -6: name = "tbar"  ; break;
  case   92: name = "frag string" ; break;
  default:
    break;
  }

  if (name.isEmpty()) {
    const HepPDT::ParticleData * data = particleData( abs(pdgcode) );
    if (data)
      name = (pdgcode<0?"anti-":"")+QString(data->name().c_str());//fixme: anything [[:alpha:]](+|-) we
                                                                  //    change + and -
    else
      name = Imp::m_badName;
  }

  Imp::m_particleAbsPDGCodeToName[pdgcode] = name;
  ok = (name != Imp::m_badName);
  return name;
}
