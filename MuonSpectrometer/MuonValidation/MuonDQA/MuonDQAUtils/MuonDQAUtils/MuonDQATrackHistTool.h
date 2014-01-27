/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONDQATRACKHISTTOOL_H
#define MUON_MUONDQATRACKHISTTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/TrackCollection.h"
//#include "TrkAlignEvent/AlignTrack.h"

#include "MuonDQAUtils/MuonDQAHitHistTool.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

static const InterfaceID IID_MuonDQATrackHistTool("Muon::MuonDQATrackHistTool",1,0);


class MsgStream;

namespace Trk {
  class IResidualPullCalculator;
}

namespace Muon {
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
  class MuonSegment;
}

namespace Trk {
  class Track;
  class MeasurementBase;
  class IPropagator;
}

class TH1;
class TH2;

namespace Muon {


  /**
     @brief Tool to fill track/segment histograms
     The tool can be used to:
     - extract information from the track into a data structure where it can be used for further analysis.
     The following information can be extracted
     - track/segment parameters
     - hit local positions, residuals and pulls 
     - hit counts e.g. number of mdt hits, number of mdt chambers, etc.
     - fill the extracted information into a set of histograms

  */
  
  class MuonDQATrackHistTool : public AlgTool {
  public:
    struct TrackData {
      TrackData();
      double chi2;
      double ndof;
      double charge;
      Amg::Vector3D gpos;
      Amg::Vector3D gdir;
      int layers;
      int mlayers;
      MuonDQAHitHistTool::HitData hitData;
    };
    
    struct TrackHistos {
      TrackHistos();
      bool wasBooked;
      bool wasRegistered;
      TH1* chi2;
      TH1* ndof;
      TH1* prob;
      TH1* r;
      TH1* z;
      TH1* phi;
      TH1* eta;
      TH1* dirphi;
      TH1* direta;
      TH1* momentum;
      TH1* pt;
      TH1* layers;
      TH1* mlayers;
      //      TH2* hitsVSphi;
      MuonDQAHitHistTool::HitHistos hitHistos;
    };

    /** @brief a pair of MeasurementBase + TrackParameters is the minimum input to calculate a residual */
    typedef std::pair<const Trk::MeasurementBase*,const Trk::TrackParameters*> HitPair;
    typedef std::vector<HitPair>                                               HitPairVec;
    typedef HitPairVec::const_iterator                                         HitPairCit;
    
  public:
    /** @brief constructor */
    MuonDQATrackHistTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    ~MuonDQATrackHistTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonDQATrackHistTool; }

    /** @brief book a set of track histograms, the prefix is added to the histogram names */
    bool book( TrackHistos& histos, std::string prefix = "", bool doMDT = true, bool doRPC = true, bool doTGC = true, bool doCSC = false ) const; 

    /** @brief fill histograms for a given track */
    bool fill( TrackHistos& histos, const TrackData& trackData ) const;

    /** @brief fill histograms for a given track */
    bool fill( TrackHistos& histos, const Trk::Track& track ) const;

    /** @brief fill histograms for a given segment */
    bool fill( TrackHistos& histos, const MuonSegment& segment ) const;

    /** @brief fill histograms for a given track collection */
    //bool fill( TrackHistos& histos, const TrackCollection& tracks ) const;

    /** @brief fill histograms for a given hit */
    bool fill( MuonDQAHitHistTool::HitHistos& histos, const Trk::Track& track, const double chi2 ) const;

    /** @brief fill histograms for a given MuonSegment */
    bool fill( MuonDQAHitHistTool::HitHistos& histos, const MuonSegment& segment, const double chi2 ) const;

    /** @brief register histograms to a monitoring group */
    //    bool registerHist( ManagedMonitorToolBase::MonGroup&  groupShift, ManagedMonitorToolBase::MonGroup&  groupDebug, ManagedMonitorToolBase::MonGroup&  groupBarrel, ManagedMonitorToolBase::MonGroup&  groupEC, TrackHistos& histos ) const; 
    bool registerHist( ManagedMonitorToolBase::MonGroup&  groupShift, ManagedMonitorToolBase::MonGroup&  groupDebug, ManagedMonitorToolBase::MonGroup&  groupBarrelA, ManagedMonitorToolBase::MonGroup&  groupBarrelC, ManagedMonitorToolBase::MonGroup&  groupEC, ManagedMonitorToolBase::MonGroup&  groupECA, ManagedMonitorToolBase::MonGroup&  groupECC, TrackHistos& histos ) const;

    /** @brief extract hit information from a HitPair vector */
    void extract( MuonDQAHitHistTool::HitData& hitData, const HitPairVec& hits, const double chi2 ) const;

    /** @brief extract all track information from a track */
    void extract( TrackData& trackData, const Trk::Track& track ) const;

    /** @brief extract hit data from a track */
    void extract( MuonDQAHitHistTool::HitData& hitData, const Trk::Track& track, const double chi2 ) const;

  private:

    ToolHandle<Muon::MuonEDMHelperTool>          m_helperTool;
    ToolHandle<Muon::MuonIdHelperTool>           m_idHelperTool;
    ToolHandle<Muon::MuonDQAHitHistTool>         m_hitHistTool;
    ToolHandle<Trk::IResidualPullCalculator>     m_pullCalculator;     //<! tool to calculate residuals and pulls
    ToolHandle<Trk::IPropagator>                 m_propagator;

    //    ToolHandle<Muon::MuonIdHelper> m_MuonIdHelper;
    //    const MuonIdHelper* m_MuonIdHelper;  
    const MuonGM::MuonDetectorManager* m_muonMgr;

    MsgStream* m_log;

  };

}

#endif
