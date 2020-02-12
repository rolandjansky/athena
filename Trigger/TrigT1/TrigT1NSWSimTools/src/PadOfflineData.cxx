/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


//Local includes
#include "TrigT1NSWSimTools/PadOfflineData.h"
#include "TrigT1NSWSimTools/sTGCenumeration.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "TrigT1NSWSimTools/tdr_compat_enum.h"
#include "TVector3.h"

#include <string>
#include <math.h>

namespace NSWL1 {

    PadOfflineData::PadOfflineData(Identifier id, float time, uint16_t bc_tag, const MuonGM::MuonDetectorManager* detMgr) :
    PadData(),
    m_id(id),m_detMgr(detMgr),m_time(time),m_bc_tag(bc_tag)
    {
    }

    PadOfflineData::~PadOfflineData() {
        // PaddOfflineData doesn't own the helper, so nothing to do here
    }
 
    //! methods for retrieving the bare data
    uint32_t   PadOfflineData::id()       const { return m_id.get_identifier32().get_compact(); }
    Identifier PadOfflineData::Identity()     const {return m_id;}
    uint16_t   PadOfflineData::BC_Tag()   const { return m_bc_tag; }
    float      PadOfflineData::time()     const { return m_time; }

    //! helper  methods decoding the hit fragment position within the NSW detector
    int PadOfflineData::sideId()      const { 
        if (m_detMgr)  return (m_detMgr->stgcIdHelper()->stationEta(m_id)<0)? 0 : 1;
        return -1;
    }

    int PadOfflineData::moduleId()    const {
        if (m_detMgr)  return std::abs(m_detMgr->stgcIdHelper()->stationEta(m_id));
        return -1;
    }

    int PadOfflineData::sectorId()    const {
        if(m_detMgr) { return m_detMgr->stgcIdHelper()->stationPhi(m_id); }
        return -1;
    }

    int PadOfflineData::sectorType()    const {
        if(m_detMgr) { return m_detMgr->stgcIdHelper()->isSmall(m_id) ? 0 : 1; }
        return -1;
    }

    int PadOfflineData::multipletId() const {
        if (m_detMgr) return m_detMgr->stgcIdHelper()->multilayer(m_id);
        return -1;
    }

    int PadOfflineData::gasGapId()    const {
        if (m_detMgr) return m_detMgr->stgcIdHelper()->gasGap(m_id);
        return -1;
    }

    int PadOfflineData::channelId()   const {
        if (m_detMgr) return m_detMgr->stgcIdHelper()->channel(m_id);
        return -1;
    }

    int PadOfflineData::padEtaId()    const {
        if (m_detMgr) return m_detMgr->stgcIdHelper()->padEta(m_id);
        return -1;
    }

    int PadOfflineData::padPhiId()    const {
        if (m_detMgr) return m_detMgr->stgcIdHelper()->padPhi(m_id);
        return -1;
    }

    float PadOfflineData::stationPhiAngle() const{
        float phiAngle=-999;
        switch( this->sectorType() ){

            case(0)://S
                phiAngle=(2*this->sectorId()-1 )*(M_PI/8);
                break;

            case(1)://L
                phiAngle=(this->sectorId()-1)*(M_PI/4);
                break;

            default:
                break;
        }

        if(phiAngle > M_PI) phiAngle-=2*M_PI;
        
        return phiAngle;
    }

    int PadOfflineData::padNumber() const {//pad numbers from Y.R 
        if(m_detMgr == nullptr ) return nullPadNumber;
        int modId=moduleId();//id to use pad encoding {1,2,3}/S {4,5,6} L
        if(sectorType() ==1) modId+=3;
        return 1000*modId+(padPhiId()-1)*18+padEtaId();
    }
    int PadOfflineData::triggerSectorNumber()  const{
        if(m_detMgr == nullptr ) return -1;
        if(this->sectorType() == 0 ){//S
            return this->sectorId()*2;
        }
        else{//L
            return 2*this->sectorId()-1;
        }        
    }

    void PadOfflineData::fillGeometricInformation(){
        if (m_detMgr ==nullptr) return;
        const MuonGM::sTgcReadoutElement* rdoEl = m_detMgr->getsTgcReadoutElement(this->Identity());
        const Trk::PlaneSurface &surface = rdoEl->surface(this->Identity());
        std::vector<Amg::Vector2D> local_pad_corners;
        //From MuonPadDesign... read pad local corners
        bool check=rdoEl->padCorners(this->Identity(),local_pad_corners);
        if(! check ){
            std::cout<<"Unable to get pad corners!"<<std::endl;
            return;
        }
        Amg::Vector3D pad_corner_global;
        for(unsigned int i=0; i<4; i++) {
            surface.localToGlobal(local_pad_corners.at(i), pad_corner_global, pad_corner_global);
            this->m_cornerXyz[i][0] = pad_corner_global.x(); 
            this->m_cornerXyz[i][1] = pad_corner_global.y(); 
            this->m_cornerXyz[i][2] = pad_corner_global.z(); 
        }
        return;
    }


}
