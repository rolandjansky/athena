/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_ITK
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_ITK_H
#define SiSpacePointsSeedMaker_ITK_H

#include <list>
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "InDetPrepRawData/PixelCluster.h" // needed for ITK long barrel  
#include "InDetIdentifier/PixelID.h" // needed for ITK long barrel   
#include "InDetIdentifier/SCT_ID.h" // needed for ITK long barrel   
#include "InDetReadoutGeometry/PixelModuleDesign.h" // needed for ITK long barrel

#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeedITK.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsProSeedITK.h" 

class MsgStream   ;
class IBeamCondSvc;

class PixelID; // needed for ITK long barrel  
class SCT_ID; // needed for ITK long barrel
class PixelModuleDesign; // needed for ITK long barrel  

namespace Trk {
  class IPRD_AssociationTool;
}

namespace InDet {


  class SiSpacePointsSeedMaker_ITK : 
    virtual public ISiSpacePointsSeedMaker, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      SiSpacePointsSeedMaker_ITK
	(const std::string&,const std::string&,const IInterface*);
      virtual ~SiSpacePointsSeedMaker_ITK();
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
      bool                        m_useassoTool                   ;
      bool                        m_trigger                       ;
      bool                        m_checketa                      ;
      bool                        m_isvertex                      ;
      bool                        m_dbm                           ;
      int                         m_outputlevel                   ;
      int                         m_nprint                        ;
      int                         m_state                         ;
      int                         m_nspoint                       ;
      int                         m_mode                          ;
      int                         m_nlist                         ;
      int                         m_maxsize                       ;
      int                         m_iteration                     ;
      int                         m_iteration0                    ;
      unsigned int                m_maxNumberVertices             ;
      float                       m_etamin, m_etamax              ;
      float                       m_r1min, m_r1minv               ;
      float                       m_r1max, m_r1maxv               ;
      float                       m_r2min, m_r2minv               ;
      float                       m_r2max, m_r2maxv               ;
      float                       m_r3min                         ;
      float                       m_r3max                         ;
      float                       m_drmin, m_drminv               ;
      float                       m_drmax                         ;
      float                       m_rapcut                        ;
      float                       m_dzdrmin0                      ;
      float                       m_dzdrmax0                      ;
      float                       m_dzdrmin                       ;
      float                       m_dzdrmax                       ;
      float                       m_zmin                          ;
      float                       m_zmax                          ;
      float                       m_zminU                         ;
      float                       m_zmaxU                         ;
      float                       m_zminB                         ;
      float                       m_zmaxB                         ;
      float                       m_ftrig                         ;
      float                       m_ftrigW                        ;
      float                       r_rmax                          ;
      float                       r_rmin                          ;
      float                       r_rstep                         ;
      float                       m_dzver                         ;
      float                       m_dzdrver                       ;
      float                       m_diver                         ;
      float                       m_diverpps                      ;
      float                       m_diversss                      ;
      float                       m_divermax                      ;
      float                       m_dazmax                        ;
      float                       m_ptmin                         ;
      float                       m_ipt                           ;
      float                       m_ipt2                          ;
      float                       m_COF                           ;
      float                       m_K                             ;
      float                       m_ipt2K                         ;
      float                       m_ipt2C                         ;
      float                       m_COFK                          ;  
      float                       m_umax                          ;
      float                       m_dzmaxPPP                      ;
      float                       m_RTmin                         ;
      float                       m_RTmax                         ;
      int r_size                                                  ;
      int r_first                                                 ;
      int rf_size                                                 ;
      int rfz_size                                                ;
      std::list<InDet::SiSpacePointForSeedITK*>* r_Sorted            ;
      std::vector<InDet::SiSpacePointForSeedITK*>  rfz_Sorted [   2211] ;
      std::list<InDet::SiSpacePointForSeedITK*>  rfzv_Sorted[   300] ;
      std::list<InDet::SiSpacePointForSeedITK*>  l_spforseed         ;
      std::list<InDet::SiSpacePointForSeedITK*>::iterator i_spforseed; 
      std::vector<InDet::SiSpacePointForSeedITK*>::iterator m_rMin     ;

