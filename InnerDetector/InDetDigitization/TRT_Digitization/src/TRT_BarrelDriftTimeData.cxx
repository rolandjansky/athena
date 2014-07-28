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
TRT_BarrelDriftTimeData::TRT_BarrelDriftTimeData(unsigned int digversion, bool isArgonInFlag) { // second argument added by Sasha. implementation of Argon straws

  ////////////////////////////////////////////////////////////////////////////
  // The data made available by Peter Cwetanski by detailed gas simulations //
  ////////////////////////////////////////////////////////////////////////////

  isArgon = isArgonInFlag;

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

  if (!isArgon) /// Added by Sasha. Using Argon or Xenon RT tables according to 'isArgon' flag.
    {
      //----------------------------//
      // Data for no external field //
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
      // Data for max external field //
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

    } else {

      //----------------------------//
      // Data for no external field //
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
      // Data for max external field //
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
double TRT_BarrelDriftTimeData::RMSofSpreadRelativeToDriftTime(const double& dist) const {
  //Returns the RMS of the diffusion spread relative to the average
  //drifttime at a certain distance. This is an empirical formula
  //found by fitting to Peters simulated data.

  //Simple (12ns on my laptop):
  /////    return 0.0282156*dist*dist*dist*dist-0.145674*dist*dist*dist+0.28297*dist*dist-0.257124*dist+0.130335;

  // Reordered to save 6 multiplications (10mult,4add->4mult,4add). 9ns instead.
  if (!isArgon){
    return 0.130335 + dist*(-0.257124+dist*(0.28297 + dist*(0.0282156*dist-0.145674))) ;
  }
  else{
    // added by Sasha. Obtained from Konstantin's diffusion tables
    // FIXME Table with no magnetic field was used only!!! need to check table with max magnetic field!!!
    return 0.171935 + dist*(-0.378806+dist*(0.444491 + dist*(0.0465891*dist-0.237386))) ;
  }
}

