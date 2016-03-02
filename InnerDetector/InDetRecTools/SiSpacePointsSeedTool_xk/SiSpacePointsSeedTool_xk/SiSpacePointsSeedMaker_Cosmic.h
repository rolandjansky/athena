/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_Cosmic
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_Cosmic_H
#define SiSpacePointsSeedMaker_Cosmic_H

#include <list>
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"

#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeed.h"

class MsgStream   ;

namespace Trk {

  class IPRD_AssociationTool; 
}

namespace InDet {


  class SiSpacePointsSeedMaker_Cosmic : 
    virtual public ISiSpacePointsSeedMaker, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      SiSpacePointsSeedMaker_Cosmic
	(const std::string&,const std::string&,const IInterface*);
      virtual ~SiSpacePointsSeedMaker_Cosmic();
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

      MsgStream&    dump          (MsgStream   & out) const;
      std::ostream& dump          (std::ostream& out) const;

    protected:

              /**    @name Disallow default instantiation, copy, assignment */
  //@{
  SiSpacePointsSeedMaker_Cosmic() = delete;
  SiSpacePointsSeedMaker_Cosmic(const SiSpacePointsSeedMaker_Cosmic&) = delete;
  SiSpacePointsSeedMaker_Cosmic &operator=(const SiSpacePointsSeedMaker_Cosmic&) = delete;
  //@}
      
      ///////////////////////////////////////////////////////////////////
      // Protected data and methods
      ///////////////////////////////////////////////////////////////////
      
      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle ;
      MagField::IMagFieldSvc*                m_fieldService       ;
      ToolHandle<Trk::IPRD_AssociationTool>  m_assoTool           ;

      bool                        m_pixel                         ;
      bool                        m_sct                           ;
      bool                        m_endlist                       ;
      bool                        m_useOverlap                    ;
      bool                        m_useassoTool                   ;
      bool                        m_checketa                      ;
      int                         m_outputlevel                   ;
      int                         m_nprint                        ;
      int                         m_state                         ;
      int                         m_nspoint                       ;
      int                         m_mode                          ;
      int                         m_nlist                         ;
      int                         m_maxsize                       ;
      int                         m_nseeds                        ;
      float                       m_ptmin                         ;
      float                       m_drmin                         ;
      float                       m_drmax                         ;
      float                       m_rapcut                        ;
      float                       m_dzdrmin                       ;
      float                       m_dzdrmax                       ;
      float                       m_zmin                          ;
      float                       m_zmax                          ;
      float                       r_rmax                          ;
      float                       r_rstep                         ;
      float                       m_diver                         ;
      float                       m_dazmax                        ;
      float                       m_diverpps                      ;
      float                       m_diversss                      ;
      float                       m_etamin, m_etamax              ;
      int r_size                                                  ;
      int rf_size                                                 ;
      int rfz_size                                                ;
      std::list<InDet::SiSpacePointForSeed*>* r_Sorted            ;
      std::list<InDet::SiSpacePointForSeed*>  rf_Sorted  [    53] ; 
      std::list<InDet::SiSpacePointForSeed*>  rfz_Sorted [   583] ;
      std::list<InDet::SiSpacePointForSeed*>  l_spforseed         ;
      std::list<InDet::SiSpacePointForSeed*>::iterator i_spforseed; 
      std::list<InDet::SiSpacePointForSeed*>::iterator m_rMin     ;

      int m_ns,m_nsaz                                             ;
      int m_fNmax                                                 ;
      int m_fNmin                                                 ;
      int m_zMin                                                  ;
      int  m_nr     ; int* r_index   ; int* r_map                 ;
      int  m_nrf    , rf_index   [ 53], rf_map   [ 53]            ;
      int  m_nrfz   , rfz_index  [583], rfz_map  [583]            ;
      int rfz_b[583],rfz_t[593],rfz_ib[583][12],rfz_it[583][12]   ;
      float m_sF                                                  ;

      ///////////////////////////////////////////////////////////////////
      // Tables for 3 space points seeds search
      ///////////////////////////////////////////////////////////////////
     
      int                     m_maxsizeSP                         ;                    
      InDet::SiSpacePointForSeed** m_SP                           ;
      float               *  m_R                                  ;
      float               *  m_Tz                                 ;
      float               *  m_Er                                 ;
      float               *  m_U                                  ;
      float               *  m_V                                  ;

      std::multimap<float,InDet::SiSpacePointsSeed*>           l_seeds;
      std::multimap<float,InDet::SiSpacePointsSeed*>::iterator i_seed ;
      std::multimap<float,InDet::SiSpacePointsSeed*>::iterator i_seede;
      InDet::SiSpacePointsSeed*                                m_seeds;

