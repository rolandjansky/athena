/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MooSegmentCombinationFinder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONMOOSEGMENTFINDER_H
#define MUONMOOSEGMENTFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonEDM_AssociationObjects/MuonSegmentCombPatternCombAssociationMap.h"
#include "MuonSegmentCombinerToolInterfaces/IMooSegmentCombinationFinder.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"

#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "Identifier/Identifier.h"
#include <map>


class ICscSegmentFinder;
class MdtIdHelper;

namespace Muon 
{
    class IMuonHoughPatternFinderTool;
    class IMuonPatternSegmentMaker;
    class IMuonCurvedSegmentCombiner;
    class IMuonSegmentCombinationCleanerTool;
    class IMuonSegmentSelectionTool;
    class MuonEDMPrinterTool;
    class MuonIdHelperTool;

  /** @class MooSegmentCombinationFinder 
      
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
      
      /** This method cannot currently be const, since it needs to call non-const methods of child tools. 
      However this should be okay for MT as long as this tool is a private tool of the parent Algorithm.*/
      void findSegments(const std::vector<const MdtPrepDataCollection*>& mdtCols,
                  			const std::vector<const CscPrepDataCollection*>& cscCols,
                  			const std::vector<const TgcPrepDataCollection*>& tgcCols,
                  			const std::vector<const RpcPrepDataCollection*>& rpcCols,
                  			IMooSegmentCombinationFinder::Output& output);
      
    private:

      /** helper functions to print summary output for the different stages */
      void printStage( std::string stageTag) const;
      void printSummary( std::string stageTag, const MuonSegmentCombinationCollection* col ) const;
      void printSummary( std::string stageTag, const MuonPatternCombinationCollection* col ) const;
      void printSummary( std::string stageTag, const Trk::SegmentCollection* col ) const;

      /** helper functions to write out intermediate results */
      void postProcess(  MuonSegmentCombinationCollection* col, MuonSegmentCombPatternCombAssociationMap& segmentPatternMap) ;

      /** extract a segment collection from a segment combination collection */
      void extractSegmentCollection( const MuonSegmentCombinationCollection* combiCol, Trk::SegmentCollection& segments  ) const;
      
      /** select segment on quality */
      bool goodSegment( const MuonSegment& segment ) const;
      std::pair<int,int> hitsInMultilayer( const Muon::MuonSegment& segment ) const;
      bool firstIsBest( const Muon::MuonSegment& seg1, const Muon::MuonSegment& seg2 ) const;

      /** class member version of retrieving MsgStream */
      bool                                            m_doSummary; //<! print summary after each stage
      
      bool                                            m_doCscSegments; //<! run CSC segment finding
      bool                                            m_doMdtSegments; //<! run MDT segment finding
      bool                                            m_doSegmentCombinations; //<! run segment combination finding
      bool                                            m_doSegmentCombinationCleaning; //<! run segment combination cleaning
      
      ToolHandle<MuonEDMPrinterTool>                 m_edmPrinter;
      ServiceHandle<IMuonEDMHelperSvc>               m_edmHelperSvc {this, "edmHelper", 
        "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
        "Handle to the service providing the IMuonEDMHelperSvc interface" };
      ToolHandle<MuonIdHelperTool>                   m_idHelperTool;
      ToolHandle<ICscSegmentFinder>                  m_csc2dSegmentFinder;
      ToolHandle<ICscSegmentFinder>                  m_csc4dSegmentFinder;
      ToolHandle<IMuonHoughPatternFinderTool>        m_houghPatternFinder;
      ToolHandle<IMuonPatternSegmentMaker>           m_patternSegmentMaker;
      ToolHandle<IMuonCurvedSegmentCombiner>         m_curvedSegmentCombiner;
      ToolHandle<IMuonSegmentCombinationCleanerTool> m_segmentCombinationCleaner;
      ToolHandle<IMuonSegmentSelectionTool>          m_segmentSelector;    

      bool m_cloneSegments;

      /** counters */
      mutable std::atomic_uint m_nevents;
      mutable std::atomic_uint m_ncsc2SegmentCombinations;
      mutable std::atomic_uint m_ncsc4SegmentCombinations;
      mutable std::atomic_uint m_npatternCombinations;
      mutable std::atomic_uint m_nmdtSegmentCombinations;
      mutable std::atomic_uint m_ncombinedSegmentCombinations;
      mutable std::atomic_uint m_ncleanedSegmentCombinations;
      mutable std::atomic_uint m_nsegments;
      mutable std::atomic_uint m_nsegmentsStraight;
      mutable std::atomic_uint m_nsegmentsCurved;
      mutable std::atomic_uint m_nremovedBadSegments;

    };

} // end of namespace

#endif 
