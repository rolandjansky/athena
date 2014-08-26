/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ICALOCLUSTERROI_BUILDER_H
#define ICALOCLUSTERROI_BUILDER_H

#include "GaudiKernel/IAlgTool.h"

#include "xAODCaloEvent/CaloClusterFwd.h"

namespace Trk{
  class CaloClusterROI;
}

namespace InDet
{

static const InterfaceID IID_caloClusterROI_Builder("ICaloClusterROI_Builder",1,0);


class ICaloClusterROI_Builder : virtual public IAlgTool
{
 public:
	static const InterfaceID& interfaceID() { return IID_caloClusterROI_Builder; };

  ~ICaloClusterROI_Builder() {}; 

	virtual Trk::CaloClusterROI* buildClusterROI( const xAOD::CaloCluster* cl) const = 0;
  
};

} //End namespace

#endif //ICALOCLUSTERROI_BUILDER_H
