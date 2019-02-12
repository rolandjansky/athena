/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1AlgsBatch/VP1BatchOnLatestEvent.h"

#include "VP1UtilsBase/VP1FileUtilities.h"

#include "StorageSvc/DbType.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventIncident.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <unistd.h>
#include <ctime> /* time_t, time, ctime */
#include <random> // C++11
#include <string>




VP1BatchOnLatestEvent::VP1BatchOnLatestEvent(const std::string& name, ISvcLocator* svcLocator):
AthAlgorithm(name, svcLocator),
//  m_runNumber(0),
//  m_eventNumber(0),
//  m_timeStamp(0),
//  m_humanTimestamp(""),
m_evtInfoDone(false),
m_eventInfo{},
m_nEvent(0),
m_indexFile(0),
m_lastIndexFile(0)
{
	declareProperty("VP1ConfigFile", m_inputVP1CfgFile="");
	declareProperty("DestinationDirectory", m_destinationDir=""); // produce files in the run directory by default
	declareProperty("InputDirectory", m_inputDir=""); // the directory where the input data files (e.g. ESDs) are stored
	declareProperty("UseRandomVP1ConfigFile", m_isGetRandomFile = false); // get random configuration files if TRUE

	//  declareProperty("MaxNumberOfFiles", m_maxProducedFiles=5);     // keep 5 event files in the run directory
}

// TODO: add  DestinationDirectory as argument to the -batch VP1 command option, so we can configure output folder from JobOption


VP1BatchOnLatestEvent::~VP1BatchOnLatestEvent()
{
}

StatusCode VP1BatchOnLatestEvent::initialize()
{
	msg(MSG::INFO) << " in initialize() " << endmsg;

	StatusCode result = StatusCode::SUCCESS;

	// use the incident service to register a handle
	IIncidentSvc* incsvc = 0;
	StatusCode status = service("IncidentSvc", incsvc, true);

	if(status.isFailure() || incsvc==0)
		msg(MSG::WARNING) << "Unable to get IncidentSvc!" << endmsg;
	else
		incsvc->addListener(this, "BeginEvent", 0);

	return result;
}

StatusCode VP1BatchOnLatestEvent::execute()
{
	msg(MSG::DEBUG) <<" in execute() " << endmsg;



	//----------------------------
	// Event information
	//---------------------------
	m_eventInfo = 0; //NOTE: Everything that comes from the storegate direct from the input files is const!

	// ask the event store to retrieve the xAOD EventInfo container
	//CHECK( evtStore()->retrieve( m_eventInfo, "EventInfo") );  // the second argument ("EventInfo") is the key name
	//CHECK( evtStore()->retrieve( m_eventInfo, "McEventInfo") );  // the second argument ("McEventInfo") is the key name
	//	   CHECK( evtStore()->retrieve( m_eventInfo) );
	StatusCode status = evtStore()->retrieve( m_eventInfo);
	// if there is only one container of that type in the xAOD (as with the EventInfo container), you do not need to pass
	// the key name, the default will be taken as the only key name in the xAOD

	if(status.isSuccess() && m_eventInfo!=0) {
		m_evtInfoDone = true;
	}

	if (m_eventInfo) getEventDetails();

	return StatusCode::SUCCESS;
}

StatusCode VP1BatchOnLatestEvent::finalize()
{
	msg(MSG::VERBOSE) <<"in finalize() " << endmsg;

	// Let VP1FileUtilities handle the output of the last event
	if(m_nEvent) {
		makeEventDisplay();
	}

	return StatusCode::SUCCESS;
}


void VP1BatchOnLatestEvent::handle(const Incident& inc)
{
	msg(MSG::INFO) << "Handling incident '" << inc.type() << "'" <<  endmsg;

	const EventIncident* eventInc  = dynamic_cast<const EventIncident*>(&inc);
	if(eventInc == 0) {
		msg(MSG::WARNING) << " Unable to cast incident type" << endmsg;
		return;
	} else {
		msg(MSG::DEBUG) << " Event incident casting successful" << endmsg;
	}

	// Let VP1FileUtilities handle the output of the previous event
	if(m_nEvent) {
		makeEventDisplay();
	}
	m_nEvent++;
}



