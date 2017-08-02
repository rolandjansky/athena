/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_HeavyIon
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_HeavyIon_H
#define SiSpacePointsSeedMaker_HeavyIon_H

#include <list>
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeed.h"

class MsgStream   ;
class IBeamCondSvc;

namespace InDet {


  class SiSpacePointsSeedMaker_HeavyIon : 
    virtual public ISiSpacePointsSeedMaker, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      SiSpacePointsSeedMaker_HeavyIon
	(const std::string&,const std::string&,const IInterface*);
      virtual ~SiSpacePointsSeedMaker_HeavyIon();
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
  SiSpacePointsSeedMaker_HeavyIon() = delete;
  SiSpacePointsSeedMaker_HeavyIon(const SiSpacePointsSeedMaker_HeavyIon&) = delete;
  SiSpacePointsSeedMaker_HeavyIon &operator=(const SiSpacePointsSeedMaker_HeavyIon&) = delete;
  //@}     
      
      ///////////////////////////////////////////////////////////////////
      // Protected data and methods
      ///////////////////////////////////////////////////////////////////
      
      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle ;
      MagField::IMagFieldSvc*                m_fieldService       ;

      IBeamCondSvc* p_beam                                        ;
      bool                        m_pixel                         ;
      bool                        m_sct                           ;
      bool                        m_endlist                       ;
      bool                        m_useOverlap                    ;
      bool                        m_trigger                       ;
      bool                        m_izvertex                      ;
      int                         m_outputlevel                   ;
      int                         m_nprint                        ;
      int                         m_state                         ;
      int                         m_nspoint                       ;
      int                         m_mode                          ;
      int                         m_nlist                         ;
      int                         m_maxsize                       ;
      unsigned int                m_maxNumberVertices             ;
      float                       m_r1min, m_r1minv               ;
      float                       m_r1max, m_r1maxv               ;
      float                       m_r2min, m_r2minv               ;
      float                       m_r2max, m_r2maxv               ;
      float                       m_r3min                         ;
      float                       m_r3max                         ;
      float                       m_drmin, m_drminv               ;
      float                       m_drmax                         ;
      float                       m_rapcut                        ;
      float                       m_dzdrmin                       ;
      float                       m_dzdrmax                       ;
      float                       m_zmin                          ;
      float                       m_zmax                          ;
      float                       m_zminU                         ;
      float                       m_zmaxU                         ;
      float                       m_ftrig                         ;
      float                       m_ftrigW                        ;
      float                       r_rmax                          ;
      float                       r_rstep                         ;
      float                       m_dzver                         ;
      float                       m_dzdrver                       ;
      float                       m_diver                         ;
      float                       m_diverpps                      ;
      float                       m_diversss                      ;
      float                       m_dazmax                        ;
      float                       m_ptmin                         ;
      float                       m_ipt                           ;
      float                       m_ipt2                          ;
      float                       m_COF                           ;
      float                       m_K                             ;
      int r_size                                                  ;
      int rf_size                                                 ;
      int rfz_size                                                ;
      std::list<InDet::SiSpacePointForSeed*>* r_Sorted            ;
      std::list<InDet::SiSpacePointForSeed*>  rfz_Sorted [   583] ;
      std::list<InDet::SiSpacePointForSeed*>  rfzv_Sorted[   300] ;
      std::list<InDet::SiSpacePointForSeed*>  l_spforseed         ;
      std::list<InDet::SiSpacePointForSeed*>::iterator i_spforseed; 
      std::list<InDet::SiSpacePointForSeed*>::iterator m_rMin     ;

      int m_ns,m_nsaz,m_nsazv                                     ;
      int m_fNmax,m_fvNmax                                        ;
      int m_fNmin,m_fvNmin                                        ;
      int m_zMin                                                  ;
      int  m_nr     ; int* r_index   ; int* r_map                 ;
      int  m_nrfz   , rfz_index  [583], rfz_map  [583]            ;
      int  m_nrfzv  , rfzv_index [300], rfzv_map [300]            ;
      int rfz_b[583],rfz_t[593],rfz_ib[583][9],rfz_it[583][9]     ;
      int rfzv_n[300],rfzv_i[300][6]                              ;
      float m_sF                                                  ;
      float m_sFv                                                 ;

      ///////////////////////////////////////////////////////////////////
      // Tables for 3 space points seeds search
      ///////////////////////////////////////////////////////////////////
     
      int    m_maxsizeSP                                          ;                    
      InDet::SiSpacePointForSeed** m_SP                           ;
      float               *  m_Zo                                 ; 
      float               *  m_Tz                                 ;
      float               *  m_R                                  ;
      float               *  m_U                                  ;
      float               *  m_V                                  ;
      float               *  m_Er                                 ;

      std::list<InDet::SiSpacePointsSeed*>           l_seeds      ;
      std::list<InDet::SiSpacePointsSeed*>::iterator i_seed       ; 
      std::list<InDet::SiSpacePointsSeed*>::iterator i_seede      ;

      std::multimap<float,InDet::SiSpacePointsSeed*> m_mapOneSeeds;
      InDet::SiSpacePointsSeed*                      m_OneSeeds   ;
      int                                            m_maxOneSize ;
      int                                            m_nOneSeeds  ;
      std::list<float>                               l_vertex     ;
 
      ///////////////////////////////////////////////////////////////////
      // Beam geometry
      ///////////////////////////////////////////////////////////////////
 
