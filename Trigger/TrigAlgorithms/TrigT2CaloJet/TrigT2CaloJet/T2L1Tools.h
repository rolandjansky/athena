/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2L1Tools.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet
//
// AUTHOR:   Matthew Tamsett, tamsett@cern.ch
//
// Description: Common tools and definitions for the L1.5 jet trigger
// ********************************************************************
#ifndef TRIGT2CALOJET_T2L1TOOLS_H
# define TRIGT2CALOJET_T2L1TOOLS_H 1
#ifndef GAUDIKERNEL_CLASSID_H
# include "GaudiKernel/ClassID.h"
#endif

#include <string>
#include <map>


class T2L1Tools{
    public:
        
        enum roi_types_enums {
            NONE           = 0x01,  
            L1SW8x8        = 0x02, 
            L2CONE         = 0x03, 
            A4TT           = 0x04,
            A4TT_JES       = 0x05,
            A4TT_TC        = 0x06,
            A4TT_TC_JES    = 0x07,
            A4TT_PU_SUB    = 0x08,
            A10TT          = 0x09,
            A4JE           = 0x10,
            A4JE_JES       = 0x11,
            A4JE_TC        = 0x12,
            A4JE_TC_JES    = 0x13,
            A4JE_PU_SUB    = 0x14,
            A4CC           = 0x15,
            A4CC_JES       = 0x16,
            A10JE          = 0x20,
            UNCALIBRATED   = 0x30, 
            CALIBRATED     = 0x31, 
            UNKNOWN        = 0x40,        
            SET_INPUT      = 0x10000,
            GET_INPUT      = 0x10000*255,
            SET_OUTPUT     = 0x100,
            GET_OUTPUT     = 0x100*255,
            BLANKWORD      = 0x70000000
        };
        
        int MapStringToEnum(std::string name_string){        
            
            std::map <std::string, int> L15NameMap;
            L15NameMap["NONE"]         = NONE;
            L15NameMap["L1SW8x8"]      = L1SW8x8;  
            L15NameMap["L2CONE"]       = L2CONE;  
            L15NameMap["A4TT"]         = A4TT;
            L15NameMap["A4TT_JES"]     = A4TT_JES;
            L15NameMap["A4TT_TC"]      = A4TT_TC;
            L15NameMap["A4TT_TC_JES"]  = A4TT_TC_JES;
            L15NameMap["A4TT_PU_SUB"]  = A4TT_PU_SUB;
            L15NameMap["A10TT"]        = A10TT;
            L15NameMap["A4JE"]         = A4JE;
            L15NameMap["A4JE_JES"]     = A4JE_JES;
            L15NameMap["A4JE_TC"]      = A4JE_TC;
            L15NameMap["A4JE_TC_JES"]  = A4JE_TC_JES;
            L15NameMap["A4JE_PU_SUB"]  = A4JE_PU_SUB;
            L15NameMap["A10JE"]        = A10JE;
            L15NameMap["A4CC"]         = A4CC;
            L15NameMap["A4CC_JES"]     = A4CC_JES;
            L15NameMap["CALIBRATED"]   = CALIBRATED; 
            L15NameMap["UNCALIBRATED"] = UNCALIBRATED; 
            
            std::map <std::string, int>::const_iterator mask_map_iterator = L15NameMap.find(name_string);
            if (mask_map_iterator == L15NameMap.end()){
                return UNKNOWN;
            } else {
                return mask_map_iterator->second;
            }
         }
         
        std::string MapEnumToString(int enum_int){        
            
            std::map <int, std::string> L15NameMap;
            L15NameMap[NONE]         = "NONE";
            L15NameMap[L1SW8x8]      = "L1SW8x8";  
            L15NameMap[L2CONE]       = "L2CONE";  
            L15NameMap[A4TT]         = "A4TT";
            L15NameMap[A4TT_JES]     = "A4TT_JES";
            L15NameMap[A4TT_TC]      = "A4TT_TC";
            L15NameMap[A4TT_TC_JES]  = "A4TT_TC_JES";
            L15NameMap[A4TT_PU_SUB]  = "A4TT_PU_SUB";
            L15NameMap[A10TT]        = "A10TT";
            L15NameMap[A4JE]         = "A4JE";
            L15NameMap[A4JE_JES]     = "A4JE_JES";
            L15NameMap[A4JE_TC]      = "A4JE_TC";
            L15NameMap[A4JE_TC_JES]  = "A4JE_TC_JES";
            L15NameMap[A4JE_PU_SUB]  = "A4JE_PU_SUB";
            L15NameMap[A10JE]        = "A10JE";
            L15NameMap[A4CC]         = "A4CC";
            L15NameMap[A4CC_JES]     = "A4CC_JES";
            L15NameMap[CALIBRATED]   = "CALIBRATED"; 
            L15NameMap[UNCALIBRATED] = "UNCALIBRATED"; 
            
            std::map <int, std::string>::const_iterator mask_map_iterator = L15NameMap.find(enum_int);
            if (mask_map_iterator == L15NameMap.end()){
                return "UNKNOWN";
            } else {
                return mask_map_iterator->second;
            }
        }
         
        void RoIWordToStrings(int RoIWord, std::string & input_type_string, std::string & output_type_string, int & counter){             
            if ( (RoIWord & BLANKWORD) == BLANKWORD) { // L1.5 jets
                unsigned int output_enum =  (RoIWord >> 8) & 255 ;
                unsigned int input_enum  =  (RoIWord >> 16) & 255 ;
                input_type_string  = MapEnumToString(input_enum);
                output_type_string  = MapEnumToString(output_enum);
                counter = RoIWord & 255;
            } else { // others, probably L2 jets
                input_type_string = "NON_L15";
                output_type_string = "L2CONE";
                counter = 0;
            }
                
        }


}; // end of class
    

#endif // TRIGT2CALOJET_T2L1TOOLS_H
