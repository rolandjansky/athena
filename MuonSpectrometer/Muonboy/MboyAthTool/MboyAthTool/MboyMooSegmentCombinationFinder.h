/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyMooSegmentCombinationFinder_H
#define MboyMooSegmentCombinationFinder_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

#include "MboyAthToolInterfaces/IMboyMooSegmentCombinationFinder.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

class MboyVarByName;
class IMboyAthToolHelper;

  /**
   @class MboyMooSegmentCombinationFinder

  @author samusog@cern.ch
  
  */

namespace Muon {

class MboyMooSegmentCombinationFinder:public AthAlgTool, virtual public IMboyMooSegmentCombinationFinder{
public:
    MboyMooSegmentCombinationFinder(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyMooSegmentCombinationFinder 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();


   /** Get Segments from PrepRawData */
   virtual std::vector<const Muon::MuonSegment*>* findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData, MboyROAStoreAth* pMboyROAExternalStoreAth ) ;
   virtual std::vector<const Muon::MuonSegment*>* findFromPrep( const std::vector<const Trk::PrepRawData*>& VectorOfpPrepData) ;
   
   /** Get Segments vector from Prep collections */
   virtual Muon::IMboyMooSegmentCombinationFinder::Output* findSegments( const std::vector<const MdtPrepDataCollection*>& mdtCols, 
                                 const std::vector<const CscPrepDataCollection*>& cscCols, 
                                 const std::vector<const TgcPrepDataCollection*>& tgcCols, 
                                 const std::vector<const RpcPrepDataCollection*>& rpcCols ) ;

private:
///////////////////////////////////

   std::string m_ToolName ;

   /** Controled proprieties  */
   void declareControledProperty( std::string Name1, double Value, std::string Comment = " No Comment ") ; //!< Controled proprieties 
   void PrintControledProperty()                                                                         ; //!< Controled proprieties 
   int    m_NberOfCtrlPpty                                                                               ; //!< Controled proprieties 
   std::vector<std::string>  m_NameOfCtrlPpty                                                            ; //!< Controled proprieties 
   double m_ValueOfCtrlPpty[100]                                                                         ; //!< Controled proprieties 
   std::vector<std::string>  m_CommentOfCtrlPpty                                                         ; //!< Controled proprieties 

   /** Define configuration variables to call the Svc */
   MboyVarByName* p_MboyVarByName   ; //!< configuration variables
   
   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper

   int m_Clean ;  //!< if 1 clean segments
   
};

}

#endif

