/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1BatchUtilities              //
//                                                         //
//  Author: Riccardo Maria Bianchi <rbianchi@cern.ch>      //
//                                                         //
//  Initial version: Jan 2019                              //
//                                                         //
/////////////////////////////////////////////////////////////


#include "VP1UtilsBase/VP1BatchUtilities.h"

#include <string>
#include <vector>
#include <iostream>
#include <random> // C++11
#include <iomanip>
#include <functional>



VP1BatchUtilities::VP1BatchUtilities(std::vector<std::string> files) : m_indexFile(0), m_lastIndexFile(0)
{
	m_files = files;
	std::cout << "Got vector of " << files.size() << " items" << std::endl;
	// Iterate and print values of vector
	for(std::string n : m_files) {
	        std::cout << n << '\n';
	}
}


std::string VP1BatchUtilities::getRandomConfigFile()
{
	std::cout <<"VP1BatchUtilities::getRandomConfigFile()" << std::endl;

	std::string configFile;


	int nConfigFiles = m_files.size();
	std::cout << " ===> # config files: " << nConfigFiles << std::endl;

	// setup random generator in [0, nConfigFiles]
    int nPositions = nConfigFiles - 1;
	auto seed = std::random_device{}();
	auto randomDist = std::bind(std::uniform_int_distribution<int>(0, nPositions ),
			std::mt19937(seed));

	// get a random index,
	m_indexFile = randomDist();
	// avoiding having the same index in a row
	while ( m_indexFile == m_lastIndexFile ) {
		m_indexFile = randomDist();
	}
	m_lastIndexFile = m_indexFile;
	std::cout << " ===> random index: " << m_indexFile << std::endl;

	configFile = m_files[m_indexFile];
	std::cout << " ===> random file: " << configFile << std::endl;

	return configFile;

}

// Overlay the ATLAS logo to the image
void VP1BatchUtilities::overlayATLASlogo()
{
    /*
     * different example commands for logo overlay:
     *
	 * std::string commandStr =  "convert -composite `cat latest_vp1image` $TestArea/InstallArea/share/ATLAS-Logo-New_300pixels.png -geometry 150x150+0+0 -depth 8 test.png"; // this set the logo size to 150px and it draws it at (0,0)px
	 * std::string commandStr =  "convert -composite `cat latest_vp1image` $TestArea/InstallArea/share/ATLAS-Logo-New_300pixels.png -geometry +10+10 -depth 8 test.png"; // this uses the original logo size and it draws it at (10,10)px
     */

	std::string commandStr = "convert -composite `cat latest_vp1image` $TestArea/InstallArea/share/ATLAS-Logo-New_300pixels.png -geometry +10+10 -depth 8 `cat latest_vp1image`";

	std::cout << " ===> overlay the ATLAS logo: " << commandStr << std::endl;
	try {
		system(commandStr.c_str());
	} catch (std::runtime_error& err) {
		std::cout << "Exception caught: " << err.what() << std::endl;
		std::cout << "Unable to run 'convert'!" << std::endl;
	}
}

// Overlay the event details to the image
// it replaces the original image with a version having event details on it
void VP1BatchUtilities::overlayEventDetails(unsigned long runNumber, unsigned long eventNumber, std::string humanTimestamp)
{

	std::string nRun = std::to_string(runNumber);
	std::string nEvent = std::to_string(eventNumber);

	/* 
     * example of different bash command for event details overlay:
     *
	 * nRun=0; nEvent=4;                   img=`cat latest_vp1image`; width=$(identify -format %W ${img}); width=$(( ${width} * 3 / 10 )); convert -background '#0008' -gravity west -fill white -size ${width}x80  -font Courier -density 72 -pointsize 18 -interline-spacing 4 caption:'Run number: '${nRun}'\nEvent number: '${nEvent}'\n2015-02-02, 15:10:00' ${img} +swap -gravity SouthEast -composite ${img}-30
	 * nRun=0; nEvent=4; timestamp='ciao'; img=`cat latest_vp1image`; width=$(identify -format %W ${img}); width=$(( ${width} * 3 / 10 )); convert -background '#0008' -gravity west -fill white -size ${width}x80  -font Courier -density 72 -pointsize 18 -interline-spacing 4 caption:'Run number: '${nRun}'\nEvent number: '${nEvent}'\n'${timestamp}         ${img} +swap -gravity SouthEast -composite ${img}-31
	 * nRun=0; nEvent=4; timestamp='2015-02-02T10:10:00'; img=`cat latest_vp1image`; width=$(identify -format %W ${img}); width=$(( ${width} * 3 / 10 )); convert -background '#0008' -gravity west -fill white -size ${width}x80  -font Courier -density 72 -pointsize 18 -interline-spacing 4 caption:'Run number: '${nRun}'\nEvent number: '${nEvent}'\n'${timestamp} ${img} +swap -gravity SouthEast -composite ${img}-36
    */

	std::string commandStr;

	// setting bash variables
	commandStr += "nRun="+nRun+"; ";
	commandStr += "nEvent="+nEvent+"; ";
	if (humanTimestamp != "0") commandStr += "timestamp='"+humanTimestamp+"'; "; // 'timestamp' must not have white spaces

	// get input image
	commandStr += "img=`cat latest_vp1image`; ";           // get the filename of the latest image produced
	commandStr += "width=$(identify -format %W ${img}); "; // get the width of the image
	commandStr += "width=$(( ${width} * 3 / 10 )); ";      // set the caption width as a fraction of the image width

	// ImageMagik 'convert' command settings. (ImageMagik is installed by default on SLC LXPLUS machines)
	commandStr = commandStr
			+ "convert "
			+  "-background '#0008' "  // semi-transparent grey bkg
			+  "-geometry +20+20 "     // set an offset to the label position
			+  "-gravity West "        // set text position inside the caption space (justification)
			+  "-fill white "          // text font color
			+  "-size ${width}x80 "    // set text size relative to 'width'

			+  "-font Courier "        // text font
			+  "-density 72 "          // dots-per-inch resolution
			+  "-pointsize 18 "        // text size in points
			//+  "-interline-spacing 4 " // additional number of pixels between lines of text (only with ImageMagik >= 6.5.5-8!!! Lxplus has 6.7 but not all SLC6 machines...)

			//+  "caption:'Run number: ${nRun}' " // set the caption text
			//+  (m_timeStamp > 0 ? "caption:'Run number: '${nRun}'\\nEvent number: '${nEvent}'\\n'${timestamp} " : "caption:'Run number: '${nRun}'\\nEvent number: '${nEvent}'\\n'${timestamp} ") // set the caption text; '\n' needs to be double-escaped while passed to system()
			+  "caption:'Run number: '${nRun}'\\nEvent number: '${nEvent}'\\n'${timestamp} " // set the caption text; '\n' needs to be double-escaped while passed to system()

			+  "${img} "             // imput image
			+  "+swap "
			+  "-gravity SouthEast "   // set overall caption position
			+  "-composite "
			+  "${img}";              // output image: here we replace the original image


	std::cout << " ===> overlay the event details: " << commandStr << std::endl;
	try {
		system(commandStr.c_str());
	} catch (std::runtime_error& err) {
		std::cout << "Exception caught: " << err.what() << std::endl;
		std::cout << "Unable to run 'convert'!" << std::endl;
	}
}


