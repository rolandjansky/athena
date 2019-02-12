/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRECHELPERTOOLS_H
#define MUONRECHELPERTOOLS_H


#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkParameters/TrackParameters.h"

#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/MuonPatternCollection.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include <string>

static const InterfaceID IID_MuonEDMPrinterTool("Muon::MuonEDMPrinterTool",1,0);

class Identifier;
class MsgStream;

namespace Trk {
  class Track;
  class MuonTrackSummary;
  class ResidualPull;
  class MeasurementBase;
  class PrepRawData;
  class ITrackSummaryHelperTool;  
}

namespace Muon{
  
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class MuonSegment;
  class MuonSegmentCombination;
  class MuonPattern;
  class MuonPatternCombination;
  class MuonPatternChamberIntersect;
}

namespace Muon{

  /**
     @brief Helper tool to print EDM objects to string in a fix format

  */
  class MuonEDMPrinterTool : public AthAlgTool {
  public:
    /** @brief constructor */
    MuonEDMPrinterTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    ~MuonEDMPrinterTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonEDMPrinterTool; }
      
    /** @brief print track parameters to string */
    std::string print( const Trk::Track& track ) const;

    /** @brief print stations on track to string */
    std::string printStations( const Trk::Track& track ) const;
    
    /** @brief print stations on track to string */
    std::string print( const Trk::MuonTrackSummary& summary ) const;
    
    /** @brief produce a string indicating who produced the track */
    std::string printPatRec( const Trk::Track& track ) const;

    /** @brief print segment parameters to string */
    std::string print( const MuonSegment& segment ) const;

    /** @brief print vector of segments  */
    std::string print( const std::vector< const MuonSegment* >& segs ) const;
    std::string print( std::vector< std::unique_ptr<MuonSegment> >& segs ) const;

    /** @brief print Muon MeasurementBase to string */
    std::string print( const Trk::MeasurementBase& measurement ) const;

    /** @brief print Muon PrepRawData to string */
    std::string print( const Trk::PrepRawData& prd ) const;

    /** @brief print vector of measurement base to string */
    std::string print( const std::vector< const Trk::MeasurementBase* >& measurements ) const;

    /** @brief print MuonSegmentCombinationCollection */
    std::string print( const MuonSegmentCombinationCollection& combiCol ) const;

    /** @brief print MuonSegmentCombination */
    std::string print( const MuonSegmentCombination& combi ) const;

    /** @brief print MuonPattern */
    std::string print( const MuonPattern& pattern ) const;

    /** @brief print MuonPatternCollection */
    std::string print( const MuonPatternCollection& patCol ) const;

    /** @brief print MuonPatternCollection */
    std::string print( const MuonPrdPatternCollection& patCol ) const;

    /** @brief print MuonPatternCombination */
    std::string print( const MuonPatternCombination& pattern ) const;

    /** @brief print MuonPatternCombinationCollection */
    std::string print( const MuonPatternCombinationCollection& combiCol ) const;

    /** @brief print MuonPatternChamberIntersect */
    std::string print( const MuonPatternChamberIntersect& intersect ) const;

    /** @brief print Trk::TrackParameters */
    std::string print( const Trk::TrackParameters& pars ) const;

    /** @brief  print ResidualPull object to string*/
    std::string print( const Trk::ResidualPull& resPull ) const;

    /** @brief print measurements on track to string */
    std::string printMeasurements( const Trk::Track& track ) const;

    /** @brief print data part of Muon MeasurementBase to string */
    std::string printData( const Trk::MeasurementBase& measurement ) const;

    /** @brief print identifier part of Muon MeasurementBase to string */
    std::string printId( const Trk::MeasurementBase& measurement ) const;

  private:
    
    ToolHandle<MuonIdHelperTool>  m_idHelper;
    ToolHandle<MuonEDMHelperTool> m_helper;
    ToolHandle<Trk::ITrackSummaryHelperTool> m_summaryHelper;
    ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator;

  };

}
#endif