      ///////////////////////////////////////////////////////////////////
      // Space points container
      ///////////////////////////////////////////////////////////////////
      
//      std::string                        m_spacepointsSCTname     ;
//      std::string                        m_spacepointsPixelname   ;
//      std::string                        m_spacepointsOverlapname ; 
      SG::ReadHandle<SpacePointContainer>         m_spacepointsSCT         ;
      SG::ReadHandle<SpacePointContainer>         m_spacepointsPixel       ;
      SG::ReadHandle<SpacePointOverlapCollection> m_spacepointsOverlap     ;

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out) const;

      void buildFrameWork()                                       ;

      SiSpacePointForSeed* newSpacePoint
	(Trk::SpacePoint*const&)                                  ;
      SiSpacePointForSeed* newSpacePoint
	(Trk::SpacePoint*const&,const float*)                     ;
      void newSeed
      (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
       const float&)                                              ;
      void newSeed
      (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
       const Trk::SpacePoint*&,const float&)                      ;
      void fillLists     ()                                       ;
      void erase         ()                                       ;
      void production2Sp ()                                       ;
      void production3Sp ()                                       ;
      void production3Sp
	(std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 int,int,float,float)                                     ;

      void production3SpWithoutField()                            ;
      void production3SpWithoutField
	(std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 int,int,float)                                           ;


			  
      bool isUsed(const Trk::SpacePoint*)                         ; 
   };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_Cosmic&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_Cosmic&); 

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const SiSpacePointsSeed* SiSpacePointsSeedMaker_Cosmic::next()
    {
      if(i_seed==i_seede) return 0;
      SiSpacePointsSeed* sp = (*i_seed).second; ++i_seed;
      return(sp);
    }
    
  ///////////////////////////////////////////////////////////////////
  // New space point for seeds 
  ///////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeed* SiSpacePointsSeedMaker_Cosmic::newSpacePoint
    (Trk::SpacePoint*const& sp) 
    {
      SiSpacePointForSeed* sps;

      float r[3] = {float(sp->globalPosition().x()),
		    float(sp->globalPosition().y()),
		    float(sp->globalPosition().z())}; 

      if(i_spforseed!=l_spforseed.end()) {
	sps = (*i_spforseed++); sps->set(sp,r); 
      }
      else                               {
	l_spforseed.push_back((sps=new SiSpacePointForSeed(sp,r)));
	i_spforseed = l_spforseed.end();	
      }
      
      return sps;
    }

  ///////////////////////////////////////////////////////////////////
  // New space point for seeds with error correction
  ///////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeed* SiSpacePointsSeedMaker_Cosmic::newSpacePoint
    (Trk::SpacePoint*const& sp,const float* sc) 
    {
      SiSpacePointForSeed* sps;

      float r[3]; r[0]=sp->globalPosition().x(); r[1]=sp->globalPosition().y(); r[2]=sp->globalPosition().z(); 

      if(i_spforseed!=l_spforseed.end()) {
	sps = (*i_spforseed++); sps->set(sp,r,sc); 
      }
      else                               {
	l_spforseed.push_back((sps=new SiSpacePointForSeed(sp,r,sc)));
	i_spforseed = l_spforseed.end();	
      }
      
      return sps;
    }

  ///////////////////////////////////////////////////////////////////
  // New 2 space points seeds 
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_Cosmic::newSeed
      (const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
       const float& z) 
    {

      if(m_nseeds < m_maxsize) {

	m_seeds[m_nseeds].erase     (  ); 
	m_seeds[m_nseeds].add       (p1); 
	m_seeds[m_nseeds].add       (p2); 
	m_seeds[m_nseeds].setZVertex(0.);
	l_seeds.insert(std::make_pair(z,m_seeds+m_nseeds));
	++m_nseeds;
      }
      else                     {

	std::multimap<float,InDet::SiSpacePointsSeed*>::reverse_iterator l = l_seeds.rbegin();
	if((*l).first <= z) return;
	InDet::SiSpacePointsSeed* s = (*l).second;
	l_seeds.erase((*l).first);

	s->erase     (  ); 
	s->add       (p1); 
	s->add       (p2); 
	s->setZVertex(0.);
	l_seeds.insert(std::make_pair(z,s));
      }
    }

  ///////////////////////////////////////////////////////////////////
  // New 3 space points seeds 
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_Cosmic::newSeed
      (const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
       const Trk::SpacePoint*& p3,const float& z) 
    {
      if(m_nseeds < m_maxsize) {

	m_seeds[m_nseeds].erase     (  ); 
	m_seeds[m_nseeds].add       (p1); 
	m_seeds[m_nseeds].add       (p2); 
	m_seeds[m_nseeds].add       (p3); 
	m_seeds[m_nseeds].setZVertex(0.);
	l_seeds.insert(std::make_pair(z,m_seeds+m_nseeds));
	++m_nseeds;
      }
      else                     {

	std::multimap<float,InDet::SiSpacePointsSeed*>::reverse_iterator l = l_seeds.rbegin();
	if((*l).first <= z) return;
	InDet::SiSpacePointsSeed* s = (*l).second;
	l_seeds.erase((*l).first);

	s->erase     (  ); 
	s->add       (p1); 
	s->add       (p2); 
	s->add       (p3); 
	s->setZVertex(0.);
	l_seeds.insert(std::make_pair(z,s));
      }
    }
  
} // end of name space

#endif // SiSpacePointsSeedMaker_Cosmic_H

