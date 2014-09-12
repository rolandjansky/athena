/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDDetectorStore_H
#define AGDDDetectorStore_H

class AGDDDetector;

#include <map>
#include <string>

typedef std::map<std::string,AGDDDetector*> detectorList;

class AGDDDetectorStore {
public:
	static AGDDDetectorStore* GetDetectorStore();
	AGDDDetector* GetDetector(std::string) {return 0;}
	void RegisterDetector(AGDDDetector*);
	void PrintAllDetectors();
	detectorList& GetDetectorList() {return the_detectors;}
private:
	AGDDDetectorStore();
	detectorList the_detectors;
};

#endif
