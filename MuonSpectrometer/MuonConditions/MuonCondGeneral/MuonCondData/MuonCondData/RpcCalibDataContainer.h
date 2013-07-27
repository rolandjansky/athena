/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCCALIBDATACONTAINER_H
#define RPCCALIBDATACONTAINER_H
/**
        @class RpcCondDataContainer

        @author andrea.di.simone@cern.ch

        @brief holds all information of the cscRPC Conditions data

        Holds conditions constants from the cool database. Designed for internal use by RpcCoolStrSvc. based on similar CSC code

**/
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "MuonCondData/RpcCalibData.h"

namespace MuonCalib {

class RpcCalibDataContainer : public DataVector<RpcCalibData> 
{
 public:
  RpcCalibDataContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS)
	  : DataVector<RpcCalibData>(own) {}
  virtual ~RpcCalibDataContainer() {};
};

}

CLASS_DEF(MuonCalib::RpcCalibDataContainer, 1225699625, 1)

#endif
