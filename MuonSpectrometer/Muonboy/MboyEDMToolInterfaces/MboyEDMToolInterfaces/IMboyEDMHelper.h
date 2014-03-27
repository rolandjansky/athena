/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyEDMHelper_H
#define IMboyEDMHelper_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

#include "TrkTrack/Track.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkSurfaces/PlaneSurface.h"

namespace Rec {
  class  TrackParticle;
  class  TrackParticleContainer;
}

namespace Trk {
  class  MeasuredPerigee;
  class  RIO_OnTrack;
  class  Segment;
  class  MeasurementBase ;
}

#include "Identifier/Identifier.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

  /**
   @class IMboyEDMHelper

   This class is collecting number of utility methods
   on MboyAthEvt and EDM objects

  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyEDMHelper("IMboyEDMHelper", 1, 0);

class IMboyEDMHelper:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   enum StationType{Mdt, RpcPhi, RpcEta, TgcPhi, TgcEta, CscPhi, CscEta, Unknown};

public:
///////////////////////////////////

   /** Align TrackParticleContainers */
   virtual std::vector<const Rec::TrackParticle*> AlignTrackParticleContainers(
                                const Rec::TrackParticleContainer* pTrackParticleContainer ,
                                const Rec::TrackParticleContainer* pTrackParticleContainer_Aux
   ) = 0;

   /** return a secured AtAplane */
   virtual const Trk::TrackParameters* GetSecuredAtaPlane (
     const Amg::Vector3D       &      aGlobalPosition ,
     const Amg::Vector3D       &      aGlobalMomentum ,
     double                           TheCharge       ,
     const Trk::PlaneSurface   &      aPlaneSurface
   ) = 0;

   /** Dumps Trk::Track*/
   virtual StatusCode dump_TrkTrack(const Trk::Track* pTrack,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dump_Perigee(const Trk::Perigee* pPerigee,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dump_AtaCylinder(const Trk::AtaCylinder* pAtaCylinder,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dump_AtaDisc(const Trk::AtaDisc* pAtaDisc,std::ofstream* pOutCurrent) = 0;

   /** Dumps Trk::Segment*/
   virtual StatusCode dump_TrkSegment(
              const Trk::Segment*  pSegment,
              std::ofstream* pOutCurrent) = 0;

   /** Dumps Rotd of Trk::Track*/
   virtual StatusCode dump_TrkTrackDigits(
              const Trk::Track* pTrack,
              std::ofstream* pOutCurrent) = 0;

   /** Dumps Rots of Trk::Segment*/
   virtual StatusCode dump_TrkSegmentDigits(
              const Trk::Segment*  pSegment,
              std::ofstream* pOutCurrent) = 0;

   /** Dumps Holes of Trk::Segment*/
   virtual StatusCode dump_TrkSegmentHoles(
              const Trk::Segment*  pSegment,
              std::ofstream* pOutCurrent) = 0;

   /** Dumps Rot */
   virtual StatusCode dump_rot(const Trk::RIO_OnTrack* rot,std::ofstream* pOutCurrent) = 0;

   /** Dumps Ids */
   virtual StatusCode dump_id(Identifier channelId,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dump_mdt(Identifier channelId,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dump_rpc(Identifier channelId,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dump_tgc(Identifier channelId,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dump_csc(Identifier channelId,std::ofstream* pOutCurrent) = 0;

   /** Dumps Eve */
   virtual StatusCode dump_mdt(Identifier channelId,std::stringstream* pOutCurrent) = 0;
   virtual StatusCode give_mdt(Identifier channelId,std::string& name, int& eta, int& phi, int& multilayer, int& tubeLayer, int& tube) = 0;
   virtual StatusCode give_rpc(Identifier channelId,std::string& name, int& eta, int& phi, int& doubletR, int& doubletZ, int& doubletPhi, int& gasGap, int& measuresPhi, int& strip) = 0;
   virtual StatusCode give_tgc(Identifier channelId, std::string& name, int& eta, int& phi, int& gasGap, int& isStrip, int& channel) = 0;
   virtual StatusCode give_csc(Identifier channelId,std::string& name, int& eta, int& phi, int& chamberLayer, int& wireLayer, int& measuresPhi, int& strip) = 0;
 
   /** Identify type of hits */
   virtual StationType is_rotStation(const Trk::RIO_OnTrack* rot) = 0;
 
   /** Get TGC gas Gap */
   virtual int tgcGasGap(Identifier channelId) = 0;
   
   /** Compute Eloss */
   virtual StatusCode BackTrackingEloss(
                            const Rec::TrackParticle* pTrackParticle, 
                            double& Eloss) = 0;
   virtual StatusCode BackTrackingEloss(
                            const Trk::Track* pTrack, 
                            double& Eloss) = 0;

   /** Compute distance to cylinders */
   virtual double DistanceToCaloCylinder(
                            double x, double y, double z) = 0;
   virtual double DistanceToSpectroCylinder(
                            double x, double y, double z) = 0;
   virtual double DistanceToCylinder(
                            double TheHalfZ, double TheR, 
                            double x, double y, double z) = 0;
   /** Flag position wrt to mid Calo */
   virtual int IsThisPositionBeyondThelimitCaloSpectro(
                            double x, double y, double z) = 0;

   /** Compute Eloss */
   virtual StatusCode BackTrackingEloss(
                            const Rec::TrackParticle* muonSpecTP, 
                            const Rec::TrackParticle* extrapTP, 
                            double& Eloss)  = 0;  

   /** Compute Eloss */
   virtual StatusCode BackTrackingEloss(
                            const Trk::Track* muonSpecTT, 
                            const Trk::Track* extrapTT, 
                            double& Eloss)  = 0;   

   /** Compute parametrized Eloss */
   virtual double parametrisedEnergyLoss(double Eta, double Momentum) = 0;

   /** Segment Associated Station Name */
   virtual int SegmentAssociatedStationIndex(const Trk::Segment*  pSegment) = 0;
   virtual void SegmentAssociatedStationNames(const Trk::Segment*  pSegment, std::vector<std::string >& VectorOfStationNames ) = 0 ;
   virtual std::string SegmentAssociatedStationName(const Trk::Segment*  pSegment) = 0;
   virtual int StationLevel  (const std::string& TheSegmentStationName ) = 0;
   virtual bool IsEndCap     (const std::string& TheSegmentStationName ) = 0;
   virtual bool IsLargeSector(const std::string& TheSegmentStationName ) = 0;

   /** Get nber of precision measurments on this track */
   virtual int GetNberOfPrecisionDigitsInThisTrack( 
					    const Trk::Segment*  pSegment,  
					    const Trk::Track*    pTrack) = 0;
   /** Is this Segment in this Track? */
   virtual int IsThisSegmentInThisTrack(
                            const Trk::Segment*  pSegment,
                            const Trk::Track*    pTrack) = 0;

   /** Is this rot in this Track? */
   virtual int IsThisRotInThisTrack(
                            const Trk::RIO_OnTrack* rot,
                            const Trk::Track* pTrack) = 0;

   /** Are these Rots Indentical? */
   virtual int AreTheseRotsIdentical(
                            const Trk::RIO_OnTrack* Rot1,
                            const Trk::RIO_OnTrack* Rot2) = 0;

   /** Trk::Segment validity */
   virtual int IsValid(const Trk::Segment*  pSegment) = 0;

   /** Get X axis of Trk::Segment  */
   virtual void GetXaxis(
                 const Trk::Segment*  pSegment,
                 double& X_Xaxis,
                 double& Y_Xaxis,
                 double& Z_Xaxis
		 ) = 0;
   virtual Amg::Vector3D GetXaxis(const Trk::Segment*  pSegment) = 0;

   /** Get Y axis of Trk::Segment  */
   virtual void GetYaxis(
                 const Trk::Segment*  pSegment,
                 double& X_Yaxis,
                 double& Y_Yaxis,
                 double& Z_Yaxis
		 ) = 0;
   virtual Amg::Vector3D GetYaxis(const Trk::Segment*  pSegment) = 0;

   /** Get Point of Trk::Segment  */
   virtual void GetPoint(
                 const Trk::Segment*  pSegment,
                 double& Xpt,
                 double& Ypt,
                 double& Zpt
		 ) = 0;
   virtual Amg::Vector3D GetPoint(const Trk::Segment*  pSegment) = 0;

   /** Get Direction of Trk::Segment  */
   virtual void GetDirection(
                 const Trk::Segment*  pSegment,
                 double& Xdir,
                 double& Ydir,
                 double& Zdir
                 ) = 0;
   virtual Amg::Vector3D GetDirection(const Trk::Segment*  pSegment) = 0;

   /** Get fitQuality of Trk::Segment  */
   virtual double GetfitQuality(
                 const Trk::Segment*  pSegment
		 ) = 0;

   /** Get number of digits of Trk::Segment  */
   virtual int GetNberOfDigi    (const Trk::Segment*  pSegment) = 0;
   virtual int GetNberOfDigiMDT (const Trk::Segment*  pSegment) = 0;
   virtual int GetNberOfDigiRPC (const Trk::Segment*  pSegment) = 0;
   virtual int GetNberOfDigiTGC (const Trk::Segment*  pSegment) = 0;
   virtual int GetNberOfDigiCSC (const Trk::Segment*  pSegment) = 0;
   virtual int GetNberOfDigiCSC1(const Trk::Segment*  pSegment) = 0;
   virtual int GetNberOfDigiCSC2(const Trk::Segment*  pSegment) = 0;
   virtual int GetNberOfPrecisionDigitsInCommon(const Trk::Segment*  pSegment,
					const Trk::Segment*  pSegment2) = 0;
   virtual void GetNberOfDigi(
		      const Trk::Segment*  pSegment, 
		      int& nd, int& ndMDT1, int& ndMDT2
		      ) = 0;

   virtual int HitsPerML( const Trk::Segment*  pSegment,  int ML ) = 0;
   virtual std::string EtaStationString( const Trk::Segment* pSegment ) = 0;

   /** returns vector with number of digits in mdt, 
       csc, rpc, and tgc chambers, respectively,
       in each coordinate (eta,phi) */
   virtual void GetNberOfDigiVec(
			 const Trk::Segment*  pSegment,
			 std::vector<int>& etaDigitVec, 
			 std::vector<int>& phiDigitVec
			 ) = 0;

   /** Determine if the segment has 2nd coordinate  */
   virtual bool SegmentHas2ndCoordMeasurement(const Trk::Segment* pSegment) = 0;

   /** Get Cov Matrix of segment  */
   virtual void GetSegmentCovMatrix(const Trk::Segment*  pSegment, 
			    double error[4], double& corX, double& corY) = 0;


   /**Exercise track propagation: TrackParameters at different locations*/
    virtual void GiveTrackParametersAtBeamAndCalo(
                                           const Rec::TrackParticle* pTrackParticle,
                                           const Trk::TrackParameters*& pTrackParametersAtBeam,
                                           const Trk::TrackParameters*& pTrackParametersAtCalo,
                                           const Trk::TrackParameters*& pTrackParametersAtSpectro
                                         ) = 0;

   /**Exercise track propagation: TrackParameters at different locations*/
   virtual void GiveTrackParametersAtBeamAndCalo(
                                           const Trk::Track* pTrack,
                                           const Trk::TrackParameters*& pTrackParametersAtBeam,
                                           const Trk::TrackParameters*& pTrackParametersAtCalo,
                                           const Trk::TrackParameters*& pTrackParametersAtSpectro
                                         ) = 0;
            
   virtual int  GetISC(const Trk::MeasurementBase* pMeasurementBase) = 0;
   virtual int  GetISC(const Trk::RIO_OnTrack* pRIO_OnTrack) = 0;
   virtual int  GetISC(const Identifier& channelId) = 0;
   virtual int  GetISC(int StationNber) = 0;
};

inline const InterfaceID& IMboyEDMHelper::interfaceID() { return IID_IIMboyEDMHelper; }

#endif
