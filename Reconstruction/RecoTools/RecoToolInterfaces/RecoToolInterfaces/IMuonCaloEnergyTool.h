/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonCaloEnergyTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKMuonCaloEnergyTool_H
#define ITRKTRACKMuonCaloEnergyTool_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODBase/IParticle.h"
#include <vector>

// TRACK INCLUDES
#include "TrkTrack/Track.h"

class CaloCellContainer;


namespace Rec 
{
  

  static const InterfaceID IID_IMuonCaloEnergyTool("Rec::IMuonCaloEnergyTool", 1, 0);

  /** @brief Interface for extending Particles with calo cells */
  class IMuonCaloEnergyTool : virtual public IAlgTool {
  public:

    virtual void calculateMuonEnergies( const Trk::Track* trk, 
                                double deltaE, double meanIoni, double sigmaIoni,  
                                double& E, double& sigma, double& E_FSR, double& E_expected,
                                double &E_em_meas, double &E_em_exp, double &E_tile_meas, double &E_tile_exp,
                                double &E_HEC_meas, double &E_HEC_exp, double &E_dead_exp,  
                                std::vector<Identifier>* crossedCells = 0, std::vector<double>* sigmaNoise_cell = 0, std::vector<double>* E_exp_cell = 0) const=0;
 
    static const InterfaceID& interfaceID( ) ;
  };

  inline const InterfaceID& IMuonCaloEnergyTool::interfaceID() {
    return IID_IMuonCaloEnergyTool; 
  }

} // end of namespace

#endif 
