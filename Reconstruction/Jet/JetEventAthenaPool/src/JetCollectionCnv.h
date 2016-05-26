///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCollectionCnv.h 
// Header file for class JetCollectionCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECATHENAPOOL_JETCOLLECTIONCNV_H 
#define RECATHENAPOOL_JETCOLLECTIONCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/JetCollection_p1.h"
#include "JetEventTPCnv/JetCollection_p2.h"
#include "JetEventTPCnv/JetCollection_p3.h"
#include "JetEventTPCnv/JetCollection_p4.h"
#include "JetEventTPCnv/JetCollection_tlp2.h"
#include "JetEventTPCnv/JetCollection_tlp5.h"
#include "JetEventTPCnv/JetCollection_tlp6.h"
#include "JetEventTPCnv/JetCollectionCnv_tlp2.h"
#include "JetEventTPCnv/JetCollectionCnv_tlp5.h"
#include "JetEventTPCnv/JetCollectionCnv_tlp6.h"
#include "JetEventTPCnv/JetCollectionCnv_p1.h"
#include "JetEventTPCnv/JetCollectionCnv_p2.h"
#include "JetEventTPCnv/JetCollectionCnv_p3.h"
#include "JetEventTPCnv/ParticleJetContainerCnv_p1.h"



// JetEvent includes
#include "JetEvent/JetCollection.h"


// Forward declaration

// the latest persistent representation type of DataCollection:
typedef JetCollection_tlp6  JetCollection_PERS;

typedef T_AthenaPoolCustomCnv<
  JetCollection,
  JetCollection_PERS
  > JetCollectionCnvBase;

/// Converter reponsible for reading in the top level object (tlp1)
/// and sending the data off to the real tlp converter that does
/// the translation work.
///
///  - We have to read in the old format of T/P seperation that had no
///    tlp object. Hence the JetCollection_p1 references.
///  - B-tagging code will add subclasses in its own packages that
///    are stored as a part fo this colleciton. To support this
///    we must declare this as an Extension converter.

class JetCollectionCnv: public JetCollectionCnvBase,
			public AthenaPoolCnvTPExtension
{

  // make the factory for this converter our friend
  friend class CnvFactory<JetCollectionCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  JetCollectionCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual JetCollection_PERS*
    createPersistent( JetCollection* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual JetCollection* createTransient();

  /** Connect up the sub-converters that we use to do the
      actual work.
  */
  StatusCode initialize(void);

  /** Return our top level converter. Used by other converters
      that want to extend this converter.
  */
  inline AthenaPoolTopLevelTPCnvBase *getTopLevelTPCnv(void);

private:
  /** The master translator.
   */
  JetCollectionCnv_tlp6 m_TPConverter;

  /** Log file output for the translators.
   */
  IMessageSvc *m_msgSvc;
  MsgStream m_log;

  // Converters to be allocated once only
  JetCollectionCnv_tlp5       m_tlp5_cnv;
  JetCollectionCnv_tlp2       m_tlp2_cnv;
  JetCollectionCnv_p1         m_p1_cnv;
  JetCollectionCnv_p2         m_p2_cnv;
  JetCollectionCnv_p3         m_p3_cnv;
  ParticleJetContainerCnv_p1  m_pjp1_cnv;

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

AthenaPoolTopLevelTPCnvBase *JetCollectionCnv::getTopLevelTPCnv(void)
{
  return &m_TPConverter;
}

#endif //> RECATHENAPOOL_JETCOLLECTIONCNV_H
