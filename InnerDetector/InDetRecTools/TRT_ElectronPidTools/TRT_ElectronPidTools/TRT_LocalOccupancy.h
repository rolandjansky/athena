/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_LocalOccupancy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRT_LOCALOCCUPANCY_H
#define INDETTRT_LOCALOCCUPANCY_H

#include "TRT_ElectronPidTools/ITRT_LocalOccupancy.h"
#include "AthenaBaseComps/AthAlgTool.h"       // Exchange IN
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/Track.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

#include <vector>

class AtlasDetectorID;
class ITRT_StrawStatusSummarySvc ;
class AtlasDetectorID;
class TRT_ID;
namespace InDetDD{ class TRT_DetectorManager; }

namespace Trk{
	class Track;
}


namespace InDet
{
  /** @class TRT_LocalOccupancy 

      TRT_LocalOccupancy is a tool to compute the TRT occupancy for the elements crossed by a track.

      This tool has to be called with: BeginEvent() before checking occupancy for a given track, so the different
      volumes are computed.

      Then, for each track call LocalOccupancy( Track ) and a  will be returned. 

      @author  Alejandro Alonso: <Alejandro.Alonso@cern.ch>
  */

  class TRT_LocalOccupancy : public AthAlgTool,
				 virtual public ITRT_LocalOccupancy{
  public:
   TRT_LocalOccupancy(const std::string&,const std::string&,const IInterface*);

   /** default destructor */
   virtual ~TRT_LocalOccupancy ();
      
   /** standard Athena-Algorithm method */
   virtual StatusCode initialize();

   /** standard Athena-Algorithm method */
   virtual StatusCode finalize  (); 

   /** Event Initiazliation, so we do not count several times the local volume occupancies. */
   StatusCode BeginEvent();

   /** Return the local occupancy for the sectors crossed by a given track */ 
   //virtual double LocalOccupancy( const Trk::Track track);
   virtual std::vector<float> LocalOccupancy( const Trk::Track track);


   /** Return the global occupancy of the event*/ 
   /** 5 Floats: TRT, Barrel A / C, endcap A/C */ 
   virtual std::vector<float> GlobalOccupancy( );

  private:
   /** Arrays to store the occupancy for each event */
   float m_occdc_array  	[4][14][32]	;
   float m_occdc_array_phi	[4][32]		;
   float m_occdc_array_det	[4]		;


   /** tools to deal with arrays */
   void resetOccArrays();
   void resetArrays(float array_all [][14][32], float array_phi[][32], float array_det[]);
   void printArrays(float array_all [][14][32], float array_phi[][32], float array_det[]);
  

   /** To convert from array index to det id and viceversa */
   int detToArrayIndex (int det);
   int ArrayIndexToDet (int arrayindex);

   /** To prevent the creation of the maps more than 1 time per event, keep the event number */
   int m_eventnumber;
   float m_averageocc; 

   /** External tools:  */
   const TRT_ID *m_TRTHelper;
   ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawStatusSummarySvc;

   }; 
}
#endif 
