/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetDumper.h

#ifndef JetDumper_H
#define JetDumper_H

// David Adams
// January 2014
//
// ASG tool to dump information about jets, clusters or psuedojets.

#include "AsgTools/AsgTool.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#ifdef USE_BOOST_FOREACH
#include <boost/foreach.hpp>
#endif
#include "fastjet/PseudoJet.hh"
#include "AthLinks/ElementLinkVector.h"
#include "JetInterface/IJetExecuteTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODJet/Jet.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/IConstituentUserInfo.h"

namespace xAOD {
class IParticle;
}

/// Tool to dump jets to the log.
/// Tool properties:
///   ContainerName: Name of the container to be dumped
///   Detail: 0 for container info, 1 for one line per object, 2 for object properties
///   LineDetail: 0 for object index, 2 for kinematics, 2 for more
///   Prefix: 0 for no prefix, 1 for type, 2 for container, 3 for type+contaienr
///   MaxObject: Maximum number of objects for which detail is shown
///   FloatMax: Above this value, floats are displayed in scientific notation.
///   FloatMoments: Names of the float moments to display
///   IntMoments: Names of the integer moments to display
///   BoolMoments: Names of the bool moments to display
///   CBoolMoments: Names of the bool moments storaed as char to display
///   StringMoments: Names of the string moments to display
///   FourVectorMoments: Names of the four-vector moments to display
///   ElementLinkMoments: Names of the element link moments to display
///   IntVectorMoments: Names of the int vector moments to display
///   FloatVectorMoments: Names of the float vector moments to display
///   AssociatedParticleVectors: Names of the associated object vectors to display

class JetDumper
: virtual public IJetExecuteTool,
  public asg::AsgTool {
ASG_TOOL_CLASS(JetDumper, IJetExecuteTool)

public:  // typedefs

  typedef std::string Name;
  typedef std::vector<Name> NameList;
  typedef xAOD::JetFourMom_t FourVector;
  typedef std::vector<const xAOD::IParticle*> APVector;
  typedef ElementLink<xAOD::IParticleContainer> APEL;
  typedef std::vector<APEL> APELVector;
  typedef std::vector<const xAOD::MuonSegment*> MSVector;
  typedef ElementLink<xAOD::MuonSegmentContainer> MSEL;
  typedef std::vector<MSEL> MSELVector;

public:  // methods

  // Ctor.
  JetDumper(std::string myname);

  // Process a jet.
  // Returns 0 for success.
  template<typename T>
  int dump_object(const T* pjet) const;

  // Process a collection of jets.
  // Returns 0 for success.
  template<typename TList>
  int dump_collection(const TList* pjets) const;

  // Process the current event.
  int execute() const;

  // Print state.
  void print() const;

private:  // methods

  // Write the label for a single object.
  template<typename T>
  std::string object_label(const T* pjet, std::string label) const;
  std::string object_label(const fastjet::PseudoJet& jet, std::string label) const;

  // Process a single jet after writing prefix.
  template<typename T>
  int dump_object_after_prefix(const T* pjet) const;
  int dump_object_after_prefix(const fastjet::PseudoJet& jet) const;
  int dump_object_after_prefix(const xAOD::MuonSegment* seg) const;

  // Fetch a moment.
  template<typename TObj, typename TMom>
  void get_moment(TObj* pobj, std::string name, TMom& val) const;
  template<typename TObj>
  void get_moment(TObj* pobj, std::string name, FourVector& val) const;
  template<typename TObj>
  void get_moment(TObj* pobj, std::string name, std::string& val) const;
  template<typename TObj>
  void get_moment(TObj* pobj, std::string name, std::vector<int>&) const;
  template<typename TObj>
  void get_moment(TObj* pobj, std::string name, std::vector<float>&) const;
  template<typename TMom>
  void get_moment(const xAOD::Jet* pobj, std::string name, TMom& val) const;
  void get_moment(const xAOD::Jet* pobj, std::string name, FourVector& val) const;
  void get_moment(const xAOD::Jet* pobj, std::string name, std::string& val) const;
  void get_moment(const xAOD::Jet* pobj, std::string name, std::vector<int>& vals) const;
  void get_moment(const xAOD::Jet* pobj, std::string name, std::vector<float>& vals) const;
  template<typename TObj>
  std::string get_moment_as_string(const TObj& obj, std::string name) const;
  std::string get_moment_as_string(const xAOD::Jet* pobj, std::string name) const;

  template<typename TObj>
  void getAssociatedParticles(TObj* pobj, std::string name, APVector& val) const;
  void getAssociatedParticles(const xAOD::Jet* pobj, std::string name, APVector& val) const;
  template<typename TObj>
  void getAssociatedLinks(TObj* pobj, std::string name, APELVector& val) const;
  void getAssociatedLinks(const xAOD::Jet* pobj, std::string name, APELVector& val) const;
  template<typename TObj>
  void getAssociatedParticles(TObj* pobj, std::string name, MSVector& val) const;
  void getAssociatedParticles(const xAOD::Jet* pobj, std::string name, MSVector& val) const;
  template<typename TObj>
  void getAssociatedLinks(TObj* pobj, std::string name, MSELVector& val) const;
  void getAssociatedLinks(const xAOD::Jet* pobj, std::string name, MSELVector& val) const;

  // Extra information to appear at the end of the line
  // with the object kinematics.
  template<typename T>
  void extra_info(const T*, std::ostream& out, int) const;
  void extra_info(const xAOD::Jet* pjet, std::ostream& out, int iopt) const;
  void extra_info(const fastjet::PseudoJet* ppsj, std::ostream& out, int linedetail) const;

private:  // data

  // Job options.
  std::string m_cname;
  int m_detail;
  int m_linedetail;
  int m_prefix;
  int m_maxobj;
  float m_floatmax;
  NameList m_fmoms;   /// Float moments
  NameList m_imoms;   /// Int moments
  NameList m_bmoms;   /// Bool moments
  NameList m_cbmoms;  /// Bool stored as char moments
  NameList m_smoms;   /// String moments
  NameList m_fvmoms;  /// Four-vector moments
  NameList m_elmoms;  /// Element link moments
  NameList m_vimoms;  /// Vector<int> moments
  NameList m_vfmoms;  /// Vector<float> moments
  NameList m_apmoms;  /// Associated particvle moments

  // Object type name.
  mutable Name m_objtypename;
};

