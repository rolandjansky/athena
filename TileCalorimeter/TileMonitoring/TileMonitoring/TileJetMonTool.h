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

class ITileBadChanTool;

#include "TH1F.h"
#include "TH2F.h"

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
    bool isGoodJet(const xAOD::Jet& jet);

    /* copy & paste from JetD3PDMaker/src/JetCaloUtilsFillerTool.h. It is defined
     as private, so we cannot directly include that file
     */
    typedef enum {
      LooseBad, MediumBad, TightBad
    } BadJetCategory;

    bool isBad(BadJetCategory criteria, double quality, double NegE, double emf, double hecf,
        double time, double fmax, double eta, double chf, double HecQ);
  
  
  private:

    float m_jetPtMin;
    float m_jetEtaMax;
    std::string m_jetCollectionName;
    std::string m_THistSvcStreamName;
    float m_energyChanMin;
    float m_energyChanMax;
    bool m_do_1dim_histos;
    bool m_do_2dim_histos;
    bool m_do_enediff_histos;
    float m_enediff_threshold;

    std::string m_partname[NPART];
    std::set<Identifier> used_cells;  // cells already used in the given event

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

  // vector for enediff histograms
  std::vector<TH1F*> m_TileEneDiff_LG[NPART];
  std::vector<TH1F*> m_TileEneDiff_HG[NPART];

  // vector for total cell histograms
  std::vector<TH1F*> m_TilePartCellTimeHG[NPART];
  std::vector<TH1F*> m_TilePartCellTimeLG[NPART];
  std::vector<TH1F*> m_TilePartCellEneHG[NPART];
  std::vector<TH1F*> m_TilePartCellEneLG[NPART];

  std::vector<float> cell_ene_hg_up;
  std::vector<float> cell_ene_lg_up;
};

#endif
