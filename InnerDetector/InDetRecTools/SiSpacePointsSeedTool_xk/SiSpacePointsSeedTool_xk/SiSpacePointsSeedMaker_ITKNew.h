// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_ITKNew
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_ITKNew_H
#define SiSpacePointsSeedMaker_ITKNew_H

#include <list>
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeedITK.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsProSeedITK.h" 
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMakerEventData.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TFile.h"
#include "TTree.h"


class MsgStream   ;
class IBeamCondSvc;

namespace InDet {

  using EventData = SiSpacePointsSeedMakerEventData;

  class SiSpacePointsSeedMaker_ITKNew : 
    virtual public ISiSpacePointsSeedMaker, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      SiSpacePointsSeedMaker_ITKNew
	(const std::string&,const std::string&,const IInterface*);
      virtual ~SiSpacePointsSeedMaker_ITKNew() = default;
      virtual StatusCode               initialize();
      virtual StatusCode               finalize  ();

      ///////////////////////////////////////////////////////////////////
      /// @name Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////
  
      void newEvent (EventData& data, int iteration);
      void newEvent (int iteration){ newEvent(m_data,iteration); }
      void newRegion(EventData& data,
		     const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT);
      void newRegion(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT){ newRegion(m_data,vPixel,vSCT); }
      void newRegion(EventData& data,
		     const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,
		     const IRoiDescriptor& iRD);
      void newRegion(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT, const IRoiDescriptor& iRD){ newRegion(m_data,vPixel,vSCT,iRD); }

      ///////////////////////////////////////////////////////////////////
      /// @name Methods to initilize different strategies of seeds production
      ///////////////////////////////////////////////////////////////////

      /// With two space points with or without vertex constraint
      void find2Sp(const std::list<Trk::Vertex>&){};

      /// with three space points with or without vertex constraint
      void find3Sp(EventData& data, const std::list<Trk::Vertex>& lv);
      void find3Sp(const std::list<Trk::Vertex>& lv){ find3Sp(m_data,lv); }

      /// with three space points with or without vertex constraint
      /// with information about min and max Z of the vertex
      void find3Sp(EventData& data, const std::list<Trk::Vertex>& lv, const double* zVertex);
      void find3Sp(const std::list<Trk::Vertex>& lv, const double* zVertex){ find3Sp(m_data,lv,zVertex); }

      /// with variable number space points with or without vertex constraint
      /// Variable means (2,3,4,....) any number space points
      void findVSp(EventData& data, const std::list<Trk::Vertex>& lv);
      void findVSp(const std::list<Trk::Vertex>& lv){ findVSp(m_data,lv); }

      ///////////////////////////////////////////////////////////////////
      /// @name Iterator through seeds pseudo collection
      /// produced accordingly methods find    
      ///////////////////////////////////////////////////////////////////
      const SiSpacePointsSeed* next(EventData& data);
      const SiSpacePointsSeed* next(){ return next(m_data); }

      virtual void writeNtuple(const SiSpacePointsSeed* seed, const Trk::Track* track, int seedType, long eventNumber) const override;

      virtual bool getWriteNtupleBoolProperty() const override;


      ///////////////////////////////////////////////////////////////////
      /// @name Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
      MsgStream&    dump          (MsgStream   & out) const {return dump(m_data, out);}
      std::ostream& dump          (std::ostream& out) const {return out;}


    protected:

      /// enum for array sizes
      /// Note that this stores the maximum capacities, the actual binnings 
      /// do not always use the full size. See data members below for the 
      /// actual binning paramaters, which are determined in buildFramework. 
      enum Size {arraySizePhi=200,     ///< capacity of the 1D phi arrays 
		 arraySizeZ=11,       ///< capacity of the 1D z arrays
		 arraySizePhiZ=arraySizePhi*arraySizeZ,   ///< capacity for the 2D phi-z arrays 
		 arraySizeNeighbourBins=9,  ///< array size to store neighbouring phi-z-regions in the seed finding
		 arraySizePhiV=100,         ///< array size in phi for vertexing 
		 arraySizeZV=3,             ///< array size in z for vertexing
		 arraySizePhiZV=arraySizePhiV*arraySizeZV,      ///< array size in phi-Z 2D for the vertexing
		 arraySizeNeighbourBinsVertex=6};       ///< array size to store neighbouring phi-z regions for the vertexing

      
      ///////////////////////////////////////////////////////////////////
      // Protected data and methods
      ///////////////////////////////////////////////////////////////////

      mutable EventData m_data;

