/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetDumper.cxx

#include "JetRec/JetDumper.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include <iostream>

using std::string;
using xAOD::Jet;
using jet::PseudoJetVector;
using jet::IConstituentUserInfo;

typedef JetDumper::Name Name;
typedef JetDumper::NameList NameList;

//**********************************************************************

JetDumper::JetDumper(std::string myname)
: AsgTool(myname), m_objtypename("Unknown") {
  ATH_MSG_INFO("JetDumper::ctor: Declaring properties.");
  declareProperty("ContainerName", m_cname ="");
  declareProperty("Detail", m_detail =1);
  declareProperty("LineDetail", m_linedetail =1);
  declareProperty("Prefix", m_prefix =1);
  declareProperty("MaxObject", m_maxobj =10);
  declareProperty("FloatMax", m_floatmax = 0.999e6);
  declareProperty("FloatMoments", m_fmoms);
  declareProperty("IntMoments", m_imoms);
  declareProperty("BoolMoments", m_bmoms);
  declareProperty("CBoolMoments", m_cbmoms);
  declareProperty("StringMoments", m_smoms);
  declareProperty("FourVectorMoments", m_fvmoms);
  declareProperty("ElementLinkMoments", m_elmoms);
  declareProperty("IntVectorMoments", m_vimoms);
  declareProperty("FloatVectorMoments", m_vfmoms);
  declareProperty("AssociatedParticleVectors", m_apmoms);
  ATH_MSG_DEBUG("JetDumper::ctor: Done.");
}

//**********************************************************************

template<>
JetDumper::NameList get_moment_keys<Jet, float>(const Jet* pjet) {
  static NameList empty;
  if ( pjet == 0 ) return empty;
  return empty;
}

template<>
JetDumper::NameList get_moment_keys<Jet, int>
  (const Jet*) {
  return JetDumper::NameList();
}

//**********************************************************************

int JetDumper::execute() const {
  ATH_MSG_DEBUG("Looping over collection types...");
  if ( evtStore()->contains<PseudoJetVector>(m_cname) ) {
    ATH_MSG_DEBUG("Collection is pseudojets.");
    m_objtypename = "Pseudojet";
    const PseudoJetVector* pobjs = nullptr;
    StatusCode sc = evtStore()->retrieve(pobjs, m_cname);
    if ( !sc.isFailure() && pobjs != nullptr ) return dump_collection(pobjs);
  } else if ( evtStore()->contains<xAOD::CaloClusterContainer>(m_cname) ) {
    ATH_MSG_DEBUG("Collection is xAOD clusters.");
    m_objtypename = "Cluster";
    const xAOD::CaloClusterContainer* pclus = nullptr;
    pclus = evtStore()->retrieve<const xAOD::CaloClusterContainer>(m_cname);
    //StatusCode sc = evtStore()->retrieve(pclus, m_cname);
    if ( pclus != nullptr ) return dump_collection(pclus);
  } else if ( evtStore()->contains<xAOD::JetContainer>(m_cname) ) {
    ATH_MSG_DEBUG("Collection is xAOD jets.");
    m_objtypename = "Jet";
    const xAOD::JetContainer* pobjs = nullptr;
    pobjs = evtStore()->retrieve<const xAOD::JetContainer>(m_cname);
    if ( pobjs != nullptr ) {
      return dump_collection(pobjs);
      ATH_MSG_DEBUG("Retrieved xAOD jets.");
    } else {
      ATH_MSG_ERROR("xAOD jet retrieval failed.");
    }
  } else if ( evtStore()->contains<xAOD::MuonSegmentContainer>(m_cname) ) {
    ATH_MSG_DEBUG("Collection is xAOD muon segments.");
    m_objtypename = "Muon segment";
    const xAOD::MuonSegmentContainer* psegs = nullptr;
    psegs = evtStore()->retrieve<const xAOD::MuonSegmentContainer>(m_cname);
    if ( psegs != nullptr ) return dump_collection(psegs);
  }
  ATH_MSG_ERROR("Unable to retrieve input collection: " << m_cname);
  return 2;
}

//**********************************************************************

void JetDumper::print() const {
  std::string myname = "JetDumper::print: ";
  ATH_MSG_INFO("Properties for JetDumper " << name());
  ATH_MSG_INFO("      Container: " << m_cname);
  ATH_MSG_INFO("         Detail: " << m_detail);
  ATH_MSG_INFO("     LineDetail: " << m_linedetail);
  ATH_MSG_INFO("  Max # objects: " << m_maxobj);
}

//**********************************************************************

string JetDumper::object_label(const fastjet::PseudoJet& jet, string label) const {
  string sout;
  if ( jet.has_user_info<IConstituentUserInfo>() ) {
    const IConstituentUserInfo& cui = jet.user_info<IConstituentUserInfo>();
    return cui.label() + " pseudojet";
  } else {
    return "No-CUI pseudojet";
  }
  return label;
}

//**********************************************************************

int JetDumper::dump_object_after_prefix(const fastjet::PseudoJet& jet) const {
  const double mevtogev = 0.001;
  // One line summary.
  std::ostringstream ssjetline;
  double px = mevtogev*jet.px();
  double py = mevtogev*jet.py();
  double pt = sqrt(px*px+py*py);
  if ( m_linedetail > 0 ) {
    ssjetline << "pT ="  << std::setw(6) << std::fixed << std::setprecision(1) << pt << " GeV";
    ssjetline << ", m =" << std::setw(6) << std::fixed << std::setprecision(1)
              << mevtogev*jet.m() << " GeV";
    ssjetline << ", eta =" << std::setw(5) << std::fixed << std::setprecision(2) << jet.eta();
    ssjetline << ", phi =" << std::setw(5) << std::fixed << std::setprecision(2) << jet.phi();
  }
  if ( m_linedetail > 1 ) extra_info(&jet, ssjetline, m_linedetail);
  msg() << ssjetline.str() << endmsg;
  return 0;
}

