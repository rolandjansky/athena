/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventAthenaPool/src/HGTD_ClusterContainerCnv.h
 * @author Noemi Calace <noemi.calace@cern.ch>
 * @date June, 2021
 * @brief
 */

#ifndef HGTD_CLUSTERCONTAINERCNV_H
#define HGTD_CLUSTERCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/MsgStream.h"
#include "HGTD_PrepRawData/HGTD_ClusterCollection.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"

#include "HGTD_EventTPCnv/HGTD_ClusterContainerCnv_p1.h"

// the latest persistent representation type of DataCollection:
typedef HGTD_ClusterContainer_p1 HGTD_ClusterContainer_PERS;
typedef T_AthenaPoolCustomCnv<HGTD_ClusterContainer,
                              HGTD_ClusterContainer_PERS>
    HGTD_ClusterContainerCnvBase;

/**
** Create derived converter to customize the saving of identifiable
** container
**/

class HGTD_ClusterContainerCnv : public HGTD_ClusterContainerCnvBase {
  friend class CnvFactory<HGTD_ClusterContainerCnv>;

  // Converters need to be initialized (use ID helpers)
  // Thus they can't be local

public:
  HGTD_ClusterContainerCnv(ISvcLocator* svcloc);
protected:
  virtual HGTD_ClusterContainer_PERS*
  createPersistent(HGTD_ClusterContainer* transCont) override;
  virtual HGTD_ClusterContainer* createTransient() override;

private:
  HGTD_ClusterContainerCnv_p1 m_converter_p1;
};

#endif // HGTD_CLUSTERCONTAINERCNV_H