      SG::ReadHandle<SpacePointContainer>         m_spacepointsSCT    ;
      SG::ReadHandle<SpacePointContainer>         m_spacepointsPixel  ;
      SG::ReadHandle<SpacePointOverlapCollection> m_spacepointsOverlap;

      ToolHandle<Trk::IPRD_AssociationTool>       m_assoTool          ;
      std::string                                 m_beamconditions    ;
      IBeamCondSvc*                               m_beamCondService   ;

      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle ;
      MagField::IMagFieldSvc*                m_fieldService       ;

      /// @name Properties, which will not be changed after construction
      bool                        m_useassoTool                   ;
      bool                        m_pixel                         ;
      bool                        m_sct                           ;
      bool                        m_useOverlap                    ;
      bool                        m_fastTracking                  ;
      bool                        m_isLRT                         ;
      bool                        m_useSeedConfirmation           ;
      int                         m_maxsize                       ;
      int                         m_maxsizeSP                     ;
      /// maximum number of seeds to keep per central space point. 
      /// the top N seeds sorted by quality are preserved if more candidates
      /// than the limit exist 
      int                         m_maxOneSize                    ;
      int                         m_maxOneSizeSSS                 ;
      int                         m_maxOneSizePPP                 ;
      /// This flag will lead to all confirmed seeds (seeds where a second compatible seed
      /// with a different top spacepoint is found) being kept, even in excess of 
      /// maxSeedsForSpacePoint above 
      bool                        m_alwaysKeepConfirmedPixelSeeds ;
      bool                        m_alwaysKeepConfirmedStripSeeds ;
      bool                        m_optimisePhiBinning            ;
      float                       m_etamax                        ;
      float                       m_drmaxPPP                      ;
      float                       m_drmaxSSS                      ;
      float                       m_drminPPP                      ;
      float                       m_drminSSS                      ;
      float                       m_r_rmax                        ;
      float                       m_r_rmin                        ;
      float                       m_binSizeR                      ;
      float                       m_zmin                          ;
      float                       m_zmax                          ;
      float                       m_dzver                         ;
      float                       m_dzdrver                       ;
      float                       m_rminSSS                       ;
      float                       m_rmaxSSS                       ;

      /// @name Properties, which can be updated in buildFrameWork method which is called in initialize method
      float                       m_etamin                        ;
      float                       m_maxdImpact                    ;
      float                       m_maxdImpactSSS                 ;
      /// these flags allow to dynamically tighten the d0 cut on non-confirmed seeds based on 
      /// the penalty score they receive for kinking in the r-z plane. 
      /// The cut is adapted as cut_value = original - slope x penalty
      float                       m_dImpactCutSlopeUnconfirmedSSS ;
      float                       m_dImpactCutSlopeUnconfirmedPPP ;
      float                       m_ptmin                         ;

      /// @name Properties, which can be updated in newEvent method. checketa is prepared in EventData.
      bool                        m_checketa                      ;

      /// Scoring modifiers applied when ranking seeds. 
      /// These are used within the newOneSeedWithCurvaturesComparison method. 
      /// Be aware that a negative score is considered "better", so these should be 
      /// set to negative numbers. 
      float                       m_seedScoreBonusPPP             ;
      float                       m_seedScoreBonusSSS             ;
      float                       m_seedScoreBonusConfirmationSeed;
      float                       m_maxScore                      ;

      /// @name Data members, which are updated only in buildFrameWork in initialize
      /// conversion factors and cached cut values
      float m_dzdrmin0{0.};   ///< implicitly store eta cut
      float m_dzdrmax0{0.};   ///< implicitly store eta cut
      float m_ipt{0.};    ///< inverse of 90% of the ptmin cut 
      float m_ipt2{0.};   ///< inverse square of 90% of the pt min cut 
      static constexpr float m_COF{134*.05*9};    ///< appears to be an approximated term related to multiple-scattering of particles traversing the ID during the seed formation 
      float m_rminPPPFast {70.};
      float m_dzMaxFast   {200.};
      float m_R2MaxFast   {2500.};
      float m_rmaxPPP     {140.};
      float m_dzmaxSSS    {900.};
      float m_zmaxSSS     {2700.};
      float m_zmaxPPP     {2700.};
      float m_drminSeedConf{5.};

      /// @name Binning parameters 
      int m_nBinsR{0};              ///<  number of bins in the radial coordinate 
      int m_maxPhiBin{0};           ///<  number of bins in phi 
      float m_inverseBinSizePhi{0};   ///<  cache the inverse bin size in phi which we use - needed to evaluate phi bin locations

      /// Rmin and Rmax parameters set in fillLists method;
      float m_RTmin;
      float m_RTmax;      

