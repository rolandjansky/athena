/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyRTMdtDriftCircleOnTrackCreator_H
#define MuonboyRTMdtDriftCircleOnTrackCreator_H


#include "MboyEDMTool/MboyEDMToolMisc.h"

#include "MboyCoreEvt/MuonboyRTAbstract.h"

#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"

#include "MboyAthEvt/MuonRecDigiStoreAth.h"

  /**
   @class MuonboyRTMdtDriftCircleOnTrackCreator

   This class is insuring connection to MdtDriftCircleOnTrackCreator. 
   It derives from MuonboyRTAbstract
   
  @author samusog@cern.ch
  
  */

class MuonboyRTMdtDriftCircleOnTrackCreator: public MuonboyRTAbstract{
public:
   MuonboyRTMdtDriftCircleOnTrackCreator();
   virtual ~MuonboyRTMdtDriftCircleOnTrackCreator();

public:
///////////////////////////////////

   /**Get radius */
   virtual void RfT(int& TheRimRTSvc,
                    int& Ident,
                    double& XXXDCA,double& YYYDCA,double& ZZZDCA,
                    double& XXXDCd,double& YYYDCd,double& ZZZDCd,
                    double& XWiCen,double& YWiCen,double& ZWiCen,
                    double& XWidir,double& YWidir,double& ZWidir,
                    double& RADIUS,
                    int& MboyMdtStatus) ;

   /**Get drift time */
   virtual void TfTDC(int&  Ident,int& TDCcount,double& TimeFromTDC);

   /**Set the connection */
   void SetLinks();

   /**Get RS line */
   virtual std::string PrintRsLine(int Ident, int WireSag, double AdMaxima);

public:
///////////////////////////////////
// Data

   Muon::IMdtDriftCircleOnTrackCreator* p_IMdtDriftCircleOnTrackCreator ; //!< Pointer On IMdtDriftCircleOnTrackCreator
  
   const MuonRecDigiStoreAth* p_MuonRecDigiStoreAth ; //!< The pointer on MuonRecDigiStoreAth
 
   std::vector<const Muon::MdtPrepData*>  pMdtPrepDataSet ; //!< Store digits connections
   std::vector<int>                       Ilinear2MDTrank ; //!< Store digits connections
   std::vector< Amg::Vector3D >           HepPoint3DSet   ; //!< Store digits connections
   std::vector< Amg::Vector3D >           HepVector3DSet   ; //!< Store digits connections

};
#endif

