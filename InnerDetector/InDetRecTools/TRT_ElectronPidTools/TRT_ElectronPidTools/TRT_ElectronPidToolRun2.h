/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidToolRun2.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRT_ELECTRONPIDTOOLRUN2_H
#define INDETTRT_ELECTRONPIDTOOLRUN2_H

#include "AthenaBaseComps/AthAlgTool.h"       // Exchange IN

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "TrkToolInterfaces/ITRT_ElectronPidTool.h"
#include "TRT_ElectronPidTools/ITRT_ElectronToTTool.h"
#include "TRT_ConditionsData/HTcalculator.h"
#include "TRT_ConditionsNN/TRTPIDNN.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "TRT_ElectronPidTools/ITRT_LocalOccupancy.h"

#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h"


#include <vector>
#include <string>

class AtlasDetectorID;
class Identifier;
class TRT_ID;
namespace InDetDD{ class TRT_DetectorManager; }

namespace InDet{
	class ITRT_LocalOccupancy;
}

class ITRT_StrawSummaryTool;

class ITRT_ToT_dEdx;

namespace Trk {
  class Track;
}

namespace InDet 
{

  /** @class TRT_ElectronPidToolRun2 

      TRT_ElectronPidToolRun2 is a tool for identification of electrons based on information
      mainly from the TRT and partially from the whole ID. Given a track, multiple quantities
      are calculated (see also TrkTrackSummary/TrackSummary.h):

      1: eProbabilityComb - combined probability from High Threshold (HT) and Time-over-
      Threshold (ToT) information (see below).

      2: eProbabilityHT - likelihood calculated using High Threshold (HT) information.
      Sensitive to Transition Radiation (TR) photons emitted by electrons, when traversing
      the TRT radiator material between the straws. Electrons start to emit TR at 500 MeV,
      but only fully at 5 GeV.

      3: eProbabilityToT - likelihood calculated using Time-over-Threshold (ToT) information.
      Sensitive to (increased) ionization by electrons compared to heavier particles (eg. pions,
      protons, etc.). ToT is computed only for straws without a HT hit (to avoid correlations).
      The separation is largest at low momentum, and deminishes with increased energy.

      4: eProbabilityBrem - Electron probability from Brem fitting (DNA).

      5: eProbabilityNN - neural network that combines HT, ToT and other hit- and track-level
      properties into a combined classifier.

      6: TRTTrackOccupancy - fraction of TRT straws that fired in the modules that are traversed
      by the track.

      7: TRTdEdx - calibrated dE/dx measurement from all TRT hits along the track
      (see TRT_ToT_dEdx.h).

      8: eProbabilityNumberOfTRTHitsUsedFordEdx - number of good TRT hits that entered the dE/dx
      calculation.
  */

  class TRT_ElectronPidToolRun2;

  class TRT_ElectronPidToolRun2 : virtual public Trk::ITRT_ElectronPidTool, virtual public ITRT_ElectronToTTool, public AthAlgTool
  {
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //Athena specific, called externally:
  public:
    TRT_ElectronPidToolRun2(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~TRT_ElectronPidToolRun2 ();
      
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;

    /** standard Athena-Algorithm method */
    virtual StatusCode finalize() override;

    /** Electron probabilities to be returned */
    virtual std::vector<float> electronProbability(
      const EventContext& ctx,
      const Trk::Track& track) const override final;

    /** Electron probabilities to be returned */
    std::vector<float> electronProbability_old(const Trk::Track& track);

    virtual double probHT(const double pTrk,
                          const Trk::ParticleHypothesis hypothesis,
                          const int HitPart,
                          const int Layer,
                          const int Strawlayer) const override final;

    virtual double probHTRun2(const EventContext& ctx,
                              float pTrk,
                              Trk::ParticleHypothesis hypothesis,
                              int TrtPart,
                              int GasType,
                              int StrawLayer,
                              float ZR,
                              float rTrkWire,
                              float Occupancy) const override final;

  private:

      //Check valid TRT straw:
    bool CheckGeometry(int BEC, int Layer, int Strawlayer) const;

    double inline sqr(double a) {return a*a;} 

    const TRT_ID*              m_trtId;               // TRT ID helper (identifying barrel/wheels and global position)
    Trk::ParticleMasses        m_particlemasses;      // Particle masses. (initalized in default constructor)
    unsigned int               m_minTRThits;          // Minimum number of TRT hits to give PID.
    float                      m_ptMinNN;             // Minimum track pt to calculate NN response for PID
    bool                       m_calculateNN;         // Decide whether to use NN PID

    ToolHandle<ITRT_ToT_dEdx> m_TRTdEdxTool{ this,
                                             "TRT_ToT_dEdx_Tool",
                                             {},
                                             "TRT ToT dEdx Tool"};
    ToolHandle<InDet::ITRT_LocalOccupancy> m_LocalOccTool{
      this,
      "TRT_LocalOccupancyTool",
      {},
      "TRT Local occupancy tool"
    };
    ToolHandle<ITRT_StrawStatusSummaryTool> m_TRTStrawSummaryTool{
      this,
      "TRTStrawSummaryTool",
      "InDetTRTStrawStatusSummaryTool",
      "TRT straw summary tool"
    };

    SG::ReadCondHandleKey<HTcalculator> m_HTReadKey{ this,
                                                     "HTcalculator",
                                                     "HTcalculator",
                                                     "HTcalculator in-key" };

    SG::ReadCondHandleKey<InDet::TRTPIDNN> m_TRTPIDNNReadKey{ this,
                                                     "TRTPIDNN",
                                                     "TRTPIDNN",
                                                     "TRTPIDNN in-key" };
   }; 
} // end of namespace

#endif 