      /** Seed score thresholds defined based on the modifiers defined 
       * as configurables above.
       * These allow to categorise the seeds based on their quality score.
       * The modifiers above are much larger than the range of the raw 
       * unmodified scores would be, resulting in a grouping of the scores
       * based on the modifiers applied. The thresholds are just below 
       * the value reachable without the additional modifier
       **/   
      float m_seedScoreThresholdPPPConfirmationSeed{0.};    ///< max (score is assigned negative sign) score for PPP seeds with confirmation seed requirement. 
      float m_seedScoreThresholdSSSConfirmationSeed{0.};    ///< max (score is assigned negative sign) score for SSS seeds with confirmation seed requirement. 

      /// arrays associating bins to each other for SP formation
      std::array<int,arraySizePhiZ> m_nNeighbourCellsBottom;  ///< number of neighbouring phi-z bins to consider when looking for "bottom SP" candidates for each phi-z bin
      std::array<int,arraySizePhiZ> m_nNeighbourCellsTop;  ///< number of neighbouring phi-z bins to consider when looking for "top SP" candidates for each phi-z bin
      std::array<std::array<int, arraySizeNeighbourBins>, arraySizePhiZ> m_neighbourCellsBottom; ///< mapping of neighbour cells in the 2D phi-z binning to consider  for the "bottom SP" search for central SPs in each phi-z bin. Number of valid entries stored in m_nNeighboursPhiZbottom
      std::array<std::array<int, arraySizeNeighbourBins>, arraySizePhiZ> m_neighbourCellsTop; ///< mapping of neighbour cells in the 2D phi-z binning to consider  for the "top SP" search for central SPs in each phi-z bin. Number of valid entries stored in m_nNeighboursPhiZtop

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////
      MsgStream&    dump          (EventData& data, MsgStream& out)   const;
      MsgStream&    dumpConditions(EventData& data, MsgStream   & out) const;
      MsgStream&    dumpEvent     (EventData& data, MsgStream   & out) const;

      /// prepare several data members with cached cut values,
      /// conversion factors, binnings, etc 
      void buildFrameWork();
      /* updates the beam spot information stored in the event data
       * object. 
       * @param[out] data: Event data, receives update to the x/y/zbeam members 
       **/
      void buildBeamFrameWork(EventData& data) const;
      
      /** Determine the expected azimuthal trajectory displacement
       *  in phi in presence of the magnetic field for a particle 
       *  with momentum pTmin and impact parameter maxd0, 
       *  moving from a radial coordinate Rmin outward to Rmax.
       *  This method is used to determine the optimal binning 
       *  of the phi-z regions we consider in the seed making,
       *  to ensure we contain the hits from our softest tracks
       *  in a set of consecutive bins.
       *  @param[in] pTmin: minimum pt cut applied in MeV
       *  @param[in] maxD0: maximum d0 allowed 
       *  @param[in] Rmin: starting radius for trajectory displacement
       *  @param[in] Rmax: end radius for trajectory displacement
       **/
      float azimuthalStep(const float pTmin,const float maxd0,const float Rmin,const float Rmax) const;

      /** Create a SiSpacePointForSeed from the space point. 
       * This will also add the point to the data object's
       * l_spforseed list and update its i_spforseed iterator 
       * to point to the entry after the new SP 
       * for further additions.
       * Returns a nullptr if the SP fails the eta cut, 
       * should we apply one 
       * @param[in,out] data: Provides beam spot location, receives updates to the l_spforseed and i_spforseed members 
       * @param[in] sp: Input space point. 
       **/
      SiSpacePointForSeedITK* newSpacePoint(EventData& data, Trk::SpacePoint*const& sp);
      SiSpacePointForSeedITK* newSpacePoint(EventData& data, Trk::SpacePoint*const& sp, float* r, bool usePixSctInform=false);

      //void newSeed(EventData& data, SiSpacePointForSeed*& p1, SiSpacePointForSeed*& p2, float z) const;

      /** This inserts a seed into the set of saved seeds. 
       * It internally respects the user-configured max number of seeds per central 
       * space point. Once this is exceeded, the new seed will replace worse-quality 
       * seeds if there are any, otherwise it will not insert anything. 
       * @param[in,out] data Event data - update OneSeeds_Pro and mapOneSeeds_Pro members 
       * @param[in] p1 First space point for this seed
       * @param[in] p2 Second space point for this seed
       * @param[in] p3 Third space point for this seed
       * @param[in] z z0 IP estimate
       * @param[in] quality quality estimate (based on d0, plus modifiers) 
       **/ 
      void newOneSeed(EventData& data,
		      SiSpacePointForSeedITK*& p1, SiSpacePointForSeedITK*& p2,
		      SiSpacePointForSeedITK*& p3, float z, float quality) const;

