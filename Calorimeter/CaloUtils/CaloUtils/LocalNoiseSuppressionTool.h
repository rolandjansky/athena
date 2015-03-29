/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_LOCALNOISESUPPRESSIONTOOL_H
#define CALOUTILS_LOCALNOISESUPPRESSIONTOOL_H


/********************************************************************

NAME:     LocalNoiseSuppressionTool.h 
PACKAGE:  offline/Calorimeter/CaloUtils

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  Feb 8, 2005

PURPOSE: Performs Local Noise Suppression CaloCell objects Inherits
          from ICellWeightTool, which is the base class for
          individual cell correction classes.

********************************************************************/

class CaloCell;
class CaloCellContainer;

#include "CaloInterface/ICellWeightTool.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/ToolHandle.h" 
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "StoreGate/StoreGateSvc.h"

class CaloDetDescrManager; 

class LocalNoiseSuppressionTool : public AthAlgTool, virtual public ICellWeightTool
{

 public:

  LocalNoiseSuppressionTool(const std::string& type, const std::string& name,
		    const IInterface* parent);
  virtual ~LocalNoiseSuppressionTool();

  // Main access method: Correct cells in cellCollection:
  StatusCode initialize() ;  

  /// Implementation for ICellWeightTool.
  virtual double wtCell(const CaloCell*) ; 

  // special functions for this tool (hide interface)
  ///Sets the calocell container used for finding neighbors. 
  virtual void   setCaloCellContainer(const CaloCellContainer* ); 

  /// Test if the cell should be cut based on neighbors and the cell's energy.
  virtual bool   cutCell(const CaloCell*) ; 

  /// Get the prior probability that this cell is noise based on the neighbors and a given test statistic.
  virtual double getPrior(const CaloCell*) ; 

  /// Get the test statistic (which is based on neighbors) used to distinguish noisy cells from energetic cells.
  virtual double getTestStatistic(const CaloCell*); 

  /* ----------versions which explicitly expose the interface--------------*/
  /** returns the weight that should be used to scale this cells
   energy.  This is not a calibration, but the weighted average
   between the measured energy in the cell and 0 (the true energy
   of a noisy cell).*/
  virtual double wtCell(const CaloCellContainer*, const CaloCell*); 

  /// Test if the cell should be cut based on neighbors and the cell's energy.
  virtual bool   cutCell(const CaloCellContainer*, const CaloCell*); 

  /// Get the prior probability that this cell is noise based on the neighbors and a given test statistic.
  virtual double getPrior(const CaloCellContainer*, const CaloCell*); 

  /// Get the test statistic (which is based on neighbors) used to distinguish noisy cells from energetic cells.
  virtual double getTestStatistic(const CaloCellContainer*, const CaloCell*); 

  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

 private:

  // properties

  /** The maximum prior returned by getPrior(). If the getPrior()
      returns 1, then wtCell() will return 0 even if the cell is very
      energetic.  This is meaningful in terms of Bayes theorem, but
      probably not the desired behavior.  In order to avoid this, set
      maxPrior (via job options) to something less than 1.  If
      maxPrior=.99 a cell with 5sigma will have weight = 1.
   */
  double m_maxPrior;

  /** The cut on getPrior() used to remove noisy cells.  This tool can
      either be used to re-weight cells or to cut on them.  Cuts on
      cells are controlled by *both* the prior (based on neighbors)
      and the cell itself.  If the prior is large, then the cell is
      probably noisy; however, it will not be cut unless it also fails
      the cut on energy in terms of sigma.
     
  */
  double m_cutThresholdOnPrior;

  /** The cut on the cell's energy in sigma to remove noisy cells.  If
      a cell has a prior greater than m_cutThresholdOnPrior AND E <
      m_cutInsigma then it will be cut.
   */	
  double m_cutInSigma;  

  /** Toggle for symmetric / asymmetric cut.  If m_symmetricCut==true
      then a cell will be kept even if it's energy is < 0.  If
      m_symmetricCut==false, then it will be kept only if it's energy
      is > 0.
   */
  bool m_symmetricCut;

  /// setup for CaloNoiseTool
  bool m_usePileUp;  
  /// setup for CaloNoiseTool
  //int m_nMinBias;
  
  std::string m_caloCellContainerName;
  std::string m_testStatistic;
  std::string m_neighborOption;

  double m_s0, m_s1, m_o0, m_o1;  

  // used internally
  double cellSigma(const CaloCell* theCell);
  double scale() ;
  double offset();

  int m_nNeighbors;  
  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;

  const CaloDetDescrManager* m_calo_dd_man; 
  const CaloCell_ID* m_calo_id;

  const CaloCellContainer* m_caloCellContainer;
};

#endif
