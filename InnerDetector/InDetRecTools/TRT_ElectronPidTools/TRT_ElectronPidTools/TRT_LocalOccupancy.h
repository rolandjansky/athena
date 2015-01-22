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
   /** 7 Floats: TRT, Barrel A / C, endcapA/B A/C */ 
   virtual std::vector<float> GlobalOccupancy( );

   virtual int  *getOccTotal() {return m_occ_total;}
   virtual int **getOccLocal() {return m_occ_local;}
   virtual int  *getHitTotal() {return m_hit_total;}
   virtual int **getHitLocal() {return m_hit_local;}
   virtual int  *getStwTotal() {return m_stw_total;}
   virtual int **getStwLocal() {return m_stw_local;}

   virtual int **getOccLocalWheel()  {return m_occ_local_wheel;}
   virtual int **getOccLocalStraw()	{return m_occ_local_straw;}
   virtual int **getOccLocalPrivate(){return m_occ_local_private;}
   virtual int **getHitLocalWheel()  {return m_hit_local_wheel;}
   virtual int **getHitLocalStraw()	{return m_hit_local_straw;}
   virtual int **getHitLocalPrivate(){return m_hit_local_private;}
   virtual int **getStwLocalWheel()  {return m_stw_local_wheel;}
   virtual int **getStwLocalStraw()	{return m_stw_local_straw;}
   virtual int **getStwLocalPrivate(){return m_stw_local_private;}


  private:
   /** Arrays to store the occupancy for each event */
      // m_occ_total: 1d array: [7]; Element meanings in order:
      // Total TRT, (B, ECA, ECB)side C, (B, ECA, ECB)side A
      // m_occ_local: 2d array: [32][10]; 10 elements meaning:
      // (barrel 0,1,2, ECA, ECB)side C, (barrel 0,1,2, ECA, ECB)side A
      int  *m_occ_total;
      int **m_occ_local;
      int  *m_hit_total;
      int **m_hit_local;
      int  *m_stw_total;
      int **m_stw_local;

      int **m_occ_local_wheel; // same as local above, split EC into wheels
      // (barrel 0,1,2, EC 0,1,2, ..., 13) side C, side A ([32][34])
      int **m_occ_local_straw; // same as local_wheel, split barrel into short, long straws
      // (barrel 0short, 0long, 1,2, EC 0,1,2,...,13) side C, side A ([32][36])
      int **m_occ_local_private;
      // Very small granularity...still need to figure out how small. 
      // Don't actually make m_occ_local_private yet.
      int **m_hit_local_wheel;
      int **m_stw_local_wheel;
      int **m_hit_local_straw;
      int **m_stw_local_straw;
      int **m_hit_local_private;
      int **m_stw_local_private;

      bool isMiddleBXOn(unsigned int word);

   /** tools to deal with arrays */
   void resetOccArrays();
   void resetArrays(float array_total [7], float array_local[10][32], float array_mod[34][32]);
   void printArrays(float array_all [][14][32], float array_phi[][32], float array_det[]);
  

   /** To convert from array index to det id and viceversa */
   int detToArrayIndex (int det);
   int ArrayIndexToDet (int arrayindex);
   int findArrayTotalIndex(const int det, const int lay);
   int findArrayLocalIndex(const int det, const int lay);
   int findArrayLocalWheelIndex(const int det, const int lay);
   int findArrayLocalStrawIndex(const int det, const int lay, const int strawlay);

   /** To prevent the creation of the maps more than 1 time per event, keep the event number */
   int m_eventnumber;

   /** External tools:  */
   const TRT_ID *m_TRTHelper;
   ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawStatusSummarySvc;

   }; 
}
#endif 
