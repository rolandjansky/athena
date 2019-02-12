/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1BatchUtilities                //
//                                                         //
//  Author: Riccardo Maria Bianchi <rbianchi@cern.ch>      //
//                                                         //
//  Initial version: Jan 2019                              //
//                                                         //
/////////////////////////////////////////////////////////////


#ifndef VP1UTILSBASE_VP1TimeUtilities
#define VP1UTILSBASE_VP1TimeUtilities


//NB: There should never be any Qt (or Athena of course) includes in this file!!!
#include <ctime> /* time_t, time, ctime */
#include <string>


class VP1TimeUtilities
{
public:
	static std::string getHumanReadableTimestamp(time_t t_timestamp);

private:
	VP1TimeUtilities();
	~VP1TimeUtilities();
};

#endif
