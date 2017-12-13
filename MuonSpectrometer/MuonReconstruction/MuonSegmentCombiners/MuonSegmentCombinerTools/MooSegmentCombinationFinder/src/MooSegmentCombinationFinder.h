/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MooSegmentCombinationFinder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONMOOSEGMENTFINDER_H
#define MUONMOOSEGMENTFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonEDM_AssociationObjects/MuonSegmentCombPatternCombAssociationMap.h"
#include "MuonSegmentCombinerToolInterfaces/IMooSegmentCombinationFinder.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"

#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "Identifier/Identifier.h"
#include "StoreGate/WriteHandleKey.h"
#include <map>


class ICscSegmentFinder;
class StoreGateSvc;
class MdtIdHelper;

namespace Muon 
{
    class IMuonHoughPatternFinderTool;
    class IMuonPatternSegmentMaker;
    class IMuonCurvedSegmentCombiner;
    class IMuonSegmentCombinationCleanerTool;
    class IMuonSegmentOverlapRemovalTool;
    class IMuonSegmentSelectionTool;
    class MuonEDMPrinterTool;
    class MuonEDMHelperTool;
    class MuonIdHelperTool;

  /** @class MooSegmentCombinationFinder 

      This is for the Doxygen-Documentation.  
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.
      
      @author  Edward Moyse <edward.moyse@cern.ch>
  */  

  class MooSegmentCombinationFinder : virtual public IMooSegmentCombinationFinder, public AthAlgTool
    {
    public:
      MooSegmentCombinationFinder(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~MooSegmentCombinationFinder ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      IMooSegmentCombinationFinder::Output* 
	findSegments( const std::vector<const MdtPrepDataCollection*>& mdtCols,  
		      const std::vector<const CscPrepDataCollection*>& cscCols,  
		      const std::vector<const TgcPrepDataCollection*>& tgcCols,  
		      const std::vector<const RpcPrepDataCollection*>& rpcCols );
      
    private:

      /** helper functions to print summary output for the different stages */
      void printStage( std::string stageTag) const;
      void printSummary( std::string stageTag, const MuonSegmentCombinationCollection* col ) const;
      void printSummary( std::string stageTag, const MuonPatternCombinationCollection* col ) const;
      void printSummary( std::string stageTag, const Trk::SegmentCollection* col ) const;

      /** helper functions to write out intermediate results */
      void postProcess(  MuonSegmentCombinationCollection* col, SG::WriteHandleKey<MuonSegmentCombinationCollection> &colLocation );

      /** extract a segment collection from a segment combination collection */
      void extractSegmentCollection( const MuonSegmentCombinationCollection& combiCol, Trk::SegmentCollection& segments  ) const;
      
      /** select segment on quality */
      bool goodSegment( const MuonSegment& segment ) const;
      std::pair<int,int> hitsInMultilayer( const Muon::MuonSegment& segment ) const;
      bool firstIsBest( const Muon::MuonSegment& seg1, const Muon::MuonSegment& seg2 ) const;

      template<class I>
      void auditorBefore( ToolHandle< I >& tool ) {
	auditorBefore( &*tool );
      }
      template<class I, class T>
      void auditorAfter( ToolHandle< I >& tool, const T* result ) {
	auditorAfter( &*tool, result );
      }
      void auditorBefore( IAlgTool* tool );
      void auditorAfter( IAlgTool* tool, bool status );


      /** class member version of retrieving MsgStream */
      bool                                            m_doSummary; //<! print summary after each stage
      
      bool                                            m_doCscSegments; //<! run CSC segment finding
      bool                                            m_doMdtSegments; //<! run MDT segment finding
      bool                                            m_doSegmentCombinations; //<! run segment combination finding
      bool                                            m_doSegmentCombinationCleaning; //<! run segment combination cleaning
      bool                                            m_auditorExecute; //<! audit the subtools during "execute"
      
      ToolHandle<MuonEDMPrinterTool>                 m_edmPrinter;
      ToolHandle<MuonEDMHelperTool>                  m_helperTool;
      ToolHandle<MuonIdHelperTool>                   m_idHelperTool;
      ToolHandle<ICscSegmentFinder>                  m_csc2dSegmentFinder;
      ToolHandle<ICscSegmentFinder>                  m_csc4dSegmentFinder;
      ToolHandle<IMuonHoughPatternFinderTool>        m_houghPatternFinder;
      ToolHandle<IMuonPatternSegmentMaker>           m_patternSegmentMaker;
      ToolHandle<IMuonCurvedSegmentCombiner>         m_curvedSegmentCombiner;
      ToolHandle<IMuonSegmentCombinationCleanerTool> m_segmentCombinationCleaner;
      ToolHandle<IMuonSegmentOverlapRemovalTool>     m_overlapRemovalTool;    
      ToolHandle<IMuonSegmentSelectionTool>          m_segmentSelector;    

      SG::WriteHandleKey<MuonSegmentCombinationCollection> m_csc2dLocation;
      SG::WriteHandleKey<MuonSegmentCombinationCollection> m_csc4dLocation;
      SG::WriteHandleKey<MuonSegmentCombinationCollection> m_mdtSegmentCombinationLocation;
      SG::WriteHandleKey<MuonSegmentCombinationCollection> m_curvedCombinationLocation;
      MuonSegmentCombPatternCombAssociationMap m_segmentPatternMap;
      bool m_cloneSegments;

      /** counters */
      mutable unsigned int m_nevents;
      mutable unsigned int m_ncsc2SegmentCombinations;
      mutable unsigned int m_ncsc4SegmentCombinations;
      mutable unsigned int m_npatternCombinations;
      mutable unsigned int m_nmdtSegmentCombinations;
      mutable unsigned int m_ncombinedSegmentCombinations;
      mutable unsigned int m_ncleanedSegmentCombinations;
      mutable unsigned int m_nsegments;
      mutable unsigned int m_nsegmentsStraight;
      mutable unsigned int m_nsegmentsCurved;
      mutable unsigned int m_nremovedSegments;
      mutable unsigned int m_nremovedBadSegments;

    }; 
} // end of namespace

#endif 
