/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ListItem.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "MuonErrorScalingTools/MuonErrorScaleDbTool.h"

//________________________________________________________________________
Muon::MuonErrorScaleDbTool::MuonErrorScaleDbTool(const std::string& type,
						 const std::string& name, 
						 const IInterface* parent)
  : AthAlgTool(type,name,parent)
  , m_muonIdHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool")
  , m_muonFolder("/MUON/TrkErrorScaling")
  , m_do_mdt(false)
  , m_do_tgc(false)
  , m_do_rpc(false)
  , m_do_csc(false)
  , m_scaling_mdt_barrel(std::vector<double>(0))
  , m_scaling_mdt_endcap(std::vector<double>(0))
  , m_scaling_tgcPhi(std::vector<double>(0))
  , m_scaling_tgcEta(std::vector<double>(0))
  , m_scaling_rpcPhi(std::vector<double>(0))
  , m_scaling_rpcEta(std::vector<double>(0))
  , m_scaling_cscPhi(std::vector<double>(0))
  , m_scaling_cscEta(std::vector<double>(0))
{

  declareInterface<Muon::IMuonErrorScaleDbTool>(this);
  
  declareProperty("MuonIdHelperTool", m_muonIdHelperTool);  


}

//________________________________________________________________________
Muon::MuonErrorScaleDbTool::~MuonErrorScaleDbTool()
{

}

