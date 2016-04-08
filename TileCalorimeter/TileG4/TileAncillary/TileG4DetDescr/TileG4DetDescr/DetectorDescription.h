/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorDescription_H
#define DetectorDescription_H

#include <string>
#include <vector>

namespace FADS {

class DetectorDescription {
protected:
	std::string name;
	bool isPointed;
public:
	DetectorDescription(std::string n);
	DetectorDescription(const DetectorDescription&);
	virtual ~DetectorDescription() {}
	bool IsPointed() {return isPointed;}
	void SetPointed() {isPointed=true;}
	virtual void print() {;}
	std::string GetName() const;

private:
        // Avoid coverity warning.
        DetectorDescription& operator= (const DetectorDescription&);
};

}	// end namespace

#endif
