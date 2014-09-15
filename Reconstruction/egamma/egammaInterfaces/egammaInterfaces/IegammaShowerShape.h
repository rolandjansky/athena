/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IegammaShowerShape.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMASHOWERSHAPE_H
#define EGAMMAINTERFACES_IEGAMMASHOWERSHAPE_H

/** @class IegammaShowerShape
  Interface for the Reconstruction/egamma/egammaCaloTools/egammaShowerShape

  @author Frederic Derue derue@lpnhe.in2p3.fr

CREATED : 01/09/2008
MODIFIED :
*/

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
class CaloCellContainer;

static const InterfaceID IID_IegammaShowerShape("IegammaShowerShape", 1, 0);

class IegammaShowerShape : virtual public IAlgTool {

  
 public:
  /** @brief Virtual destructor */
  virtual ~IegammaShowerShape(){};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  
  
  // pure virtual
  /** @brief AlgTool method.*/
  virtual StatusCode execute(const xAOD::CaloCluster *cluster, const CaloCellContainer *cell_container) = 0;
    
  /** @brief energy in a 1x1 window in cells in eta X phi*/
  virtual double e011() const = 0;
  /** @brief  energy in a 3x3 window in cells in eta X phi*/
  virtual double e033() const = 0;
  
  // First sampling
  /** @brief  total energy */
  virtual double etot()    const = 0;
  /** @brief energy in 3x2 strips*/
  virtual double e132()    const = 0;
  /** @brief energy in 15x2 strips */
  virtual double e1152()   const = 0;
  /** @brief width in 3 strips */
  virtual double width3()  const = 0;
  /** @brief corrected width in 3 strips*/
  virtual double width3c() const = 0;
  /** @brief relative position within cell*/
  virtual double poscs1()  const = 0;
  /** @brief asymmetry of signal with three strips*/
  virtual double asy3()    const = 0;
  /** @brief width in eta*/
  virtual double wtot()    const = 0;
  /** eta position*/
  virtual double eta1()    const = 0;
  /** @brief difference between track and shower position in +/- 1 cells*/
  virtual double pos()     const = 0;
  /** @brief difference between track and shower position in +/- 7 cells*/
  virtual double pos7()    const = 0;
  /** @brief fraction of energy in sample 1*/
  virtual double f1()      const = 0;
  /** @brief fraction of energy in core of sample 1*/
  virtual double f1core() const = 0;
  /** @brief fraction of energy in two highest energy strips*/
  virtual double f2()      const = 0;
  /** @brief shower width on 5 strip around max*/
  virtual double width5()  const = 0;
  /** @brief energy of the second local maximum */
  virtual double esec()    const = 0;
  /** @brief energy of strip with second max 2*/
  virtual double esec1()   const = 0;
  /** @brief energy of strip with min e  */
  virtual double emin()    const = 0;
  /** @brief energy of strip with max e*/
  virtual double emax()    const = 0;
  /** @brief Michal Seman's valley */
  virtual double val()     const = 0;
  /** @brief M.S.'s fraction  */
  virtual double fracm()   const = 0;
  /** @brief eta of the hottest cell */
  virtual double etamax()  const = 0;
  /** @brief phi of the hottest cell*/
  virtual double phimax()  const = 0;
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
  virtual bool success()   const = 0;//
  
  /** @brief shower shapes in the second sampling*/
  /** @brief energy in a 3x3 window in cells in eta X phi*/
  virtual double e233()   const = 0; 
  /** @brief energy in a 3x5 window in cells in eta X phi*/
  virtual double e235()   const = 0; 
  /** @brief energy in a 3x7 window in cells in eta X phi*/
  virtual double e237()   const = 0; 
  /** @brief energy in a 5x5 window in cells in eta X phi*/
  virtual double e255()   const = 0; 
  /** @brief energy in a 7x7 window in cells in eta X phi*/
  virtual double e277()   const = 0; 
  /** @brief weighted eta in a 3X5 window in eta X phi */
  virtual double etaw()   const = 0; 
  /** @brief weighted phi in a 3X5 window in eta X phi*/
  virtual double phiw()   const = 0; 
  /** @brief uncorrected width in a 3X5 window in eta X phi*/
  virtual double width()  const = 0; 
  /** @brief relative position in eta within cell*/
  virtual double poscs2() const = 0; 
  
  // Third sampling
  /** @brief ratio of energy deposit in 3rd sampling over all energy*/
  virtual double f3()   const = 0; 
  /** @brief ratio of energy deposited in the third longitudinal compartment
      of the total energy in the cluster*/
  virtual double f3core()   const = 0; 
  /** @brief uncor. energy in 3x3 cells*/
  virtual double e333() const = 0;
  /** @brief uncor. energy in 3x5 cells*/
  virtual double e335() const = 0;
  /** @brief uncor. energy in 5x5 cells*/
  virtual double e355() const = 0;
  /** @brief uncor. energy in 3x7 cells */
  virtual double e337() const = 0;
  /** @brief uncor. energy in 7x7 cells*/
  virtual double e377() const = 0;
  
  /** Combined shower shapes*/
  /** @brief energy in core of shower */
  virtual double ecore()    const = 0; 
  /** @brief
      e 3X3 / e 3X7, the ratio of energies deposited 
      in a 3X3 and 3X7 clusters. More precisely, 
      for the 3X3 cluster, sampling by sampling, the cell energies
      are summed on the following eta by phi windows
      1X1 in the presampler (m_e011)
      3X2 in the strips     (m_e132)
      3X3 in the middle     (m_e233)
      3X3 in the back       (m_e333)
      for the 7X3 cluster, sampling by sampling, the cell energies
      are summed on the following eta by phi windows
      3X3 in the presampler (m_e033)
      15X2 in the strips     (m_e1152) 
      3X7 in the middle     (m_e237)
      3X7 in the back       (m_e337)*/
  virtual double reta3337_allcalo()      const = 0; 

};

inline const InterfaceID& IegammaShowerShape::interfaceID()
{
  return IID_IegammaShowerShape;
}

#endif // EGAMMAINTERFACES_IEGAMMASHOWERSHAPE_H
