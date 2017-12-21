/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECOSMICALGS_TILEMUONFITTER_H
#define TILECOSMICALGS_TILEMUONFITTER_H

/********************************************************************
 *
 * NAME:     TileMuonFitter
 * PACKAGE:  offline/TileCalorimeter/TileCosmicAlgs
 *
 * AUTHOR :  J. Maneira
 * CREATED:  10-Jul-2006
 * REVISED:  February-2008  (w/ L. Andrade)
 * REVISED:  November-2008  (w/ L. Andrade)
 * REVISED:  February-2009  (w/ M. Soares)
 *
 * PURPOSE:  Create Cell vectors from CaloCellContainer, fit a straight muon
 *           track through a selected subset, calculate horizontal
 *           plane crossing time.
 *           (see .cxx and mainpage.h for more details)
 *
 *  Input: TileCell (one by one or from CaloCellContainer)
 *  Output: ComissioningEvent/ComTime object
 *  Output: TileEvent/TileCosmicMuon object
 *   
 ********************************************************************/

// Tile includes
#include "TileCosmicAlgs/TileMuonTrackDistance.h"
#include "TileEvent/TileContainer.h"

// Calo includes
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CommissionEvent/ComTime.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"


// forward declarations
class HWIdentifier;
class IdentifierHash;
class TileID;
class TileHWID;
class TileDetDescrManager;

// C++ STL includes
#include <string>
#include <algorithm>

//using namespace ROOT::Minuit2;

// Auxiliar class to save info about each cell
// used in Hough Transform computation
class CellInfo {
  public:

    float x, y, z;     // position
    float dist;        // RoI
    float e, ev;       // energy and energy density
    bool use;         // cell is enabled
    bool is_out;      // cell is not in current track
    int track_index; // each cell can belong to only one track
};

/**
 *
 * @class TileMuonFitter
 * @brief Fits straight cosmic muon track to TileCal event
 *
 * This class implements a cosmic muon track reconstruction
 * from TileCal information only. The algorithm is described in
 * the ATL-TILECAL-INT-2007-003 note. It uses CaloCells as input,
 * fits a straight line to the cells, weighted with energy or energy
 * density and outputs single track parameters. Cell timing is also
 * used to provide the time at which the muon crossed the horizontal
 * (y=0) plane.
 */

class TileMuonFitter: public AthAlgorithm {
  public:

    /** Constructor */
    TileMuonFitter(std::string name, ISvcLocator* pSvcLocator);
    virtual ~TileMuonFitter();

    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

    /** Creates an internal cell container (just vectors) from the
     input CaloCellContainer. Throws away cells below threshold
     or flagged from jobOptions.*/
    void buildCells();

    /** Reset variables.*/
    void setEventDefaults();

    /** Checks if there are good cells on the top and bottom modules.
     If not, skips event. */
    bool eventSelection();

    /** Fits a straight track to the cell centers, using the auxiliary class
     TileMuonTrackDistance. */
    int fitTrack();

    /** Fits a straight track to the cells centers, using a Hough Transform
     algorithm. */
    int houghTrack();

    /** Calculates time in reference plane */
    void calculateTime();
    /** Extrapolates cell time to y=0 */
    void calculateTimeAtYequal0();
    /** Extrapolates cell time to z=0 */
    void calculateTimeAtZequal0();

    /** Selects between the two next methods. */
    void buildTileCosmicMuon(int fitok);
    /** Creates output TileCosmicMuon object in StoreGate. */
    void buildTileCosmicMuonAtYequal0(int fitok);
    /** Creates output TileCosmicMuon object in StoreGate. */
    void buildTileCosmicMuonAtZequal0(int fitok);

    /** Selects between the two next methods. */
    void buildComTime(int fitok);
    /** Creates output ComTime object in StoreGate. */
    void buildComTimeAtYequal0(int fitok);
    /** Creates output ComTime object in StoreGate. */
    void buildComTimeAtZequal0(int fitok);

  private:

    /** Calculates length of track intersection with TileCal (by sampling).*/
    void trackIntersection(std::vector<double> & ltop, std::vector<double> & lbot, int index);
    /** Calculates length of track intersection with TileCal (by sampling and module).*/
    void trackSegmentIntersection(std::vector<double> & segPath, std::vector<int> & segPartition
                                  , std::vector<int> & segModule, std::vector<int> & segSampling, int index);

    /** Sums up energy in TileCal cells close to the track (by sampling).*/
    void energyInTrack(std::vector<double> & etop, std::vector<double> & ebot
                      , std::vector<IdentifierHash> & cells, int index);

