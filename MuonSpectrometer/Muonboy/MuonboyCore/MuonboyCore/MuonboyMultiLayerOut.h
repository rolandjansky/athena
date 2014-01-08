/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyMultiLayerOut_H
#define MuonboyMultiLayerOut_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyMultiLayerOut

   this class manages update of missing MultiLayers

  @author samusog@cern.ch
  
  */

class MuonboyMultiLayerOut{
public:
    MuonboyMultiLayerOut();
   ~MuonboyMultiLayerOut();

public:
///////////////////////////////////

   /**Set MultiLayer out Data */
   void   SetMultiLayerOut(std::vector<std::string>& MultiLayerOut);

private:
///////////////////////////////////

   /** Define removed stations*/
   void AnalyseMultiLayerOut(const std::vector<std::string>& MultiLayerOut) ;

   std::vector<std::string> m_OffMultiLayerOutK ; //!< Tell which jtyp/jff/jzz/jobj are to be removed
   std::vector<std::string> m_OffStationName    ; //!< Tell which jtyp/jff/jzz/jobj are to be removed
   std::vector<int>         m_OffJtyp           ; //!< Tell which jtyp/jff/jzz/jobj are to be removed
   std::vector<int>         m_OffJff            ; //!< Tell which jtyp/jff/jzz/jobj are to be removed
   std::vector<int>         m_OffJzz            ; //!< Tell which jtyp/jff/jzz/jobj are to be removed
   std::vector<int>         m_OffJobj           ; //!< Tell which jtyp/jff/jzz/jobj are to be removed

   std::vector<std::string> m_CSCWireLayerMultiLayerOutK ; //!< Tell which jtyp/jff/jzz/jobj/jsl are to be removed
   std::vector<std::string> m_CSCWireLayerStationName    ; //!< Tell which jtyp/jff/jzz/jobj/jsl are to be removed
   std::vector<int>         m_CSCWireLayerJtyp           ; //!< Tell which jtyp/jff/jzz/jobj/jsl are to be removed
   std::vector<int>         m_CSCWireLayerJff            ; //!< Tell which jtyp/jff/jzz/jobj/jsl are to be removed
   std::vector<int>         m_CSCWireLayerJzz            ; //!< Tell which jtyp/jff/jzz/jobj/jsl are to be removed
   std::vector<int>         m_CSCWireLayerJobj           ; //!< Tell which jtyp/jff/jzz/jobj/jsl are to be removed
   std::vector<int>         m_CSCWireLayerJsl            ; //!< Tell which jtyp/jff/jzz/jobj/jsl are to be removed

   std::vector<std::string> m_MDTOffMultiLayerOutK ; //!< Tell which jtyp/jff/jzz/jobj/jsl/Jtube are to be removed
   std::vector<std::string> m_MDTOffStationName    ; //!< Tell which jtyp/jff/jzz/jobj/jsl/Jtube are to be removed
   std::vector<int>         m_MDTOffJtyp           ; //!< Tell which jtyp/jff/jzz/jobj/jsl/Jtube are to be removed
   std::vector<int>         m_MDTOffJff            ; //!< Tell which jtyp/jff/jzz/jobj/jsl/Jtube are to be removed
   std::vector<int>         m_MDTOffJzz            ; //!< Tell which jtyp/jff/jzz/jobj/jsl/Jtube are to be removed
   std::vector<int>         m_MDTOffJob            ; //!< Tell which jtyp/jff/jzz/jobj/jsl/Jtube are to be removed
   std::vector<int>         m_MDTOffJsl            ; //!< Tell which jtyp/jff/jzz/jobj/jsl/Jtube are to be removed
   std::vector<int>         m_MDTOffJtube          ; //!< Tell which jtyp/jff/jzz/jobj/jsl/Jtube are to be removed

   std::vector<std::string> m_RPCOffMultiLayerOutK ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<std::string> m_RPCOffStationName    ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_RPCOffJtyp           ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_RPCOffJff            ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_RPCOffJzz            ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_RPCOffJob            ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_RPCOffJspli          ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_RPCOffJsl            ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_RPCOffJsz            ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_RPCOffJstri          ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed

   std::vector<std::string> m_CSCOffMultiLayerOutK ; //!< Tell which jtyp/jff/jzz/jobj/Jsl/Jtube/SZflag are to be removed
   std::vector<std::string> m_CSCOffStationName    ; //!< Tell which jtyp/jff/jzz/jobj/Jsl/Jtube/SZflag are to be removed
   std::vector<int>         m_CSCOffJtyp           ; //!< Tell which jtyp/jff/jzz/jobj/Jsl/Jtube/SZflag are to be removed
   std::vector<int>         m_CSCOffJff            ; //!< Tell which jtyp/jff/jzz/jobj/Jsl/Jtube/SZflag are to be removed
   std::vector<int>         m_CSCOffJzz            ; //!< Tell which jtyp/jff/jzz/jobj/Jsl/Jtube/SZflag are to be removed
   std::vector<int>         m_CSCOffJob            ; //!< Tell which jtyp/jff/jzz/jobj/Jsl/Jtube/SZflag are to be removed
   std::vector<int>         m_CSCOffJsl            ; //!< Tell which jtyp/jff/jzz/jobj/Jsl/Jtube/SZflag are to be removed
   std::vector<int>         m_CSCOffJtube          ; //!< Tell which jtyp/jff/jzz/jobj/Jsl/Jtube/SZflag are to be removed
   std::vector<double>      m_CSCOffSZflag         ; //!< Tell which jtyp/jff/jzz/jobj/Jsl/Jtube/SZflag are to be removed

   std::vector<std::string> m_TGCOffMultiLayerOutK ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<std::string> m_TGCOffStationName    ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_TGCOffJtyp           ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_TGCOffJff            ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_TGCOffJzz            ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_TGCOffJob            ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_TGCOffJspli          ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_TGCOffJsl            ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_TGCOffJsz            ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed
   std::vector<int>         m_TGCOffJstri          ; //!< Tell which jtyp/jff/jzz/jobj/Jspli/Jsl/Jsz/Jstri are to be removed

   std::string m_SeparatorString ; //!< Separator
   std::string m_WhateverString  ; //!< the whatever string
   std::string m_MDTstring       ; //!< the MDT string
   std::string m_RPCstring       ; //!< the RPC string
   std::string m_TGCstring       ; //!< the TGC string
   std::string m_CSCstring       ; //!< the CSC string

   std::vector<int>  GiveSeparatorPositions(const std::string& MLcurrent)   ; //!< Tell how many separators there is in the string

   bool  ConvertIntoInt(const std::string& Field, const std::string& MLcurrent, int& ResultInt)   ; //!< Convert in a Field in int if possible

   void Reset();
   
   void ExtractField(int FieldRank, std::string& Field, const std::string& MLcurrent, const std::vector<int>&  SepPos );
   
   bool CheckSyntaxAndConvert(
                              const std::string& MLcurrent,
                              int& SepPosSize ,
                              std::string& Field_0  ,
                              std::string& Field_1  ,
                              std::string& Field_2  ,
                              std::string& Field_3  ,
                              std::string& Field_4  ,
                              std::string& Field_5  ,
                              std::string& Field_6  ,
                              std::string& Field_7  ,
                              std::string& Field_8  ,
                              std::string& Field_9  ,
                              int& iField_1  ,
                              int& iField_2  ,
                              int& iField_4  ,
                              int& iField_5  ,
                              int& iField_6  ,
                              int& iField_7  ,
                              int& iField_8  ,
                              int& iField_9  
                              );

};

#endif
