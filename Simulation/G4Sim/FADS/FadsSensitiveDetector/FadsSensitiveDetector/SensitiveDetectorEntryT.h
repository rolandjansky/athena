/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SensitiveDetectorEntryT_H
#define SensitiveDetectorEntryT_H

#include "FadsSensitiveDetector/SensitiveDetectorEntry.h"
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include <string>

namespace FADS {

template <class T> class SensitiveDetectorEntryT: public SensitiveDetectorEntry {
private:
	FadsSensitiveDetector* theSensitiveDetector;
public:

	SensitiveDetectorEntryT<T>(std::string n):
					SensitiveDetectorEntry(n)
	{
//		std::cout<<" this is the templated c-tor"<<std::endl;
		theSensitiveDetector=0;
	}
	void Construct(std::string n, bool allowMods=false)
	{
		if (!theSensitiveDetector) 
		{
			theSensitiveDetector=new T(n);
			theSensitiveDetector->Register();
			theSensitiveDetector->setAllowMods(allowMods);
		}
//		std::cout<<"this is construct "<<n<<std::endl;
	}

};

}	// end namespace

#endif	// SensitiveDetectorEntryT
	
