/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyDetectorLimitsTool_H
#define MboyDetectorLimitsTool_H

#include "AthenaBaseComps/AthAlgTool.h"

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

#include "MboyEDMToolInterfaces/IMboyDetectorLimitsTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

  /**
   @class MboyDetectorLimitsTool

  @author samusog@cern.ch
  
  */

namespace Muon {

class MboyDetectorLimitsTool: public AthAlgTool, virtual public IMboyDetectorLimitsTool{
public:
    MboyDetectorLimitsTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyDetectorLimitsTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   virtual int AreaTag(
                       const Trk::TrackParameters* pTrackParameters
                      ) ;
   virtual int AreaTag(
               double XPosi  ,
               double YPosi  ,
               double ZPosi  
              );

   virtual int IsOnCylinder(
                       const Trk::TrackParameters* pTrackParameters
                      )  ;


private:
///////////////////////////////////

   std::string m_ToolName ;

   double m_EpsMinus    ; //!< Surface definitions
   double m_RadiusID    ; //!< Surface definitions
   double m_fabsZID     ; //!< Surface definitions
   double m_RadiusCalo  ; //!< Surface definitions
   double m_fabsZCalo   ; //!< Surface definitions

};

}

#endif