void VP1BatchOnLatestEvent::makeEventDisplay() {

	// if the user specified empty config file name and declared 'VP1BatchOnLatestEvent.UseRandomVP1ConfigFile=True' in the jobOption
	if (m_isGetRandomFile) {
		msg(MSG::INFO)
				<< "--> RANDOM MODE: Using a random VP1 configuration file..."
				<< endmsg;
		m_inputVP1CfgFile = getRandomConfigFile();
	}

	msg(MSG::INFO) << "--> Input VP1 Configuration file: " << m_inputVP1CfgFile
			<< endmsg;

	// build the command to launch VP1-Batch on the latest-produced ESD file
	std::string commandStr = "vp1 -batch";

	// add custom output folder, if user specified it
	if (m_destinationDir != "") {
		msg(MSG::INFO) << " --> Using user-defined output folder: "
				<< m_destinationDir << endmsg;
		commandStr += " -batch-output-folder=" + m_destinationDir;
	}

	commandStr += " `cat latest_vp1event` " + m_inputVP1CfgFile;

	bool vp1OK = false;
	msg(MSG::INFO) << " ===> launching VP1-Batch: " << commandStr << endmsg;
	try {
		system(commandStr.c_str());
		vp1OK = true;
	} catch (std::runtime_error& err) {
		msg(MSG::WARNING) << "Exception caught: " << err.what() << endmsg;
		msg(MSG::WARNING)
				<< "Unable to launch VP1-Batch on the latest-produced event file"
				<< endmsg;
	}
	if (vp1OK) {
		// Overlay the ATLAS logo to the image
		overlayATLASlogo();
		// Overlay the event details to the image
		overlayEventDetails();
	}
}



std::string VP1BatchOnLatestEvent::getRandomConfigFile()
{
	msg(MSG::DEBUG) <<" in getRandomConfigFile() " << endmsg;


	std::string configFile;

	// a list of possible config files, which are stored in the "share/" folder
	std::vector<std::string> configFiles;
	configFiles.push_back("vp1_conf_ATLASatHOME_truth_event_wTRTGeo.vp1");
	configFiles.push_back("vp1_conf_ATLASatHOME_truth_event_wTRTGeo_IAview.vp1");
	configFiles.push_back("vp1_conf_ATLASatHOME_truth_event_wTRTGeo_wBarrel.vp1");
	configFiles.push_back("vp1_conf_ATLASatHOME_truth_event_wPixel_wSCT_wOpenCalo_wGeo_3D.vp1");
	configFiles.push_back("vp1_conf_ATLASatHOME_truth_event_wPixel_wSCT_wGeo_3D.vp1");
	configFiles.push_back("vp1_conf_ATLASatHOME_truth_event_wPixel_3D.vp1");
	configFiles.push_back("vp1_conf_ATLASatHOME_truth_event_wIDGeo.vp1");
	configFiles.push_back("vp1_conf_ATLASatHOME_truth_event_wGeo_frontView.vp1");

	int nConfigFiles = configFiles.size();
	msg(MSG::DEBUG) << " ===> # config files: " << nConfigFiles << endmsg;
    int nPositions = nConfigFiles - 1;

	// setup random generator in [0, nConfigFiles]
	auto seed = std::random_device{}();
	auto randomDist = std::bind(std::uniform_int_distribution<int>(0, nPositions ),
			std::mt19937(seed));

	// get a random index,
	// avoiding having the same index in a row
	while ( m_indexFile == m_lastIndexFile )
		m_indexFile = randomDist();
	m_lastIndexFile = m_indexFile;
	msg(MSG::DEBUG) << " ===> random index: " << m_indexFile << endmsg;

	//std::string configFile = "vp1_conf_ATLASatHOME_truth_event_wTRTGeo.vp1";
	configFile = configFiles[m_indexFile];
	msg(MSG::DEBUG) << " ===> random file: " << configFile << endmsg;

	return configFile;

}

// Overlay the ATLAS logo to the image
void VP1BatchOnLatestEvent::overlayATLASlogo()
{
	//std::string commandStr =  "convert -composite `cat latest_vp1image` $TestArea/InstallArea/share/ATLAS-Logo-New_300pixels.png -geometry 150x150+0+0 -depth 8 test.png"; // this set the logo size to 150px and it draws it at (0,0)px
	//std::string commandStr =  "convert -composite `cat latest_vp1image` $TestArea/InstallArea/share/ATLAS-Logo-New_300pixels.png -geometry +10+10 -depth 8 test.png"; // this uses the original logo size and it draws it at (10,10)px
	std::string commandStr = "convert -composite `cat latest_vp1image` $TestArea/InstallArea/share/ATLAS-Logo-New_300pixels.png -geometry +10+10 -depth 8 `cat latest_vp1image`";

	msg(MSG::DEBUG) << " ===> overlay the ATLAS logo: " << commandStr << endmsg;
	try {
		system(commandStr.c_str());
	} catch (std::runtime_error& err) {
		msg(MSG::WARNING) << "Exception caught: " << err.what() << endmsg;
		msg(MSG::WARNING) << "Unable to run 'convert'!" << endmsg;
	}
}

