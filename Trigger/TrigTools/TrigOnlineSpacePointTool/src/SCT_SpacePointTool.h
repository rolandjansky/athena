/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SpacePointTool_H
#define SCT_SpacePointTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrigInDetEvent/TrigSiSpacePointCollection.h"

#include "GaudiKernel/ToolHandle.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include <vector>

class ITrigL2LayerNumberTool;
class SCT_GCBuilder;
class SCT_ID;
class TrigTimer;

class SCT_SpacePointTool : public AthAlgTool {
  public:
    typedef std::vector<const InDet::SCT_ClusterCollection*> ClusterCollectionVector;
    typedef std::vector<TrigSiSpacePoint*> SiSpacePointData;

    // Constructor 
    SCT_SpacePointTool( const std::string& type, 
        const std::string& name, 
        const IInterface* parent );

    static const InterfaceID& interfaceID();

    virtual StatusCode initialize() override;

    virtual StatusCode finalize() override;

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

        const InDet::SCT_ClusterCollection* phiWafer() const {return m_wafer_phi;}

        const InDet::SCT_ClusterCollection* uvWafer() const {return m_wafer_uv;}

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
    IdContext m_cntx_sct; 

    bool m_unassociatedPhi;

    TrigSiSpacePointContainer* m_spacepointContainer;
    std::string m_spacepointContainerName;

    double m_xVertex;
    double m_yVertex;
    double m_zVertex;
    double m_xCenter;
    double m_yCenter;

    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
    bool m_useBeamSpot;
    bool m_useOfflineAlgorithm;

    ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

#define SCTSP_NTIMERS 7
    TrigTimer* m_timer[SCTSP_NTIMERS];
    bool m_timers;

};

#endif 
