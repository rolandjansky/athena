/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTOND3PDMAKER_VXTRACKATVERTEXASSOCIATIONTOOL_H
#define PHOTOND3PDMAKER_VXTRACKATVERTEXASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include <vector>

namespace Trk{
  class VxCandidate;
  class VxTrackAtVertex;
}

namespace D3PD {

  class VxTrackAtVertexAssociationTool
    : public MultiAssociationTool<Trk::VxCandidate, Trk::VxTrackAtVertex> 
    {
    public:
      typedef MultiAssociationTool<Trk::VxCandidate, Trk::VxTrackAtVertex> Base;
      
      VxTrackAtVertexAssociationTool (const std::string& type,
				  const std::string& name,
				  const IInterface* parent);
      
      /**
       * @brief Start the iteration for a new association.
       * @param p The object from which to associate.
       */
      virtual StatusCode reset (const Trk::VxCandidate& p);
      
      /**
       * @brief Return a pointer to the next element in the association.
       *
       * Return 0 when the association has been exhausted.
       */
      virtual const Trk::VxTrackAtVertex* next();
      
    private:
            
      std::vector<Trk::VxTrackAtVertex*>::const_iterator m_trkItr;
      std::vector<Trk::VxTrackAtVertex*>::const_iterator m_trkEnd;
            
    };

} // namespace D3PD


#endif // not PHOTOND3PDMAKER_VXTRACKATVERTEXASSOCIATIONTOOL_H