      /** This creates all possible seeds with the passed central and bottom SP, using all top SP 
       * candidates which are stored in the data.CmSp member.  Seeds are scored by a quality score 
       * seeded by abs(d0), and modified if there is a second-seed confirmation or in case of PPP/SSS 
       * topologies. Then, they are written out via the newOneSeed method. 
       * @param[in] SPb Bottom Space point for the seed creation
       * @param[in] SP0 Central Space point for the seed creation
       * @param[in] Zob z0 estimate 
       **/ 
      void newOneSeedWithCurvaturesComparisonSSS
        (EventData& data, SiSpacePointForSeedITK*& SPb, SiSpacePointForSeedITK*& SP0, float Zob) const;
      void newOneSeedWithCurvaturesComparisonPPP
        (EventData& data, SiSpacePointForSeedITK*& SPb, SiSpacePointForSeedITK*& SP0, float Zob) const;
      void newOneSeedWithCurvaturesComparisonSeedConfirmation
        (EventData& data, SiSpacePointForSeedITK*& SPb, SiSpacePointForSeedITK*& SP0, float Zob) const;

      /// fills the seeds from the mapOneSeeds_Pro member into the l_seeds_Pro member of the data object, applying some more 
      /// quality requirements on the way. 
      void fillSeeds(EventData& data) const;

      /** this method populates the data object's "histograms" (implemented as nested vectors). 
       * using the list of r-binned space points in the object assumed to have been previously
       * set (for example via the newEvent method of this class). 
       **/ 
      void fillLists(EventData& data);
      void fillListsFast(EventData& data);
      void erase(EventData& data) const;
      
      /** Top-level method for 3-SP seed production. 
       * This method loops over each eta-Z region, and in each region 
       * calls the extended production3Sp method below to do the actual work. 
       **/
      void production3Sp(EventData& data);

      /** \brief: Seed production from space points. 
       * 
       * This method will try to find 3-SP combinations within a 
       * local phi-z region in the detector. 
       * 
       * The central SP of the seed will be taken from this region
       * (technically via the first entry of the bottom candidate array, 
       * which always points to the phi-z bin of interest itself). 
       * 
       * The top SP is allowed to come from the same or one of several close-by
       * phi-Z bins, as is the bottom SP. 
       * 
       * All SP collections are expected to be internally sorted in the radial coordinate.
       * 
       * @param[in,out] data: Event data
       * @param[in,out] iter_bottomCands: collection of iterators over SP collections for up to 9 phi-z cells to consider for the bottom space-point search 
       * @param[in,out] iter_endBottomCands: collection of end-iterators over the 
       * SP collections  for up to 9 phi-z cells to consider for the bottom space-point search 
       * @param[in,out] iter_topCands: collection of iterators over SP collections for up to 9 phi-z cells to consider for the top space-point search 
       * @param[in,out] iter_endTopCands: collection of end-iterators over the 
       * SP collections  for up to 9 phi-z cells to consider for the top space-point search 
       * @param[in] numberBottomCells: Number of bottom cells to consider. Determines how many entries in iter_(end)bottomCands are expected to be valid. 
       * @param[in] numberTopCells: Number of top cells to consider.Determines how many entries in iter_(end)topCands are expected to be valid. 
       * @param[out] nseed: Number of seeds found 
       **/ 
      void production3SpSSS
	(EventData& data,
	 std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_bottomCands,
	 std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_endBottomCands,
	 std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_topCands,
	 std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_endTopCands,
	 const int numberBottomCells, const int numberTopCells, int& nseed) const;

      void production3SpPPP
	(EventData& data,
	 std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_bottomCands,
	 std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_endBottomCands,
	 std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_topCands,
	 std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & iter_endTopCands,
	 const int numberBottomCells, const int numberTopCells, int& nseed);

      /// as above, but for the trigger 
      void production3SpTrigger
      (EventData& /*data*/,
       std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & /*rb*/,
       std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & /*rbe*/,
       std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & /*rt*/,
       std::array<std::list<InDet::SiSpacePointForSeedITK*>::iterator, arraySizeNeighbourBins> & /*rte*/,
       const int /*numberBottomCells*/, const int /*numberTopCells*/, int& /*nseed*/) const{}

      /** This method updates the EventData based on the passed list of vertices. 
       * The list may be empty. 
       * Updates the isvertex, l_vertex, zminU and zmaxU members of the data object.
       * Always returns false.  
       * @param[in,out] data  Event data to update 
       * @param[in] lV possibly empty list of vertices
       **/
      bool newVertices(EventData& data, const std::list<Trk::Vertex>& lV) const;

