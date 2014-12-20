/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IegammaIso.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMAISO_H
#define EGAMMAINTERFACES_IEGAMMAISO_H

/// @class IegammaIso
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaIso
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <vector>

// Forward declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
class CaloCellList; 

static const InterfaceID IID_IegammaIso("IEgammaIso", 1, 0);
        
class IegammaIso : virtual public IAlgTool {

     
 public:
  /** @brief Virtual destructor */
  virtual ~IegammaIso(){};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /// IsoSpecifier specifie the type of isolation to do. It uses
  /// the standard introduced in the functions below.
  
  /// R1 is the inner ring; R1 == 0 means do cone
  /// R2 is the outer ring size
  /// NsigNoise = Number of sigma above noise, if -1 no cut on noise
  /// flagCell  = 1 EM, 2 EM+HAD
  /// flagNoise = 1 elec, 2 total
  /// symmetric : symmetric cut is a cut on abs(e)>meannoise instead of e>meannoise


  /** @brief  method: Method to just calculate hadronic leakage*/
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, CaloCellList* had) =0;
  
  /** @brief total hadronic calorimeter et in cone*/
  virtual double ethad()  const = 0; 
  /** @brief energy in first hardon layer in cone*/
  virtual double ehad1()  const = 0; 
  /** @brief et in first hadron cal layer in cone*/
  virtual double ethad1() const = 0; 

};

inline const InterfaceID& IegammaIso::interfaceID()
{
  return IID_IegammaIso;
}

#endif // EGAMMAINTERFACES_IEGAMMAISO_H
