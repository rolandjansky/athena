/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PD_L1TgcTandpFillerTool_h
#define D3PD_L1TgcTandpFillerTool_h

#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/VoidBlockFillerTool.h"

namespace Rec { class TrackParticle; }
namespace Trk { class VxCandidate; }
namespace Trk { class IVertexFitter; }
namespace Trk { class V0Tools; }
namespace InDet { class VertexPointEstimator; }
namespace InDet { class ConversionFinderUtils; }



namespace D3PD {
  class L1TgcTandpFillerTool : public BlockFillerTool<void>
  {
    public:
      L1TgcTandpFillerTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

      // Standard Gaudi initialize method.
      virtual StatusCode initialize();

      virtual StatusCode book();
      virtual StatusCode fill();

      static const double MUON_MASS;
      static const double JPSI_MASS;
      static const double Z_MASS;

    private:
      ToolHandle <InDet::VertexPointEstimator> m_vertexPointEstimator;
      ToolHandle <InDet::ConversionFinderUtils> m_conversionFinderUtils;
      ToolHandle <Trk::IVertexFitter> m_vKalVrtFitter;
      ToolHandle <Trk::IVertexFitter> m_v0VertexFitter;
      ToolHandle <Trk::V0Tools> m_v0Tools;

      std::string m_muonSgKey;
      std::string m_trackSgKey;
      std::string m_primaryVertexSgKey;
      double m_minPt;
      bool m_hasOppositeCharge;
      bool m_useV0Fitter;
      double m_oniaMassLowerLimit;
      double m_oniaMassUpperLimit;
      double m_zMassLowerLimit;
      double m_zMassUpperLimit;

      std::vector<int> *m_tag_index;
      std::vector<int> *m_probe_index;
      std::vector<int> *m_vertex_ndof;
      std::vector<float> *m_vertex_chi2;
      std::vector<float> *m_vertex_posX;
      std::vector<float> *m_vertex_posY;
      std::vector<float> *m_vertex_posZ;
      std::vector<float> *m_vertex_probability;
      std::vector<float> *m_v0_mass;
      std::vector<float> *m_v0_mass_error;
      std::vector<float> *m_v0_mass_probability;
      std::vector<float> *m_v0_px;
      std::vector<float> *m_v0_py;
      std::vector<float> *m_v0_pz;
      std::vector<float> *m_v0_pt;
      std::vector<float> *m_v0_pt_error;

      std::vector<int> *m_nVerticies;
      std::vector<std::vector<int> > *m_vertex_index_vec;
      std::vector<std::vector<float> > *m_lxy_vec;
      std::vector<std::vector<float> > *m_lxy_error_vec;
      std::vector<std::vector<float> > *m_tau_vec;
      std::vector<std::vector<float> > *m_tau_error_vec;

      bool isCandidate(const Rec::TrackParticle* tag,
                       const Rec::TrackParticle* probe) const;

      StatusCode getVxCandidates(std::vector<const Trk::VxCandidate*>& vertices) const;

      Trk::VxCandidate* doVertexing(const Rec::TrackParticle* tag,
                                    const Rec::TrackParticle* probe) const;
  };
}
#endif
/* eof */
