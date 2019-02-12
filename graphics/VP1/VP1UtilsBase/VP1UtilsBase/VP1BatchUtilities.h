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


#ifndef VP1UTILSBASE_VP1BatchUtilities
#define VP1UTILSBASE_VP1BatchUtilities

//NB: There should never be any Qt (or Athena of course) includes in this file!!!
#include <string>
#include <vector>


class VP1BatchUtilities
{
public:
	VP1BatchUtilities(std::vector<std::string> files);
	~VP1BatchUtilities() {};

	std::string getRandomConfigFile();

	static void overlayATLASlogo();
	static void overlayEventDetails(unsigned long runNumber, unsigned long eventNumber, std::string humanTimestamp);

private:
	std::vector<std::string> m_files;
	int m_indexFile;
	int m_lastIndexFile;
};

#endif
