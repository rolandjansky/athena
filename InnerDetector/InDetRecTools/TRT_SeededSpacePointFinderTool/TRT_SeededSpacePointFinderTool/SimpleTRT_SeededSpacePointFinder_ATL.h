/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ITRT_SeededSpacePointFinder.h"

#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h" 
#include "IRegionSelector/IRegSelSvc.h"

class MsgStream;
class SCT_ID;
class TRT_ID;

namespace Trk{
  class IPRD_AssociationTool;
}

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
      virtual StatusCode               initialize();
      virtual StatusCode               finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Methods to satisfy the interface
      ///////////////////////////////////////////////////////////////////

      /** obsolete, do not do anything. */
      void newEvent ();
      void newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&);      
      const SiSpacePointsSeed* next();

      /** main method, calls the private methods and returns a pointer to
          the list of SpacePointpairs. The list itself is a private member of this
          class */
      std::list<std::pair<const Trk::SpacePoint*,const Trk::SpacePoint*> >* find2Sp (const Trk::TrackParameters&);
      
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump          (MsgStream   & out) const;
      std::ostream& dump          (std::ostream& out) const;

    private:
      
      //StoreGateSvc*                          m_storeGate             ; 
      ToolHandle<Trk::IPRD_AssociationTool>  m_assoTool              ;  //Association tool
      //int                                    m_outputlevel           ;

      /** Controls, if SP have to be checked with the AssociationTool of the
          forward tracking and to avoid double use of measurements.
	  It is set true, if an AssociationTool is specified in the setup */
      bool                                   m_useAssoTool;

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



      /** Lookup table that contains the SCT Layers to be considered to provide
	  SP for the pairing in dependence on the TRT part the track parameter is on */
      modulLookupTable                   m_modulLookupTable[2*SIMPLE_TRT_INDEX_OFFSET+1]      ;
      std::set<int>                      m_modulLookupTableIndex[2*SIMPLE_TRT_INDEX_OFFSET+1] ;


       /** Region Selector */
       ServiceHandle<IRegSelSvc> m_pRegionSelector;

       /**ID SCT helper*/
       const SCT_ID* m_sctId;

       /**ID TRT helper*/
       const TRT_ID* m_trtId;

       
       /** List with SP pairs as seed for the Si part of the back-track. It is cleared and filled  in
           every call of find2Sp, the pointer to it is returned. */
       std::list<std::pair<const Trk::SpacePoint*, const Trk::SpacePoint*> > m_listOfSpacePointPairsBuffer;
       
       /** determines the width of the considered ROI around the TP. deltaPhi and deltaEta are return values */
       //void getSearchRange(const Trk::TrackParameters& directionTRT, double& deltaPhi, double& deltaEta);       	 
       void getSearchRange(double& deltaPhi, double& deltaEta);       	 

       /** retrieves SP Collections of modules in the ROI and sorts them by SCT layer */
       void getSpacePointsInROI(std::set<IdentifierHash>& setOfSCT_Hashes, int modulTRT,  std::multimap<int,const Trk::SpacePoint*>& relevantSpacePoints);
       
       /** obtains the hashes of modules in the ROI */ 
       void getHashesInROI(const Trk::TrackParameters& directionTRT, std::set<IdentifierHash>& setOfSCT_Hashes);

       /** builds pairs of SP according to the kook-up table */
       void combineSpacePoints(const std::multimap<int,const Trk::SpacePoint*>& relevantSpacePoints,
			       const Trk::TrackParameters& directionTRT, int modulTRT);

       /** applies rough cuts on the quality of a SP pair */
       bool pairIsOk(const Trk::SpacePoint* sp1, const Trk::SpacePoint* sp2, const Trk::TrackParameters& directionTRT );

       /** the name says it */
       void setupLookUpTable();
       void printLookupTable();

       /** returns the number of the TRT wheel or barrel where the TP belongs to */
       int TRT_Module(const Trk::TrackParameters& directionTRT);

    };

  MsgStream&    operator << (MsgStream&   ,const SimpleTRT_SeededSpacePointFinder_ATL&);
  std::ostream& operator << (std::ostream&,const SimpleTRT_SeededSpacePointFinder_ATL&); 

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const SiSpacePointsSeed* SimpleTRT_SeededSpacePointFinder_ATL::next()
    {
      return 0;
    }


  inline void SimpleTRT_SeededSpacePointFinder_ATL::newEvent (){}
  
} // end of name space

#endif // SimpleTRT_SeededSpacePointFinder_ATL_H

