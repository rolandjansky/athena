/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SpacePointTool_H
#define SCT_SpacePointTool_H
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"
#include "TrigOnlineSpacePointTool/SCT_GCBuilder.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetIdentifier/SCT_ID.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>

class TrigSiSpacePoint;
class TrigTimer;
class IBeamCondSvc;
class ITrigL2LayerNumberTool;


class SCT_SpacePointTool : public AthAlgTool {
  public:
    typedef std::vector<const InDet::SCT_ClusterCollection*> ClusterCollectionVector;
    typedef std::vector<TrigSiSpacePoint*> SiSpacePointData;

    // Constructor 
    SCT_SpacePointTool( const std::string& type, 
        const std::string& name, 
        const IInterface* parent );

    static const InterfaceID& interfaceID();

    virtual StatusCode initialize();

    virtual StatusCode finalize();

    // Convert clusters to space points
    StatusCode fillCollections(ClusterCollectionVector& clusterCollData, std::vector<int>&);

  private:

    class PhiUVPair {
      public:
        PhiUVPair(const InDet::SCT_ClusterCollection* wafer_phi, const InDet::SCT_ClusterCollection* wafer_uv):
          m_wafer_phi(wafer_phi), m_wafer_uv(wafer_uv){}

        void addPhi(const InDet::SCT_ClusterCollection* wafer) {
          if (m_wafer_phi != 0)
            std::cout << "Inconsistency when adding phi wafer" <<std::endl;
          m_wafer_phi=wafer;
        }

        void addUV(const InDet::SCT_ClusterCollection* wafer) {
          if (m_wafer_uv != 0)
            std::cout << "Inconsistency when adding uv wafer" <<std::endl;
          m_wafer_uv=wafer;
        }

        const InDet::SCT_ClusterCollection* phiWafer() {return m_wafer_phi;}

        const InDet::SCT_ClusterCollection* uvWafer() {return m_wafer_uv;}

        void prnt() {

          std::cout << std::endl;

          if ( m_wafer_phi != 0) {
            std::cout << " phi wafer:" << std::endl;
            // m_wafer_phi->prnt();
          }
          if ( m_wafer_uv != 0)  {
            std::cout << " uv wafer:" << std::endl;
            // m_wafer_uv->prnt();
          } 
        }
      private:
        const InDet::SCT_ClusterCollection* m_wafer_phi;
        const InDet::SCT_ClusterCollection* m_wafer_uv;
    };

    SCT_GCBuilder* m_builder;

    const SCT_ID* m_id_sct;
    const InDetDD::SiDetectorManager * m_mgr;
    IdContext m_cntx_sct; 

    bool m_unassociatedPhi;

    TrigSiSpacePointContainer* m_spacepointContainer;
    std::string m_spacepointContainerName;

    double m_xVertex;
    double m_yVertex;
    double m_zVertex;
    double m_xCenter;
    double m_yCenter;

    IBeamCondSvc* m_iBeamCondSvc; 
    bool m_useBeamSpot;
    bool m_useOfflineAlgorithm;

    ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;


#define SCTSP_NTIMERS 7
    TrigTimer* m_timer[SCTSP_NTIMERS];
    bool m_timers;

};

#endif 
