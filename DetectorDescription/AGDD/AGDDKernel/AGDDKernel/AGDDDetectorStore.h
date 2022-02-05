/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
	AGDDDetectorStore();
	AGDDDetector* GetDetector(const std::string& s) {return m_the_detectors[s];}
	AGDDTechnology* GetTechnology(const std::string& s) {return m_the_technologies[s];}
	void RegisterDetector(AGDDDetector*);
	void RegisterTechnology(AGDDTechnology*);
	void PrintAllDetectors();
	detectorList& GetDetectorList() {return m_the_detectors;}
	
        const detectorList& GetDetectorList() const ;
	std::vector<AGDDDetector*> GetDetectorsByType(const std::string&) const ;
	AGDDDetector* GetDetectorByID(const std::string&) const ;
	template<class T> std::vector<T*> GetDetectorsByType() const ;
	template<class T> T* GetDetectorByID(const std::string&) const ;
private:
	detectorList m_the_detectors;
	technologyList m_the_technologies;
};

#endif
