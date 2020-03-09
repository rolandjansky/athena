///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainerCnv_p5.h 
// Header file for class TruthParticleContainerCnv_p5
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINERCNV_P5_H 
#define MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINERCNV_P5_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// DataModel converters
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthParticleContainer_p5.h"

// Forward declaration
class MsgStream;
class StoreGateSvc;
class ITruthParticleCnvTool;

class TruthParticleContainerCnv_p5 : public T_AthenaPoolTPCnvConstBase<
                                              TruthParticleContainer, 
				              TruthParticleContainer_p5
                                            >  
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  TruthParticleContainerCnv_p5();

  /** Constructor with parameters: 
   */
  TruthParticleContainerCnv_p5( ITruthParticleCnvTool* cnvTool );


  /** Method creating the transient representation of @c TruthParticleContainer
   *  from its persistent representation @c TruthParticleContainer_p5
   */
  virtual void persToTrans( const TruthParticleContainer_p5* persObj, 
			    TruthParticleContainer* transObj, 
			    MsgStream& msg ) const override;

  /** Method creating the persistent representation @c TruthParticleContainer_p5
   *  from its transient representation @c TruthParticleContainer
   */
  virtual void transToPers( const TruthParticleContainer* transObj, 
			    TruthParticleContainer_p5* persObj, 
			    MsgStream& msg ) const override;

 protected: 

  /** Pointer to the IAlgTool providing the conversion from a 
   *  @c McEventCollection to a @c TruthParticleContainer
   */
  ITruthParticleCnvTool* m_cnvTool;

  /** Converter for the ElementLink<McEventCollection> data member
   */
  ElementLinkCnv_p1<ElementLink<McEventCollection> > m_genEvtCnv;

  /** Converter for the ElementLink<TruthEtIsolationsContainer> data member
   */
  ElementLinkCnv_p1<ElementLink<TruthEtIsolationsContainer> > m_etIsolCnv;

}; 


inline TruthParticleContainerCnv_p5::TruthParticleContainerCnv_p5() :
  m_cnvTool   ( 0 ),
  m_genEvtCnv (   ),
  m_etIsolCnv (   )
{}

inline 
TruthParticleContainerCnv_p5::TruthParticleContainerCnv_p5( ITruthParticleCnvTool* cnvTool ) :
  m_cnvTool   ( cnvTool ),
  m_genEvtCnv (   ),
  m_etIsolCnv (   )
{}

template<>
class T_TPCnv<TruthParticleContainer, TruthParticleContainer_p5>
  : public TruthParticleContainerCnv_p5
{
public:
  T_TPCnv();
  ~T_TPCnv();
};

#endif //> MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINERCNV_P5_H
