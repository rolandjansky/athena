/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonPatternSegmentAssociationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONMUONPATTERNSEGMENTASSOCIATIONTOOL_H
#define MUONMUONPATTERNSEGMENTASSOCIATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRecToolInterfaces/IMuonPatternSegmentAssociationTool.h"

class AtlasDetectorID;
class Identifier;

namespace Muon 
{

  /** @class MuonPatternSegmentAssociationTool 

      This is for the Doxygen-Documentation.  
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.
      
      @author  Edward Moyse <Edward.Moyse@cern.ch>
  */  

  class MuonPatternSegmentAssociationTool : virtual public IMuonPatternSegmentAssociationTool, public AthAlgTool
    {
    public:
      MuonPatternSegmentAssociationTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~MuonPatternSegmentAssociationTool ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      virtual const IMuonPatternSegmentAssociationTool::AssociationMap& map() const;

      virtual void insert(const MuonSegmentCombination* segmentCombi, const MuonPatternCombination* associatedPatternCombi);

      virtual void reset();

      virtual AssociationMapRange find(const MuonSegmentCombination* segmentCombi) const;
      
      unsigned int count(const MuonSegmentCombination* segmentCombi) const;
      
    private:
      
      /** member variables for algorithm properties: */
      //MuonSegPatAssMap m_map;
      IMuonPatternSegmentAssociationTool::AssociationMap m_map;
      
    }; 
} // end of namespace


inline 
const Muon::IMuonPatternSegmentAssociationTool::AssociationMap& 
Muon::MuonPatternSegmentAssociationTool::map() const {
   return m_map;
}

inline 
void 
Muon::MuonPatternSegmentAssociationTool::insert( const Muon::MuonSegmentCombination* segmentCombi, 
                                                 const Muon::MuonPatternCombination* associatedPatternCombi)
{
    //std::pair<IMuonPatternSegmentAssociationTool::AssociationMap::iterator, bool> retVal;
    m_map.insert(std::make_pair(segmentCombi, associatedPatternCombi) );
    return; // true if successfully inserted into map
}

inline 
Muon::IMuonPatternSegmentAssociationTool::AssociationMapRange 
Muon::MuonPatternSegmentAssociationTool::find(const Muon::MuonSegmentCombination* segmentCombi) const
{
    return m_map.equal_range(segmentCombi);
}

inline 
unsigned int 
Muon::MuonPatternSegmentAssociationTool::count(const MuonSegmentCombination* segmentCombi) const
{
    return m_map.count(segmentCombi);    
}


inline 
void
Muon::MuonPatternSegmentAssociationTool::reset(){
   m_map.clear();
}

#endif 
