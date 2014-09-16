/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUFASTSTATIONFITTER_H
#define  TRIGL2MUONSA_MUFASTSTATIONFITTER_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "TrigL2MuonSA/MdtData.h"
#include "TrigL2MuonSA/TgcFitResult.h"
#include "TrigL2MuonSA/RpcFitResult.h"
#include "TrigL2MuonSA/SuperPointData.h"
#include "TrigL2MuonSA/TrackData.h"

class StoreGateSvc;

namespace TrigL2MuonSA {

class MuFastStationFitter: public AlgTool
{
   public:
      
      static const InterfaceID& interfaceID();

   public:

      MuFastStationFitter(const std::string& type, 
                          const std::string& name,
                          const IInterface*  parent);
    
      ~MuFastStationFitter();
    
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();
    
   public:
      StatusCode findSuperPoints(const LVL1::RecMuonRoI*    p_roi,
				 TrigL2MuonSA::RpcFitResult rpcFitResult,
				 std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns);

      StatusCode findSuperPoints(const LVL1::RecMuonRoI*    p_roi,
				 TrigL2MuonSA::TgcFitResult tgcFitResult,
				 std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns);

      StatusCode superPointFitter(TrigL2MuonSA::TrackPattern& trackPattern);
      
   private:

      /** @brief Pointer to MsgStream.*/
      MsgStream* m_msg;

      /**
       * @brief Accessor method for the MsgStream.
       * @return handle to the MsgStream.
       */
      inline MsgStream& msg() const { return *m_msg; }

      /**
       * @brief Accessor method for the message level variable.
       * @return value of the message level for this algorithm.
       */
      inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }

      // Reference to StoreGateSvc;
      ServiceHandle<StoreGateSvc>    m_storeGateSvc;

 private:
      float SetDriftSpace(float tdr, float rad, float zeta, float phim, float phiDir);
      void  Xline(float *, float *, float *, int *, int ,
		  float *, float *, float *, float *, float *, float *);
      void  Circfit (int, float *, float *, float *, float *, int *,
		     float *, float *, float DAB[2][2], float *);
      void  Circles (int, float *, float *, float *, float *, int *,
		     float *, float *, float DAB[2][2], float *, float *);
      int   Evlfit (int, TrigL2MuonSA::PBFitResult& fitres);
};

} // namespace TrigL2MuonSA

#endif  // MUFASTSTATIONFITTER_H
