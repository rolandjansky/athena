/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : TileTrigger.h
// Author   : Jose Maneira
// Created  : March 2007
//
// DESCRIPTION:
//    TileTrigger is the class that contains the event trigger information 
//    from the simulated Cosmics trigger boards. The actual trigger logic 
//    calculations are done in the TileRecAlgs/TileCosmicsTrigger algorithm.
//    This class contais information about which TileCal towers cave a trigger
//    in each of three possible trigger modes: single tower, back-2-back, and
//    board coincidence.
//    * For single tower, the ID and sum signal peak (in mv) is given for the
//    maximum signal tower in the whole range of connected modules.
//    * For board coincidence mode, the ID and sum signal peak are given for
//    the maximum signal tower in each board (up to a maximum of 8 boards).
//    * For back-2-back coincidence mode, the ID and sum signal peak (in mv) 
//    is given for each tower that passes the trigger requirements (two 
//    diametrically opposite towers above threshold).
//
//    This allows the trigger logic to be recalculated for different thresholds 
//    (higher) than the one chosen in the jobOptions. 
//
// HISTORY:
//
// ***************************************************************************

#ifndef TILETRIGGER_H
#define TILETRIGGER_H
#include "AthenaKernel/CLASS_DEF.h"

#include "Identifier/Identifier.h"
#include <vector>

class TileTrigger 
{
public:
   
    /* Constructors */

    TileTrigger() : m_ID(0), m_maxTowerSum(0.0) {}

    TileTrigger(int id, const Identifier& mtid, double mtsum, 
		std::vector<Identifier>& boardtid, std::vector<double>& boardtsum,
		std::vector<Identifier>& backtid, std::vector<double>& backtsum);

    TileTrigger(const Identifier& mtid, double mtsum, 
		std::vector<Identifier>& boardtid, std::vector<double>& boardtsum,
		std::vector<Identifier>& backtid, std::vector<double>& backtsum);

    ~TileTrigger() {}

    /* Set methods */

    void Set(int id, const Identifier& mtid, double mtsum, 
		std::vector<Identifier>& boardtid, std::vector<double>& boardtsum,
		std::vector<Identifier>& backtid, std::vector<double>& backtsum);

    void Set(const Identifier& mtid, double mtsum, 
		std::vector<Identifier>& boardtid, std::vector<double>& boardtsum,
		std::vector<Identifier>& backtid, std::vector<double>& backtsum);

    void SetID(int id) {m_ID = id; }
    void SetMaxTowerID(const Identifier& mtid) {m_maxTowerID = mtid; }
    void SetMaxTowerSum(double mtsum)          {m_maxTowerSum = mtsum; }
    void SetBoardTowerID(std::vector<Identifier>& boardtid) {m_boardTowerID = boardtid; }
    void SetBoardTowerSum(std::vector<double>& boardtsum) {m_boardTowerSum = boardtsum; }
    void SetBackTowerID(std::vector<Identifier>& backtid) {m_backTowerID = backtid; }
    void SetBackTowerSum(std::vector<double>& backtsum) {m_backTowerSum = backtsum; }

    /* Access methods */

    int                            GetID()             const {return m_ID; }
    Identifier                     GetMaxTowerID()     const {return m_maxTowerID; }
    double                         GetMaxTowerSum()    const {return m_maxTowerSum; }
    const std::vector<Identifier>& GetBoardTowerID()   const {return m_boardTowerID; }
    const std::vector<double>&     GetBoardTowerSum()  const {return m_boardTowerSum; }
    const std::vector<Identifier>& GetBackTowerID()    const {return m_backTowerID; }
    const std::vector<double>&     GetBackTowerSum()   const {return m_backTowerSum; }
	
    /* Print methods */

    std::string whoami   (void) const { return "TileTrigger"; }
    void        print    (void) const;
    // Convertion operator to a std::string
    // Can be used in a cast operation : (std::string) TileTrigger
    operator std::string() const;

private:

    /* Member variables: */
    int m_ID; // some ID - don't know what, can be used in future
    Identifier m_maxTowerID;
    double     m_maxTowerSum;
  
    std::vector<Identifier>  m_boardTowerID;
    std::vector<double>      m_boardTowerSum;

    std::vector<Identifier>  m_backTowerID;
    std::vector<double>      m_backTowerSum;
  
};
#endif  //TILEEVENT_TILETRIGGER_H

