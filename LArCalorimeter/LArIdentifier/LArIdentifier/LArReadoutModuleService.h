/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef ILARREADOUTMODULESERVICE_H
#define ILARREADOUTMODULESERVICE_H

#include <string>
#include "Identifier/HWIdentifier.h"

/**
 * @brief Service for creating and interpreting LAr readout modules identifiers 

 * @author F. Ledroit
 * @version 00-00-00 
 * @stereotype interface
 */
class LArReadoutModuleService {

public:

    /**
     * create a HWIdentifier  <br><br>
     * <b>valid parameter values</b><br>
     * atlas_tb : 0 atlas , 1 test beam <br>
     * barrel_ec : 0 barrel , 1 end-cap <br>
     * pos_neg : 0 negative eta side (C side) , 1 positive eta side (A side) <br>
     * em_hec_fcal : 0 em , 1 hec, 2 FCAL <br>
     * rosId : 0 -> 27 (EMB), 0 -> 17 (EMEC), 0 -> 2 (HEC), 0 -> 1 (FCAL)
     * rodFragId : 0 -> 223 (EMB), 0 -> 137 (EMEC), 0 -> 23 (HEC), 0 -> 13 (FCAL)
     * rodCrate : 1 -> 4 (EMB), 1 -> 3 (EMEC), 1 (HEC), 1 (FCAL)
     * rodSlot : 1 -> 14 (EMB), 1 -> 13 (EMEC), 1 -> 6 (HEC), 1 -> 4 (FCAL) <br><br>
     *   (reference: xls table by Fred Wickens)
     *
     * Throw  a LArID_Exception if one parameter is out of range. <br><br>
     * 
     * <PRE>
     * // usage example 
     * 
     * try { 
     *  myCablingService->createLArReadoutModuleID( 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0) ;
     * }
     * catch( LArID_Exception & except ) { 
     *  ...
     * }
     * 
     * </PRE>
     */
     HWIdentifier createReadoutModuleID(int atlas_tb, int barrel_ec, int pos_neg, int em_hec_fcal, int rosId, int rodFragId, int rodCrate, int rodSlot);


    /**
     * return 0 for ATLAS , 1 for Test Beam
     */
     int atlas_tb( const HWIdentifier & id)   ;
    
    /**
     * return location (0: barrel or 1: end cap )
     */
     int barrel_ec(const HWIdentifier & id)  ;
    
    /**
     * return side <br>
     * positive side : 1 <br>
     * negative side : 0 <br>
     */
     int pos_neg(const HWIdentifier & id)  ;

    /**
     * return subdetector <br>
     * em : 0, hec : 1, fcal : 2 <br>
     */
     int em_hec_fcal(const HWIdentifier & id)  ;



    /**
     * return sub-detector identifier <br>
     */
     int subDet(const HWIdentifier & id)  ;

    /**
     * return ROB fragment identifier ( == ROL == RODFrag identifier ) <br>
     */
     int robFragId(const HWIdentifier & id)  ;
    
    /**
     * return ROL identifier ( == RODFrag == ROBFrag identifier ) <br>
     */
     int rol(const HWIdentifier & id)  ;
    
    /**
     * return ROS identifier <br>
     */
     int rosId(const HWIdentifier & id)  ;
    
    /**
     * return ROD fragment identifier ( == ROL == ROB identifier ) <br>
     */
     int rodFragId(const HWIdentifier & id)  ;
    
    /**
     * return ROD crate number <br>
     */
     int rodCrate(const HWIdentifier & id)  ;
    
    /**
     * return ROD slot number <br>
     */
     int rodSlot(const HWIdentifier & id)  ;
    
    /**
     * convert a HWIdentifier into a STL string 
     */
     std::string cnvToString(const HWIdentifier & id)   ;
    /**
     * convert a HWIdentifier into a STL string, TDAQ convention for sub-detector numbering
     */
     std::string cnvToStringTDAQ(const HWIdentifier & id)   ;

     std::string checkReadoutModuleValues(int atlas_tb, int barrel_ec, int pos_neg, int em_hec_fcal, int rosId, int rodFragId, int rodCrate, int rodSlot);
    /** destructor */
     ~LArReadoutModuleService() { }

 private:
     static const unsigned int m_atlas_tb  ;
     static const unsigned int m_barrel_ec ;
     static const unsigned int m_pos_neg ;

     // for LArReadoutModuleID
     static const unsigned int m_emhf ;
     static const unsigned int s_rosId ;
     static const unsigned int s_rodFragId ;
     static const unsigned int s_rodCrate ;
     static const unsigned int s_rodSlot ;

     static const int s_LAR_EM_BARREL_A_SIDE ;
     static const int s_LAR_EM_BARREL_C_SIDE ;
     static const int s_LAR_EM_ENDCAP_A_SIDE ;
     static const int s_LAR_EM_ENDCAP_C_SIDE ;
     static const int s_LAR_HAD_ENDCAP_A_SIDE ;
     static const int s_LAR_HAD_ENDCAP_C_SIDE ;
     static const int s_LAR_FCAL_A_SIDE ;
     static const int s_LAR_FCAL_C_SIDE ;

    
};
#endif //LARREADOUTMODULESERVICE_H
