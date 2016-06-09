// This file's extension implies that it's C, but it is really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOCLUSTERCORRECTION_H
#define CALOCLUSTERCORRECTION_CALOCLUSTERCORRECTION_H 
/********************************************************************

NAME:     CaloClusterCorrection.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec  1999

PURPOSE:  Base class: CaloClusterProcessor
          Individual cluster correction classes inherit from this class 
          and use the "set*" methods to correct the CaloCluster objects

          execute(CaloCluster *) method to be implemented 
          in the derived class
          

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 3, 2001    (HM)
          QA.
Updated:  May 2001, (SR) Move to StoreGate

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  January, 2005    (MB)
          Move to Calorimeter/CaloClusterCorrection.

Updated:  February, 2006 (DLelas)
          New method added:
          "makeCorrectionOnClusCollection(CaloClusterContainer *clusColl)"
          Which enables corrections to be applied to a whole
          CaloCluster collection. It is needed by SW cell weights
          calculation.

********************************************************************/

// INCLUDE HEADER FILES:
#include "CaloRec/CaloClusterProcessor.h"
#include "CaloRec/ToolWithConstantsMixin.h"

class CaloClusterCorrection
  : public CaloClusterProcessor,
    public CaloRec::ToolWithConstantsMixin
{

 public:
  // Destructor 
  virtual ~CaloClusterCorrection() override;


  /**
   * @brief Standard initialization method.
   */
  virtual StatusCode initialize() override;


  using CaloClusterProcessor::setProperty;
  /**
   * @brief Method to set a property value.
   * @param propname The name of the property to set.
   * @param value The value to which to set it.
   *
   * Defined here as required by @c ToolWithConstantsMixin.
   */
  virtual StatusCode setProperty (const std::string& propname,
                                  const std::string& value) override;

  /**
   * @brief Method to set a property value.
   * @param p The property name/value to set.
   *
   * Defined here as required by @c ToolWithConstantsMixin.
   */
  virtual StatusCode setProperty (const Property& p) override;


  // modifying CaloCluster object 
  virtual void setsample(xAOD::CaloCluster* cluster,
                         CaloSampling::CaloSample sampling,
			 float em, float etam, float phim, 
			 float emax, float etamax, float phimax, 
			 float etas, float phis) const;

  // set energy of cluster.  if samplings=true, rescale the energy
  // in each sampling accordingly.  default is to do the rescaling.
  virtual void setenergy(xAOD::CaloCluster* cluster, float energy) const;

  // derived class implement the real correction.
  virtual void makeCorrection(const EventContext& ctx,
                              xAOD::CaloCluster*) const = 0;

  using CaloClusterProcessor::execute;
  StatusCode execute(xAOD::CaloCluster* cluster) override;

 protected:
  /**
   * @brief Constructor.
   * @param type The type of this tool.
   * @param name The name of this tool.
   * @param parent The parent of this tool.
   * @param toolcls Set this to override the class name for this tool
   *                that gets saved to the database.
   */
  CaloClusterCorrection(const std::string& type,
 		        const std::string& name,
	 	        const IInterface* parent,
                        const std::string& toolcls = "");

};

#endif
