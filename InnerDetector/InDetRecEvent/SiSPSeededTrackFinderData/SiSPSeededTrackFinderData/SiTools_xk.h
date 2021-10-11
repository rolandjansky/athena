/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiTools_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for trajector elements in Pixels and SCT
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiTools_xk_H
#define SiTools_xk_H

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "TrkExInterfaces/IPatternParametersPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkToolInterfaces/IPatternParametersUpdator.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IBoundaryCheckTool.h"

namespace InDet{

  class SiTools_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      SiTools_xk();
      SiTools_xk(const SiTools_xk&) = default;
      ~SiTools_xk() = default;
      SiTools_xk& operator  = (const SiTools_xk&) = default;

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////
      
      const Trk::MagneticFieldProperties& fieldTool  () const {return *m_fieldtool  ;}

      const Trk::IPatternParametersPropagator*  propTool      () const {return m_proptool   ;}
      const Trk::IPatternParametersUpdator*     updatorTool   () const {return m_updatortool;}
      const AtlasFieldCacheCondObj*             fieldCondObj  () const {return m_fieldCondObj;}

      const Trk::IRIO_OnTrackCreator*           rioTool    () const {return m_riotool    ;}
      const IInDetConditionsTool*               pixcond    () const {return m_pixcond    ;}
      const IInDetConditionsTool*               sctcond    () const {return m_sctcond    ;}
      const Trk::IBoundaryCheckTool*            boundaryCheckTool() const {return m_boundaryCheckTool;}
      const double&                       xi2max     () const {return m_xi2max     ;}
      const double&                       xi2maxBrem () const {return m_xi2maxBrem ;}
      const double&                       xi2maxNoAdd() const {return m_xi2maxNoAdd;}
      const double&                       xi2maxlink () const {return m_xi2maxlink ;}
      const double&                       xi2multi   () const {return m_xi2multi   ;}
      const double&                       pTmin      () const {return m_pTmin      ;}
      const int&                          maxholes   () const {return m_nholesmax  ;}
      const int&                          maxdholes  () const {return m_dholesmax  ;}
      const int&                          clustersmin() const {return m_nclusmin   ;}
      const int&                          maxclusters() const {return m_maxclusters;}
      bool                   usePRDtoTrackAssociation() const {return m_useassoTool;}
      const bool&                         multiTrack () const {return m_multitrack ;}
      const bool&                         bremNoise  () const {return m_bremnoise  ;}
      const bool&                         electron   () const {return m_electron   ;}
      const bool&                         heavyion   () const {return m_heavyion   ;}
      const bool&                         useFastTracking() const {return m_doFastTracking;}
      const bool&                         isITkGeometry() const {return m_ITkGeometry;}

      void setTools
	(const Trk::IPatternParametersPropagator* ,
	 const Trk::IPatternParametersUpdator*    , 
	 const Trk::IRIO_OnTrackCreator*
	 );  

      void setFieldCondObj(const AtlasFieldCacheCondObj* fieldCondObj);
        
      void setPRDtoTrackMap(const Trk::PRDtoTrackMap* prd_to_track_map) {
        m_prdToTrackMap = prd_to_track_map;
        if (!m_prdToTrackMap) m_useassoTool=false;
      }
      const Trk::PRDtoTrackMap* PRDtoTrackMap() const { return m_prdToTrackMap; }


      void setTools(const Trk::MagneticFieldProperties*);
      void setTools(const IInDetConditionsTool*, const IInDetConditionsTool*);
      void setTools(const Trk::IBoundaryCheckTool*);
      void setXi2pTmin(const double&,const double&,const double&,const double&);
      void setHolesClusters(const int&,const int&,const int&);
      void setAssociation(const int&);
      void setMultiTracks(const int,double);
      void setBremNoise  (bool,bool);
      void setHeavyIon   (bool);

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      const Trk::MagneticFieldProperties* m_fieldtool; // Magnetic field properties
      const AtlasFieldCacheCondObj*  m_fieldCondObj;  // Magnetic field conditions object to access cache


      const Trk::IPatternParametersPropagator* m_proptool; // Propagator tool
      const Trk::IPatternParametersUpdator* m_updatortool; // Updator    tool
      const Trk::IRIO_OnTrackCreator* m_riotool    ;  // RIOonTrack creator
      const IInDetConditionsTool*     m_pixcond    ;  // Condtionos for pixels 
      const IInDetConditionsTool*     m_sctcond    ;  // Conditions for sct
      const Trk::IBoundaryCheckTool*  m_boundaryCheckTool; // Boundary checking tool for detector sensitivities
      const Trk::PRDtoTrackMap*       m_prdToTrackMap = nullptr; ///< PRD to track association maps