// Overlay the event details to the image
// it replaces the original image with a version having event details on it
void VP1BatchOnLatestEvent::overlayEventDetails()
{



	std::string nRun = std::to_string(m_runNumber);
	std::string nEvent = std::to_string(m_eventNumber);

	// bash command:
	// nRun=0; nEvent=4;                   img=`cat latest_vp1image`; width=$(identify -format %W ${img}); width=$(( ${width} * 3 / 10 )); convert -background '#0008' -gravity west -fill white -size ${width}x80  -font Courier -density 72 -pointsize 18 -interline-spacing 4 caption:'Run number: '${nRun}'\nEvent number: '${nEvent}'\n2015-02-02, 15:10:00' ${img} +swap -gravity SouthEast -composite ${img}-30
	// nRun=0; nEvent=4; timestamp='ciao'; img=`cat latest_vp1image`; width=$(identify -format %W ${img}); width=$(( ${width} * 3 / 10 )); convert -background '#0008' -gravity west -fill white -size ${width}x80  -font Courier -density 72 -pointsize 18 -interline-spacing 4 caption:'Run number: '${nRun}'\nEvent number: '${nEvent}'\n'${timestamp}         ${img} +swap -gravity SouthEast -composite ${img}-31
	// nRun=0; nEvent=4; timestamp='2015-02-02T10:10:00'; img=`cat latest_vp1image`; width=$(identify -format %W ${img}); width=$(( ${width} * 3 / 10 )); convert -background '#0008' -gravity west -fill white -size ${width}x80  -font Courier -density 72 -pointsize 18 -interline-spacing 4 caption:'Run number: '${nRun}'\nEvent number: '${nEvent}'\n'${timestamp} ${img} +swap -gravity SouthEast -composite ${img}-36
	std::string commandStr;

	// setting bash variables
	commandStr += "nRun="+nRun+"; ";
	commandStr += "nEvent="+nEvent+"; ";
	if (m_timeStamp > 0) commandStr += "timestamp='"+m_humanTimestamp+"'; "; // 'timestamp' must not have white spaces

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


	msg(MSG::DEBUG) << " ===> overlay the event details: " << commandStr << endmsg;
	try {
		system(commandStr.c_str());
	} catch (std::runtime_error& err) {
		msg(MSG::WARNING) << "Exception caught: " << err.what() << endmsg;
		msg(MSG::WARNING) << "Unable to run 'convert'!" << endmsg;
	}
}


void VP1BatchOnLatestEvent::getEventDetails()
{
	// Update run_number/event_number/time_stamp
	msg(MSG::DEBUG) << "getEventDetails()" << endmsg;

	if(m_evtInfoDone) {

		ATH_MSG_DEBUG(*(m_eventInfo->event_ID()));

		m_eventNumber = m_eventInfo->event_ID()->event_number();
		m_runNumber = m_eventInfo->event_ID()->run_number();
		m_timeStamp = m_eventInfo->event_ID()->time_stamp(); // posix time in seconds from 1970, 32 bit unsigned

		ATH_MSG_DEBUG("run number: "<< m_runNumber
				<< ", event number: "
				<< m_eventNumber << " : timestamp (UNIX): "
				<< m_timeStamp
				<< "] ");
		std::stringstream stream;
		stream << "Event type: user type '"
				<< m_eventInfo->event_type()->user_type()
				<< "' - ";
		for (unsigned int i = 0; i < m_eventInfo->event_type()->n_mc_event_weights (); ++i) {
			stream << " weight " << i << ": " << m_eventInfo->event_type()->mc_event_weight(i);
		}
		ATH_MSG_DEBUG(stream.str());
	}


	if (!m_evtInfoDone) {
		msg(MSG::ERROR) << " Unable to retrieve EventInfo (or McEventInfo) from StoreGate!!!" << endmsg;
		m_eventNumber = 0;
		m_runNumber = 0;
		m_timeStamp = 0;
	}

	// get a human-readable timestamp from UNIX time
	getHumanReadableTimestamp();
}

void VP1BatchOnLatestEvent::getHumanReadableTimestamp()
{
	time_t t_timestamp = m_timeStamp;
	tm *ltm = localtime(&t_timestamp);

	// print various components of tm structure.
	msg(MSG::DEBUG) << "Year: "<< 1900 + ltm->tm_year
			<< " - " << "Month: "<< 1 + ltm->tm_mon<< " - "  // tm_mon is in the range [0, 11], so 1 must be added to get real months
			<< "Day: "<<  ltm->tm_mday
			<< " - " "Time: "<< ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec // << "CEST" FIXME: check if time zone is available on data file
			<< endmsg;

	std::ostringstream ostri;
	ostri  << 1900 + ltm->tm_year
			<< "-" << 1 + ltm->tm_mon  // tm_mon is in the range [0, 11], so 1 must be added to get real months
			<< "-" << ltm->tm_mday
			<< "T" << ltm->tm_hour << "-" << ltm->tm_min << "-" << ltm->tm_sec; // << "CEST"; FIXME: check if time zone is available on data file

	m_humanTimestamp = ostri.str();
	msg(MSG::DEBUG) << "'human readable' timestamp: " << m_humanTimestamp << endmsg;
}

