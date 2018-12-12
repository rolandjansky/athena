/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileJetMonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHOR:   Tomas Davidek (Tomas.Davidek@cern.ch)
//	     
//
// ********************************************************************
#ifndef TILEMONITORING_TILEJETMONTOOL_H
#define TILEMONITORING_TILEJETMONTOOL_H

#include "xAODJet/Jet.h"

#include "TileMonitoring/TileFatherMonTool.h"
#include "AsgTools/ToolHandle.h"
// JVT
#include "JetInterface/IJetUpdateJvt.h"
// Jet cleaning
#include "JetInterface/IJetSelector.h"
// Event cleaning
#include "JetSelectorTools/IEventCleaningTool.h"

class ITileBadChanTool;

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"

// Define flags & constants not defined anywhere else
#define FLAG_OF2 0x2   // OF2 has been applied
#define NPART 4 // number of Tilecal partitions

/** @class TileJetMonTool
 *  @brief Class for Tile Jet based monitoring
 */

class TileJetMonTool: public TileFatherMonTool {
  
  public:
    TileJetMonTool(const std::string & type, const std::string & name, const IInterface* parent);
    //  : TileFatherMonTool(type, name, parent);

    ~TileJetMonTool();

    virtual StatusCode initialize();
    //  StatusCode execute();
    virtual StatusCode fillHistograms();
    virtual StatusCode bookHistograms();
    virtual StatusCode procHistograms();
    StatusCode bookTimeHistograms();
    void clearTimeHistograms();
    StatusCode fillTimeHistograms(const xAOD::Jet& jet, uint32_t LumiBlock);

    unsigned int find_index(const int gain, const float energy);
    bool isGoodChannel(int part, int mod, int pmt, uint32_t bad, unsigned int qbit, Identifier id);
    bool passesJvt(const xAOD::Jet& jet);
    bool isGoodJet(const xAOD::Jet& jet);
    bool isGoodEvent();

    /* copy & paste from JetD3PDMaker/src/JetCaloUtilsFillerTool.h. It is defined
     as private, so we cannot directly include that file
     */
    typedef enum {
      LooseBad, MediumBad, TightBad
    } BadJetCategory;

  private:

    float m_jetPtMin;
    float m_jetEtaMax;
    std::string m_jetCollectionName;
    float m_energyChanMin;
    float m_energyChanMax;
  //    float m_averageMinEvt;
    bool m_do_1dim_histos;
    bool m_do_2dim_histos;
    bool m_do_enediff_histos;
    float m_enediff_threshold;
    bool m_do_energy_profiles;
  
    std::string m_partname[NPART];
    std::set<Identifier> m_usedCells;  // cells already used in the given event

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //Tile bad channel tool

    // this variable holds the histograms and is initialized in bookTimeHistograms,
    // called from initialize
    std::vector<TH2F*> m_TileChanTime[NPART];
    //  std::vector<TProfile*> m_TileChanTime[NPART];
    std::vector<TH1F*> m_TileChanTime1D[NPART];

    // this variable holds the sum of all time histograms for a given partition
    std::vector<TH1F*> m_TilePartTime;
    // this variable holds the sum of all time histograms for EBA, EBC
    // channels 0, 1, 12, 13 are not included in the sum
    std::vector<TH1F*> m_TileEBTime_NoScint;

    // this variable holds the summary DQ histograms, per partition
    std::vector<TProfile2D*> m_TilePartTimeDQ;

    // vector for enediff histograms
    std::vector<TH1F*> m_TileEneDiff_LG[NPART];
    std::vector<TH1F*> m_TileEneDiff_HG[NPART];

    // vector for total cell histograms
    std::vector<TH1F*> m_TilePartCellTimeHG[NPART];
    std::vector<TH1F*> m_TilePartCellTimeLG[NPART];
    std::vector<TH1F*> m_TilePartCellEneHG[NPART];
    std::vector<TH1F*> m_TilePartCellEneLG[NPART];

  // vector for cell energy histograms, contained in TProfile instead of vector
  // of TH1F
    std::vector<TProfile*> m_TilePartCellEneHGProf;
    std::vector<TProfile*> m_TilePartCellEneLGProf;

    std::vector<float> m_cell_ene_hg_up;
    std::vector<float> m_cell_ene_lg_up;

    bool m_do_event_cleaning;
    bool m_do_jet_cleaning;
    float m_jet_tracking_eta_limit;
    float m_jet_jvt_threshold;
    float m_jet_jvt_ptmax;
    bool m_first_event;
    // JVT
    ToolHandle<IJetUpdateJvt> m_jvt;
    std::string m_JvtDecorator;
    std::string m_OrDecorator;
    // event/jet cleaning
    ToolHandle<IJetSelector> m_cleaningTool;
    ToolHandle<ECUtils::IEventCleaningTool> m_ECTool;
};

#endif
