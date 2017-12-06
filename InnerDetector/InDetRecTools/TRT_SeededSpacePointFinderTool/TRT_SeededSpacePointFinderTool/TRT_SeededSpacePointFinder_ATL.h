/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************************
  Header file for class TRT_SeededSpacePointFinder_ATL
  (c) ATLAS Detector software
  Class for Si seed candidates generation using space point information
  Version 1.0: 04/15/2006 
  Authors: Thomas Koffas
  email  : Thomas.Koffas@cern.ch
********************************************************************************/

#ifndef TRT_SeededSpacePointFinder_ATL_H
#define TRT_SeededSpacePointFinder_ATL_H

#include <list>

//Tool Handle
//
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h" 
#include "SiSpacePointsSeed/SiSpacePointsSeed.h"

#include "InDetRecToolInterfaces/ITRT_SeededSpacePointFinder.h"

#include "TrkGeometry/MagneticFieldProperties.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

class MsgStream   ;
class SCT_ID      ;

namespace Trk{
  //class IMagFieldSvc;
  class IPRD_AssociationTool;
}

namespace InDet{

  /** Sorting function according to space point radial position */
  class MyNewDataSortPredicate
    : public std::binary_function<std::pair<const Trk::SpacePoint*,int>, std::pair<const Trk::SpacePoint*,int>, bool> {
    public:
    bool operator()(std::pair<const Trk::SpacePoint*,int> sp1, std::pair<const Trk::SpacePoint*,int> sp2) const
      {return (sp1.first)->r() > (sp2.first)->r();}
  };
}

namespace InDet{

  /**
  @class TRT_SeededSpacePointFinder_ATL 
  
  InDet::TRT_SeededSpacePointFinderATL is a tool which produces Si track
  seeds using pairs of space points in the last 3 layers of the SCT.
  @author Thomas.Koffas@cern.ch     
  */

  class TRT_SeededSpacePointFinder_ATL : 
    virtual public ITRT_SeededSpacePointFinder, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** Standard tool methods                                        */
      ///////////////////////////////////////////////////////////////////

      TRT_SeededSpacePointFinder_ATL
	(const std::string&,const std::string&,const IInterface*);
      virtual ~TRT_SeededSpacePointFinder_ATL();
      virtual StatusCode               initialize();
      virtual StatusCode               finalize  ();

      ///////////////////////////////////////////////////////////////////
      /** Method to initialize tool for new event                      */
      ///////////////////////////////////////////////////////////////////

