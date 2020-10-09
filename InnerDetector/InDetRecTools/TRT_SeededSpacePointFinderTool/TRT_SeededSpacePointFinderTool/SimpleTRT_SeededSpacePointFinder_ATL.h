/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SimpleTRT_SeededSpacePointFinder_ATL
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 12-APR-2007 Martin Siebel
/////////////////////////////////////////////////////////////////////////////////

#ifndef SimpleTRT_SeededSpacePointFinder_ATL_H
#define SimpleTRT_SeededSpacePointFinder_ATL_H
#define SIMPLE_TRT_INDEX_OFFSET 14
#include <list>

//Tool Handle
//
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ITRT_SeededSpacePointFinder.h"

#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h" 
#include "IRegionSelector/IRegSelTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

class MsgStream;
class SCT_ID;
class TRT_ID;

namespace InDet{

  class SimpleTRT_SeededSpacePointFinder_ATL : 
    virtual public ITRT_SeededSpacePointFinder, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      typedef std::list<std::pair< int,int > > modulLookupTable;


      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////
      SimpleTRT_SeededSpacePointFinder_ATL
	(const std::string&,const std::string&,const IInterface*);
      virtual ~SimpleTRT_SeededSpacePointFinder_ATL();
      virtual StatusCode               initialize() override;
      virtual StatusCode               finalize  () override;

      ///////////////////////////////////////////////////////////////////
      // Methods to satisfy the interface
      ///////////////////////////////////////////////////////////////////

      /** obsolete, do not do anything. */
      virtual std::unique_ptr<InDet::ITRT_SeededSpacePointFinder::IEventData> newEvent () const override;
      virtual std::unique_ptr<InDet::ITRT_SeededSpacePointFinder::IEventData> newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&) const override;
      const SiSpacePointsSeed* next(InDet::ITRT_SeededSpacePointFinder::IEventData &event_data) const override;

      /** main method, calls the private methods and returns a pointer to
          the list of SpacePointpairs. The list itself is a private member of this
          class */
      std::list<std::pair<const Trk::SpacePoint*,const Trk::SpacePoint*> > find2Sp (const EventContext& ctx,
                                                                                    const Trk::TrackParameters&,
                                                                                    InDet::ITRT_SeededSpacePointFinder::IEventData &event_data) const override;
      
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump          (MsgStream   & out) const override;
      std::ostream& dump          (std::ostream& out) const override;

    private:
      
      /** Controls, if SP have to be checked with the AssociationTool of the
          forward tracking and to avoid double use of measurements.
	  It is set true, if an AssociationTool is specified in the setup */

      /** controls if the ROI is restricted around the TRT trackparameters */
      bool                                   m_useROI;

      ///////////////////////////////////////////////////////////////////
      // Overall selection criteria cuts. See source code for explanation
      ///////////////////////////////////////////////////////////////////
      int                            m_maxLayers;
      /** controls how many not considered SCT layers are allowed between two SP in order
          to form a seed pair */
      int                            m_maxHoles;

      /** rough cuts on the quality of the suggested SP pair */
      double                         m_perigeeCut;
      double                         m_directionPhiCut;
      double                         m_directionEtaCut;
      double                         m_positionPhiCut;
      double                         m_positionZ_Cut;


      ///////////////////////////////////////////////////////////////////
      // Space points container
      ///////////////////////////////////////////////////////////////////


      SG::ReadHandleKey<SpacePointContainer>         m_spacepointsSCTname {this,"SpacePointsSCTName","SCT_SpacePoints","RHK to retrieve SCT SpacePointContainer"}            ;
      SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlapname {this,"SpacePointsSCTName","OverlapSpacePoints","RHK to retrieve OverlapCollection"}        ;
      SG::ReadHandleKey<Trk::PRDtoTrackMap>          m_prdToTrackMap
         {this,"PRDtoTrackMap",""};

      /** Lookup table that contains the SCT Layers to be considered to provide
	  SP for the pairing in dependence on the TRT part the track parameter is on */
      modulLookupTable                   m_modulLookupTable[2*SIMPLE_TRT_INDEX_OFFSET+1]      ;
      std::set<int>                      m_modulLookupTableIndex[2*SIMPLE_TRT_INDEX_OFFSET+1] ;


       /** Region Selector */
       ToolHandle<IRegSelTool> m_pRegionSelector{ this, "RegSelTool_SCT", "RegSelTool/RegSelTool_SCT" };

       /**ID SCT helper*/
       const SCT_ID* m_sctId;

       /**ID TRT helper*/
       const TRT_ID* m_trtId;

       
       /** List with SP pairs as seed for the Si part of the back-track. It is cleared and filled  in
           every call of find2Sp, the pointer to it is returned. */
       
       /** determines the width of the considered ROI around the TP. deltaPhi and deltaEta are return values */
       //void getSearchRange(const Trk::TrackParameters& directionTRT, double& deltaPhi, double& deltaEta);
       void getSearchRange(double& deltaPhi, double& deltaEta) const;

       /** retrieves SP Collections of modules in the ROI and sorts them by SCT layer */
       void getSpacePointsInROI(const EventContext& ctx,
                                std::set<IdentifierHash>& setOfSCT_Hashes,
                                int modulTRT,
                                std::multimap<int,
                                const Trk::SpacePoint*>& relevantSpacePoints) const;

       /** obtains the hashes of modules in the ROI */ 
       void getHashesInROI(const Trk::TrackParameters& directionTRT, std::set<IdentifierHash>& setOfSCT_Hashes) const;

       /** builds pairs of SP according to the kook-up table */
       void combineSpacePoints(const std::multimap<int,const Trk::SpacePoint*>& relevantSpacePoints,
			       const Trk::TrackParameters& directionTRT,
                               int modulTRT,
                               std::list<std::pair<const Trk::SpacePoint*, const Trk::SpacePoint*> > &listOfSpacePointPairsBuffer) const;

       /** applies rough cuts on the quality of a SP pair */
       bool pairIsOk(const Trk::SpacePoint* sp1, const Trk::SpacePoint* sp2, const Trk::TrackParameters& directionTRT ) const;

       /** the name says it */
       void setupLookUpTable();
       void printLookupTable() const;

       /** returns the number of the TRT wheel or barrel where the TP belongs to */
       int TRT_Module(const Trk::TrackParameters& directionTRT) const;

    };

  MsgStream&    operator << (MsgStream&   ,const SimpleTRT_SeededSpacePointFinder_ATL&);
  std::ostream& operator << (std::ostream&,const SimpleTRT_SeededSpacePointFinder_ATL&); 

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const SiSpacePointsSeed* SimpleTRT_SeededSpacePointFinder_ATL::next(InDet::ITRT_SeededSpacePointFinder::IEventData &) const
    {
      return nullptr;
    }


  inline std::unique_ptr<InDet::ITRT_SeededSpacePointFinder::IEventData> SimpleTRT_SeededSpacePointFinder_ATL::newEvent () const {
     return std::unique_ptr<InDet::ITRT_SeededSpacePointFinder::IEventData>();
   }
  
} // end of name space

#endif // SimpleTRT_SeededSpacePointFinder_ATL_H

