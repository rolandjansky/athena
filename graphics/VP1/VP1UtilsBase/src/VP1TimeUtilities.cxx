/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1TimeUtilities              //
//                                                         //
//  Author: Riccardo Maria Bianchi <rbianchi@cern.ch>      //
//                                                         //
//  Initial version: Jan 2019                              //
//                                                         //
/////////////////////////////////////////////////////////////


#include "VP1UtilsBase/VP1TimeUtilities.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <iostream>


std::string VP1TimeUtilities::getHumanReadableTimestamp(time_t t_timestamp)
{
	tm *ltm = localtime(&t_timestamp);

	std::string humanTimestamp;

	std::ostringstream ostri;
	ostri  << 1900 + ltm->tm_year
			<< "-" << 1 + ltm->tm_mon  // tm_mon is in the range [0, 11], so 1 must be added to get real months
			<< "-" << ltm->tm_mday
			<< "T" << ltm->tm_hour << "-" << ltm->tm_min << "-" << ltm->tm_sec; // << "CEST"; FIXME: check if time zone is available on data file

	humanTimestamp = ostri.str();
	//std::cout << "'human readable' timestamp: " << m_humanTimestamp << std::endl;
	return humanTimestamp;
}

