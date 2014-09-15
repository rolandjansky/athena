/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyEDMHelper_H
#define MboyEDMHelper_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

//Can not be forwarded due to typedef
#include "TrkParameters/TrackParameters.h"

#include "TrkTrack/Track.h"

#include "TrkSurfaces/PlaneSurface.h"

class AtlasDetectorID ;
class MdtIdHelper ;
class RpcIdHelper ;
class TgcIdHelper ;
class CscIdHelper ;

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

#include "MboyEDMToolInterfaces/IMboyEDMHelper.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

  /**
   @class MboyEDMHelper

   This class is collecting number of utility methods
   on MboyAthEvt and EDM objects

  @author samusog@cern.ch
  
  */

class MboyEDMHelper:public AthAlgTool, virtual public IMboyEDMHelper{
public:
    MboyEDMHelper(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyEDMHelper 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

public:
///////////////////////////////////

   /** Align TrackParticleContainers */
   virtual std::vector<const Rec::TrackParticle*> AlignTrackParticleContainers(
                                const Rec::TrackParticleContainer* pTrackParticleContainer ,
                                const Rec::TrackParticleContainer* pTrackParticleContainer_Aux
   );

   /** return a secured AtAplane */
   virtual const Trk::TrackParameters* GetSecuredAtaPlane (
     const Amg::Vector3D &            aGlobalPosition ,
     const Amg::Vector3D &            aGlobalMomentum ,
     double                           TheCharge       ,
     const Trk::PlaneSurface   &      aPlaneSurface
   );

   /** Dumps Trk::Track*/
   virtual StatusCode dump_TrkTrack(const Trk::Track* pTrack,std::ofstream* pOutCurrent);
   virtual StatusCode dump_Perigee(const Trk::Perigee* pPerigee,std::ofstream* pOutCurrent);
   virtual StatusCode dump_AtaCylinder(const Trk::AtaCylinder* pAtaCylinder,std::ofstream* pOutCurrent);
   virtual StatusCode dump_AtaDisc(const Trk::AtaDisc* pAtaDisc,std::ofstream* pOutCurrent);

   /** Dumps Trk::Segment*/
   virtual StatusCode dump_TrkSegment(
              const Trk::Segment*  pSegment,
              std::ofstream* pOutCurrent);

   /** Dumps Rotd of Trk::Track*/
   virtual StatusCode dump_TrkTrackDigits(
              const Trk::Track* pTrack,
              std::ofstream* pOutCurrent);

   /** Dumps Rots of Trk::Segment*/
   virtual StatusCode dump_TrkSegmentDigits(
              const Trk::Segment*  pSegment,
              std::ofstream* pOutCurrent);

   /** Dumps Holes of Trk::Segment*/
   virtual StatusCode dump_TrkSegmentHoles(
              const Trk::Segment*  pSegment,
              std::ofstream* pOutCurrent);

   /** Dumps Rot */
   virtual StatusCode dump_rot(const Trk::RIO_OnTrack* rot,std::ofstream* pOutCurrent);

   /** Dumps Ids */
   virtual StatusCode dump_id(Identifier channelId,std::ofstream* pOutCurrent);
   virtual StatusCode dump_mdt(Identifier channelId,std::ofstream* pOutCurrent);
   virtual StatusCode dump_rpc(Identifier channelId,std::ofstream* pOutCurrent);
   virtual StatusCode dump_tgc(Identifier channelId,std::ofstream* pOutCurrent);
   virtual StatusCode dump_csc(Identifier channelId,std::ofstream* pOutCurrent);

   /** Dumps Eve */
   virtual StatusCode dump_mdt(Identifier channelId,std::stringstream* pOutCurrent);
   virtual StatusCode give_mdt(Identifier channelId,std::string& name, int& eta, int& phi, int& multilayer, int& tubeLayer, int& tube);
   virtual StatusCode give_rpc(Identifier channelId,std::string& name, int& eta, int& phi, int& doubletR, int& doubletZ, int& doubletPhi, int& gasGap, int& measuresPhi, int& strip);
   virtual StatusCode give_tgc(Identifier channelId, std::string& name, int& eta, int& phi, int& gasGap, int& isStrip, int& channel);
   virtual StatusCode give_csc(Identifier channelId,std::string& name, int& eta, int& phi, int& chamberLayer, int& wireLayer, int& measuresPhi, int& strip);
 
   /** Identify type of hits */
   virtual StationType is_rotStation(const Trk::RIO_OnTrack* rot);
 
   /** Get TGC gas Gap */
   virtual int tgcGasGap(Identifier channelId);

   
   /** Compute Eloss */
   virtual StatusCode BackTrackingEloss(
                            const Rec::TrackParticle* pTrackParticle, 
                            double& Eloss);
   virtual StatusCode BackTrackingEloss(
                            const Trk::Track* pTrack, 
                            double& Eloss);

   /** Compute distance to cylinders */
   virtual double DistanceToCaloCylinder(
                            double x, double y, double z);
   virtual double DistanceToSpectroCylinder(
                            double x, double y, double z);
   virtual double DistanceToCylinder(
                            double TheHalfZ, double TheR, 
                            double x, double y, double z);
   /** Flag position wrt to mid Calo */
   virtual int IsThisPositionBeyondThelimitCaloSpectro(
                            double x, double y, double z);

   /** Compute Eloss */
   virtual StatusCode BackTrackingEloss(
                            const Rec::TrackParticle* muonSpecTP, 
                            const Rec::TrackParticle* extrapTP, 
                            double& Eloss) ;  

   /** Compute Eloss */
   virtual StatusCode BackTrackingEloss(
                            const Trk::Track* muonSpecTT, 
                            const Trk::Track* extrapTT, 
                            double& Eloss) ;   

   /** Compute parametrized Eloss */
   virtual double parametrisedEnergyLoss(double Eta, double Momentum);

   /** Segment Associated Station Name */
   virtual int SegmentAssociatedStationIndex(const Trk::Segment*  pSegment);
   virtual void SegmentAssociatedStationNames(const Trk::Segment*  pSegment, std::vector<std::string >& VectorOfStationNames ) ;
   virtual std::string SegmentAssociatedStationName(const Trk::Segment*  pSegment);
   virtual int StationLevel  (const std::string& TheSegmentStationName );
   virtual bool IsEndCap     (const std::string& TheSegmentStationName );
   virtual bool IsLargeSector(const std::string& TheSegmentStationName );

   /** Get nber of precision measurments on this track */
   virtual int GetNberOfPrecisionDigitsInThisTrack( 
					    const Trk::Segment*  pSegment,  
					    const Trk::Track*    pTrack);
   /** Is this Segment in this Track? */
   virtual int IsThisSegmentInThisTrack(
                            const Trk::Segment*  pSegment,
                            const Trk::Track*    pTrack);

   /** Is this rot in this Track? */
   virtual int IsThisRotInThisTrack(
                            const Trk::RIO_OnTrack* rot,
                            const Trk::Track* pTrack);

   /** Are these Rots Indentical? */
   virtual int AreTheseRotsIdentical(
                            const Trk::RIO_OnTrack* Rot1,
                            const Trk::RIO_OnTrack* Rot2);

   /** Trk::Segment validity */
   virtual int IsValid(const Trk::Segment*  pSegment);

   /** Get X axis of Trk::Segment  */
   virtual void GetXaxis(
                 const Trk::Segment*  pSegment,
                 double& X_Xaxis,
                 double& Y_Xaxis,
                 double& Z_Xaxis
		 );
   virtual Amg::Vector3D GetXaxis(const Trk::Segment*  pSegment);

   /** Get Y axis of Trk::Segment  */
   virtual void GetYaxis(
                 const Trk::Segment*  pSegment,
                 double& X_Yaxis,
                 double& Y_Yaxis,
                 double& Z_Yaxis
		 );
   virtual Amg::Vector3D GetYaxis(const Trk::Segment*  pSegment);

   /** Get Point of Trk::Segment  */
   virtual void GetPoint(
                 const Trk::Segment*  pSegment,
                 double& Xpt,
                 double& Ypt,
                 double& Zpt
		 );
   virtual Amg::Vector3D GetPoint(const Trk::Segment*  pSegment);

   /** Get Direction of Trk::Segment  */
   virtual void GetDirection(
                 const Trk::Segment*  pSegment,
                 double& Xdir,
                 double& Ydir,
                 double& Zdir
                 );
   virtual Amg::Vector3D GetDirection(const Trk::Segment*  pSegment);

   /** Get fitQuality of Trk::Segment  */
   virtual double GetfitQuality(
                 const Trk::Segment*  pSegment
		 );

   /** Get number of digits of Trk::Segment  */
   virtual int GetNberOfDigi    (const Trk::Segment*  pSegment);
   virtual int GetNberOfDigiMDT (const Trk::Segment*  pSegment);
   virtual int GetNberOfDigiRPC (const Trk::Segment*  pSegment);
   virtual int GetNberOfDigiTGC (const Trk::Segment*  pSegment);
   virtual int GetNberOfDigiCSC (const Trk::Segment*  pSegment);
   virtual int GetNberOfDigiCSC1(const Trk::Segment*  pSegment);
   virtual int GetNberOfDigiCSC2(const Trk::Segment*  pSegment);
   virtual int GetNberOfPrecisionDigitsInCommon(const Trk::Segment*  pSegment,
					const Trk::Segment*  pSegment2);
   virtual void GetNberOfDigi(
		      const Trk::Segment*  pSegment, 
		      int& nd, int& ndMDT1, int& ndMDT2
		      );

   virtual int HitsPerML( const Trk::Segment*  pSegment,  int ML );
   virtual std::string EtaStationString( const Trk::Segment* pSegment );

   /** returns vector with number of digits in mdt, 
       csc, rpc, and tgc chambers, respectively,
       in each coordinate (eta,phi) */
   virtual void GetNberOfDigiVec(
			 const Trk::Segment*  pSegment,
			 std::vector<int>& etaDigitVec, 
			 std::vector<int>& phiDigitVec
			 );

   /** Determine if the segment has 2nd coordinate  */
   virtual bool SegmentHas2ndCoordMeasurement(const Trk::Segment* pSegment);

   /** Get Cov Matrix of segment  */
   virtual void GetSegmentCovMatrix(const Trk::Segment*  pSegment, 
			    double error[4], double& corX, double& corY);


   /**Exercise track propagation: TrackParameters at different locations*/
    virtual void GiveTrackParametersAtBeamAndCalo(
                                           const Rec::TrackParticle* pTrackParticle,
                                           const Trk::TrackParameters*& pTrackParametersAtBeam,
                                           const Trk::TrackParameters*& pTrackParametersAtCalo,
                                           const Trk::TrackParameters*& pTrackParametersAtSpectro
                                         );

   /**Exercise track propagation: TrackParameters at different locations*/
   virtual void GiveTrackParametersAtBeamAndCalo(
                                           const Trk::Track* pTrack,
                                           const Trk::TrackParameters*& pTrackParametersAtBeam,
                                           const Trk::TrackParameters*& pTrackParametersAtCalo,
                                           const Trk::TrackParameters*& pTrackParametersAtSpectro
                                         );
            
   virtual int  GetISC(const Trk::MeasurementBase* pMeasurementBase);
   virtual int  GetISC(const Trk::RIO_OnTrack* pRIO_OnTrack);
   virtual int  GetISC(const Identifier& channelId);
   virtual int  GetISC(int StationNber);

private:
///////////////////////////////////
   
   const MdtIdHelper * m_mdtId ; //!< IdHelper
   const CscIdHelper * m_cscId ; //!< IdHelper
   const RpcIdHelper * m_rpcId ; //!< IdHelper
   const TgcIdHelper * m_tgcId ; //!< IdHelper

   const AtlasDetectorID* m_detID ; //!< Pointer On AtlasDetectorID

   void SetISC();
   std::vector<int> mb_ISC ;

};

#endif
