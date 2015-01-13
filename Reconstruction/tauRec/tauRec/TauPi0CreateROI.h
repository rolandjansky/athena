/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0CREATEROI_H
#define	TAUREC_TAUPI0CREATEROI_H

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"
#include "tauRec/TauToolBase.h"

#include "xAODTau/TauJet.h"

class ICaloCellMakerTool;

/**
 * @brief Create ROIs for the Pi0 finder.
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 * @author Stephanie Yuen <stephanie.yuen@cern.ch>
 */

class TauPi0CreateROI : public TauToolBase {
public:
    TauPi0CreateROI(const std::string& type,
            const std::string& name,
            const IInterface *parent);
    virtual ~TauPi0CreateROI();

    virtual StatusCode initialize();
    virtual StatusCode eventInitialize(TauCandidateData *data);
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode eventFinalize(TauCandidateData *data);
    virtual StatusCode finalize();

private:

    /** @brief store cell in output container */ 
    void storeCell(const CaloCell* /* cell*/);

    /** @brief tool handles */
    ToolHandle<ICaloCellMakerTool> m_cellMakerTool;

    /** @brief calo cell navigation */
    const CaloDetDescrManager* m_calo_dd_man;
    const CaloCell_ID* m_calo_id;
 
    /** @brief all calo cell container name */
    std::string m_caloCellContainerName;
  
    /** @brief output cell container and name*/ 
    CaloCellContainer *m_pPi0CellContainer;
    std::string m_pi0CellContainerName;

    /** @brief hash map in order to keep track, which cells have been added to output cell container*/
    std::vector<CaloCell*> m_addedCellsMap;
};

#endif	/* TAUPI0CREATEROI_H */

