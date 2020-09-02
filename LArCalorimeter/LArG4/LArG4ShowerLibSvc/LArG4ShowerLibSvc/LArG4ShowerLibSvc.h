/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef G4SHOWERLIBSVC_G4SHOWERLIBSVC_H
#define G4SHOWERLIBSVC_G4SHOWERLIBSVC_H

#include "LArG4ShowerLibSvc/ILArG4ShowerLibSvc.h"

#include "AthenaBaseComps/AthService.h"
#include "LArG4Code/EnergySpot.h"
#include "G4FastTrack.hh"

#include "GaudiKernel/ServiceHandle.h"

#include <string>
#include <vector>
#include <map>

namespace ShowerLib {
  class IShowerLib;
  class ShowerLibStatistics;
}


  /**
   *
   *   @short Class for shower library service
   *
   *          Converts tracks into a list of energy deposits 
   *          using frozen shower libraries.
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Sasha Glazov, DESY Hamburg, Germany
   *
   * @version $Id: LArG4ShowerLibSvc.h 576157 2013-12-18 16:57:49Z gsedov $
   *
   */

class LArG4ShowerLibSvc: public extends<AthService, ILArG4ShowerLibSvc> {

public:

    LArG4ShowerLibSvc(const std::string& name, ISvcLocator* svc);

    virtual ~LArG4ShowerLibSvc();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual bool                    checkLibrary(G4int particleCode, int detectorTag);

    //! return list of energy depositions for given track (interface implementation)
#ifdef DEBUG_FrozenShowers
    virtual std::vector<EnergySpot> getShower(const G4FastTrack& track, int detectorTag);
#else
    virtual std::vector<EnergySpot> getShower(const G4FastTrack& track, int detectorTag) const;
#endif
    virtual double                  getContainmentZ(const G4FastTrack& track, int detectorTag);
    virtual double                  getContainmentR(const G4FastTrack& track, int detectorTag);

private:

    //! get shower library from StoreGate by track (using current volume name)
    const ShowerLib::IShowerLib* getShowerLib(G4int particleCode, int detectorTag) const;

    typedef std::map<int, const ShowerLib::IShowerLib*> libmap;
    libmap m_libraryMap;                                              //!< mapping StoreGate key to handle in StoreGate
    typedef std::map<const ShowerLib::IShowerLib*, ShowerLib::ShowerLibStatistics*> statmap;
    statmap m_statisticsMap;
    std::map<int,std::string> m_locations;
    std::map<std::string,int> m_detmap;

    StringArrayProperty      m_fileNameList;                          //!< property, list of library files
    
};

#endif // G4SHOWERLIBSVC_G4SHOWERLIBSVC_H