      float m_xbeam[4];    // x,ax,ay,az - center and x-axis direction
      float m_ybeam[4];    // y,ax,ay,az - center and y-axis direction
      float m_zbeam[4];    // z,ax,ay,az - center and z-axis direction

      ///////////////////////////////////////////////////////////////////
      // Space points container
      ///////////////////////////////////////////////////////////////////
      
//      std::string                        m_spacepointsSCTname     ;
//      std::string                        m_spacepointsPixelname   ;
//      std::string                        m_spacepointsOverlapname ; 
      std::string                        m_beamconditions         ;
      SG::ReadHandle<SpacePointContainer>         m_spacepointsSCT         ;
      SG::ReadHandle<SpacePointContainer>         m_spacepointsPixel       ;
      SG::ReadHandle<SpacePointOverlapCollection> m_spacepointsOverlap     ;

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out) const;

      void buildFrameWork()                                       ;
      void buildBeamFrameWork()                                   ;

      SiSpacePointForSeed* newSpacePoint
	(Trk::SpacePoint*const&)                                  ;
      void newSeed
      (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
       const float&)                                              ; 
      void newSeed
      (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
       const Trk::SpacePoint*&,const float&)                      ;

      void newOneSeed
      (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
       const Trk::SpacePoint*&,const float&,const float&)         ;
      void fillSeeds()                                            ;

      void fillLists     ()                                       ;
      void erase         ()                                       ;
      void production2Sp ()                                       ;
      void production3Sp ()                                       ;
      void production3Sp
	(std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 int,int,int&);
      void production3SpTrigger
	(std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 int,int,int&);
     void production3SpNoVertex
	(std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 std::list<InDet::SiSpacePointForSeed*>::iterator*,
	 int,int,int&);

      bool newVertices(const std::list<Trk::Vertex>&)             ;
      void findNext()                                             ;
      bool isZCompatible     (float&,float&,float&)               ;
      float dZVertexMin(float&)                                   ;
      void convertToBeamFrameWork(Trk::SpacePoint*const&,float*)  ;
   };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_HeavyIon&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_HeavyIon&); 

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const SiSpacePointsSeed* SiSpacePointsSeedMaker_HeavyIon::next()
    {
      if(i_seed==i_seede) {findNext(); if(i_seed==i_seede) return 0;} 
      return(*i_seed++);
    }

  inline bool SiSpacePointsSeedMaker_HeavyIon::isZCompatible  
    (float& Zv,float& R,float& T)
    {
      if(Zv < m_zmin || Zv > m_zmax) return false;
      if(!m_izvertex               ) return true ;

      std::list<float>::iterator v=l_vertex.begin(),ve=l_vertex.end(); 

      float dZmin = fabs((*v)-Zv); ++v;

      for(; v!=ve; ++v) {
	float dZ = fabs((*v)-Zv); if(dZ<dZmin) dZmin=dZ;
      }
      return dZmin < (m_dzver+m_dzdrver*R)*sqrt(1.+T*T);
    }

  inline float SiSpacePointsSeedMaker_HeavyIon::dZVertexMin(float& Z)
    {
      float dZm = 1.E10;
      std::list<float>::iterator v=l_vertex.begin(),ve=l_vertex.end();

      for(; v!=ve; ++v) {float dZ = fabs((*v)-Z); if(dZ<dZm) dZm = dZ;}
      return dZm;
    }

  ///////////////////////////////////////////////////////////////////
  // New space point for seeds 
  ///////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeed* SiSpacePointsSeedMaker_HeavyIon::newSpacePoint
    (Trk::SpacePoint*const& sp) 
    {
      SiSpacePointForSeed* sps;

      float r[3]; convertToBeamFrameWork(sp,r);

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
  // New 2 space points seeds 
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_HeavyIon::newSeed
      (const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
       const float& z) 
    {
      if(i_seede!=l_seeds.end()) {
	SiSpacePointsSeed* s = (*i_seede++);
	s->erase     (  ); 
	s->add       (p1); 
	s->add       (p2); 
	s->setZVertex(double(z));
      }
      else                  {
	l_seeds.push_back(new SiSpacePointsSeed(p1,p2,z));
	i_seede = l_seeds.end(); 
      }
    }

  ///////////////////////////////////////////////////////////////////
  // New 3 space points seeds 
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_HeavyIon::newSeed
      (const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
       const Trk::SpacePoint*& p3,const float& z) 
    {
      if(i_seede!=l_seeds.end()) {
	SiSpacePointsSeed* s = (*i_seede++);
	s->erase     (  ); 
	s->add       (p1); 
	s->add       (p2); 
	s->add       (p3); 
	s->setZVertex(double(z));
      }
      else                  {
	l_seeds.push_back(new SiSpacePointsSeed(p1,p2,p3,z));
	i_seede = l_seeds.end(); 
      }
    }
  
  ///////////////////////////////////////////////////////////////////
  // Fill seeds
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_HeavyIon::fillSeeds ()
    {
      std::multimap<float,InDet::SiSpacePointsSeed*>::iterator 
	l  = m_mapOneSeeds.begin(),
	le = m_mapOneSeeds.end  ();

      for(; l!=le; ++l) {

	if(i_seede!=l_seeds.end()) {
	  SiSpacePointsSeed* s = (*i_seede++);
	  *s = *(*l).second;
	}
	else                  {
	  l_seeds.push_back(new SiSpacePointsSeed(*(*l).second));
	  i_seede = l_seeds.end(); 
	}
      }
    }
  
} // end of name space

#endif // SiSpacePointsSeedMaker_HeavyIon_H

