/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICLUSTERCLASSIFICATIONTOOL_H
#define CALOINTERFACE_ICLUSTERCLASSIFICATIONTOOL_H
/**
 * @class IClusterClassificationTool
 * @version \$Id: IClusterClassificationTool.h,v 1.1.1.1 2009-01-27 09:00:56 gunal Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 27-September-2005
 * @brief Interface class for tools classifying CaloCluster objects.
 *
 * Each classification tool has to derive from this class and provide
 * the method: CaloRecoStatus::StatusIndicator classify(const
 * CaloCluster*) which typically should return the classification
 * according to this tool for the cluster pointed to in the classify
 * call.  */

#include "GaudiKernel/IAlgTool.h"
#include "CaloEvent/CaloRecoStatus.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

class IClusterClassificationTool : virtual public IAlgTool
{
 public:

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_IClusterClassificationTool("IClusterClassificationTool", 1 , 0);
    return IID_IClusterClassificationTool; 
  }


  /**
   * @brief method to return the classification status of a cluster.
   * @param theCluster the pointer to the CaloCluster to be classified
   *
   * this method is purely virtual because every derived class needs
   * to implement it.  */
  virtual CaloRecoStatus::StatusIndicator classify(xAOD::CaloCluster* thisCluster) = 0;

  virtual StatusCode LoadConditionsData(IOVSVC_CALLBACK_ARGS) =0;

};
#endif

