/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEG4DETDESCR_DETECTORDESCRIPTIONSTORE_H
#define TILEG4DETDESCR_DETECTORDESCRIPTIONSTORE_H

#include <string>
#include <map>

namespace FADS {

  class DetectorDescription;
//class DescriptionMessenger;

  typedef std::map<std::string, DetectorDescription*, std::less<std::string> > DetDescMap;
  typedef std::map<std::string, DetDescMap*, std::less<std::string> > NameSpaces;
  typedef DetDescMap::const_iterator DetDescIterator;

  class DetectorDescriptionStore {
    private:
      DetectorDescriptionStore();
      DetDescMap* m_detDescMap;
      NameSpaces m_nameSpaces;
//	DescriptionMessenger *theMessenger;
      std::string m_oldNameSpace;
      std::string m_currentNameSpace;
    public:
      void UseNameSpace(std::string s);
      void ResetNameSpace();
      bool FindDetectorDescription(std::string s);
      static DetectorDescriptionStore* GetDetectorDescriptionStore();
      void AddDetectorDescription(DetectorDescription *d);
      void RemoveDetectorDescription(std::string nam);
      void ReplaceDetectorDescription(std::string nam, const DetectorDescription *d);
      void PrintDetectorDescription(std::string nam);
      DetectorDescription* GetDetectorDescription(std::string name);
      DetDescIterator DetDescBegin();
      DetDescIterator DetDescEnd();
      void PrintAll();
  };

}	// end namespace

#endif // TILEG4DETDESCR_DETECTORDESCRIPTIONSTORE_H
