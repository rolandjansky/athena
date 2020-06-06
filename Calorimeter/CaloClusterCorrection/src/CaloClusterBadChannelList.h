/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOCLUSTERBADCHANNELLIST_H
#define CALOCLUSTERCORRECTION_CALOCLUSTERBADCHANNELLIST_H
/********************************************************************

NAME:     CaloClusterBadChannelList.h

AUTHORS:  G.Unal
CREATED:  May, 2008

PURPOSE: Fill bad channel information from the cells in a cluster

Base class: CaloClusterCorrection (tool)

********************************************************************/

#include "GaudiKernel/ToolHandle.h"

#include "CaloClusterCorrection/CaloClusterCorrection.h"
#include "CaloConditions/ICaloBadChanTool.h"

class CaloClusterBadChannelList : public CaloClusterCorrection
{

 public:

  // constructor 
  CaloClusterBadChannelList(const std::string& type,
		   const std::string& name,
		   const IInterface* parent);
  
  // Standard initialize method.
  virtual StatusCode initialize() override;

  // virtual method in CaloClusterCorrection
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;

 private:

  // If true, the total cluster energy is set to the sum of all layer energies.
 ToolHandle<ICaloBadChanTool> m_badChannelTool;

};


#endif

