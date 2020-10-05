/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     muIsoUtil.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigmuIso
// 
// AUTHOR:   S. Giagu <stefano.giagu@cern.ch>
// 
// PURPOSE:  Utility namespace for LVL2 Muon Isolation FEX Algorithm
// ********************************************************************

#ifndef TRIGMUISO_MUISOUTIL_H 
#define TRIGMUISO_MUISOUTIL_H

namespace muIsoUtil {

  ///SGExtrapolator Functions

  ///Return deltaEta between eta\@MS entrance and eta\@Calo entrance
  /*!
  * \param double phi 
  * \param double pt 
  * \param int charge
  */
  double corrDEtaCM(double phi, double pt, int charge);

  ///Return deltaPhi between phi\@MS entrance and phi\@Calo entrance
  /*!
  * \param double eta
  * \param double pt 
  * \param int charge
  */
  double corrDPhiCM(double eta, double pt, int charge);

  ///Return deltaEta between eta\@Calo entrance and eta\@IP (dummy)
  /*!
  * \param double phi 
  * \param double pt 
  * \param int charge
  */
  double corrDEtaC(); //dummy
  //double corrDEtaC(double phi = 0., double pt = 0., int charge = 0);

  ///Return deltaPhi between phi\@Calo entrance and phi\@IP
  /*!
  * \param double eta
  * \param double pt 
  * \param int charge
  */
  double corrDPhiC(double eta, double pt, int charge);

  ///Return phi\@Calo entrance
  /*!
  * \param double phi 
  * \param double eta
  * \param double pt 
  * \param int charge
  */
  double phiATCal(double phi, double eta, double pt, int charge);

  ///Return eta\@Calo entrance
  /*!
  * \param double phi 
  * \param double eta
  * \param double pt 
  * \param int charge
  */
  double etaATCal(double eta); //dummy
  //double etaATCal(double phi, double eta, double pt, int charge);

  ///Return phi\@MS entrance
  /*!
  * \param double phi 
  * \param double eta
  * \param double pt 
  * \param int charge
  */
  double phiATMS(double phi, double eta, double pt, int charge);

  ///Return eta\@MS entrance
  /*!
  * \param double phi 
  * \param double eta
  * \param double pt 
  * \param int charge
  */
  double etaATMS(double phi, double eta, double pt, int charge);

  ///Return approximate radii of various calo samplings (in cm).
  double rsam(int sampling);

  ///Return linear interp. of track param. at various samplings
  double parATSampling(int sampling, float parc, float parm);

}
#endif
