/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUFASTTRACKEXTRAPOLATOR_H
#define  TRIGL2MUONSA_MUFASTTRACKEXTRAPOLATOR_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigL2MuonSA/TrackData.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonBackExtrapolator/ITrigMuonBackExtrapolator.h"

class StoreGateSvc;

namespace TrigL2MuonSA {
  
  class MuFastTrackExtrapolator: public AlgTool
  {
  public:
    
    static const InterfaceID& interfaceID();
    
    MuFastTrackExtrapolator(const std::string& type, 
			    const std::string& name,
			    const IInterface*  parent);
    
    ~MuFastTrackExtrapolator();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();
    
    void setExtrapolatorTool(ToolHandle<ITrigMuonBackExtrapolator>* backExtrapolator);

    StatusCode extrapolateTrack(std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns,
				double winPt);
    
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
    
    // Reference to StoreGateSvc
    ServiceHandle<StoreGateSvc>    m_storeGateSvc;

    ToolHandle<ITrigMuonBackExtrapolator>* m_backExtrapolatorTool;

  };
  
} // namespace TrigL2MuonSA

#endif  // MUFASTTRACKEXTRAPOLATOR_H
