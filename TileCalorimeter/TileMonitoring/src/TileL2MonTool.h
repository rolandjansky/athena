/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileL2MonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
//	     matteo Volpi (matteo.volpi@cern.ch)
//
// ********************************************************************
#ifndef TILEL2MONTOOL_H
#define TILEL2MONTOOL_H

#include "TileFatherMonTool.h"

/** @class TileL2MonTool
 *  @brief Class for TileL2 based monitoring
 */

class TileL2MonTool: public TileFatherMonTool {

  public:

    TileL2MonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileL2MonTool();

    StatusCode initialize();

    //pure virtual methods
    StatusCode bookHistograms();
    StatusCode fillHistograms();
    StatusCode procHistograms();
    StatusCode checkHists(bool fromFinalize);

    StatusCode bookHistTrig(int trig);

    void cleanHistVec(); //necessary to avoid problems at the eb, lumi blocks boundaries

  private:

    int32_t m_TileL2Trig;

    std::string m_TileL2ContName;
    std::string m_muidsContName;

    // Histograms

    std::vector<TH1F*> m_TileL2Tags;
    std::vector<TH1F*> m_TileL2Energy;
    std::vector<TH2F*> m_TileL2EtaPhi;
    std::vector<TH1F*> m_TileL2Eta;
    std::vector<TH1F*> m_TileL2Phi;
    std::vector<TProfile*> m_TileL2EneEta;
    std::vector<TProfile*> m_TileL2EnePhi;
    std::vector<TH1F*> m_TileL2Quality;
    std::vector<TH2F*> m_TileL2EneQuality;

    std::vector<TH1F*> m_TileL2DeltaEnergy;
    std::vector<TH2F*> m_TileL2TagMuidTag;
    std::vector<TH2F*> m_TileL2EnergyMap;
    std::vector<TH2F*> m_TileL2RelativeEne;

    /** Number of muons tagged at the ROD DSPs */
    int m_ntag;

    /** Eta coordinate of the muons tagged at the ROD DSPs */
    //std::vector<float>* m_eta;
    /** Phi coordinate of the muons tagged at the ROD DSPs */
    //std::vector<float>* m_phi;
    /** Energy (in MeV) deposited by the muons in TileCal.
     * The three first components correspond to the energy deposited by the muon in the A, BC and D cells
     * respectively and the fourth component is the sum of the energy deposited in the three cells.
     */
    //std::vector<std::vector<float> >* m_energy;
    /** Quality factor for each muon found at the ROD DSPs:
     * - set to 0 if the energy deposited by the muon is less than the high energy threshold for the 3 cells
     * - set to 1 if the energy deposited by the muon is greater than the high energy threshold for 1 of the 3 cells
     */
    //  std::vector<unsigned int>* m_quality;
    /** Name of TileL2Container in detector store */
    std::string m_tileL2Container;

};

#endif
