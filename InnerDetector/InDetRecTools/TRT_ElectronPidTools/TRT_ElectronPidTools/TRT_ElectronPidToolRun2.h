/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidToolRun2.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRT_ELECTRONPIDTOOLRUN2_H
#define INDETTRT_ELECTRONPIDTOOLRUN2_H

#include "AthenaBaseComps/AthAlgTool.h"       // Exchange IN

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "TrkToolInterfaces/ITRT_ElectronPidTool.h"
#include "TRT_ElectronPidTools/ITRT_ElectronToTTool.h"
#include "TRT_ConditionsData/HTcalculator.h"
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
      mainly from the TRT and partially from the whole ID. Given a track, three methods are used:

      1: High Threshold (HT) information - sensitive to Transition Radiation (TR) photons
      emitted by electrons, when traversing the TRT radiator material between the straws.
      Electrons start to emit TR at 500 MeV, but only fully at 5 GeV.

      2: Time-over-Threshold (ToT) information - sensitive to (increased) ionization by
      electrons compared to heavier particles (eg. pions, protons, etc.). ToT is computed
      only for straws without a HT hit (to avoid correlations).
      The separation is largest at low momentum, and deminishes with increased energy.

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
    virtual StatusCode finalize  () override;

    /** Electron probabilities to be returned */
    virtual std::vector<float> electronProbability(const Trk::Track& track) const override;

    /** Electron probabilities to be returned */
    std::vector<float> electronProbability_old(const Trk::Track& track);

    virtual double probHT( const double pTrk, const Trk::ParticleHypothesis hypothesis, const int HitPart, const int Layer, const int Strawlayer) const override;
    virtual double probHTRun2( float pTrk, Trk::ParticleHypothesis hypothesis, int TrtPart, int GasType, int StrawLayer, float ZR, float rTrkWire, float Occupancy ) const override;

  // get the ToT from the bitpattern and correct for local variations
  // Jared - Remove ToT calc
    //double GetToT(unsigned int bitpattern, double HitZ, double HitR, int BEC, int Layer, int Strawlayer) const;
  
    // get the distance used to normalize the ToT
  // Jared - Remove ToT calc
    //double GetD(double R_track) const;

  private:
    
    bool m_DATA;

      //Check valid TRT straw:
    bool CheckGeometry(int BEC, int Layer, int Strawlayer) const;

    //Turn the Bitpattern into a human readable string
    std::string PrintBitPattern(unsigned int bitpattern);
      
    //Count how many HT and LT bits there are.
    int CountLTBitPattern(unsigned int bitpattern);
    int CountHTBitPattern(unsigned int bitpattern);

    double inline sqr(double a) {return a*a;} 

    const TRT_ID*              m_trtId;               // TRT ID helper (identifying barrel/wheels and global position)
    const InDetDD::TRT_DetectorManager* m_TRTdetMgr;  // TRT detector manager (to get ID helper)
    Trk::ParticleMasses        m_particlemasses;      // Particle masses. (initalized in default constructor)
    unsigned int               m_minTRThits;          // Minimum number of TRT hits to give PID.
    bool                       m_OccupancyUsedInPID;   // DEPRECATED!!!

    ToolHandle<ITRT_ToT_dEdx> m_TRTdEdxTool;     //!< the track selector tool
    ToolHandle<InDet::ITRT_LocalOccupancy> m_LocalOccTool;     //!< the track selector tool
    ToolHandle<ITRT_StrawStatusSummaryTool> m_TRTStrawSummaryTool;
    SG::ReadCondHandleKey<HTcalculator> m_HTReadKey{this,"HTcalculator","HTcalculator","HTcalculator in-key"};

   }; 
} // end of namespace

#endif 
