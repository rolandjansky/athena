/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDDetectorStore_H
#define AGDDDetectorStore_H

class AGDDDetector;
class AGDDTechnology;

#include <map>
#include <string>
#include <vector>

typedef std::map<std::string,AGDDDetector*> detectorList;
typedef std::map<std::string,AGDDTechnology*> technologyList;

class AGDDDetectorStore {
public:
	static AGDDDetectorStore* GetDetectorStore();
	AGDDDetector* GetDetector(std::string s) {return m_the_detectors[s];}
	AGDDTechnology* GetTechnology(std::string s) {return m_the_technologies[s];}
	void RegisterDetector(AGDDDetector*);
	void RegisterTechnology(AGDDTechnology*);
	void PrintAllDetectors();
	detectorList& GetDetectorList() {return m_the_detectors;}
	
	detectorList& GetDetectorList() const ;
	std::vector<AGDDDetector*> GetDetectorsByType(std::string) const ;
	AGDDDetector* GetDetectorByID(std::string) const ;
	template<class T> std::vector<T*> GetDetectorsByType() const ;
	template<class T> T* GetDetectorByID(std::string) const ;
private:
	AGDDDetectorStore();
	detectorList m_the_detectors;
	technologyList m_the_technologies;
};

#endif