      int m_ns,m_nsaz,m_nsazv                                     ;
      int m_fNmax[2],m_fvNmax                                        ;
      int m_fNmin,m_fvNmin                                        ;
      int m_zMin                                                  ;
      int  m_nr     ; int* r_index   ; int* r_map                 ;
      int  m_nrfz   , rfz_index  [2211], rfz_map  [2211]            ;
      int  m_nrfzv  , rfzv_index [300], rfzv_map [300]            ;
      int rfz_b[2][2211],rfz_t[2][2211],rfz_ib[2][2211][9],rfz_it[2][2211][9]     ;
      int rfzv_n[300],rfzv_i[300][6]                              ;
      float m_sF[2]                                                  ;
      float m_sFv                                                 ;

      ///////////////////////////////////////////////////////////////////
      // Switches and cuts for ITK extended barrel
      ///////////////////////////////////////////////////////////////////
      bool m_usePixelClusterCleanUp;              // switch to reject pixel clusters before seed search
      bool m_usePixelClusterCleanUp_sizeZcutsB;   // sizeZ cuts for barrel
      bool m_usePixelClusterCleanUp_sizePhicutsB; // size-phi cuts for barrel
      bool m_usePixelClusterCleanUp_sizeZcutsE;   // sizeZ cuts for end-cap
      bool m_usePixelClusterCleanUp_sizePhicutsE; // size-phi cuts for end-cap

      int m_pix_sizePhiCut; // cut on the size-phi of pixel clusters
      float m_pix_sizePhi2sizeZCut_p0B; // cut on size-phi/sizeZ of pixel clusters in barrel: p0/sizeZ-p1 
      float m_pix_sizePhi2sizeZCut_p1B; // cut on size-phi/sizeZ of pixel clusters in barrel: p0/sizeZ-p1 
      float m_pix_sizePhi2sizeZCut_p0E; // cut on size-phi/sizeZ of pixel clusters in end-cap: p0/sizeZ-p1 
      float m_pix_sizePhi2sizeZCut_p1E; // cut on size-phi/sizeZ of pixel clusters in end-cap: p0/sizeZ-p1 

      bool m_useITKseedCuts;          // global switch to use special ITK cuts
      bool m_useITKseedCuts_dR;       // cut to reject seeds with both links in the same barrel pixel layer
      bool m_useITKseedCuts_PixHole;  // cut to reject seeds with Pixel hole
      bool m_useITKseedCuts_SctHole;  // cut to reject seeds with SCT hole
      bool m_useITKseedCuts_hit;      // cut to require bottom hit in barrel layer-0,1 if middle link is in layer-0 of endcap rings
      bool m_useITKseedCuts_dSize;    // cut to reject pixel barrel hits if |sizeZ-predicted_sizeZ| is too large
      bool m_useITKseedCuts_sizeDiff; // cut to reject pixel barrel hits if |sizeZ(layer-i)-sizeZ(layer-j)| is too large

      float m_Nsigma_clSizeZcut; // size of the cut on the cluster sizeZ at cluster clean-up stage: |sizeZ-predicted|<m_Nsigma_clSizeZcut*sigma 
      float parR_clSizeZ0cut[3][5]; // right RMS for sizeZ-predicted_sizeZ(Z=0)
      float parL_clSizeZ0cut[5]; // left RMS for sizeZ-predicted_sizeZ(Z=0)
      float parR_clSizeZcut[2][5]; // right RMS for sizeZ-predicted_sizeZ(Z=Zvx)
      float parL_clSizeZcut[5]; // left RMS for sizeZ-predicted_sizeZ(Z=Zvx)

      ///////////////////////////////////////////////////////////////////
      // Tables for 3 space points seeds search
      ///////////////////////////////////////////////////////////////////
     
      int    m_maxsizeSP                                          ;                    
      InDet::SiSpacePointForSeedITK** m_SP                        ;
      float               *  m_Zo                                 ; 
      float               *  m_Tz                                 ;
      float               *  m_R                                  ;
      float               *  m_U                                  ;
      float               *  m_V                                  ;
      float               *  m_X                                  ;
      float               *  m_Y                                  ;
      float               *  m_Er                                 ;

      InDet::SiSpacePointsSeed* m_seedOutput                      ;

      std::list<InDet::SiSpacePointsProSeedITK*>           l_seeds;
      std::list<InDet::SiSpacePointsProSeedITK*>::iterator i_seed ; 
      std::list<InDet::SiSpacePointsProSeedITK*>::iterator i_seede ;