      /** This method is called within next() when we are out of vertices. 
       * It will internally trigger a re-run of production3Sp if we are out of seeds 
       * and data.endlist is not set (indicating the search is not finished). 
       **/ 
      void findNext(EventData& data);

      /** This method popualtes the r array 
       * with the space point's coordinates 
       * relative to the beam spot. 
       * @param[in] data Event data 
       * @param[in] sp: Space point to take the global position from 
       * @param[out] r: array, will be populated with the relative coordinates 
       **/
      void convertToBeamFrameWork(EventData& data, const Trk::SpacePoint*const& sp, float* r) const;

      bool isUsed(const Trk::SpacePoint* sp) const;

      void pixInform(Trk::SpacePoint*const& sp, float* r);
      void sctInform(EventData& data,Trk::SpacePoint*const& sp, float* r);

      void initializeEventData(EventData& data) const;

      /** Helper method to determine if a seed 
       * is 'confirmed' - this means that a second
       * seed exists with compatible curvature, 
       * the same bottom and central SP, but a 
       * different third SP. This information 
       * is stored in a modification of the 
       * seed quality, which we check here. 
       * @param[in] bottomSP: bottom space point
       * @param[in] topSP: top space point 
       * @param[in] quality: seed quality
       * @return true if the seed is confirmed, false otherwise 
       **/ 
      bool isConfirmedSeed(const InDet::SiSpacePointForSeedITK* bottomSP, const InDet::SiSpacePointForSeedITK* topSP, float quality) const;
   
      void sort(std::vector<FloatInt>& s, int start, int size);
      void QuickSort(std::vector<FloatInt>& A, int start, int size);
      void SmallSort(std::vector<FloatInt>& A, int start, int size);
      int  Partition(std::vector<FloatInt>& A, int start, int size);
      void Middle   (std::vector<FloatInt>& A, int start, int size);

      // for validation purpose
      bool                        m_writeNtuple                   ;
      ITHistSvc*                  m_thistSvc                      ;
      TTree*                      m_outputTree                    ;
      std::string                 m_treeName                      ;
      std::string                 m_treeFolder                    ;
      mutable float               m_d0                            ;
      mutable float               m_z0                            ;
      mutable float               m_pt                            ;
      mutable float               m_eta                           ;
      mutable double              m_x1                            ;
      mutable double              m_x2                            ;
      mutable double              m_x3                            ;
      mutable double              m_y1                            ;
      mutable double              m_y2                            ;
      mutable double              m_y3                            ;
      mutable double              m_z1                            ;
      mutable double              m_z2                            ;
      mutable double              m_z3                            ;
      mutable double              m_r1                            ;
      mutable double              m_r2                            ;
      mutable double              m_r3                            ;
      mutable float               m_quality                       ;
      mutable int                 m_type                          ;
      mutable double              m_dzdr_t                        ;
      mutable double              m_dzdr_b                        ;
      mutable bool                m_givesTrack                    ;
      mutable float               m_trackPt                       ;
      mutable float               m_trackEta                      ;
      mutable long                m_eventNumber                   ;
   
    }; 
  
}

///////////////////////////////////////////////////////////////////
// Object-function for curvature seeds comparison
///////////////////////////////////////////////////////////////////

class comCurvatureITK {
public:
  bool operator ()
  (const std::pair<float,InDet::SiSpacePointForSeedITK*>& i1,
   const std::pair<float,InDet::SiSpacePointForSeedITK*>& i2)
  {
    return i1.first < i2.first;
  }
};



namespace InDet {

  inline
  bool SiSpacePointsSeedMaker_ITKNew::isUsed(const Trk::SpacePoint* sp) const
  {
    const Trk::PrepRawData* d = sp->clusterList().first;
    if (!d || m_assoTool->isUsed(*d)) return false;
    d = sp->clusterList().second;
    if (!d || m_assoTool->isUsed(*d)) return true;
    return false;
  }

  ///////////////////////////////////////////////////////////////////
  // The procedure sorts the elements into ascending order.
  ///////////////////////////////////////////////////////////////////
  
  inline
  void SiSpacePointsSeedMaker_ITKNew::sort(std::vector<FloatInt>& s, int start, int size)
  {
    //QuickSort for fast tracking currently buggy
    //TBC if really faster than std::sort
    //Using std::sort in all cases for now
    std::sort(s.begin()+start,s.begin()+start+size,[](const FloatInt a,const FloatInt b)->bool {return a.Fl < b.Fl;});
  }

}




#endif // SiSpacePointsSeedMaker_ITKNew_H

