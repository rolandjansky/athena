/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1Gui                           //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1GUI_H
#define VP1GUI_H

//NB: There should never be any Qt (or Athena of course) includes in this file!!!

#include <string>
#include <vector>
#include <cstdint> // for uint64_t (unsigned long long)

class StoreGateSvc;
class IToolSvc;
class ISvcLocator;

class VP1Gui {
public:

  VP1Gui(StoreGateSvc* sg, StoreGateSvc* detstore,
	 ISvcLocator* svclocator,IToolSvc*toolSvc,
	 const std::vector<std::string>& initialvp1files=std::vector<std::string>(),
	 const std::string& initialCruiseMode = "NONE", unsigned initialCruiseSeconds = 10,
	 const std::string& singleEventSource = "", const std::string& singleEventLocalTmpDir = "",
	 unsigned localFileCacheLimit = 10,
	 const std::vector<std::string>& availableLocalInputDirectories = std::vector<std::string>() );
  ~VP1Gui();

  //We can check the validity of the argument already during algorithm initialise:
  bool argumentsAreValid() const;

  //Called in the first algorithm refresh to launch the gui:
  void init();
  bool hasBeenInitialised();

  //Called every algorithm refresh:
  bool executeNewEvent( const int& run,
			const uint64_t & event,
			const unsigned& triggerType = 0,//When available
			const unsigned& time = 0 //When available (seconds since 1970)
			);
  //Called in algorithm finalise:
  void cleanup();

  std::vector<std::string> userRequestedFiles();

  //Called when handling end-of-file incident (in multi-file mode)
  std::string nextRequestedEventFile() const;//When mode where next input
				             //file is chosen dynamically,
				             //the choice is reported
				             //here. If empty is returned,
				             //the algorithm is expected to
				             //just take the latest file
				             //from the directory.
private:

  VP1Gui(const VP1Gui & );
  VP1Gui & operator= (const VP1Gui & );

  class Imp;
  Imp * d;

};

#endif