      std::multimap<float,InDet::SiSpacePointsProSeedITK*> m_seeds          ;
      std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator m_seed ;

      std::multimap<float,InDet::SiSpacePointsProSeedITK*> m_mapOneSeeds;
      InDet::SiSpacePointsProSeedITK*                      m_OneSeeds   ;
      int                                               m_maxOneSize ;
      int                                               m_nOneSeeds  ;
      int                                               m_fillOneSeeds;
      std::set<float>                                   l_vertex     ;
      std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>> m_CmSp; 
      std::vector<std::pair<float,int>> m_Bot;
      std::vector<std::pair<float,int>> m_Top;
 

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
//     std::string                        m_spacepointsOverlapname ; 
      std::string                        m_beamconditions         ;
      SG::ReadHandle<SpacePointContainer>         m_spacepointsSCT         ;
      SG::ReadHandle<SpacePointContainer>         m_spacepointsPixel       ;
      SG::ReadHandle<SpacePointOverlapCollection> m_spacepointsOverlap     ;

      ToolHandle<Trk::IPRD_AssociationTool>  m_assoTool           ;

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out) const;

      void buildFrameWork()                                       ;
      void buildBeamFrameWork()                                   ;

      SiSpacePointForSeedITK* newSpacePoint
	(Trk::SpacePoint*const&)                                  ;
      void newSeed
      (SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float)         ; 

      void newOneSeed
	(SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,
	 SiSpacePointForSeedITK*&,float,float)                       ;

      void newOneSeedWithCurvaturesComparison
	(SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float);
      void newOneSeedWithCurvaturesComparisonPPP
	(SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float);
      void newOneSeedWithCurvaturesComparisonSSS
	(SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float);
      void newOneSeedWithCurvaturesComparisonMIXED
	(SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float);

      void fillSeeds()                                            ;
      void fillLists     ()                                       ;
      void erase         ()                                       ;
      void production2Sp ()                                       ;
      void production3Sp ()                                       ;
      void production3SpSSS     ()                                ;
      void production3SpPPP     ()                                ;
      void production3SpPPPold  ()                                ;
      void production3SpMIXED   ()                                ;
      void production3SpMIXEDold()                                ;

      void production3SpSSS
	(std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 int,int,int&);
      void production3SpPPP
	(std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 int,int,int&);
      void production3SpPPPold
	(std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 int,int,int&);
      void production3SpMIXED
	(std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 int,int,int&);
      void production3SpMIXEDold
	(std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 int,int,int&);
       void production3SpTrigger
	(std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 int,int,int&);
 
      bool newVertices(const std::list<Trk::Vertex>&)             ;
      void findNext()                                             ;
      bool isZCompatible     (float&,float&,float&)               ;
      void convertToBeamFrameWork(Trk::SpacePoint*const&,float*)  ;
      bool isUsed(const Trk::SpacePoint*); 
      bool pixInform(Trk::SpacePoint*const&,bool&,int&);
      void sctInform(Trk::SpacePoint*const&,bool&,int&,float*);
      float AzimuthalStep(float,float,float,float);

      // new methods for ITK long barrel -------------------------------------
      double predictedClusterLength(const InDet::SiSpacePointForSeedITK*,  double thickness, float seed_zvx);
      double predictedClusterLength(const Trk::SpacePoint*,  double thickness, float seed_zvx);
      int deltaSize(double sizeZ, double predictedSize, double pixel_pitch);
      bool isAwayFromChipBoundary(const InDet::SiSpacePointForSeedITK* sp, double size_phi, double size_z, double pitch_phi, double pitch_z);
      bool isAwayFromChipBoundary(const Trk::SpacePoint* sp, double size_phi, double size_z, double pitch_phi, double pitch_z);
      bool ClusterSizeCuts(const InDet::SiSpacePointForSeedITK* sp_low,const InDet::SiSpacePointForSeedITK* sp_high, const double z_seed);