      void newEvent ();
      void newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&);
      
      ///////////////////////////////////////////////////////////////////
      /** Main method of seed production                               */
      ///////////////////////////////////////////////////////////////////

      std::list<std::pair<const Trk::SpacePoint*,const Trk::SpacePoint*> >* find2Sp (const Trk::TrackParameters&);

      ///////////////////////////////////////////////////////////////////
      /** Iterator through seed collection.Not used in this implementation */
      ///////////////////////////////////////////////////////////////////
      
      const SiSpacePointsSeed* next();
      
      ///////////////////////////////////////////////////////////////////
      /** Print internal tool parameters and status                    */
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump          (MsgStream   & out) const;
      std::ostream& dump          (std::ostream& out) const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      /** Protected data and methods                                   */
      ///////////////////////////////////////////////////////////////////
      
      std::string                            m_fieldmode             ;  /** Magnetic field mode  */
      ServiceHandle<MagField::IMagFieldSvc>    m_fieldServiceHandle             ;  /** Magnetic field tool name  */

      MagField::IMagFieldSvc*                m_fieldService;

      ToolHandle<Trk::IPRD_AssociationTool>  m_assotool              ;  /** Association tool  */
      Trk::MagneticFieldProperties           m_fieldprop             ;  /** Magnetic field properties  */
      int                                    m_nprint                ;

      /** List of produced space point seeds  */
      std::list<std::pair<const Trk::SpacePoint*,const Trk::SpacePoint*> > m_outputListBuffer;

      /**ID SCT helper*/
      const SCT_ID* m_sctId;

      ///////////////////////////////////////////////////////////////////
      /** Seed selection criteria                                      */
      ///////////////////////////////////////////////////////////////////
     
      double                         m_ptmin                         ;  /** Minimum pT cut   */
      double                         m_r_rmin                        ;  /** Minimum SCT radius to be searched  */
      double                         m_r_rmax                        ;  /** Maximum STC radius to be searched  */
      double                         m_r_rstep                       ;  /** Step size for space point storage  */
      double                         m_r1max                         ;  /** Max radius of last SCT layer  */
      double                         m_r12min                        ;  /** Min radius of last SCT layer  */
      double                         m_r2min                         ;  /** Min radius to search for SP pairs  */
      double                         m_dzdrmin                       ;  /** Min R-z direction cut  */
      double                         m_dzdrmax                       ;  /** Max R-z direction cut  */
      double                         m_xiC                           ;  /** qOverP based chi2 cut  */
      double                         m_xiTC                          ;  /** theta based chi2 cut  */
      double                         m_xiFC                          ;  /** phi based chi2 cut  */
      bool                           m_search                        ;  /** Do full neighbor search  */
      bool                           m_useasso                       ;  /** Use Si cluster association tool  */
      bool                           m_loadFull                      ;  /** Load full Si space point container  */
      bool                           m_doCosmics                     ;  /** Disable seed selection cuts during reconstruction of cosmics tracks  */

      ///////////////////////////////////////////////////////////////////
      /** Tables for 2 space points seeds search                       */
      ///////////////////////////////////////////////////////////////////
     
      int m_r_size                                                   ;
      std::list<const Trk::SpacePoint*>*          m_r_Sorted         ;
      std::list<std::pair<const Trk::SpacePoint*,int> > m_rf_Sorted[530]   ;
      std::list<std::pair<const Trk::SpacePoint*,int> > m_newRfi_Sorted    ;

      int m_ns                                                       ;
      int m_fNmax                                                    ;
      int  m_nr       ; int* m_r_index   ; int* m_r_map              ;
      int  m_nrf     , m_rf_index   [ 530], m_rf_map   [ 530]        ;
      double m_sF                                                    ;

      ///////////////////////////////////////////////////////////////////
      /** Space points containers                                      */
      ///////////////////////////////////////////////////////////////////

      SG::ReadHandleKey<SpacePointContainer>         m_spacepointsPixname {this,"SpacePointsPixelName","PixelSpacePoints","RHK to retrieve Pixel SpacePointContainer"}            ;  
      SG::ReadHandleKey<SpacePointContainer>         m_spacepointsSCTname {this,"SpacePointsSCTName","SCT_SpacePoints","RHK to retrieve SCT SpacePointContainer"}           ;         
      SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlapname {this,"SpacePointsOverlapName","OverlapSpacePoints","RHK to retrieve OverlapCollection"}        ; 

      ///////////////////////////////////////////////////////////////////
      /** Protected methods                                            */
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out) const;

      /** Build framework to hold Si space points at beginning of run */
      void buildFrameWork()  
                                        ;
      /** Fill the space point container lists at beginning of each event */
      void fillLists     ()                                          ;

      /** Clean-up space point container lists at beginning of each event */
      void erase()                                                   ;

      /** Form possible space point combinations within allowed radial and pseudorapidity ranges */

      // // // // // // // // // // // // // // // // //
      void production2Spb (const Trk::TrackParameters&,int)           ;
      // // // // // // // // // // // // // // // // //

      /** Obtain geo model info for a specific space point  */
      void geoInfo(const Trk::SpacePoint*, int&, int&)               ;

      /** Get magnetic field properties  */
      void magneticFieldInit(); 

      // ptr to SP-specific data to be stored after calculated only once
      // instead of repeating the same computations many times in the
      // nested loop (memory allocated and deallocated in production2Spb)
      struct bypass_struct {
	double X, Y, Z;
	double R, invR;
	double a, b;
      };

      // place to keep scalar values needed for cuts, common to all seeds
      // updated only once per production2Sp call instead or each cutTPb
      struct invar_bypass_struct { 
	  double min_theta, max_theta, min_phi, max_phi, invp_min, invp_max,
	  invp_min2, invp_max2;
      };
      
      /** Cut on chi2 based on TRT segment qOverP, theta and phi track parameters */
 
      // // // // // // // // // // // // // // // // //
      bool cutTPb(const invar_bypass_struct &invar_bypass, const std::vector<bypass_struct> &prod_bypass,long, long, double);
      // // // // // // // // // // // // // // // // //

    };

  MsgStream&    operator << (MsgStream&   ,const TRT_SeededSpacePointFinder_ATL&);
  std::ostream& operator << (std::ostream&,const TRT_SeededSpacePointFinder_ATL&); 

  ///////////////////////////////////////////////////////////////////
  /** Inline methods                                               */
  ///////////////////////////////////////////////////////////////////

  /** Method to iterate through seeds.Not implemented              */
  inline const SiSpacePointsSeed* TRT_SeededSpacePointFinder_ATL::next()
    {
      return 0;
    }
  
} // end of name space

#endif // TRT_SeededSpacePointFinder_ATL_H

