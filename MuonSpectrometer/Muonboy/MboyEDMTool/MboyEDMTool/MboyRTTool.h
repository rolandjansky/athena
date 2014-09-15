/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyRTTool_H
#define MboyRTTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyEDMToolInterfaces/IMboyRTTool.h"

namespace Muon {
  class  IMdtDriftCircleOnTrackCreator;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class MuonboyRTAdjustableT0SvcRt;
class MuonboyRTAbstract;
class MuonRecDigiStoreAth;
class MuonboyRTMdtDriftCircleOnTrackCreator;

namespace AdjT0 {
  class  IAdjustableT0Tool;
}

  /**
   @class MboyRTTool

   This class is the tool insuring connection to RT services. 
   
  @author samusog@cern.ch
  
  */

class MboyRTTool:public AthAlgTool, virtual public IMboyRTTool{
public:
    MboyRTTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyRTTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   
   /** Get acces to the selected MuonboyRTAbstract*/
   virtual StatusCode GetMuonboyRTAbstractWithDigiUpdate(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               MuonRecDigiStoreAth* pMuonRecDigiStoreAth,
               int WireSag
              );

   /** Get acces to the selected MuonboyRTAbstract*/
   virtual StatusCode GetMuonboyRTAbstractWithDigiUpdateT0Ctrl(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               MuonRecDigiStoreAth* pMuonRecDigiStoreAth,
               int WireSag
              );

   /** Get acces to the selected MuonboyRTAbstract*/
   virtual StatusCode GetMuonboyRTAbstract(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               const MuonRecDigiStoreAth* pMuonRecDigiStoreAth
              );

   /** Get acces to the selected MuonboyRTAbstract*/
   virtual StatusCode GetMuonboyRTAbstractT0Ctrl(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               const MuonRecDigiStoreAth* pMuonRecDigiStoreAth
              );


private:
///////////////////////////////////

   int m_RToption ; //!< RT options
   
   double m_AdMaxima ; //!< Step size for  R vs S scan
   

   MuonboyRTAdjustableT0SvcRt*                             p_MuonboyRTAdjustableT0SvcRt                     ; //!< Pointer On MuonboyRTAdjustableT0SvcRt
   MuonboyRTMdtDriftCircleOnTrackCreator*            p_MuonboyRTMdtDriftCircleOnTrackCreatorAdjustableT0    ; //!< Pointer On MuonboyRTMdtDriftCircleOnTrackCreator
   ToolHandle< AdjT0::IAdjustableT0Tool >            p_IAdjustableT0Tool                                    ; //!< Pointer On IAdjustableT0Tool
   ToolHandle< Muon::IMdtDriftCircleOnTrackCreator > p_IMdtDriftCircleOnTrackCreatorAdjustableT0            ; //!< Pointer On IMdtDriftCircleOnTrackCreator

   MuonboyRTMdtDriftCircleOnTrackCreator*            p_MuonboyRTMdtDriftCircleOnTrackCreator    ; //!< Pointer On MuonboyRTMdtDriftCircleOnTrackCreator
   ToolHandle< Muon::IMdtDriftCircleOnTrackCreator > p_IMdtDriftCircleOnTrackCreator            ; //!< Pointer On IMdtDriftCircleOnTrackCreator

};

#endif

