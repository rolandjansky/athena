/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONERRORSCALINGTOOLS_MUONERRORSCALEDBTOOL_H
#define MUONERRORSCALINGTOOLS_MUONERRORSCALEDBTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonRecToolInterfaces/IMuonErrorScaleDbTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IIOVSvc.h"

/**
   @file MuonErrorScaleDbTool.h
   @class MuonErrorScaleDbTool
   
   @brief Tool for retrieving values from conditions database
   to be used for error scaling of muon hits in track fitting.

   @date 20 May 2010   
*/

namespace Muon {

  class MuonIdHelperTool;

  class MuonErrorScaleDbTool : virtual public IMuonErrorScaleDbTool, public AthAlgTool {

  public:
    MuonErrorScaleDbTool(const std::string& type, const std::string& name,
			 const IInterface* parent);
    virtual ~MuonErrorScaleDbTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    bool doMdt() const { return m_do_mdt; }
    bool doTgc() const { return m_do_tgc; }
    bool doRpc() const { return m_do_rpc; }
    bool doCsc() const { return m_do_csc; }

    std::vector<double> errScale(const Identifier id) const;

    virtual MsgStream& dump(MsgStream&) const;

 private:
    
    void registerParameters(bool&, std::vector<double>*, const std::string&);

    //! conditions data handling: call-back entry to re-set scaling parameters when an IOV is new or changed.
    StatusCode callback( IOVSVC_CALLBACK_ARGS );

    //! internal code structuring: formatted output for scaling parameters
    const std::string makeInfoString(const std::string&,const bool,
                                     const std::vector<double>&) const;
    
    ///////////////////////////////////////////////////////////////////
    // Private data members:
    ///////////////////////////////////////////////////////////////////

    ToolHandle<MuonIdHelperTool> m_muonIdHelperTool; //!< Muon ID helper tool

    //! conditions data handling: COOL folder name for Muon scaling paremeters
    const std::string m_muonFolder;

    //! map holding the scaling parameters for each detector region
    typedef std::map<std::string,std::vector<double>*> ParamMap;

    /** @brief map holding the scaling parameters for each detector region
	
        The parammap is used at call-back/initialisation to avoid map
        search at every scaling call. It maps directly to the
        references of the local vector variables!
    */
    ParamMap m_parammap;
    
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

  MsgStream& operator << (MsgStream&, const MuonErrorScaleDbTool&);
  
  inline MsgStream& operator <<
    (MsgStream& sl, const MuonErrorScaleDbTool& se)
  {
    return se.dump(sl);
  }
}

#endif // MUONERRORSCALINGTOOLS_MUONERRORSCALEDBTOOL_H

