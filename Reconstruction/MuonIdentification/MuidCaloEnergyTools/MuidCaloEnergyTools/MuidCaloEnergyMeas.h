/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/IMuidCaloEnergyMeas.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class CaloCellContainer;
class ICaloNoiseTool;
class Identifier;
class LArEM_ID;
class LArHEC_ID;
class TileID;

namespace Rec
{

    class caloMeas;
    class IMuidCaloEnergyParam;

class MuidCaloEnergyMeas: public AthAlgTool,
			  virtual public IMuidCaloEnergyMeas
{
public:
    MuidCaloEnergyMeas(const std::string& type, 
		       const std::string& name,
		       const IInterface* parent);
    ~MuidCaloEnergyMeas(void); // destructor

    StatusCode		initialize();
    StatusCode		finalize();
    
    /**IMuidCaloEnergyMeas interface:
       get the muon energy loss measurement from the calorimeter,
       knowing the track intersection at the em and had cals*/
    CaloMeas*		energyMeasurement(double etaEM,
					  double phiEM,
					  double etaHad,
					  double phiHad) const;
private:
    // private methods
    void				energyInCalo(CaloMeas&			caloMeas,
						     const CaloCellContainer*	cellContainer,
						     double			eta,
						     double			phi,
						     int			iSubCalo) const;
    void				isolationEnergy(CaloMeas&		caloMeas,
							const CaloCellContainer* cellContainer,
							double			eta,
							double			phi,
							int			iSubCalo) const;
    double				energyInTile(const CaloCellContainer* cellContainer,
						     double eta,
						     double phi,
						     int,
						     int) const;
    double				energyInLArHEC(const CaloCellContainer* cellContainer,
						       double eta,
						       double phi,
						       int,
						       int) const;
    double				energyInLArEM(const CaloCellContainer* cellContainer,
						      double eta,
						      double phi,
						      int,
						      int) const;
    int                                 cellCounting(const CaloCellContainer* cellContainer,
						     double eta,
						     double phi) const;
    
    // helpers, managers, tools
    ToolHandle<ICaloNoiseTool>			m_caloNoiseTool;
    ToolHandle<IMuidCaloEnergyParam>		m_caloParamTool;

    const TileID*				m_tileID;
    const LArEM_ID*				m_emID;
    const LArHEC_ID*				m_hecID;
    std::string					m_cellContainerLocation; // Cell container's name

    double 					m_measurementConeTile;
    double					m_measurementConeLArHEC;
    double					m_measurementConeLArEM;
    double					m_isolationConeTile;
    double					m_isolationConeLArHEC;
    double					m_isolationConeLArEM;

    double			m_sigmasAboveNoise;	// The minimum sigmas above the noise tool rms
    double			m_sigmasAboveNoiseCore;	// The minimum sigmas above the noise tool rms
    bool			m_useCaloNoiseTool;	// use the CaloNoiseTool?  

    mutable int			m_totalCoreCellsEM;
    mutable int			m_totalCoreCellsHEC;
    mutable int			m_totalCoreCellsTile;
    mutable int			m_totalSelectedEM;
    mutable int			m_totalSelectedHEC;
    mutable int			m_totalSelectedTile;
};

}	// end of namespace

#endif // MUIDCALOENERGYTOOLS_MUIDCALOENERGYMEAS_H