    /** Checks if x1 is within LB z coordinate bounds.*/
    bool checkLBz(double x1);
    /** Checks if x1 is within EB z coordinate bounds.*/
    bool checkEBz(double x1);
    /** Checks if x1 is within EBA z coordinate bounds.*/
    bool checkEBAz(double x1);
    /** Checks if x1 is within EBC z coordinate bounds.*/
    bool checkEBCz(double x1);
    /** Checks if x1 is within LB r coordinate bounds for sampling s1.*/
    bool checkLBr(double x1, uint8_t s1);
    /** Checks if x1 is within EB r coordinate bounds for sampling s1.*/
    bool checkEBr(double x1, uint8_t s1);
    /** Checks if x1 is within LB r coordinate bounds for any sampling*/
    bool checkLBr(double x1);
    /** Checks if x1 is within EB r coordinate bounds for any sampling*/
    bool checkEBr(double x1);
    /** Returns sampling index if x1 is within EB r coordinate bounds*/
    int whichEBr(double x1);
    /** Returns sampling index if x1 is within LB r coordinate bounds*/
    int whichLBr(double x1);
    /** Returns module index for TVector3 input*/
    int whichModule(CLHEP::Hep3Vector tempvec);

    // Hough Transform auxiliar routines ---------------------------------------

    std::vector<CellInfo> m_cellInfo;

    void cart2hough(float x1, float y1, float x2, float y2, double &raio, double &angu);
    void hough2cart(double r, double a, double offset, double &aa, double &bb);
    float dist2line(CellInfo &ci, float *pos, float *w);
    void points2dir(CellInfo &ci1, CellInfo &ci2, float *w);
    unsigned int CntCells(unsigned int index1, unsigned int index2, double &skew);
    bool guessTrack(unsigned int &index1, unsigned int &index2);
    unsigned int buildCellInfoVector();
    float selectCells(float *p, float *w);
    bool isHaloMuon(double azy);
    void doHough(double &rxy, double &axy, double &rzy, double &azy);
    void addTrack(double aa, double bb, double cc, double dd);

  protected:

    // Tile objects
    const TileID* m_tileID;
    const TileHWID* m_tileHWID;
    const TileDetDescrManager* m_tileMgr;
    const CaloCellContainer *m_caloCells;

    /** Auxiliary class representing the function to be minimized - weighted sum of
     squares of orthogonal distances from track to cells)*/
    ROOT::Minuit2::TileMuonTrackDistance* m_theTrack;

    /** Cell energy threshold */
    double m_eThreshold;
    /** Cell Delta Time cut*/
    double m_deltaTimeCut;
    /** Flag to weigh or not the chi-square with an energy parameter.*/
    bool m_doWeighted;
    /** Flag defining the energy weighting parameter: energy density or plain
     energy.*/
    bool m_doDensity;
    /** Flag to indicate: cosmics, singlebeam or collisions. */
    std::string m_beamType;
    /** Flag to use Hough Transform instead of Fit. */
    bool m_doHoughTransform;
    /** Radial bounds of the 3 samplings in LB, loaded from Detector Description.*/
    std::vector<double> m_tileDD_radiusLB;
    /** Radial bounds of the 3 samplings in EB, loaded from Detector Description.*/
    std::vector<double> m_tileDD_radiusEB;
    /** Z bounds of EBA, loaded from Detector Description.*/
    std::vector<double> m_tileDD_zEBA;
    /** Z bounds of EBC, loaded from Detector Description.*/
    std::vector<double> m_tileDD_zEBC;
    /** Z bounds of LB, loaded from Detector Description.*/
    std::vector<double> m_tileDD_zLB;

    /** Number of cells selected for fit.*/
    int m_nCells;
    /** Minimum number of cells needed for fit.*/
    int m_minimumCells;
    /** Position of selected cell's center.*/
    std::vector<CLHEP::Hep3Vector> m_cellPosition;
    /** Selected cell's energy.*/
    std::vector<double> m_cellEnergy;
    /** Selected cell's weight for fit.*/
    std::vector<double> m_cellWeight;
    /** Selected cell's time.*/
    std::vector<double> m_cellTime;
    /** Selected cell's time difference between two PMTs.*/
    std::vector<double> m_cellDeltaTime;
    /** Selected cell's identifier hash.*/
    std::vector<IdentifierHash> m_cellHash;

    /** Vector with the fitted four track parameters.*/
    std::vector<std::vector<double> > m_linePar;
    /** Chi-square minumum.*/
    std::vector<double> m_fitMinimum;
    /** Time at y=0.*/
    std::vector<double> m_zeroCrossingTime;

    double m_meanX;
    double m_meanY;
    double m_meanZ;
    double m_weightedMeanX;
    double m_weightedMeanY;
    double m_weightedMeanZ;

    int m_maxBottomIndex;
    int m_maxTopIndex;
    bool m_reg1to2;

    static CaloCell_ID::SUBCALO m_caloIndex;

    SG::ReadHandleKey<CaloCellContainer> m_cellContainerKey{this, "CaloCellContainer", "AllCalo",
                                                            "Input CaloCellContainer name" };

    SG::WriteHandleKey<TileCosmicMuonContainer> m_cosmicMuonContainerKey{this,
                                                                         "TileCosmicMuonKey",
                                                                         "TileCosmicMuonMF",
                                                                         "Output TileCosmicMuonContainer name"};

    SG::WriteHandleKey<ComTime> m_comTimeKey{this, "ComTimeKey", "ComTimeTileMuon", "Output ComTime name"};

};

#endif
