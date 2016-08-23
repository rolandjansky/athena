/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEG4DETDESCR_DETECTORDESCRIPTION_H
#define TILEG4DETDESCR_DETECTORDESCRIPTION_H

#include <string>
#include <vector>

namespace FADS {

  class DetectorDescription {
    protected:
      std::string m_name;
      bool m_isPointed;
    public:
      DetectorDescription(std::string n);
      DetectorDescription(const DetectorDescription&);
      virtual ~DetectorDescription() {}
      bool IsPointed() {
        return m_isPointed;
      }
      void SetPointed() {
        m_isPointed = true;
      }
      virtual void print() {;}
      std::string GetName() const;

    private:
      // Avoid coverity warning.
      DetectorDescription& operator=(const DetectorDescription&);
  };

}	// end namespace

#endif // TILEG4DETDESCR_DETECTORDESCRIPTION_H
