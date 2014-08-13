/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_ATLxk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for combined test geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_CTBxk_H
#define SiSpacePointsSeedMaker_CTBxk_H

#include <list>
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "TrkSpacePoint/SpacePointContainer.h" 

class MsgStream;

namespace InDet{


  class SiSpacePointsSeedMaker_CTBxk : 
    virtual public ISiSpacePointsSeedMaker, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      SiSpacePointsSeedMaker_CTBxk
	(const std::string&,const std::string&,const IInterface*);
      virtual ~SiSpacePointsSeedMaker_CTBxk();
      virtual StatusCode               initialize();
      virtual StatusCode               finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////

      void newEvent (int);
      void newRegion
 	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&);
      void newRegion
       (const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&,const IRoiDescriptor&);

      ///////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with two space points with or without vertex constraint
      ///////////////////////////////////////////////////////////////////

      void find2Sp (const std::list<Trk::Vertex>&);

      ///////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with three space points with or without vertex constraint
      ///////////////////////////////////////////////////////////////////

      void find3Sp (const std::list<Trk::Vertex>&);
      void find3Sp (const std::list<Trk::Vertex>&,const double*);

      ///////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with variable number space points with or without vertex constraint
      // Variable means (2,3,4,....) any number space points
      ///////////////////////////////////////////////////////////////////
 
      void findVSp (const std::list<Trk::Vertex>&);
      
      ///////////////////////////////////////////////////////////////////
      // Iterator through seeds pseudo collection produced accordingly
      // methods find    
      ///////////////////////////////////////////////////////////////////
      
      const SiSpacePointsSeed* next();
      
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump(MsgStream&    out) const;
      std::ostream& dump(std::ostream& out) const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected data and methods
      ///////////////////////////////////////////////////////////////////
      
      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle ;
      MagField::IMagFieldSvc*                m_fieldService       ;

      int                                      m_state    ;
      int                                      m_nspoint  ;
      int                                      m_mode     ;
      int                                      m_outputlevel;
      bool                                     m_pixel    ;
      bool                                     m_sct      ;
      bool                                     m_usefield ;
      float                                    m_ptmin    ;
      float                                    m_r1min    ;
      float                                    m_r1max    ;
      float                                    m_r2min    ;
      float                                    m_r2max    ;
      float                                    m_r3min    ;
      float                                    m_r3max    ;
      float                                    m_drmin    ;
      float                                    m_drmax    ;
      float                                    m_dzdrmin  ;
      float                                    m_dzdrmax  ;
      float                                    m_daymax   ;
      float                                    m_dazmax   ;
      float                                    m_dzver    ;
      float                                    m_dzdrver  ;
      std::vector<const Trk::SpacePoint*>      x_sorted   ;
      std::list <SiSpacePointsSeed*>           l_seeds    ;
      std::list <SiSpacePointsSeed*>::iterator i_seed     ; 
      std::list <SiSpacePointsSeed*>::iterator i_seede    ;
      std::list<Trk::Vertex> l_vertex                     ;

      ///////////////////////////////////////////////////////////////////
      // Space points container
      ///////////////////////////////////////////////////////////////////
      
      std::string                        m_spacepointsSCTname     ;
      std::string                        m_spacepointsPixelname   ;
      std::string                        m_spacepointsOverlapname ; 
      const SpacePointContainer*         m_spacepointsSCT         ;
      const SpacePointContainer*         m_spacepointsPixel       ;
      const SpacePointOverlapCollection* m_spacepointsOverlap     ;

      void findSp(int,const std::list<Trk::Vertex>&)      ;
      void production2 ()                                 ;
      void production3 ()                                 ; 
      bool newVertices(const std::list<Trk::Vertex>&)     ;
      void sortRadius()                                   ;
    };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_CTBxk&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_CTBxk&); 

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const SiSpacePointsSeed* InDet::SiSpacePointsSeedMaker_CTBxk::next()
    {
      if(i_seed==i_seede) return 0; return(*i_seed++);
    }
  
} // end of name space

#endif // SiSpacePointsSeedMaker_CTBxk_H
