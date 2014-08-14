/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCELLVARIABLES_H
#define	TAUREC_TAUCELLVARIABLES_H

#include "tauRec/TauToolBase.h"

class LArEM_ID;
class TileID;

/**
 * @brief Calculate tau calorimeter variables from cell information.
 * 
 * @authors  Srini Rajagopalan, Anna Kaczmarska, Felix Friedrich
 */

class TauCellVariables : public TauToolBase {

public:
    TauCellVariables(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    ~TauCellVariables();

    virtual StatusCode initialize();
    virtual StatusCode eventInitialize(TauCandidateData *data);
    virtual StatusCode execute(TauCandidateData *data);

private:
    double m_cellEthr;  //!< EM cell E threshold
    double m_stripEthr; //!< cell E threshold for strips
    double m_EMSumThr;  //!< threshold for 4-vector EM sum
    double m_EMSumR;    //!< radius for 4-vector EM sum
    double m_cellCone;  //!< outer cone for cells used in calculations

    const LArEM_ID* m_emid;
    const TileID* m_tileid;

    /** 
     * enable cell origin correction 
     * eta and phi of the cells are corrected wrt to the origin of the tau vertex
     */
    bool m_doCellCorrection;
};

#endif	/* TAUREC_TAUCELLVARIABLES_H */

