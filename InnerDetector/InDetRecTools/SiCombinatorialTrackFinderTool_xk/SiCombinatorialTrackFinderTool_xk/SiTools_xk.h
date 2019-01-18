/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "TrkGeometry/MagneticFieldProperties.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkToolInterfaces/IPatternParametersUpdator.h"
#include "TrkExInterfaces/IPatternParametersPropagator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"

namespace InDet{

  class SiTools_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      SiTools_xk();
      SiTools_xk(const SiTools_xk&);
      ~SiTools_xk();
      SiTools_xk& operator  = (const SiTools_xk&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////
      
      const Trk::MagneticFieldProperties& fieldTool  () const {return m_fieldtool  ;}

      Trk::IPatternParametersPropagator*  propTool   () const {return m_proptool   ;}
      Trk::IPatternParametersUpdator*     updatorTool() const {return m_updatortool;}
      MagField::IMagFieldSvc*             magfield   () const {return m_fieldService;}  

      Trk::IRIO_OnTrackCreator*           rioTool    () const {return m_riotool    ;}
      Trk::IPRD_AssociationTool*          assoTool   () const {return m_assoTool   ;}
      IInDetConditionsSvc*                pixcond    () const {return m_pixcond    ;}
      IInDetConditionsSvc*                sctcond    () const {return m_sctcond    ;}
      const PixelID*                      pixIdHelper() const {return m_pixIdHelper;}
      const SCT_ID*                       sctIdHelper() const {return m_sctIdHelper;}
      const double&                       xi2max     () const {return m_xi2max     ;}
      const double&                       xi2maxBrem () const {return m_xi2maxBrem ;}
      const double&                       xi2maxNoAdd() const {return m_xi2maxNoAdd;}
      const double&                       xi2maxlink () const {return m_xi2maxlink ;}
      const double&                       xi2multi   () const {return m_xi2multi   ;}
      const double&                       pTmin      () const {return m_pTmin      ;}
      const int&                          maxholes   () const {return m_nholesmax  ;}
      const int&                          maxdholes  () const {return m_dholesmax  ;}
      const int&                          clustersmin() const {return m_nclusmin   ;}
      const bool&                         useassoTool() const {return m_useassoTool;}
      const bool&                         multiTrack () const {return m_multitrack ;}
      const bool&                         bremNoise  () const {return m_bremnoise  ;}
      const bool&                         electron   () const {return m_electron   ;}
      const bool&                         heavyion   () const {return m_heavyion   ;}
      const bool&                        cleanSCTClus() const {return m_cleanSCTClus;}

      void setTools
	(Trk::IPatternParametersPropagator* ,
	 Trk::IPatternParametersUpdator*    , 
	 Trk::IRIO_OnTrackCreator*          , 
	 Trk::IPRD_AssociationTool*         ,
	 MagField::IMagFieldSvc* 
	 );  
      
      void setTools
	(const Trk::MagneticFieldProperties&);
 
      void setTools(IInDetConditionsSvc*,IInDetConditionsSvc*); 

      void setTools (const PixelID* pixIdHelper,const SCT_ID* sctIdHelper);

      void setXi2pTmin(const double&,const double&,const double&,const double&);
      void setHolesClusters(const int&,const int&,const int&);
      void setAssociation(const int&);
      void setMultiTracks(const int,double);
      void setBremNoise  (bool,bool);
      void setCleanSCTClus(const int&);
      void setHeavyIon   (bool);

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      Trk::IPRD_AssociationTool*      m_assoTool   ;  // PRD-Track assosiation tool
      Trk::MagneticFieldProperties    m_fieldtool  ;  // Magnetic field properties
      MagField::IMagFieldSvc*        m_fieldService;  // Magnetic field service 
      Trk::IPatternParametersPropagator* m_proptool;  // Propagator tool
      Trk::IPatternParametersUpdator* m_updatortool;  // Updator    tool
      Trk::IRIO_OnTrackCreator*       m_riotool    ;  // RIOonTrack creator
      IInDetConditionsSvc*            m_pixcond    ;  // Condtionos for pixels 
      IInDetConditionsSvc*            m_sctcond    ;  // Conditions for sct
      const PixelID*                  m_pixIdHelper;  // Pixel ID helper
      const SCT_ID*                   m_sctIdHelper;  // SCT ID helper

      double                          m_xi2max     ;  // Max Xi2 for updator 
      double                          m_xi2maxBrem ;  // Max Xi2 for updator (brem fit)  
      double                          m_xi2maxNoAdd;  // Max Xi2 outlayer 
      double                          m_xi2maxlink ;  // Max Xi2 for search
      double                          m_xi2multi   ;  // Max Xi2 for multi tracks
      double                          m_pTmin      ;  // min pT
      int                             m_nholesmax  ;  // Max number holes
      int                             m_dholesmax  ;  // Max holes gap
      int                             m_nclusmin   ;  // Min number clusters
      bool                            m_useassoTool;  // Use assosiation tool
      bool                            m_multitrack ;  // Do multi tracks
      bool                            m_bremnoise  ;  // Do brem noise
      bool                            m_electron   ;  // Do electron mode
      bool                            m_heavyion   ;  // Is it heavy ion event
      bool                            m_cleanSCTClus; // Clean spurious SCT clusters in forward extension

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiTools_xk::SiTools_xk()
    {
      m_assoTool    = 0   ;
      m_proptool    = 0   ;
      m_updatortool = 0   ;
      m_riotool     = 0   ;  
      m_pixcond     = 0   ;
      m_sctcond     = 0   ;
      m_pixIdHelper = 0   ;
      m_sctIdHelper = 0   ;
      m_xi2max      = 9.  ;
      m_xi2maxBrem  = 15. ;
      m_xi2maxlink  = 200.;
      m_xi2multi    = 5.  ;
      m_xi2maxNoAdd = 20. ; 
      m_pTmin       = 500.;
      m_nholesmax   = 2   ;
      m_dholesmax   = 1   ;
      m_nclusmin    = 5   ;
      m_useassoTool = false;
      m_multitrack  = false; 
      m_bremnoise   = false;
      m_electron    = false;
      m_heavyion    = false;
      m_cleanSCTClus = false;
      m_fieldService= 0    ;
    }

  inline SiTools_xk::SiTools_xk(const SiTools_xk& T)
    {
      *this = T;
    }
  
  inline SiTools_xk& SiTools_xk::operator = 
    (const SiTools_xk& T) 
    {
      if(&T!=this) {
	m_assoTool    = T.m_assoTool   ;
	m_fieldtool   = T.m_fieldtool  ;
	m_fieldService= T.m_fieldService;
	m_proptool    = T.m_proptool   ;
	m_updatortool = T.m_updatortool;
	m_riotool     = T.m_riotool    ;
	m_pixcond     = T.m_pixcond    ;
	m_sctcond     = T.m_sctcond    ;
        m_pixIdHelper = T.m_pixIdHelper;
        m_sctIdHelper = T.m_sctIdHelper;
	m_xi2max      = T.m_xi2max     ;
	m_xi2maxBrem  = T.m_xi2maxBrem ;
	m_xi2maxlink  = T.m_xi2maxlink ;
	m_xi2multi    = T.m_xi2multi   ;
	m_pTmin       = T.m_pTmin      ;
	m_nholesmax   = T.m_nholesmax  ;
	m_dholesmax   = T.m_dholesmax  ;
	m_nclusmin    = T.m_nclusmin   ; 
	m_useassoTool = T.m_useassoTool;
	m_multitrack  = T.m_multitrack ; 
	m_bremnoise   = T.m_bremnoise  ;
	m_electron    = T.m_electron   ; 
        m_cleanSCTClus = T.m_cleanSCTClus;
	m_heavyion    = T.m_heavyion   ;
      }
      return(*this);
    }

  inline SiTools_xk::~SiTools_xk() {}

  inline void SiTools_xk::setTools
    (Trk::IPatternParametersPropagator*  PR,
     Trk::IPatternParametersUpdator*     UP, 
     Trk::IRIO_OnTrackCreator*           RO,
     Trk::IPRD_AssociationTool*          AS,
     MagField::IMagFieldSvc*             MS     
     )    
    {
      m_proptool    = PR;
      m_updatortool = UP;
      m_riotool     = RO;
      m_assoTool    = AS; 
      m_fieldService= MS;   
    }

  inline void SiTools_xk::setTools
    (const Trk::MagneticFieldProperties& MF)
    {
      m_fieldtool   = MF;
    }

  inline void SiTools_xk::setTools (IInDetConditionsSvc* pix,IInDetConditionsSvc* sct)
    {
      m_pixcond = pix;
      m_sctcond = sct;
    } 

  inline void SiTools_xk::setTools (const PixelID* pixIdHelper,const SCT_ID* sctIdHelper)
    {
      m_pixIdHelper = pixIdHelper;
      m_sctIdHelper = sctIdHelper;
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
      if( m_assoTool &&  A) m_useassoTool = true ; 
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

  inline void SiTools_xk::setCleanSCTClus(const int& C)
    {
      C ? m_cleanSCTClus = true : false;
    }

  inline void SiTools_xk::setHeavyIon(bool HI)
  {
    m_heavyion = HI;
  }

} // end of name space

#endif // SiTools_xk


