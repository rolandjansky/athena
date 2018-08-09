/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_PAI_physicsConstants_h
#define TRT_PAI_physicsConstants_h

/**
 * Physics constants
 */
namespace TRT_PAI_physicsConstants{
  const double c          = 2.99792458e10;      /**< Light speed          */
  const double Nav        = 6.02214e23;         /**< Avagadro constant    */
  const double erg        = 1.60218e-12;        /**< 1 ev to erg {erg}    */
  const double mb         = 1.e-18;             /**< 1mb to cm2           */
  const double Me         = 9.10943e-28;        /**< Electron mass {g}    */
  const double MeeV       = Me*(c*c)/erg;       /**< same in ev           */
  const double MProtonMeV = 938.271998;         /**< Proton mass in MeV   */
  const double Qe         = 4.80321e-10;        /**< Electron charge{ESU} */
  const double r0         = Qe*Qe/Me/(c*c);     /**< electron radius{cm}  */
  const double h          = 1.05457e-27;        /**< Plank constant{erg}  */
  const double he         = h*c/erg;            /**< same in ev           */
  const double invlog10   = 0.434294481903252;  /**< you guess...         */
}

#endif
