/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonPatternSegmentAssociationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMUONMUONPATTERNSEGMENTASSOCIATIONTOOL_H
#define IMUONMUONPATTERNSEGMENTASSOCIATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <map>


namespace Muon 
{
    class MuonSegmentCombination;
    class MuonPatternCombination;

  static const InterfaceID IID_IMuonPatternSegmentAssociationTool("Muon::IMuonPatternSegmentAssociationTool", 1, 0);

  class IMuonPatternSegmentAssociationTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;
    
    typedef std::multimap<const MuonSegmentCombination*, const MuonPatternCombination*> AssociationMap;
    typedef std::pair<AssociationMap::const_iterator, AssociationMap::const_iterator> AssociationMapRange;
    
    virtual const AssociationMap& map() const =0;
    
    virtual void insert(const MuonSegmentCombination* segmentCombi, const MuonPatternCombination* associatedPatternCombi)=0;
    
    virtual void reset()=0;
    
    virtual AssociationMapRange find(const MuonSegmentCombination* segmentCombi) const =0;
    
    virtual unsigned int count(const MuonSegmentCombination* segmentCombi) const =0;

  };

  inline const InterfaceID& Muon::IMuonPatternSegmentAssociationTool::interfaceID()
    { 
      return IID_IMuonPatternSegmentAssociationTool; 
    }

} // end of namespace

#endif 
