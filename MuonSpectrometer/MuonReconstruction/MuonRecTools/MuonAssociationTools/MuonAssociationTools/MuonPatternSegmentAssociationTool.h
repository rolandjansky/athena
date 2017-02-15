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

      virtual void insert(const MuonSegmentCombination* segmentCombi, const MuonPatternCombination* associatedPatternCombi) const;

      virtual void reset() const;

      virtual AssociationMapRange find(const MuonSegmentCombination* segmentCombi) const;
      
      unsigned int count(const MuonSegmentCombination* segmentCombi) const;
      
    private:
		std::string m_key; //!< Key for association map (defaults to 'MuonPatternSegmentAssociationMap')   
        IMuonPatternSegmentAssociationTool::AssociationMap& nonconst_map() const;
		   
    }; 
} // end of namespace


#endif 
