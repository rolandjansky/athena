/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCSTAND_TOMOTUBEDISPLACEMENTCONTINUOUS_H
#define AMDCSTAND_TOMOTUBEDISPLACEMENTCONTINUOUS_H

#include <string>

void tomotubedisplacementcontinuous_(
                         int Iside_In,
		         int optionflag,
		         const std::string& TheStationName, int jff, int jzz, int MulLay,
                         double& szt_s        , double& szt_z        , double& szt_t        , 
                         double& Delta_szt_s_c, double& Delta_szt_z_c, double& Delta_szt_t_c
		        );

#endif
