/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILECELLFAKEPROB_H
#define TILERECUTILS_TILECELLFAKEPROB_H

/*! @class TileCellFakeProb
 * 
 *  @brief Scales down the energy of cells due to simulated 
 *   failure of drawers
 *
 *  \author Kai Voss <kai.voss@cern.ch>
 *
 *  \date   August 25, 2005
 */

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICellWeightTool.h"
#include "Identifier/Identifier.h"

class TileID;
class TileHWID;
class CaloCell_ID;
class CaloCell;
class TileCablingService;

#include <string>
#include <vector>

class TileCellFakeProb: public AthAlgTool, virtual public ICellWeightTool {

  public:

    /*! Standard constructor */
    TileCellFakeProb(const std::string& type, const std::string& name,
        const IInterface* parent);

    /*! Standard destructor */
    ~TileCellFakeProb();

    /*! AlgTool Initialization */
    virtual StatusCode initialize();

    /*! implementation of ICellWeightTool weighting method */
    virtual double wtCell(const CaloCell* theCell);

  private:
    /*! Reads in properties and creates list of miscalibrated cells */
    StatusCode createMiscalibratedCellList();

    /*! Pointer to CaloCellID */
    const CaloCell_ID * m_caloID;
    /*! Pointer to TileID */
    const TileID * m_tileID;
    /*! Pointer to TileHWID */
    const TileHWID * m_tileHWID;
    /*! Pointer to TileCablingService */
    const TileCablingService* m_cabling;

    /*! Property: List of "dead" drawers */
    std::vector<std::string> m_deadDrawerInput;

    /*! List of "dead" cells and their weights */
    std::map<Identifier, double> m_celllist;

};

#endif
