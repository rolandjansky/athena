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

class IAtRndmGenSvc;

namespace CLHEP {
  class HepRandomEngine;
}

template <class TYPE> class SvcFactory;

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

class LArG4ShowerLibSvc : public AthService, virtual public ILArG4ShowerLibSvc {

public:

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_ILArG4ShowerLibSvc; }

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    // Query the interfaces.
    //   Input: riid, Requested interface ID
    //          ppvInterface, Pointer to requested interface
    //   Return: StatusCode indicating SUCCESS or FAILURE.
    // N.B. Don't forget to release the interface after use!!!
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    virtual bool                    checkLibrary(G4int particleCode, int detectorTag);

    //! return list of energy depositions for given track (interface implementation)
    virtual std::vector<EnergySpot> getShower(const G4FastTrack& track, int detectorTag);

    virtual double                  getContainmentZ(const G4FastTrack& track, int detectorTag);
    virtual double                  getContainmentR(const G4FastTrack& track, int detectorTag);

    friend class SvcFactory<LArG4ShowerLibSvc>;

    LArG4ShowerLibSvc(const std::string& name, ISvcLocator* svc);

    virtual ~LArG4ShowerLibSvc();
	
private:

    //! get shower library from StoreGate by track (using current volume name)
    const ShowerLib::IShowerLib* getShowerLib(G4int particleCode, int detectorTag);

    typedef std::map<int, const ShowerLib::IShowerLib*> libmap;
    libmap m_libraryMap;                                              //!< mapping StoreGate key to handle in StoreGate
    typedef std::map<const ShowerLib::IShowerLib*, ShowerLib::ShowerLibStatistics*> statmap;
    statmap m_statisticsMap;
    std::map<int,std::string> m_locations;
    std::map<std::string,int> m_detmap;

    StringArrayProperty      m_fileNameList;                          //!< property, list of library files
    StringProperty           m_rndmEngineName;                        //!< property, name of athena RNG engine
    
    ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc;
    CLHEP::HepRandomEngine *m_rndmEngine;
};

#endif // G4SHOWERLIBSVC_G4SHOWERLIBSVC_H

