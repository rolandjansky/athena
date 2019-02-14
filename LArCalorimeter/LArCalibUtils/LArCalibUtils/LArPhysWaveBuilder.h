// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LArPhysWaveBuilder.h,v 1.6 2006-11-07 11:31:36 mdelmast Exp $
/**
 * @file  LArCalibUtils/LArPhysWaveBuilder.h
 * @brief Build LAr wave shapes from real data.
 *
 * The LAr electronics samples the pulse from the detector
 * at intervals of 25ns.  When the RODs are run in the mode
 * where these individual samples are read out, we can use this
 * to try to measure the shape of the pulse.  It works better
 * if there's some dispersion in the trigger times, to allow
 * measuring the shape at a continuum of times, rather than
 * just at integral multiples of 25ns.
 *
 * This algorithm accumulates these measured pulse shapes
 * for each individual cell and gain.  The results can be
 * saved to the conditions database or dumped to a ROOT file.
 */

#ifndef LARPHYSWAVEBUILDER_H
#define LARPHYSWAVEBUILDER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include <vector>
#include <string>


class LArPhysWave;
class LArPhysWaveContainer;
class LArParabolaPeakRecoTool;
class ILArADC2MeVTool;
class ILArPhaseTool;
class TH1F;
class TH2F;


/**
 * @brief Build LAr wave shapes from real data.
 */
class LArPhysWaveBuilder : public AthAlgorithm
{
 public:
  /**
   * @brief Standard Algorithm constructor.
   * @brief name The name of the algorithm.
   * @brief svcloc The service locator object.
   */
  LArPhysWaveBuilder(const std::string & name, ISvcLocator * pSvcLocator);

  /// Destructor.
  ~LArPhysWaveBuilder();

  /// Standard algorithm methods.
  StatusCode initialize();
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){ return StatusCode::SUCCESS;}

  /// ===  Constants.
  /// Number of distinct gain values.
  static const int N_GAINS = 3;

  /// Number of distinct calorimeter samplings.
  static const int N_SAMPLINGS = 4;


private:
   
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  /// === Private methods
  /// Make the wave container from the accumulated waves.
  StatusCode make_container (std::unique_ptr<LArPhysWaveContainer>& larPhysWaveContainer);

  /// Dump the accumulated waves to a root file.
  StatusCode write_root (LArPhysWaveContainer* larPhysWaveContainer);

  /// === Services & Tools 
  /// Tool to fit a parabola to the shape.
  LArParabolaPeakRecoTool* m_peakParabolaTool;

  /// Tool to convert from ADC counts to energy.
  const ILArADC2MeVTool* m_adc2mevTool;

  /// Tool to retrieve per-cell phase information.
  ILArPhaseTool* m_phase_tool;

  /// === Algorithm properties.
  /// Storegate key for the @c LArDigitContainer of interest.
  std::string m_gainCont;

  /// Number of bins per sample to record.
  int m_bins_per_sample;

  /// Time in ns between samples (normally 25).
  float m_sample_clock; 

  /// Minimum number of entries in a cell to keep it for
  /// further analysis.
  int m_nentriescut;

  /// Energy threshold for each layer per cell per event
  /// required to accumulate the cell.
  float m_PSCut;
  float m_StripsCut;
  float m_MiddleCut;
  float m_BackCut;

  /// In order to accumulate a cell, its energy must be at least
  /// this many sigma over the pedestal.
  float m_sigma_cut;

  /// If true, normalize each shape to the peak value before accumulating.
  bool m_normalize;

  /// If true, use the parabola fitting tool to find the peak.
  /// If false, use a simple cubic fit.
  bool m_use_parabola;

  /// The name of the tool to get the cell-by-cell phase information.
  /// If this is the string `peak', then we instead simply line up the
  /// waveforms using the fitted peak positions.
  std::string m_phase_tool_name;

  /// If true, dump the accumulated data to a ROOT file.
  bool m_rootrawdump;

  /// Name of the ROOT file to create.
  std::string m_rootoutputfile;

  /// Folder to fill in the database.
  std::string m_fullFolderNameLArPhysWave;

  /// If true, merge the data into an existing energy
  /// in the database.
  /// (XXX Obsolete: this needs to change to COOL.)
  bool m_updateDB;

  /// === Accumulated data.

  /// Holds a cell identifier plus the waves for all gains
  /// for that cell.
  struct Cellwave
  {
    /// Cell identifier.  If we haven't looked at this cell yet,
    /// is_valid() will be false, and the gains[] pointers
    /// will be null.
    Identifier id;
    /// We also save here the online id, since it has been removed
    /// from LArPhysWave
    HWIdentifier onlineId;

    /// Pointer to waves for each gain.
    /// Null if the wave hasn't yet been created.
    /// If we've already looked at this cell and determined
    /// that it isn't valid (doesn't exist, isn't connected),
    /// then the gains pointers will be set to the special value
    /// wave_invalid.
    LArPhysWave* gains[N_GAINS];

    /// Constructor, to zero the gains[] pointers.
    Cellwave();
  };

  /// List of Cellwave structures, indexed by cell hash.
  std::vector<Cellwave> m_waves;

  /// Histogram of the (pedestal-subtracted) peak sample value.
  TH1F* m_hmaxADCpeak;

  /// Histogram of the (pedestal-substracted) fitted maximum peak value.
  TH1F* m_hADCpeak;

  /// Histogram of gain vs sampling, over all cells.
  TH2F* m_hDigiGainSampling;

  /// Histogram of energy per sampling.
  TH1F* m_henergy[N_SAMPLINGS];

  /// Histogram of peak significance, in sigma, per sampling.
  TH1F* m_hsigma[N_SAMPLINGS];

  /// Histogram of the (pedestal subtracted) peak sample value,
  /// per samplng.
  TH1F* m_hlaymax[N_SAMPLINGS];
  
  // Grouping type
  std::string m_groupingType;
};  

#endif // not LARPHYSWAVEBUILDER_H
