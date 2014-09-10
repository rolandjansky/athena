/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLANARCLUSTERCONTAINERCNV_H
#define PLANARCLUSTERCONTAINERCNV_H

#define private public
#define protected public
#include "ISF_FatrasEvent/PlanarClusterCollection.h"
#include "ISF_FatrasEvent/PlanarClusterContainer.h"
#include "GaudiKernel/MsgStream.h"
#undef private
#undef protected
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "ISF_FatrasEventTPCnv/PlanarClusterContainerCnv_p1.h"

// the latest persistent representation type of DataCollection:
typedef  iFatras::PlanarClusterContainer_p1  PlanarClusterContainer_PERS;
typedef  T_AthenaPoolCustomCnv<iFatras::PlanarClusterContainer, PlanarClusterContainer_PERS >  PlanarClusterContainerCnvBase;

/**
** Create derived converter to customize the saving of identifiable
** container
**/

class PlanarClusterContainerCnv : public PlanarClusterContainerCnvBase
{
  friend class CnvFactory<PlanarClusterContainerCnv >;

  // Converters need to be initialized (use ID helpers)
  // Thus they can't be local
  
protected:
  PlanarClusterContainerCnv (ISvcLocator* svcloc);
  ~PlanarClusterContainerCnv();
  virtual PlanarClusterContainer_PERS*   createPersistent (iFatras::PlanarClusterContainer* transCont);
  virtual iFatras::PlanarClusterContainer* createTransient ();

  // Must initialize ID helpers
  virtual StatusCode initialize();

private:
  void                  updateLog();    //!< This method modifies m_log to indicate the current key being converted
  IMessageSvc*          m_msgSvc;      //!< MsgStream svc
  MsgStream             m_log;         //!< MsgStream

  PlanarClusterContainerCnv_p1   m_converter_p1;
 

};


#endif