      double                          m_xi2max     ;  // Max Xi2 for updator 
      double                          m_xi2maxBrem ;  // Max Xi2 for updator (brem fit)  
      double                          m_xi2maxNoAdd;  // Max Xi2 outlayer 
      double                          m_xi2maxlink ;  // Max Xi2 for search
      double                          m_xi2multi   ;  // Max Xi2 for multi tracks
      double                          m_pTmin      ;  // min pT
      int                             m_nholesmax  ;  // Max number holes
      int                             m_dholesmax  ;  // Max holes gap
      int                             m_nclusmin   ;  // Min number clusters
      int                             m_maxclusters;  // Max closest clusters in detector elements
      bool                            m_useassoTool = false;  // Use assosiation tool
      bool                            m_multitrack ;  // Do multi tracks
      bool                            m_bremnoise  ;  // Do brem noise
      bool                            m_electron   ;  // Do electron mode
      bool                            m_heavyion   ;  // Is it heavy ion event
      bool                            m_doFastTracking; // Do Fast Tracking setup
      bool                            m_ITkGeometry; // Is ITk geometry

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiTools_xk::SiTools_xk()
    {
      m_fieldtool   = nullptr;
      m_proptool    = nullptr;
      m_updatortool = nullptr;
      m_riotool     = nullptr;  
      m_pixcond     = nullptr;
      m_sctcond     = nullptr;      
      m_boundaryCheckTool = nullptr;
      m_xi2max      = 9.  ;
      m_xi2maxBrem  = 15. ;
      m_xi2maxlink  = 200.;
      m_xi2multi    = 5.  ;
      m_xi2maxNoAdd = 20. ; 
      m_pTmin       = 500.;
      m_nholesmax   = 2   ;
      m_dholesmax   = 1   ;
      m_nclusmin    = 5   ;
      m_multitrack  = false; 
      m_bremnoise   = false;
      m_electron    = false;
      m_heavyion    = false;
      m_doFastTracking = false;
      m_ITkGeometry = false;
      m_maxclusters = 3;
    }

  inline void SiTools_xk::setTools
    (const Trk::IPatternParametersPropagator*  PR,
     const Trk::IPatternParametersUpdator*     UP, 
     const Trk::IRIO_OnTrackCreator*           RO
     )    
    {
      m_proptool    = PR;
      m_updatortool = UP;
      m_riotool     = RO;
    }

  inline void SiTools_xk::setFieldCondObj(const AtlasFieldCacheCondObj* fieldCondObj) 
    {
      m_fieldCondObj   = fieldCondObj;
    }
    
  inline void SiTools_xk::setTools
    (const Trk::MagneticFieldProperties* MF)
    {
      m_fieldtool = MF;
    }  
  inline void SiTools_xk::setTools
    (const Trk::IBoundaryCheckTool* bound)
    {
      m_boundaryCheckTool = bound;
    } 


  inline void SiTools_xk::setTools
    (const IInDetConditionsTool* pix,
     const IInDetConditionsTool* sct)
    {
      m_pixcond = pix;
      m_sctcond = sct;
    } 
 
  inline void SiTools_xk::setXi2pTmin
    (const double& xi2m,const double& xi2mNoAdd,const double& xi2ml,const double& pT)    
    {
      m_xi2max      = fabs(xi2m     );
      m_xi2maxNoAdd = fabs(xi2mNoAdd);
      m_xi2maxlink  = fabs(xi2ml    );
      m_pTmin       = fabs(pT       );
    }

  inline void  SiTools_xk::setHolesClusters
    (const int& h,const int& dh,const int& cl)
    {
      m_nholesmax   = h ;
      m_dholesmax   = dh;
      m_nclusmin    = cl;
    }
  inline void  SiTools_xk::setAssociation(const int& A)
    {
      if( m_prdToTrackMap &&  A) m_useassoTool = true ; 
      else                  m_useassoTool = false;  
    }
  inline void SiTools_xk::setMultiTracks(const int M,double X)
    {
      M ?  m_multitrack = true : m_multitrack = false;  
      m_xi2multi = X; 
    }

  inline void SiTools_xk::setBremNoise(bool B,bool E)
    {
      m_bremnoise = B;
      m_electron  = E;
    }

  inline void SiTools_xk::setHeavyIon(bool HI)
  {
    m_heavyion = HI;
  }

} // end of name space

#endif // SiTools_xk


