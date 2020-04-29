/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */

#ifndef INDETD3PDMAKER_TRUTHVERTEXGETTERTOOL_H
#define INDETD3PDMAKER_TRUTHVERTEXGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "GeneratorObjects/McEventCollection.h"

namespace D3PD {

typedef McEventCollection TruthVertexGetterCollectionType;
typedef HepMC::GenVertex TruthVertexGetterElementType;
typedef D3PD::SGCollectionGetterTool<TruthVertexGetterCollectionType> TruthVertexGetterBase;

class TruthVertexGetterTool
  : public TruthVertexGetterBase
{
public:
  TruthVertexGetterTool (const std::string& type,
			 const std::string& name,
			 const IInterface* parent);

  StatusCode initialize();
  size_t sizeHint(bool=false);
  StatusCode reset(bool=false);
  const void* nextUntyped();

  const std::type_info& typeinfo() const;
  const std::type_info& elementTypeinfo() const;

private:

  /* Event iterators **/
  McEventCollection::const_iterator m_evtItr;
  McEventCollection::const_iterator m_evtEnd;

  /* Vertex iterators **/
  HepMC::GenEvent::vertex_const_iterator m_vxItr;
  HepMC::GenEvent::vertex_const_iterator m_vxEnd;

  /* Settings **/
  bool m_keepOnlyPrimaryVertices;
  bool m_keepPileUpVertices;

}; // class TruthVertexGetterTool

} // namespace D3PD

#endif // not INDETD3PDMAKER_TRUTHVERTEXGETTERTOOL_H
