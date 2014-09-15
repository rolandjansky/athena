/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IegammaStripsShape.h, (c) ATLAS Detector software 2009
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMASTRIPSSHAPE_H
#define EGAMMAINTERFACES_IEGAMMASTRIPSSHAPE_H

/// @class IegammaStripsShape
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaStripsShape
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
class CaloCellContainer;

static const InterfaceID IID_IegammaStripsShape("IegammaStripsShape", 1, 0);

class IegammaStripsShape : virtual public IAlgTool {

  
 public:
  /** @brief Virtual destructor */
  virtual ~IegammaStripsShape(){};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  
  
  // pure virtual
  /** @brief AlgTool method.*/
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, const CaloCellContainer *cell_container) = 0;
  
  virtual double etot() const = 0;
  /** @brief energy in 3x2 strips*/
  virtual double e132() const = 0;
  /** @brief energy in 15x2 strips */
  virtual double e1152() const = 0;
  /** @brief width in 3 strips */
  virtual double ws3() const = 0;
  /** @brief corrected width in 3 strips*/
  virtual double ws3c() const = 0;
  /** @brief relative position within cell*/
  virtual double poscs1() const = 0;
  /** @brief asymmetry of signal with three strips*/
  virtual double asymmetrys3() const = 0;
  /** @brief width in eta*/
  virtual double wstot() const = 0;
  /** eta position*/
  virtual double etas3() const = 0;
  /** @brief difference between track and shower position in +/- 1 cells*/
  virtual double deltaEtaTrackShower() const = 0;
  /** @brief difference between track and shower position in +/- 7 cells*/
  virtual double deltaEtaTrackShower7() const = 0;
  /** @brief fraction of energy in sample 1*/
  virtual double f1() const = 0;
  /** @brief fraction of energy in core of sample 1*/
  virtual double f1core() const = 0;
  /** @brief fraction of energy in two highest energy strips*/
  virtual double f2() const = 0;
  /** @brief shower width on 5 strip around max*/
  virtual double widths5() const = 0;
  /** @brief return the energy of the second local maximum (sum of three strips) */
  virtual double esec() const = 0;
  /** @brief return the energy of strip with second local maximum */
  virtual double esec1() const = 0;
  /** @brief energy of strip with min e  */
  virtual double emins1() const = 0;
  /** @brief energy of strip with max e*/
  virtual double emaxs1() const = 0;
  /** @brief Michal Seman's valley */
  virtual double val() const = 0;
  /** @brief fraction of energy outside shower core 
      (E(+/-3strips)-E(+/-1strips))/ E(+/-1strips) */
  virtual double fside() const = 0;
  /** @brief eta of the hottest cell */
  virtual double etamax() const = 0;
  /** @brief phi of the hottest cell*/
  virtual double phimax() const = 0;
  /** @brief return the index of the hottest cell in the array*/
  virtual int ncetamax() const = 0;
  /** @brief return the index of the seed in the array*/
  virtual int ncetaseed() const = 0;
  /** @brief return the array of cells in eta */
  virtual const double* etacell_arr() const = 0;
  /** @brief return the array of cells in energy */
  virtual const double* enecell_arr() const = 0;
  /** @brief return the array of cells in granularity in eta */
  virtual const double* gracell_arr() const = 0;
  /** @brief return the array of cells in number of cells merged */
  virtual const int* ncell_arr() const = 0;

  /** @brief */
  virtual bool success() const = 0;//

};

inline const InterfaceID& IegammaStripsShape::interfaceID()
{
  return IID_IegammaStripsShape;
}

#endif // EGAMMAINTERFACES_IEGAMMASTRIPSSHAPE_H