//________________________________________________________________________
StatusCode Muon::MuonErrorScaleDbTool::initialize()
{

  // --- check scaling factors
  registerParameters(m_do_mdt,
		     &m_scaling_mdt_barrel,"MDT Barrel");
  registerParameters(m_do_mdt,
		     &m_scaling_mdt_endcap,"MDT Endcap");
  
  registerParameters(m_do_tgc,
		     &m_scaling_tgcPhi,"TGC Phi");
  registerParameters(m_do_tgc,
		     &m_scaling_tgcEta,"TGC Eta");
  
  registerParameters(m_do_rpc,
		     &m_scaling_rpcPhi,"RPC Phi");
  registerParameters(m_do_rpc,
		     &m_scaling_rpcEta,"RPC Eta");
  
  registerParameters(m_do_csc,
		     &m_scaling_cscPhi,"CSC Phi");
  registerParameters(m_do_csc,
		     &m_scaling_cscEta,"CSC Eta");
   
  // find and register callback function for Muons
  const DataHandle<CondAttrListCollection> colptr;
  if (detStore()->contains<CondAttrListCollection>(m_muonFolder)) {
    if (StatusCode::SUCCESS!=detStore()->
	regFcn(&Muon::MuonErrorScaleDbTool::callback,
	       this,colptr,m_muonFolder)) {
      msg(MSG::ERROR) << "Found the folder, but could not register a callback"
		      << " on " << m_muonFolder << endmsg;
      return StatusCode::FAILURE;
    } 
    else
      msg(MSG::INFO) << "Registered callback on COOL folder " 
		     << m_muonFolder << endmsg;
  } 
  else {
    msg(MSG::INFO) << "Folder " << m_muonFolder << " is not loaded, "
		   << "intrinsic meas't errors will be used for Muon RIOs_OnTrack." << endmsg;
    
    m_do_mdt = false;
    m_do_tgc = false;
    m_do_rpc = false;
    m_do_csc = false;
  }

  // set up MuonIdHelperTool
  if (m_muonIdHelperTool.retrieve().isSuccess()) 
    ATH_MSG_INFO("Retrieved  "<<m_muonIdHelperTool);
  else {
    ATH_MSG_FATAL("Could not get MuonIdHelperTool!");
    return StatusCode::FAILURE;
  }    
  
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode Muon::MuonErrorScaleDbTool::finalize()
{
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
void Muon::MuonErrorScaleDbTool::registerParameters(bool& do_detSystem,
						    std::vector<double>* errscaler,
						    const std::string& sdet) 
{
  // add this set of parameters to list, always set the parameters to be
  // 'used' for now
  do_detSystem=true;
  m_parammap.insert(ParamMap::value_type(sdet,errscaler));

  // set default parameters in case no info is read from condDB for this set
  errscaler->clear();
  errscaler->push_back(1.0);
  errscaler->push_back(0.0);
}

//________________________________________________________________________
MsgStream& Muon::MuonErrorScaleDbTool::dump( MsgStream& out ) const
{
  //--- need an sprintf here.
  out << std::endl << name() << " has found the following factors and constants to scale errors:"  <<std::endl;
  out << "| Detector System                factor   constant            |"           <<std::endl;
  out << "+-------------------------------------------------------------+"           <<std::endl;

  out << makeInfoString("MDT barrel    ",m_do_mdt,m_scaling_mdt_barrel) <<std::endl;
  out << makeInfoString("MDT endcap    ",m_do_mdt,m_scaling_mdt_endcap) <<std::endl;
  out << makeInfoString("RPC Phi barrel",m_do_rpc,m_scaling_rpcPhi)     <<std::endl;
  out << makeInfoString("RPC Eta barrel",m_do_rpc,m_scaling_rpcEta)     <<std::endl;
  out << makeInfoString("TGC Phi endcap",m_do_tgc,m_scaling_tgcPhi)     <<std::endl;
  out << makeInfoString("TGC Eta endcap",m_do_tgc,m_scaling_tgcEta)     <<std::endl;
  out << makeInfoString("CSC Phi endcap",m_do_csc,m_scaling_cscPhi)     <<std::endl;
  out << makeInfoString("CSC Eta endcap",m_do_csc,m_scaling_cscEta)     <<std::endl;
  out << "+-------------------------------------------------------------+"           <<std::endl;
  return out;
}


//________________________________________________________________________
const std::string 
Muon::MuonErrorScaleDbTool::makeInfoString(const std::string& sdet, 
					   const bool do_detSystem,
					   const std::vector<double>& errscaler) const 
{
  
  const int nformat=62;
  std::string s1("| ");
  s1.append(sdet);
  if (do_detSystem) {
    s1.append(" - scaled by ");
  } 
  else {
    s1.append(" - unscaled. ");
  }
  char s2[7];
  sprintf(s2,"%6.3g ",errscaler[0]);s1.append(s2);
  s1.append("* err (+) ");
  sprintf(s2,"%6.3g ",errscaler[1]);s1.append(s2);
  int n = nformat-s1.size();
  for(int i=0; i<n; ++i) s1.append(" "); 
  s1.append("|");
  return s1;
}

//________________________________________________________________________
StatusCode 
Muon::MuonErrorScaleDbTool::callback(IOVSVC_CALLBACK_ARGS_P(I,keys) ) 
{
  (void) I;
  
  // callback function when the conditions data object changes
  // loop over all the keys, action for the one we are interested in
  for (std::list<std::string>::const_iterator itr=keys.begin();itr!=keys.end();
       ++itr) {
    if (*itr==m_muonFolder) {
      ATH_MSG_DEBUG ("Callback function invoked! (Folder: " << *itr << ")");
      const CondAttrListCollection* atrlistcol=0;
      if (StatusCode::SUCCESS==detStore()->retrieve(atrlistcol,*itr)) {
	// loop through elements of the attribute list collection
	for (CondAttrListCollection::const_iterator citr=atrlistcol->begin();
	     citr!=atrlistcol->end();++citr) {
	  // attribute list format is name(string), n(int) followed by n double
	  const coral::AttributeList& alist=citr->second;
	  try {
	    const std::string& name=alist[0].data<std::string>();
	    const int nvals=alist[1].data<int>();
	    // locate pointer to parameters vector, based on name
	    // note that names unknown to the service are ignored
	    ParamMap::const_iterator loc=m_parammap.find(name);
	    if (loc!=m_parammap.end()) {
	      std::vector<double>* params=loc->second;
	      ATH_MSG_DEBUG ("Setting " << nvals << " parameters for " << name << " at location " << params);
	      params->clear();
	      for (int i=0;i<nvals;++i)
		params->push_back(alist[2+i].data<double>());
	    } else {
	      ATH_MSG_DEBUG ("Ignored unexpected parameter" << name);
	    }
	  }
	  // catch problems with the attributelist accesses
	  catch (coral::Exception& e) {
	    msg(MSG::ERROR) << "Problem with AttributeList decoding: " << e.what() << endmsg;
	    return StatusCode::FAILURE;
	  }
	}
      } else {
	msg(MSG::ERROR) << "Problem reading conditions object " << *itr << endmsg;
	return StatusCode::FAILURE;
      }
    }
  }
  // printout new constants if in debug print mode
  ATH_MSG_DEBUG ( (*this) );
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
std::vector<double> 
Muon::MuonErrorScaleDbTool::errScale(const Identifier id) const
{
  std::vector<double> val;

  if (!m_muonIdHelperTool->isMuon(id)) {
    ATH_MSG_WARNING("using MuonErrorScaleDbTool with non-muon Identifier!");
    return val;
  }
  
  if (m_muonIdHelperTool->isMdt(id)) {
    bool isEndcap=m_muonIdHelperTool->isEndcap(id);  
    return isEndcap ? m_scaling_mdt_endcap : m_scaling_mdt_barrel;
  }
  else if (m_muonIdHelperTool->isTgc(id)) {
    bool measPhi = m_muonIdHelperTool->measuresPhi(id);
    return measPhi ? m_scaling_tgcPhi : m_scaling_tgcEta;        
  }
  else if (m_muonIdHelperTool->isRpc(id)) {
    bool measPhi = m_muonIdHelperTool->measuresPhi(id);
    return measPhi ? m_scaling_rpcPhi : m_scaling_rpcEta;        
  }
  else if (m_muonIdHelperTool->isCsc(id)) {
    bool measPhi = m_muonIdHelperTool->measuresPhi(id);
    return measPhi ? m_scaling_cscPhi : m_scaling_cscEta;        
  }

  ATH_MSG_ERROR("didn't find muon type!");
  return val;

}
