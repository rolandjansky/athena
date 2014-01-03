/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PD_L1TgcTrkExtrapolationFillerTool_h
#define D3PD_L1TgcTrkExtrapolationFillerTool_h

#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/VoidBlockFillerTool.h"
#include "CLHEP/Vector/ThreeVector.h"

class ITrigEffJpsiTools;

namespace Muon { class MuonIdHelperTool; }
namespace Muon { class TgcPrepData; }
namespace D3PD { class L1TgcTrkExtrapolator; }



namespace D3PD {
  class L1TgcTrkExtrapolationFillerTool : public BlockFillerTool<void>
  {
    public:
      static const double M1_Z;
      static const double M2_Z;
      static const double M3_Z;
      static const double EI_Z;
      static const double FI_Z;
      //T.Kanno for NTUP_TRIGMU
      static const double APPROX_RPC2_R_LO;
      static const double APPROX_RPC2_R_HI;
      //T.K

      L1TgcTrkExtrapolationFillerTool(const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent);

      // Standard Gaudi initialize method.
      virtual StatusCode initialize();

      virtual StatusCode book();
      virtual StatusCode fill();

    private:
      typedef std::multimap<int, Amg::Vector3D>  Target;

      enum {
        UNDEF,
        COMBINED,
        INNER_EXTRAPOLATED,
        INDET,
        BIASED,
        UNBIASED
      };


      StatusCode fillTargetDistanceTgc(const std::string&,
                                       Target& target);
      StatusCode fillTargetDistanceRpc(const std::string&,
                                       Target& target);
      StatusCode fillExtrapolation(const Target& target);

      ToolHandle<Muon::MuonIdHelperTool> m_idHelper;
      ToolHandle<ITrigEffJpsiTools> m_trigEffJpsiTools;
      ToolHandle<D3PD::L1TgcTrkExtrapolator> m_l1TgcTrkExtrapolator;

      std::string m_tgcPrepDataPrv;
      std::string m_tgcPrepDataCur;
      std::string m_tgcPrepDataNxt;
      std::string m_rpcPrepData;
      std::string m_muonKey;
      std::string m_extrapolationMethod;
      bool m_onlyToTriggerPlane;
      int m_extrapolationType;

      std::vector<int> *m_type;
      std::vector<int> *m_index;
      std::vector<int> *m_size;

      std::vector<std::vector<int> > *m_targetVec;
      std::vector<std::vector<float> > *m_targetDistanceVec;
      std::vector<std::vector<float> > *m_targetEtaVec;
      std::vector<std::vector<float> > *m_targetPhiVec;
      std::vector<std::vector<float> > *m_targetDeltaEtaVec;
      std::vector<std::vector<float> > *m_targetDeltaPhiVec;
      std::vector<std::vector<float> > *m_targetPxVec;
      std::vector<std::vector<float> > *m_targetPyVec;
      std::vector<std::vector<float> > *m_targetPzVec;
  };
}
#endif
/* eof */
