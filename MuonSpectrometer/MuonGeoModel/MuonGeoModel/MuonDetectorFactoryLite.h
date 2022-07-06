/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDetectorFactoryLite_H
#define MuonDetectorFactoryLite_H

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "GeoModelRead/ReadGeoModel.h"
#include <string>

class GeoPhysVol;
class StoreGateSvc;
class IRDBAccessSvc;

namespace MuonGM {

    class MuonDetectorFactoryLite : public GeoVDetectorFactory {


      public:
        // Constructor:
        MuonDetectorFactoryLite(StoreGateSvc *pDetStore, GeoModelIO::ReadGeoModel * sqliteReader);

        // Destructor:
        ~MuonDetectorFactoryLite();

        // Creates the raw geometry tree: required,
        virtual void create(GeoPhysVol *world) override;

        virtual const MuonDetectorManager *getDetectorManager() const override;
        MuonDetectorManager *getDetectorManager();


        inline void setRDBAccess(IRDBAccessSvc *access);


      private:

        MuonDetectorManager* m_manager{nullptr};
        StoreGateSvc *m_pDetStore{nullptr};
        IRDBAccessSvc *m_pRDBAccess{nullptr};
	GeoModelIO::ReadGeoModel *m_sqliteReader;
    };


    void MuonDetectorFactoryLite::setRDBAccess(IRDBAccessSvc *access) { m_pRDBAccess = access; }
    

} // namespace MuonGM

#endif
