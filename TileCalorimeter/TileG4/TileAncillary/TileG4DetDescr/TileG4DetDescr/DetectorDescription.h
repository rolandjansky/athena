/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEG4DETDESCR_DETECTORDESCRIPTION_H
#define TILEG4DETDESCR_DETECTORDESCRIPTION_H

#include <string>
#include <vector>

#include "CxxUtils/checker_macros.h"

namespace FADS {

  class DetectorDescription {
    protected:
      std::string m_name;
      bool m_isPointed;
    public:
      DetectorDescription(std::string n) ATLAS_CTORDTOR_NOT_THREAD_SAFE;
      DetectorDescription(const DetectorDescription&) ATLAS_CTORDTOR_NOT_THREAD_SAFE;
      virtual ~DetectorDescription() {}
      bool IsPointed() const {
        return m_isPointed;
      }
      void SetPointed() {
        m_isPointed = true;
      }
      virtual void print() const {;}
      std::string GetName() const;

    private:
      // Avoid coverity warning.
      DetectorDescription& operator=(const DetectorDescription&);
  };

}	// end namespace

#endif // TILEG4DETDESCR_DETECTORDESCRIPTION_H
