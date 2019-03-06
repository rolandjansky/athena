/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//Local includes
#include "TrigT1NSWSimTools/StripClusterOfflineData.h"

#include <string>


namespace NSWL1 {

  StripClusterOfflineData::StripClusterOfflineData(int bandId,
                           int bcid,
                           int phiId,
						   int isSmall,
						   int moduleId,
						   int sectorId,
						   int wedge,
						   int layer,
						   int size,
						   float charge,
						   float globX,
						   float globY,
						   float globZ) : StripClusterData(),
								  m_bandId(bandId),
								  m_bcid(bcid),
								  m_phiId(phiId),
								  m_isSmall(isSmall),
								  m_moduleId(moduleId),
								  m_sectorId(sectorId),
								  m_wedge(wedge),
								  m_layer(layer),
								  m_size(size),
								  m_charge(charge),
								  m_globX(globX),
								  m_globY(globY),
								  m_globZ(globZ){ }

  StripClusterOfflineData::~StripClusterOfflineData() {
    // StripOfflineData doesn't own the helper, so nothing to do here                                                                                                                                                                    
  }


}