/*       bool ClusterCleanupSizeZCuts(const InDet::SiSpacePointForSeed* sp, const double sizeZ, const double zvx,  */
/* 				   const double pitch, const double thickness, const int layerNum, const float Nsigma_clus); */
      bool ClusterCleanupSizeZCuts(const Trk::SpacePoint* sp, const double sizePhi, const double sizeZ, const double zvx, 
				   const double pitch_phi, const double pitch_z, const double thickness, const int layerNum, const float Nsigma_clus);
      bool PixelSpacePointFilter(const Trk::SpacePoint*);
      //---------- end of new methods for ITK long barrel --------------------
   
   };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_ITK&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_ITK&); 

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

   inline const SiSpacePointsSeed* SiSpacePointsSeedMaker_ITK::next()
    {
      if(m_nspoint==3) {
	do {
	  if(i_seed==i_seede) {findNext(); if(i_seed==i_seede) return 0;} 
	} 
	while(!(*i_seed++)->set3(*m_seedOutput));
	return(m_seedOutput);	
      }
      else             {
	if(i_seed==i_seede) {findNext(); if(i_seed==i_seede) return 0;} 
	(*i_seed++)->set2(*m_seedOutput); return(m_seedOutput);
      }
      return 0;
    }

  inline bool SiSpacePointsSeedMaker_ITK::isZCompatible  
    (float& Zv,float& R,float& T)
    {
      if(Zv < m_zminU || Zv > m_zmaxU) return false;
      if(!m_isvertex) return true;

      std::set<float>::iterator v=l_vertex.begin(),ve=l_vertex.end(); 

      float dZmin = fabs((*v)-Zv); 
      for(++v; v!=ve; ++v) {
	float dZ = fabs((*v)-Zv); if(dZ >= dZmin) break; dZmin=dZ;
      }
      return dZmin < (m_dzver+m_dzdrver*R)*sqrt(1.+T*T);
    }

  ///////////////////////////////////////////////////////////////////
  // New space point for seeds 
  ///////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeedITK* SiSpacePointsSeedMaker_ITK::newSpacePoint
    (Trk::SpacePoint*const& sp) 
    {
      SiSpacePointForSeedITK* sps;

      float r[15]; convertToBeamFrameWork(sp,r);

      if(m_checketa) {

	float z = (fabs(r[2])+m_zmax);
	float x = r[0]*m_dzdrmin     ;
	float y = r[1]*m_dzdrmin     ;
	if((z*z )<(x*x+y*y)) return 0;
      }
      bool barrel;
      int  layer ;
      if(!sp->clusterList().second) {if(!pixInform(sp,barrel,layer  )) return 0;} 
      else                          {    sctInform(sp,barrel,layer,r);          }

      if(i_spforseed!=l_spforseed.end()) {
	sps = (*i_spforseed++); sps->set(sp,r,barrel,layer); 
      }
      else                               {
	l_spforseed.push_back((sps=new SiSpacePointForSeedITK(sp,r,barrel,layer)));
	i_spforseed = l_spforseed.end();	
      }
      
      return sps;
    }

  ///////////////////////////////////////////////////////////////////
  // New 2 space points seeds 
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_ITK::newSeed
    (SiSpacePointForSeedITK*& p1,SiSpacePointForSeedITK*& p2, float z) 
    {
      SiSpacePointForSeedITK* p3 = 0;

      if(i_seede!=l_seeds.end()) {
	SiSpacePointsProSeedITK* s = (*i_seede++);
	s->set(p1,p2,p3,z);
      }
      else                  {
	l_seeds.push_back(new SiSpacePointsProSeedITK(p1,p2,p3,z));
	i_seede = l_seeds.end(); 
      }
    }
  
} // end of name space

///////////////////////////////////////////////////////////////////
// Object-function for curvature seeds comparison
///////////////////////////////////////////////////////////////////

class comCurvatureITK  {
public:
  bool operator ()
  (const std::pair<float,InDet::SiSpacePointForSeedITK*>& i1, 
   const std::pair<float,InDet::SiSpacePointForSeedITK*>& i2)
  {
    return i1.first < i2.first;
  }
};

//////////////////////////////////////////////////////////////////
// Object-function for dZ/dR seeds comparison
///////////////////////////////////////////////////////////////////

class comDzDrITK  {
public:
  bool operator ()
  (const std::pair<float,int>& i1, 
   const std::pair<float,int>& i2)
  {
    return i1.first < i2.first;
  }
};


#endif // SiSpacePointsSeedMaker_ITK_H

