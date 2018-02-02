/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ////////////////////////////////////
//                                  //
//  Class: TRT_BarrelDriftTimeData  //
//                                  //
//  Author: Thomas Kittelmann       //
//                                  //
//  First Version: January 2006     //
//                                  //
 ////////////////////////////////////

#include "TRT_BarrelDriftTimeData.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <iostream>
#include <cassert>

//______________________________________________________________________________
TRT_BarrelDriftTimeData::TRT_BarrelDriftTimeData(unsigned int digversion, int strawGasType) {

  ////////////////////////////////////////////////////////////////////////////
  // The data made available by Peter Cwetanski by detailed gas simulations //
  ////////////////////////////////////////////////////////////////////////////

  m_strawGas = strawGasType;

  //----------------------------//
  // Data for no external field //
  //----------------------------//

  m_tabdists_nofield.resize(0);  m_tabdrifttime_nofield.resize(0);
  m_tabdists_maxfield.resize(0);  m_tabdrifttime_maxfield.resize(0);

  if (digversion<11) {
      std::cout << "FATAL TRT_BarrelDriftTimeData::TRT_BarrelDriftTimeData digversion < 11 ("
                << digversion << ") is no longer supported. The job will die now :(" << std::endl;
      throw;
  }

  if (m_strawGas==0) {

      //----------------------------//
      // Data for no external field // Xenon
      //----------------------------//
      m_tabdists_nofield.push_back( 15.5*CLHEP::micrometer);   m_tabdrifttime_nofield.push_back( 0*CLHEP::nanosecond/*at wire*/ );
      m_tabdists_nofield.push_back( 34.5*CLHEP::micrometer);   m_tabdrifttime_nofield.push_back(0.18332*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 84.5*CLHEP::micrometer);   m_tabdrifttime_nofield.push_back(0.6973*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 134.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(1.42695*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 184.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(2.28892*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 234.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(3.22042*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 284.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(4.20607*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 334.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(5.20461*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 384.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(6.23645*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 434.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(7.27117*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 484.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(8.33306*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 534.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(9.3964*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 584.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(10.4644*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 634.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(11.5281*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 684.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(12.5937*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 734.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(13.6413*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 784.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(14.6812*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 834.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(15.7115*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 884.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(16.7514*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 934.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(17.7596*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 984.5*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(18.7626*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1034.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(19.7583*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1084.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(20.7302*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1134.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(21.701*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1184.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(22.6817*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1234.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(23.6625*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1284.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(24.6182*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1334.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(25.6292*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1384.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(26.6164*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1434.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(27.6115*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1484.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(28.6484*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1534.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(29.6929*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1584.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(30.7602*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1634.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(31.8483*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1684.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(32.9623*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1734.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(34.1029*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1784.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(35.2888*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1834.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(36.464*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1884.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(37.7107*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1934.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(38.9861*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 1983.5*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(40.2195*CLHEP::nanosecond );
      m_tabdists_nofield.push_back( 2000.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(40.6041*CLHEP::nanosecond ); //extrapolated

      //-----------------------------//
      // Data for max external field // Xenon
      //-----------------------------//
      m_tabdists_maxfield.push_back( 15.5*CLHEP::micrometer);   m_tabdrifttime_maxfield.push_back( 0*CLHEP::nanosecond/*at wire*/ );
      m_tabdists_maxfield.push_back( 34.5*CLHEP::micrometer);   m_tabdrifttime_maxfield.push_back(0.18464*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 84.5*CLHEP::micrometer);   m_tabdrifttime_maxfield.push_back(0.70001*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 134.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(1.43068*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 184.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(2.29603*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 234.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(3.22918*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 284.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(4.22207*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 334.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(5.24324*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 384.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(6.28913*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 434.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(7.34263*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 484.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(8.41839*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 534.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(9.5061*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 584.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(10.6157*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 634.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(11.7225*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 684.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(12.8283*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 734.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(13.9349*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 784.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(15.0264*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 834.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(16.1337*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 884.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(17.2324*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 934.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(18.344*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 984.5*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(19.4328*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1034.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(20.5285*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1084.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(21.6335*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1134.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(22.7324*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1184.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(23.8728*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1234.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(25.0018*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1284.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(26.1516*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1334.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(27.3103*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1384.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(28.5097*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1434.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(29.7311*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1484.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(30.9393*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1534.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(32.2265*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1584.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(33.5512*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1634.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(34.8922*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1684.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(36.2441*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1734.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(37.6531*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1784.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(39.1284*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1834.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(40.5923*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1884.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(42.1183*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1934.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(43.6698*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 1983.5*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(45.2038*CLHEP::nanosecond );
      m_tabdists_maxfield.push_back( 2000.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(45.7089*CLHEP::nanosecond ); //extrapolated

    } else if (m_strawGas==1) {

      //------------------------------------------//
      // Data for no external field (Kr)          //
      // Scaled to be 20% between Argon and Xenon //
      // i.e. Kr = (Xe + 4*Ar)/5                  //
      //------------------------------------------//
      m_tabdists_nofield.push_back(15.5*CLHEP::micrometer);   m_tabdrifttime_nofield.push_back(0*CLHEP::nanosecond);//at wire
      m_tabdists_nofield.push_back(35.0*CLHEP::micrometer);   m_tabdrifttime_nofield.push_back(0.0856*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(85.0*CLHEP::micrometer);   m_tabdrifttime_nofield.push_back(0.4278*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(135.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(0.9318*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(185.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(1.5370*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(235.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(2.1987*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(285.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(2.8919*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(335.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(3.5956*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(385.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(4.3102*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(435.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(5.0294*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(485.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(5.7586*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(535.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(6.4913*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(585.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(7.2375*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(635.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(7.9912*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(685.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(8.7534*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(735.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(9.5214*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(785.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(10.2960*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(835.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(11.0761*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(885.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(11.8643*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(935.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(12.6662*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(985.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(13.4447*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1035.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(14.2443*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1085.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(15.0411*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1135.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(15.8412*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1185.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(16.6474*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1235.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(17.4579*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1285.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(18.2673*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1335.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(19.0931*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1385.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(19.9144*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1435.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(20.7436*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1485.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(21.5890*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1535.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(22.4383*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1585.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(23.3031*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1635.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(24.1969*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1685.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(25.0659*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1735.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(26.1003*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1785.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(26.8906*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1835.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(27.8234*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1885.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(28.7769*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1935.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(29.7530*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1985.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(30.4479*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(2000.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(31.1641*CLHEP::nanosecond); //extrapolated

      //------------------------------------------//
      // Data for max external field (Kr)         //
      // Scaled to be 20% between Argon and Xenon //
      // i.e. Kr = (Xe + 4*Ar)/5                  //
      //------------------------------------------//
      m_tabdists_maxfield.push_back(15.5*CLHEP::micrometer);   m_tabdrifttime_maxfield.push_back(0*CLHEP::nanosecond); // at wire
      m_tabdists_maxfield.push_back(35.0*CLHEP::micrometer);   m_tabdrifttime_maxfield.push_back(0.0859*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(85.0*CLHEP::micrometer);   m_tabdrifttime_maxfield.push_back(0.4298*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(135.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(0.9371*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(185.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(1.5482*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(235.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(2.2178*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(285.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(2.9231*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(335.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(3.6466*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(385.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(4.3832*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(435.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(5.1298*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(485.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(5.8982*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(535.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(6.6690*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(585.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(7.4660*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(635.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(8.2874*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(685.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(9.1011*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(735.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(9.9434*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(785.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(10.7982*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(835.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(11.6711*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(885.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(12.5572*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(935.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(13.4608*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(985.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(14.3742*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1035.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(15.3105*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1085.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(16.2479*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1135.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(17.2136*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1185.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(18.1902*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1235.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(19.1876*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1285.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(20.2066*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1335.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(21.2463*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1385.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(22.3139*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1435.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(23.4065*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1485.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(24.5178*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1535.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(25.6675*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1585.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(26.8492*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1635.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(28.1017*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1685.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(29.2983*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1735.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(30.5753*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1785.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(31.8975*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1835.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(33.2373*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1885.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(34.6227*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1935.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(36.0432*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1985.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(37.0448*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(2000.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(37.9298*CLHEP::nanosecond); //extrapolated

    } else if (m_strawGas==2) {

      //----------------------------//
      // Data for no external field // Argon
      //----------------------------//
      m_tabdists_nofield.push_back(15.5*CLHEP::micrometer);   m_tabdrifttime_nofield.push_back(0*CLHEP::nanosecond/*at wire*/ );
      m_tabdists_nofield.push_back(35.0*CLHEP::micrometer);   m_tabdrifttime_nofield.push_back(0.06111*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(85.0*CLHEP::micrometer);   m_tabdrifttime_nofield.push_back(0.36046*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(135.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(0.80805*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(185.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(1.34902*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(235.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(1.94331*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(285.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(2.56331*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(335.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(3.19340*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(385.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(3.82862*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(435.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(4.46891*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(485.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(5.11496*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(535.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(5.76497*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(585.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(6.43076*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(635.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(7.10696*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(685.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(7.79338*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(735.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(8.49140*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(785.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(9.19973*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(835.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(9.91731*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(885.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(10.6425*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(935.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(11.3928*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(985.0*CLHEP::micrometer);  m_tabdrifttime_nofield.push_back(12.1152*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1035.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(12.8658*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1085.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(13.6188*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1135.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(14.3762*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1185.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(15.1388*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1235.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(15.9068*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1285.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(16.6796*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1335.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(17.4591*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1385.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(18.2389*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1435.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(19.0266*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1485.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(19.8241*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1535.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(20.6247*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1585.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(21.4388*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1635.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(22.2840*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1685.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(23.0918*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1735.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(24.0997*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1785.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(24.7911*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1835.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(25.6632*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1885.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(26.5435*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1935.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(27.4447*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(1985.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(28.0050*CLHEP::nanosecond);
      m_tabdists_nofield.push_back(2000.0*CLHEP::micrometer); m_tabdrifttime_nofield.push_back(28.8041*CLHEP::nanosecond); //extrapolated

      //-----------------------------//
      // Data for max external field // Argon
      //-----------------------------//
      m_tabdists_maxfield.push_back(15.5*CLHEP::micrometer);   m_tabdrifttime_maxfield.push_back(0*CLHEP::nanosecond/*at wire*/ );
      m_tabdists_maxfield.push_back(35.0*CLHEP::micrometer);   m_tabdrifttime_maxfield.push_back(0.06122*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(85.0*CLHEP::micrometer);   m_tabdrifttime_maxfield.push_back(0.36227*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(135.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(0.81371*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(185.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(1.36128*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(235.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(1.96498*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(285.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(2.59837*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(335.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(3.24738*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(385.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(3.90668*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(435.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(4.57660*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(485.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(5.26812*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(535.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(5.95973*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(585.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(6.67857*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(635.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(7.42868*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(685.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(8.16936*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(735.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(8.94547*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(785.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(9.74109*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(835.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(10.5555*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(885.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(11.3884*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(935.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(12.2400*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(985.0*CLHEP::micrometer);  m_tabdrifttime_maxfield.push_back(13.1095*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1035.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(14.0060*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1085.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(14.9015*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1135.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(15.8339*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1185.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(16.7696*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1235.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(17.7340*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1285.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(18.7204*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1335.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(19.7303*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1385.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(20.7649*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1435.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(21.8253*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1485.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(22.9124*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1535.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(24.0278*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1585.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(25.1737*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1635.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(26.4041*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1685.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(27.5619*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1735.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(28.8059*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1785.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(30.0898*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1835.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(31.3985*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1885.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(32.7488*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1935.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(34.1365*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(1985.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(35.0050*CLHEP::nanosecond);
      m_tabdists_maxfield.push_back(2000.0*CLHEP::micrometer); m_tabdrifttime_maxfield.push_back(35.9850*CLHEP::nanosecond); //extrapolated

    } else {
      std::cout << "FATAL TRT_BarrelDriftTimeData::TRT_BarrelDriftTimeData strawGas ("
                << m_strawGas << ") must be 0(Xe), 1(Kr) or 2(Ar). The job will die now :(" << std::endl;
      throw;
    }

  assert(m_tabdists_nofield.size()==m_tabdrifttime_nofield.size());
  assert(m_tabdists_maxfield.size()==m_tabdrifttime_maxfield.size());

}

//______________________________________________________________________________
double TRT_BarrelDriftTimeData::MaxTabulatedField() const {
  return 2.0*CLHEP::tesla;
}

//______________________________________________________________________________
double TRT_BarrelDriftTimeData::DriftTimeAtNoField(const double& dist) const {

  if (dist <= m_tabdists_nofield.front())
    return 0.0; //We are inside the wire!

  unsigned int nTabulated = m_tabdists_nofield.size();

  // Figure out indices
  unsigned int indexHigh = nTabulated-1;
  for (unsigned int testIndex = 0; testIndex < nTabulated-1; testIndex++) {
    if (m_tabdists_nofield[testIndex] > dist) {
      indexHigh = testIndex;
      break;
    };
  };
  unsigned int indexLow = 0;
  if (indexHigh==0) {
    std::cout << "TRT_BarrelDriftTimeData::DriftTimeAtNoField::Warning"
	      << " - distance is lower than first tabulated distance" << std::endl;
    indexLow  = 0;
    indexHigh = 0;
  } else {
    indexLow = indexHigh - 1;
  };

  // Figure out weights
  double distLow  = m_tabdists_nofield[indexLow];
  double distHigh = m_tabdists_nofield[indexHigh];
  double lowDistWeight  = 1.0-(dist - distLow)/(distHigh-distLow);
  double highDistWeight = 1.0-(distHigh - dist)/(distHigh-distLow);

  // Return interpolated value
  return lowDistWeight*m_tabdrifttime_nofield[indexLow] + highDistWeight*m_tabdrifttime_nofield[indexHigh];

}

//______________________________________________________________________________
double TRT_BarrelDriftTimeData::DriftTimeAtMaxField(const double& dist) const {

  if (dist <= m_tabdists_maxfield.front())
    return 0.0; //We are inside the wire!

  const unsigned int nTabulated = m_tabdists_maxfield.size();

  // Figure out indices
  unsigned int indexHigh = nTabulated-1;
  for (unsigned int testIndex = 0; testIndex < nTabulated-1; testIndex++) {
    if (m_tabdists_maxfield[testIndex] > dist) {
      indexHigh = testIndex;
      break;
    };
  };
  unsigned int indexLow = 0;
  if (indexHigh==0) {
    std::cout << "TRTDT::Warning - distance is lower than first tabulated distance" << std::endl;
    indexLow  = 0;
    indexHigh = 0;
  } else {
    indexLow = indexHigh - 1;
  };

  // Figure out weights
  double distLow  = m_tabdists_maxfield[indexLow];
  double distHigh = m_tabdists_maxfield[indexHigh];
  double lowDistWeight  = 1.0-(dist - distLow)/(distHigh-distLow);
  double highDistWeight = 1.0-(distHigh - dist)/(distHigh-distLow);

  // Return interpolated value
  return lowDistWeight*m_tabdrifttime_maxfield[indexLow] + highDistWeight*m_tabdrifttime_maxfield[indexHigh];

}

//______________________________________________________________________________
// Diffusion code has been remove (June 2015), only represents a spread of a few ns.
// That is much smaller than the signal shaping width.
