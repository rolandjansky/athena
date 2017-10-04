/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AtlasDetectorIDHelper.h,v 1.6 2006-01-11 09:24:23 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef SRC_ATLASDETECTORIDHELPER_H
# define SRC_ATLASDETECTORIDHELPER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "Identifier/Identifier.h"
#include "Identifier/IdContext.h"
#include "Identifier/IdHelper.h"
#include <string>
#include <vector>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IdDictField;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class AtlasDetectorIDHelper
{
public:
    
    enum ERRORS { UNDEFINED = 999 };


    AtlasDetectorIDHelper(void);

    typedef Identifier::size_type 			size_type; 

    // Access to singleton
//      static AtlasDetectorIDHelper*	instance();
    
    /// Initialization from the identifier dictionary
    int         initialize_from_dictionary(const IdDictMgr& dict_mgr);

    ~AtlasDetectorIDHelper(void);
    
    size_type   pixel_region_index();
    size_type   sct_region_index();
    size_type   trt_region_index();
    size_type   lar_em_region_index();
    size_type   lar_hec_region_index();
    size_type   lar_fcal_region_index();
    size_type   lvl1_region_index();
    size_type   dm_region_index();
    size_type   tile_region_index();
    size_type   mdt_region_index();
    size_type   csc_region_index();
    size_type   rpc_region_index();
    size_type   tgc_region_index();
    size_type   stgc_region_index();
    size_type   mm_region_index();

    size_type   muon_station_index();

    size_type   alfa_region_index();
    size_type   bcm_region_index();
    size_type   lucid_region_index();
    size_type   zdc_region_index();

    const IdDictField* station_field();

    void        setMsgSvc(IMessageSvc* msgSvc) { m_msgSvc = msgSvc; }


private:

    // undefinded semantics
//     AtlasDetectorIDHelper (const AtlasDetectorIDHelper &);
//     AtlasDetectorIDHelper &operator= (const AtlasDetectorIDHelper &);


    /// Flag for slhc layout:
    bool                m_isSLHC;
    size_type		m_pixel_region_index;
    size_type		m_sct_region_index;
    size_type		m_trt_region_index;
    size_type		m_lar_em_region_index;
    size_type		m_lar_hec_region_index;
    size_type		m_lar_fcal_region_index;
    size_type		m_lvl1_region_index;
    size_type		m_dm_region_index;
    size_type		m_tile_region_index;
    size_type		m_mdt_region_index;
    size_type		m_csc_region_index;
    size_type		m_rpc_region_index;
    size_type		m_tgc_region_index;
    size_type		m_mm_region_index;
    size_type		m_stgc_region_index;
    size_type		m_muon_station_index;
    size_type           m_alfa_region_index;
    size_type           m_bcm_region_index;
    size_type           m_lucid_region_index;
    size_type           m_zdc_region_index;
    bool		m_initialized;
    IdDictField* 	m_station_field;
    /// pointer to the message service
    IMessageSvc*        m_msgSvc;
    
};



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::pixel_region_index()
{return (m_pixel_region_index);}       

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::sct_region_index()
{return (m_sct_region_index);}         

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::trt_region_index()
{return (m_trt_region_index);}         

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::lar_em_region_index()
{return (m_lar_em_region_index);}      

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::lar_hec_region_index()
{return (m_lar_hec_region_index);}     

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::lar_fcal_region_index()
{return (m_lar_fcal_region_index);}    

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::lvl1_region_index()
{return (m_lvl1_region_index);}    

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::dm_region_index()
{return (m_dm_region_index);}    

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::tile_region_index()
{return (m_tile_region_index);}        

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::mdt_region_index()
{return (m_mdt_region_index);}         

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::csc_region_index()
{return (m_csc_region_index);}         

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::rpc_region_index()
{return (m_rpc_region_index);}         

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::tgc_region_index()         
{return (m_tgc_region_index);}         

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::mm_region_index()
{return (m_mm_region_index);}         

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::stgc_region_index()         
{return (m_stgc_region_index);}         

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::muon_station_index()
{return (m_muon_station_index);}

inline const IdDictField* AtlasDetectorIDHelper::station_field()       
{return (m_station_field);}

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::alfa_region_index()
{return(m_alfa_region_index);}

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::bcm_region_index()
{return(m_bcm_region_index);}

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::lucid_region_index()
{return(m_lucid_region_index);}

inline AtlasDetectorIDHelper::size_type   AtlasDetectorIDHelper::zdc_region_index()
{return(m_zdc_region_index);}

#endif // SRC_ATLASDETECTORIDHELPER_H
