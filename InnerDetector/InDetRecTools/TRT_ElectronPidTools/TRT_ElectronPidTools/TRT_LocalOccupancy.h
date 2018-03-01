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

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"

#include <vector>

class AtlasDetectorID;
class ITRT_StrawStatusSummarySvc ;
class TRT_ID;
class ITRT_DriftFunctionTool;

namespace Trk{
	class Track;
}

namespace InDet
{

  /** @class TRT_LocalOccupancy 

      TRT_LocalOccupancy is a tool to compute the TRT occupancy for the elements crossed by a track.

      This tool has to be called with: StartEvent() before checking occupancy for a given track, so the different
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
   StatusCode StartEvent();

   /** Return the local occupancy for the sectors crossed by a given track */ 
   virtual float LocalOccupancy( const Trk::Track& track);
   virtual float LocalOccupancy(const double eta, const double phi);

   /** Return a map of the occupancy in the barrel (-1,+1) and endcaps (-2,+2) */
   virtual std::map<int, double> getDetectorOccupancy( const TRT_RDO_Container* p_trtRDOContainer ) const;

   /** Return the global occupancy of the event*/ 
   /** 7 Floats: TRT, Barrel A / C, endcapA/B A/C */ 
   virtual std::vector<float> GlobalOccupancy( );


   virtual int  *getOccTotal()          {return m_occ_total;}
   virtual int **getOccLocal()          {return m_occ_local;}

  private:
   /** Arrays to store the occupancy for each event */
      // Comments show the meaning of each element in order:
      int  *m_occ_total;       //Total TRT, (B, ECA, ECB)side C, (B, ECA, ECB)side A [7]
      int **m_occ_local;       //(barrel, ECA, ECB)side C, (barrel, ECA, ECB)side A [6][32]

      int  *m_hit_total;
      int **m_hit_local;

      int  *m_stw_total;
      int **m_stw_local;
      int **m_stw_wheel;

      int **m_track_local; // records number of track hits in each region, for each track

      bool isMiddleBXOn(unsigned int word) const;
      bool passValidityGate(unsigned int word, float t0) const;

      void  countHitsNearTrack();
      //   void  countHitsNearTrack(std::vector<IdentifierHash>* hash_vec);
  //   void  countHitsNearTrack(IdentifierHash hash);

   /** tools to deal with arrays */
   void resetOccArrays();
   void resetArrays(float array_total [7], float array_local[6][32], float array_mod[34][32]);
   void printArrays(float array_total [7], float array_local[6][32], float array_mod[34][32]);
  

   /** To convert from array index to det id and viceversa */
   int findArrayTotalIndex(const int det, const int lay);
   int findArrayLocalWheelIndex(const int det, const int lay);
   int findArrayLocalStrawIndex(const int det, const int lay, const int strawlay);
   int detToArrayIndex (int det);
   int ArrayIndexToDet (int arrayindex);

   int mapPhiToPhisector(const double phi);
   int mapEtaToPartition(const double eta);


   /** To prevent the creation of the maps more than 1 time per event, keep the event number */
   int m_eventnumber;

   /** External tools:  */
   const TRT_ID *m_TRTHelper;
   std::string                               m_trt_rdo_location   ;
   ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawStatusSummarySvc;
   ToolHandle< ITRT_DriftFunctionTool > m_driftFunctionTool; //!< DriftFunctionTool

   bool m_isTrigger;
   bool m_T0Shift; // choice to use T0shift or not
   float m_lowGate;
   float m_highGate;
   // use a wider validity gate if you're not using T0 shift:
   float m_lowWideGate; 
   float m_highWideGate;

   // for online use:
   bool region_rescaled[6][32];
   bool allOfEndcapAFound[2][32];
   float stws_ratio[2][32];

   }; 
}
#endif 
