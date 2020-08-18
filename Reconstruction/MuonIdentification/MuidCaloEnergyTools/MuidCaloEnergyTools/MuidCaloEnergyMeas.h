/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
/**@class MuidCaloEnergyMeas
 AlgTool to access the energy deposited by a muon in the
 calorimeters. The energy deposit is estimated according to
 the active measurements in the traversed calorimeter cells.

  @author Konstantinos.Nikolopoulos@cern.ch, Alan.Poppleton@cern.ch
 (c) ATLAS Combined Muon software
*/
///////////////////////////////////////////////////////////////////

#ifndef MUIDCALOENERGYTOOLS_MUIDCALOENERGYMEAS_H
#define MUIDCALOENERGYTOOLS_MUIDCALOENERGYMEAS_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloConditions/CaloNoise.h"
#include "CaloEvent/CaloCellContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/IMuidCaloEnergyMeas.h"
#include "StoreGate/ReadHandleKey.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class CaloCellContainer;
class Identifier;
class LArEM_ID;
class LArHEC_ID;
class TileID;

namespace Rec {

class caloMeas;
class IMuidCaloEnergyParam;

class MuidCaloEnergyMeas : public AthAlgTool, virtual public IMuidCaloEnergyMeas {
  public:
    MuidCaloEnergyMeas(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuidCaloEnergyMeas(void);  // destructor

    StatusCode initialize();
    StatusCode finalize();

    /**IMuidCaloEnergyMeas interface:
       get the muon energy loss measurement from the calorimeter,
       knowing the track intersection at the em and had cals*/
    CaloMeas* energyMeasurement(double etaEM, double phiEM, double etaHad, double phiHad) const;

  private:
    // private methods
    void   energyInCalo(CaloMeas& caloMeas, const CaloCellContainer* cellContainer, double eta, double phi,
                        int iSubCalo) const;
    void   isolationEnergy(CaloMeas& caloMeas, const CaloCellContainer* cellContainer, double eta, double phi,
                           int iSubCalo) const;
    double energyInTile(const CaloCellContainer* cellContainer, double eta, double phi, int, int) const;
    double energyInLArHEC(const CaloCellContainer* cellContainer, double eta, double phi, int, int) const;
    double energyInLArEM(const CaloCellContainer* cellContainer, double eta, double phi, int, int) const;
    int    cellCounting(const CaloCellContainer* cellContainer, double eta, double phi) const;

    // helpers, managers, tools
    SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{
        this,
        "CaloNoiseKey",
        "totalNoise",
        "SG Key of CaloNoise data object",
    };
    ToolHandle<IMuidCaloEnergyParam> m_caloParamTool{
        this,
        "CaloParamTool",
        "",
    };

    const TileID*                        m_tileID;
    const LArEM_ID*                      m_emID;
    const LArHEC_ID*                     m_hecID;
    SG::ReadHandleKey<CaloCellContainer> m_cellContainerLocation{
        this,
        "CellContainerLocation",
        "AllCalo",
        "calo cell container location",
    };

    double m_measurementConeTile;
    double m_measurementConeLArHEC;
    double m_measurementConeLArEM;
    double m_isolationConeTile;
    double m_isolationConeLArHEC;
    double m_isolationConeLArEM;

    double m_sigmasAboveNoise;      // The minimum sigmas above the noise tool rms
    double m_sigmasAboveNoiseCore;  // The minimum sigmas above the noise tool rms

    mutable std::atomic_int m_totalCoreCellsEM;
    mutable std::atomic_int m_totalCoreCellsHEC;
    mutable std::atomic_int m_totalCoreCellsTile;
    mutable std::atomic_int m_totalSelectedEM;
    mutable std::atomic_int m_totalSelectedHEC;
    mutable std::atomic_int m_totalSelectedTile;
};

}  // namespace Rec

#endif  // MUIDCALOENERGYTOOLS_MUIDCALOENERGYMEAS_H
