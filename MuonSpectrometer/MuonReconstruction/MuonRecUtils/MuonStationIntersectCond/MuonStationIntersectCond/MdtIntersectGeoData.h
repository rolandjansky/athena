/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONSTATIONINTERSECTCOND_MDTINTERSECTGEODATA_H
#define MUONSTATIONINTERSECTCOND_MDTINTERSECTGEODATA_H
#include <MuonStationIntersectCond/MdtIntersectGeometry.h>
#include <MuonReadoutGeometry/MuonDetectorManager.h>


/**
 * @brief Conditions data to cache the MDT chamber geometry used for the intersections and calculations
 *        of holes in a Mdt Segement  
 */


class MsgStream;
class MdtCondDbData;

namespace Muon{
      class IMuonIdHelperSvc;
      class MdtIntersectGeoData{

          ~MdtIntersectGeoData() = default;
          MdtIntersectGeoData(const MuonGM::MuonDetectorManager* detMgr, const IMuonIdHelperSvc* idHelperSvc, const  MdtCondDbData* cond_data);

          Muon::MuonStationIntersect tubesCrossedByTrack(const Identifier& id, const Amg::Vector3D& pos, const Amg::Vector3D& dir) const;

        /** @brief get geometry description of the given chamber + neighbouring chambers */
        std::vector<const MdtIntersectGeometry*> getStationGeometry(const Identifier& id) const;


        private:          
          std::array<std::unique_ptr<MdtIntersectGeometry>, MuonGM::MuonDetectorManager::MdtRElMaxHash> m_geometry{};

          
          std::vector<Identifier> binPlusneighbours(const Identifier& id) const;


          const IMuonIdHelperSvc* m_idHelperSvc{nullptr};
          const MdtCondDbData* m_dbData{nullptr};

      };
}

// #ifndef GAUDI_NEUTRAL
// namespace Muon {
//     class MdtIntersectGeoData;
// }
// CLASS_DEF(Muon::MdtIntersectGeoData, 4500, 1)
// CLASS_DEF(CondCont<Muon::MdtIntersectGeoData>, 205781622, 0)
// #endif

#endif