// Fetch moment keys.
template<typename T, typename TMom>
JetDumper::NameList get_moment_keys(const T* pobj);
template<>
JetDumper::NameList get_moment_keys<xAOD::Jet, float>(const xAOD::Jet* pjet);
template<>
JetDumper::NameList get_moment_keys<xAOD::Jet, int>(const xAOD::Jet* pjet);

//**********************************************************************

template<typename T>
int JetDumper::dump_object(const T* pobj) const {
  msg() << MSG::INFO << m_objtypename << " ";
  return dump_object_after_prefix(pobj);
}

template<typename T>
int JetDumper::dump_object_after_prefix(const T* pjet) const {
  const double mevtogev = 0.001;
  const int wname = 30;
  if ( pjet == 0 ) {
    msg() << endmsg;
    ATH_MSG_ERROR("Null jet.");
    return 1;
  }
  // One line summary.
  std::ostringstream ssjetline;
  if ( m_linedetail > 0 ) {
    ssjetline << "pT ="  << std::setw(6) << std::fixed << std::setprecision(1) << mevtogev*pjet->pt() << " GeV";
    ssjetline << ", m =" << std::setw(6) << std::fixed << std::setprecision(1) << mevtogev*pjet->m() << " GeV";
    ssjetline << ", eta =" << std::setw(5) << std::fixed << std::setprecision(2) << pjet->eta();
    ssjetline << ", phi =" << std::setw(5) << std::fixed << std::setprecision(2) << pjet->phi();
  }
  if ( m_linedetail > 1 ) extra_info(pjet, ssjetline, m_linedetail);
  msg() << ssjetline.str() << endmsg;
  // All properties.
  if ( m_detail > 1 ) {
    NameList fnames = m_fmoms;
    NameList inames = m_imoms;
    NameList bnames = m_bmoms;
    NameList cbnames = m_cbmoms;
    NameList snames = m_smoms;
    NameList fvnames = m_fvmoms;
    NameList elnames = m_elmoms;
    NameList vinames = m_vimoms;
    NameList vfnames = m_vfmoms;
    NameList apnames = m_apmoms;
    if ( fnames.size() == 0 ) fnames = get_moment_keys<T,float>(pjet);
    ATH_MSG_INFO("    " << m_objtypename << " has " << fnames.size() << " float attributes:");
    for ( NameList::const_iterator inam=fnames.begin(); inam!=fnames.end(); ++inam ) {
      Name name = *inam;
      float val;
      get_moment(pjet, name, val);
      if ( fabs(val) > m_floatmax ) {
        ATH_MSG_INFO(std::setw(wname) << name << ":" << std::setw(16) << std::scientific << std::setprecision(3) << val);
      } else {
        ATH_MSG_INFO(std::setw(wname) << name << ":" << std::setw(12) << std::fixed << std::setprecision(3) << val);
      }
    }
    if ( inames.size() == 0 ) inames = get_moment_keys<T,int>(pjet);
    ATH_MSG_INFO("    " << m_objtypename << " has " << inames.size() << " int attributes:");
    for ( NameList::const_iterator inam=inames.begin(); inam!=inames.end(); ++inam ) {
      Name name = *inam;
      int ival;
      get_moment(pjet, name, ival);
      ATH_MSG_INFO(std::setw(wname) << name << ":" << std::setw(12) << std::fixed << std::setprecision(3) << ival);
    }
    if ( bnames.size() == 0 ) bnames = get_moment_keys<T,int>(pjet);
    ATH_MSG_INFO("    " << m_objtypename << " has " << bnames.size() << " bool attributes:");
    for ( NameList::const_iterator inam=bnames.begin(); inam!=bnames.end(); ++inam ) {
      Name name = *inam;
      bool val;
      get_moment(pjet, name, val);
      std::string sval = val ? "true" : "false";
      ATH_MSG_INFO(std::setw(wname) << name << ":" << std::setw(6) << sval);
    }
    if ( cbnames.size() == 0 ) cbnames = get_moment_keys<T,int>(pjet);
    ATH_MSG_INFO("    " << m_objtypename << " has " << cbnames.size() << " cbool attributes:");
    for ( NameList::const_iterator inam=cbnames.begin(); inam!=cbnames.end(); ++inam ) {
      Name name = *inam;
      char val;
      get_moment(pjet, name, val);
      std::string sval = val ? "true" : "false";
      ATH_MSG_INFO(std::setw(wname) << name << ":" << std::setw(6) << sval);
    }
    ATH_MSG_INFO("    " << m_objtypename << " has " << snames.size() << " string attributes:");
    for ( NameList::const_iterator inam=snames.begin(); inam!=snames.end(); ++inam ) {
      Name name = *inam;
      std::string sval;
      get_moment(pjet, name, sval);
      ATH_MSG_INFO(std::setw(wname) << name << ":" << std::setw(30) << sval);
    }
    ATH_MSG_INFO("    " << m_objtypename << " has " << vinames.size() << " vector<int> attributes:");
    for ( NameList::const_iterator inam=vinames.begin(); inam!=vinames.end(); ++inam ) {
      Name name = *inam;
      std::vector<int> vals;
      get_moment(pjet, name, vals);
      if ( vals.size() == 0 ) {
        ATH_MSG_INFO(std::setw(wname) << name << ": No entries.");
      } else {
        int widx = 1;
        if ( vals.size() > 10 ) widx = 2;
        for ( unsigned int ival=0; ival<vals.size(); ++ival ) {
          ATH_MSG_INFO(std::setw(wname) << name << "[" << std::setw(widx) << ival << "]: "
                       << std::setw(8) << vals[ival]);
        }
      }
    }
    ATH_MSG_INFO("    " << m_objtypename << " has " << vfnames.size() << " vector<float> attributes:");
    for ( NameList::const_iterator inam=vfnames.begin(); inam!=vfnames.end(); ++inam ) {
      Name name = *inam;
      std::vector<float> vals;
      get_moment(pjet, name, vals);
      if ( vals.size() == 0 ) {
        ATH_MSG_INFO(std::setw(wname) << name << ": No entries.");
      } else {
        int widx = 1;
        if ( vals.size() > 10 ) widx = 2;
        for ( unsigned int ival=0; ival<vals.size(); ++ival ) {
          ATH_MSG_INFO(std::setw(wname) << name << "[" << std::setw(widx) << ival << "]: "
                       << std::setw(12) << std::fixed << std::setprecision(3) << vals[ival]);
        }
      }
    }
    ATH_MSG_INFO("    " << m_objtypename << " has " << apnames.size() << " associated particle vectors:");
    for ( NameList::const_iterator inam=apnames.begin(); inam!=apnames.end(); ++inam ) {
      Name name = *inam;
      if ( name.find("MuonSegment") != std::string::npos ) {
        MSVector vals;
        getAssociatedParticles(pjet, name, vals);
        MSELVector els;
        getAssociatedLinks(pjet, name, els);
        std::string slab = "entries";
        if ( vals.size() == 1 ) slab = "entry";
        if ( vals.size() > 0 ) slab += ":";
        ATH_MSG_INFO(std::setw(wname) << name << " has " << vals.size() << " " << slab);
        if ( els.size() == vals.size() ) {
#ifdef USE_BOOST_FOREACH
          BOOST_FOREACH(const MSEL& el, els) {
            ElementLink<xAOD::MuonSegmentContainer> newel(el);
#else
          for (const MSEL& el : els) {
#endif
            ATH_MSG_INFO(std::setw(wname+2) << "" << el.dataID() << "[" << el.index() << "]");
          }
        } else {
#ifdef USE_BOOST_FOREACH
          BOOST_FOREACH(const xAOD::MuonSegment* ppar, vals) {
#else
          for (auto ppar : vals) {
#endif
            ATH_MSG_INFO(std::setw(wname+2) << "" << ppar->container() << "[" << ppar->index() << "]");
          }
        }
      } else {
        APVector vals;
        getAssociatedParticles(pjet, name, vals);
        APELVector els;
        getAssociatedLinks(pjet, name, els);
        std::string slab = "entries";
        if ( vals.size() == 1 ) slab = "entry";
        if ( vals.size() > 0 ) slab += ":";
        ATH_MSG_INFO(std::setw(wname) << name << " has " << vals.size() << " " << slab);
        // Show container names if found, otherwise container addresses.
        if ( els.size() == vals.size() ) {
#ifdef USE_BOOST_FOREACH
          BOOST_FOREACH(const APEL& el, els) {
#else
          for (const APEL& el : els) {
#endif
            ATH_MSG_INFO(std::setw(wname+2) << "" << el.dataID() << "[" << el.index() << "]");
          }
        } else {
#ifdef USE_BOOST_FOREACH
          BOOST_FOREACH(const xAOD::IParticle* ppar, vals) {
#else
          for (auto ppar : vals) {
#endif
            ATH_MSG_INFO(std::setw(wname+2) << "" << ppar->container() << "[" << ppar->index() << "]");
          }
        }
      }
    }
    if ( fvnames.size() == 0 ) fvnames = get_moment_keys<T,float>(pjet);
    ATH_MSG_INFO("    " << m_objtypename << " has " << fvnames.size() << " four-vector attributes (pT m eta phi):");
    for ( NameList::const_iterator inam=fvnames.begin(); inam!=fvnames.end(); ++inam ) {
      Name name = *inam;
      FourVector val;
      get_moment(pjet, name, val);
      ATH_MSG_INFO(std::setw(wname) << name << ":"
        << std::setw(12) << std::fixed << std::setprecision(3) << val.Pt()
        << std::setw(12) << std::fixed << std::setprecision(3) << val.M()
        << std::setw(12) << std::fixed << std::setprecision(3) << val.Eta()
        << std::setw(12) << std::fixed << std::setprecision(3) << val.Phi()
      );
    }
    ATH_MSG_INFO("    " << m_objtypename << " has " << elnames.size() << " element-link attributes:");
    for ( NameList::const_iterator inam=elnames.begin(); inam!=elnames.end(); ++inam ) {
      std::string name = *inam;
      std::string sel = get_moment_as_string(pjet, name);
      ATH_MSG_INFO(std::setw(wname) << name << ": " << sel);
    }
  }
  return 0;
}

template<typename T>
std::string JetDumper::object_label(const T*, std::string label) const {
  return label;
}

//**********************************************************************

template<typename T>
T& const_cast_ptr(T& obj) { return  obj; }

template<typename T>
T* const_cast_ptr(T* pobj) { return const_cast<T*>(pobj); }

template<typename TList>
int JetDumper::dump_collection(const TList* pjets) const {
  int njet = pjets->size();
  ATH_MSG_INFO("  " << m_objtypename << " multiplicity: " << njet << " [" << m_cname << "]");
  std::string line = "-------------------------------------------------------------------------";
  int jstat = 0;
  if ( m_detail > 0 ) {
    unsigned int njetshow = njet;
    if ( m_maxobj > 0 && m_maxobj < njet ) njetshow = m_maxobj;
    typename TList::const_iterator itjet = pjets->begin();
    typedef typename TList::const_iterator::reference TValue;
    std::vector<std::string> labs;
    unsigned int wlab = 0;
    for ( unsigned int ijet=0; ijet<njetshow; ++ijet ) {
      TValue pjet = *itjet++;
      std::string lab = object_label(pjet, m_objtypename);
      labs.push_back(lab);
      if ( lab.size() > wlab ) wlab = lab.size();
    }
    itjet = pjets->begin();
    int widx = 1;
    if ( njetshow > 10 ) widx = 2;
    if ( njetshow > 100 ) widx = 3;
    if ( njetshow > 1000 ) widx = 4;
    for ( unsigned int ijet=0; ijet<njetshow; ++ijet ) {
      TValue pjet = *itjet++;
      if ( m_detail > 1 && ijet==0 ) ATH_MSG_INFO(line);
      // One line summary.
      msg() << MSG::INFO << "    ";
      if ( m_prefix == 1 || m_prefix > 2 ) {
        msg() << std::setw(wlab) << labs[ijet] << " ";
      }
      if ( m_prefix < 2 ) {
        msg() << std::setw(widx) << ijet;
      } else {
        msg() << m_cname;
        msg() << "[" << std::setw(widx) << ijet << "]";
      }
      msg() << ": ";
      jstat += dump_object_after_prefix(pjet);
      if ( m_detail > 1 ) ATH_MSG_INFO(line);
    }
  }
  return jstat;
}

//**********************************************************************

template<typename TObj, typename TMom>
void JetDumper::get_moment(TObj*, std::string, TMom& val) const {
  val = 0;
}

template<typename TObj>
void JetDumper::get_moment(TObj*, std::string, FourVector& val) const {
  val = FourVector();
}

template<typename TObj>
void JetDumper::get_moment(TObj*, std::string, std::string& val) const {
  val = "";
}

template<typename TObj>
void JetDumper::get_moment(TObj*, std::string, std::vector<int>& val) const {
  static std::vector<int> empty;
  val = empty;
}

template<typename TObj>
void JetDumper::get_moment(TObj*, std::string, std::vector<float>& val) const {
  static std::vector<float> empty;
  val = empty;
}

template<typename TMom>
void JetDumper::get_moment(const xAOD::Jet* pjet, std::string name, TMom& val) const {
  val = pjet->getAttribute<TMom>(name);
}

template<typename TObj>
void JetDumper::getAssociatedParticles(TObj*, std::string, APVector&) const { }

template<typename TObj>
void JetDumper::getAssociatedLinks(TObj*, std::string, APELVector&) const { }

template<typename TObj>
void JetDumper::getAssociatedParticles(TObj*, std::string, MSVector&) const { }

template<typename TObj>
void JetDumper::getAssociatedLinks(TObj*, std::string, MSELVector&) const { }

template<typename TObj>
std::string
JetDumper::get_moment_as_string(const TObj&, std::string) const {
  return " Not a jet.";
}

//**********************************************************************

template<typename T, typename TMom>
JetDumper::NameList get_moment_keys(const T*) {
  return JetDumper::NameList();
}

//**********************************************************************

template<typename T>
void JetDumper::extra_info(const T*, std::ostream& out, int) const {
  out << ", Unknown type: " << typeid(T).name();
}

//**********************************************************************

#endif
