/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewCaloTool_H
#define MboyViewCaloTool_H 

#include "AthenaBaseComps/AthAlgTool.h"

#include "Identifier/Identifier.h"

class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class TileID;

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewCaloTool

   This class 
    is a tool to view Calo Container

  @author samusog@cern.ch
  
  */

class MboyViewCaloTool:public AthAlgTool{
public:    
    MboyViewCaloTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewCaloTool();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   StatusCode initialize();
   StatusCode finalize();

   /**View */
   StatusCode ViewCalo(std::ostream* out, float& CaloMinimumE);

private: 
///////////////////////////////////
   size_t get_id_value(Identifier const & id) const;

   std::string     m_cellsName ; //!< Cell container

   const LArEM_ID   *m_emid    ; //!< Calos Ids
   const LArFCAL_ID *m_fcalid  ; //!< Calos Ids
   const LArHEC_ID  *m_hecid   ; //!< Calos Ids
   const TileID     *m_tileid  ; //!< Calos Ids    

   bool m_InitOK;
   
   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif 





