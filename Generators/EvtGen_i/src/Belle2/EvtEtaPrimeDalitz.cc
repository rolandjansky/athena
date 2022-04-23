//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package for the BelleII
//      collaboration. If you use all or part of it, please give an
//      appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtEtaFullDalitz.cc
//
// Description: Routine to decay eta' ->  pi+ pi- eta
//              with a Dalitz parametrization up to the
//              quadratic terms
//
//
// Developer: Umberto Tamponi (tamponi@to.infn.it)
//
// Modification history:
//
//    U.Tamponi    October 9th 2016   Module created
//
//------------------------------------------------------------------------
//
//#include <generators/evtgen/EvtGenModelRegister.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGen_i/EvtGenExternal/Belle2/EvtEtaPrimeDalitz.h"
#include <string>

namespace Belle2 {

  /** register the model in EvtGen */
  //  B2_EVTGEN_REGISTER_MODEL(EvtEtaPrimeDalitz);


  EvtEtaPrimeDalitz::~EvtEtaPrimeDalitz() {}

  std::string EvtEtaPrimeDalitz::getName()
  {

    return "ETAPRIME_DALITZ";

  }


  EvtDecayBase* EvtEtaPrimeDalitz::clone()
  {

    return new EvtEtaPrimeDalitz;

  }

  void EvtEtaPrimeDalitz::init()
  {

    // check that there is are arguments
    checkNArg(4);
    checkNDaug(3);


    checkSpinParent(EvtSpinType::SCALAR);

    checkSpinDaughter(0, EvtSpinType::SCALAR);
    checkSpinDaughter(1, EvtSpinType::SCALAR);
    checkSpinDaughter(2, EvtSpinType::SCALAR);
  }


  void EvtEtaPrimeDalitz::initProbMax()
  {

    setProbMax(2.5);

  }

  void EvtEtaPrimeDalitz::decay(EvtParticle* p)
  {

    const double a = getArg(0);
    const double b = getArg(1);
    const double c = getArg(2);
    const double d = getArg(3);

    p->initializePhaseSpace(getNDaug(), getDaugs());

    EvtVector4R mompip = p->getDaug(0)->getP4();
    EvtVector4R mompim = p->getDaug(1)->getP4();
    EvtVector4R mometa = p->getDaug(2)->getP4();

    double etaprime = p->mass();
    double eta = p->getDaug(2)->mass();
    double pi = p->getDaug(1)->mass();

    //Q value
    double deltaM = etaprime - 2 * pi - eta;

    //The decay amplitude comes from BESIII collab, Phys.Rev. D92 (2015) 012014

    //Kinetic energies T
    double Tpip = (mompip.get(0) - pi);
    double Tpim = (mompim.get(0) - pi);
    double Teta = (mometa.get(0) - eta);

    //Dalitz variables
    double X = sqrt(3.) * (Tpip - Tpim) / deltaM;
    double Y = ((eta + 2 * pi) / pi) * Teta / deltaM - 1. ;

    double amp2 = 1. + a * Y + b * Y * Y + c * X + d * X * X;

    EvtComplex amp(sqrt(amp2), 0.0);

    vertex(amp);

    return ;

  }

} // Belle 2 Namespace
