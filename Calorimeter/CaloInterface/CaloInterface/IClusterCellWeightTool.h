/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++--*-
#ifndef CALOINTERFACE_ICLUSTERCELLWEIGHTTOOL_H
#define CALOINTERFACE_ICLUSTERCELLWEIGHTTOOL_H
/**
 * @class IClusterCellWeightTool
 * @version \$Id: IClusterCellWeightTool.h,v 1.1.1.1 2009-01-27 09:00:56 gunal Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 27-February-2007
 * @brief Interface class for tools weighting cells in CaloCluster objects.
 *
 * Each CellWeight tool has to derive from this class and provide
 * the method: weight(CaloCluster*) which should loop over the cluster
 * constituents and weight them. */

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

class IClusterCellWeightTool : virtual public IAlgTool
{
 public:

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_IClusterCellWeightTool("IClusterCellWeightTool", 1 , 0);
    return IID_IClusterCellWeightTool; 
  }


  /**
   * @brief method to weight the cells in a cluster
   * @param theCluster the pointer to the CaloCluster to be weighted
   *
   * this method is purely virtual because every derived class needs
   * to implement it.  */
  virtual StatusCode weight(xAOD::CaloCluster* thisCluster) = 0;

  virtual StatusCode LoadConditionsData(IOVSVC_CALLBACK_ARGS) =0;

};
#endif