//**********************************************************************

int JetDumper::dump_object_after_prefix(const xAOD::MuonSegment* pseg) const {
  if ( pseg == nullptr ) return 1;
  const xAOD::MuonSegment& seg = *pseg;
  const double mmtom = 0.001;
  // One line summary.
  std::ostringstream ssline;
  double x = mmtom*seg.x();
  double y = mmtom*seg.y();
  double z = mmtom*seg.z();
  double r = sqrt(x*x+y*y);
  double phi = atan2(y,x);
  ssline << "r ="  << std::setw(7) << std::fixed << std::setprecision(2) << r << " m";
  ssline << ", z =" << std::setw(7) << std::fixed << std::setprecision(2) << z << " m";
  ssline << ", phi =" << std::setw(7) << std::fixed << std::setprecision(2) << phi;
  //if ( m_linedetail > 1 ) extra_info(&seg, ssline, m_linedetail);
  msg() << ssline.str() << endmsg;
  return 0;
}

//**********************************************************************

void JetDumper::get_moment(const xAOD::Jet* pjet, std::string name, FourVector& val) const {
  if ( name == "" || name == "jetP4()" ) val = pjet->jetP4();
  else val = pjet->getAttribute<FourVector>(name);
}

//**********************************************************************

void JetDumper::get_moment(const xAOD::Jet* pjet, std::string name,
                           std::vector<int>& vals) const {
  typedef std::vector<int> T;
  vals = pjet->getAttribute<T>(name);
}

//**********************************************************************

void JetDumper::get_moment(const xAOD::Jet* pjet, std::string name,
                           std::vector<float>& vals) const {
  typedef std::vector<float> T;
  vals = pjet->getAttribute<T>(name);
}

//**********************************************************************

void JetDumper::get_moment(const xAOD::Jet* pjet, std::string name, std::string& val) const {
  val = pjet->getAttribute<std::string>(name);
}

//**********************************************************************

void JetDumper::
getAssociatedParticles(const xAOD::Jet* pobj, std::string name, APVector& val) const {
  pobj->getAssociatedObjects(name, val);
}

//**********************************************************************

void JetDumper::
getAssociatedLinks(const xAOD::Jet* pobj, std::string name, APELVector& val) const {
  pobj->getAttribute(name, val);
}

//**********************************************************************

void JetDumper::
getAssociatedParticles(const xAOD::Jet* pobj, std::string name, MSVector& val) const {
  pobj->getAssociatedObjects(name, val);
}

//**********************************************************************

void JetDumper::
getAssociatedLinks(const xAOD::Jet* pobj, std::string name, MSELVector& val) const {
  pobj->getAttribute(name, val);
}

//**********************************************************************

std::string
JetDumper::get_moment_as_string(const xAOD::Jet* pjet, std::string name) const {
  std::ostringstream sout;
  // May want to try other types here.
  // Or find some way to access the base information of the element link.
  try {
    ElementLink<xAOD::JetContainer> eljet;
    if ( pjet->getAttribute(name, eljet) ) {
      sout << "Jet " << eljet.dataID() << "[" << eljet.index() << "]";
      return sout.str();
    }
  } catch (...) { }
  try {
    ElementLink<xAOD::VertexContainer> elvtx;
    if ( pjet->getAttribute(name, elvtx) ) {
      sout << "Vertex " << elvtx.dataID() << "[" << elvtx.index() << "]";
      return sout.str();
    }
  } catch (...) { }
  try {
    ElementLink<xAOD::IParticleContainer> elobj;
    if ( pjet->getAttribute(name, elobj) ) {
      sout << "IParticle " << elobj.dataID() << "[" << elobj.index() << "]";
      return sout.str();
    }
  } catch (...) { }
  return "Not a known element link.";
}

//**********************************************************************

void JetDumper::extra_info(const xAOD::Jet* pjet, std::ostream& out, int iopt) const {
  if ( iopt == 2 ) {
    int ncon = pjet->numConstituents();
    out << ", ncon = " << ncon;
  }
  if ( iopt > 2 ) {
    int ncon = pjet->numConstituents();
    int scon = pjet->getConstituents().size();
    string lab = ", size/ncon=";
    out << lab << scon << "/" << ncon;
  }
  if ( iopt > 3 ) {
    const fastjet::PseudoJet* ppj = pjet->getPseudoJet();
    if ( ppj == nullptr ) {
      out << ", No pseudojet";
    } else {
      out << ", npjcon=" << ppj->constituents().size();
    }
  }
}

//**********************************************************************

void JetDumper::extra_info(const fastjet::PseudoJet* ppsj, std::ostream& out, int linedetail) const {
  if ( ppsj == 0 ) {
    out << ", Null pseudojet";
    return;
  }
  const fastjet::PseudoJet& psj = *ppsj;
  if ( psj.has_user_info() ) {
    const jet::IConstituentUserInfo& cui = psj.user_info<jet::IConstituentUserInfo>();
    if ( linedetail > 1 ) {
      out << ", ilab=" << cui.index();
      if ( linedetail > 2 ) {
        const xAOD::IParticle* pip = cui.particle();
        if ( pip == 0 ) {
          out << ", Associated particle is null";
        } else {
          out << " [" << pip->container() << "/" << pip->index() << "]";
          if ( linedetail > 3 ) out << ", IParticle @ " << pip;
        }
      }
      if ( cui.isGhost() ) out << ", ghost";
    }
  } else {
    out << ", No user info";
  }
}

//**********************************************************************
