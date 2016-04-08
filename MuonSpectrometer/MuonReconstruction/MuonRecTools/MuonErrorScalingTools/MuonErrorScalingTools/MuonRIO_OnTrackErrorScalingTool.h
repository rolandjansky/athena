/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// MuonRIO_OnTrackErrorScalingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUON_MUONRIO_ONTRACKERRORSCALINGTOOL_H
#define MUON_MUONRIO_ONTRACKERRORSCALINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/Identifier.h"

#include "MuonRecToolInterfaces/IMuonRIO_OnTrackErrorScalingTool.h"
#include "TrkEventPrimitives/ParamDefs.h"


namespace Muon
{
  class IMuonErrorScaleDbTool;

  class MuonRIO_OnTrackErrorScalingTool :  public AthAlgTool, 
    virtual public Muon::IMuonRIO_OnTrackErrorScalingTool {
    
  public:
    MuonRIO_OnTrackErrorScalingTool(const std::string&, const std::string&, 
				    const IInterface*);

    /** default destructor */
    virtual ~MuonRIO_OnTrackErrorScalingTool();
    
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();

    /** standard Athena-Algorithm method */
    virtual StatusCode finalize();

    virtual bool needToScaleMdt() const;
    virtual bool needToScaleTgc() const;
    virtual bool needToScaleRpc() const;
    virtual bool needToScaleCsc() const;

    virtual Amg::MatrixX*
     createScaledMdtCovariance(const Amg::MatrixX&,
				bool is_endcap) const;
    
    virtual Amg::MatrixX*
      createScaledTgcCovariance(const Amg::MatrixX&,
				const Trk::ParamDefs=Trk::distPhi) const;

    virtual Amg::MatrixX*
      createScaledRpcCovariance(const Amg::MatrixX&,
				const Trk::ParamDefs=Trk::distPhi) const;
    
    virtual Amg::MatrixX*
      createScaledCscCovariance(const Amg::MatrixX&,
				const Trk::ParamDefs=Trk::distPhi) const;

    // 
    virtual Amg::MatrixX*
      createScaledMdtCovariance(const Amg::MatrixX&,
				Identifier id) const;

    // dumps the current scaling parameters (via error scaling DB tool)
    MsgStream& dump(MsgStream&) const;
    
  private:

    ToolHandle<IMuonErrorScaleDbTool> m_errorScaleDbTool;

    //! internal code structuring: error scaling for 2-dim measurements
    void scale2by2(Amg::MatrixX&, 
                   const std::vector<double>&,
                   const std::vector<double>&) const;
    
    /** class member version of retrieving MsgStream */
    mutable MsgStream m_log;

    bool                  m_do_mdt;
    bool                  m_do_tgc;
    bool                  m_do_rpc;
    bool                  m_do_csc;
    
    std::vector<double>   m_scaling_mdt_barrel;
    std::vector<double>   m_scaling_mdt_endcap;
    std::vector<double>   m_scaling_tgcPhi;
    std::vector<double>   m_scaling_tgcEta;
    std::vector<double>   m_scaling_rpcPhi;
    std::vector<double>   m_scaling_rpcEta;
    std::vector<double>   m_scaling_cscPhi;
    std::vector<double>   m_scaling_cscEta;   
  };

  //________________________________________________________________________
  inline bool MuonRIO_OnTrackErrorScalingTool::needToScaleMdt() const
  {
    return m_do_mdt;
  }
  inline  bool MuonRIO_OnTrackErrorScalingTool::needToScaleTgc() const
  {
    return m_do_tgc;
  }
  inline bool MuonRIO_OnTrackErrorScalingTool::needToScaleRpc() const
  {
    return m_do_rpc;
  }
  inline bool MuonRIO_OnTrackErrorScalingTool::needToScaleCsc() const
  {
    return m_do_csc;
  }

} // end of namespace

#endif // MUON_MUONRIO_ONTRACKERRORSCALINGTOOL_H

