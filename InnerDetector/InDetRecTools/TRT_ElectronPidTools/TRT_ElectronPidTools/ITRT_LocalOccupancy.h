/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITRT_LocalOccupancy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef IINDETTRT_LOCALOCCUPANCY_H
#define IINDETTRT_LOCALOCCUPANCY_H

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/Track.h"
#include "InDetRawData/TRT_RDO_Container.h"

#include <vector>
#include <map>

class Identifier;

namespace Trk { class Track ; }

  /** @class ITRT_LocalOccupancy 
      @author  Alejandro Alonso: <Alejandro.Alonso@cern.ch>
  */

namespace InDet
{
 /** Declaration of the interface ID ( interface id, major version, minor version) */
 static const InterfaceID IID_ITRT_LocalOccupancy("InDet::ITRT_LocalOccupancy", 1 , 0); 

  class ITRT_LocalOccupancy : virtual public IAlgTool {
  public:
   /** AlgTool interface methods */
   static const InterfaceID& interfaceID() ; 

   /** Virtual destructor */
   virtual ~ITRT_LocalOccupancy(){};
   virtual StatusCode initialize() = 0;
   virtual StatusCode finalize()   = 0;

   /** Event Initiazliation, so we do not count several times the local volume occupancies. */
   virtual StatusCode StartEvent() = 0;

   /** Return the local occupancy for the sectors crossed by a given track */ 
   virtual float LocalOccupancy( const Trk::Track& track) = 0;
   virtual float LocalOccupancy(const double eta, const double phi) = 0;
    
   /** Return a map of the occupancy in the barrel (-1,+1) and endcaps (-2,+2) */
   virtual std::map<int, double>  getDetectorOccupancy( const TRT_RDO_Container* p_trtRDOContainer ) const  = 0;
     

   /** Return the global occupancy of the event*/ 
   virtual std::vector<float> GlobalOccupancy( ) = 0 ;

   virtual int  *getOccTotal()        = 0;
   virtual int **getOccLocal()        = 0;
   /* virtual int **getOccLocalWheel()   = 0; */
   /* virtual int **getOccLocalStraw()   = 0; */

   /* virtual int  *getHitTotal()        = 0; */
   /* virtual int **getHitLocal()        = 0; */
   /* virtual int **getHitLocalWheel()   = 0; */
   /* virtual int **getHitLocalStraw()   = 0; */

   /* virtual int  *getStwTotal()        = 0; */
   /* virtual int **getStwLocal()        = 0; */
   /* virtual int **getStwLocalWheel()   = 0; */
   /* virtual int **getStwLocalStraw()   = 0; */

   /* virtual int **getOccLocalPrivate() = 0; */
   /* virtual int **getHitLocalPrivate() = 0; */
   /* virtual int **getStwLocalPrivate() = 0; */
  }; 

  inline const InterfaceID& InDet::ITRT_LocalOccupancy::interfaceID()
  { 
    return IID_ITRT_LocalOccupancy;
  }
}
#endif 
