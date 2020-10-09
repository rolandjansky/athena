/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAMVACALIBANALYSIS_EGAMMAMVATREE
#define EGAMMAMVACALIBANALYSIS_EGAMMAMVATREE

#include <functional>
#include <string>
#include <map>
#include <array>
#include <vector>
#include <tuple>
#include <cmath>
#include <set>
#include <cstdint>
#include <numeric>

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <TTree.h>
#include <TLorentzVector.h>

#include <AsgMessaging/AsgMessaging.h>

#include "egammaMVACalib/egammaMVALayerDepth.h"
#include "egammaMVACalib/egammaMVAFunctions.h"



/** Adaptor class: xAOD::Electron (or xAOD::Photon) to TTree for MVA calibration
 *
 *  This class is a TTree with 0 events that recreates the structure of the
 *  TTree used during the training of the MVA calibration. In this way the same
 *  input (in the same format and computed with the same code) is exposed to
 *  egammaMVACalib (via egammaMVATool).
 *
 *  The structure of the TTree is dynamically created, given the set of Branches
 *  needed. The name of the branches is the same used during the training, and
 *  follow the convenctions of PhotonD3PD. By the way this tool should not be
 *  intended as a xOAD->D3PD converter.
 *
 *  Developer note: the structure is dynamic, only the needed branches are
 *    created. In addition the class holds the internal variables linked to the
 *    branches. Since variables are of different types this means to have
 *    different containers. This is the simplest implementation, but not very
 *    maintainable. TODO: Consider the usage of type erasure, for example with
 *    boost::variant.
 **/

#ifndef __CINT__
namespace detail
{
  // best solution here should be to don't implement first template function
  // to raise compilation error. But maybe there are problems when
  // linking dictionaries
  template<typename T> inline std::string ROOT_type_name() {               //!
  //  static_assert((sizeof(T) == 0), "type is not int/float/bool");
    return ""; }                                                           //!
  template<> inline std::string ROOT_type_name<int>() { return "I"; }      //!
  template<> inline std::string ROOT_type_name<float>() { return "F"; }    //!
  template<> inline std::string ROOT_type_name<bool>() { return "O"; }     //!
}
#endif

/** Base class implementing variables defined for egamma clusters **/

class egammaMVATreeEgamma : public TTree, public asg::AsgMessaging
{
public:
  // egammaMVATreeEgamma() : TTree(), asg::AsgMessaging("egammaMVATreeEgamma") { };
  egammaMVATreeEgamma(const std::string& name,
                      const std::string& prefix,
                      const std::set<std::string>& variables,
                      bool use_layer_corrected=true);
  // Note: using pointer version, so that trigger can use the same
  // interface with egamma = nullptr
  // TODO: move to const reference, to do that
  // 1. MVA must know in advance the input variables (in the MVA weights)
  // 2. remove particleType (Rconv > 0 && Rconv < 800) from TMVACalib:
  //    particle type should be written in the MVA weights

  /** call update for every egamma object, this update the internal variables
   *  connected to the branches
   **/
  void update(const xAOD::Egamma* egamma);
  void update(const xAOD::Egamma* egamma, const xAOD::CaloCluster* cluster);
private:
  void init_variables(const std::set<std::string>& variables);
  std::string m_prefix;
  bool m_use_layer_corrected = true;
protected:
  template<typename T> const T* get_ptr(const std::string&) const
  {
    static_assert((sizeof(T) == 0), "Type must be int/float");
    return nullptr;
  }
  template<typename T, typename MAPSTRINGFUNCTION, typename CONTAINER>
  void search_var_and_add(const std::string& var_name,
                          const MAPSTRINGFUNCTION& map_string_function,
                          CONTAINER& container)
  {
    // WARNING: pay attention to iterator/pointer invalidation
    auto it_function = map_string_function.find(var_name);
    if (it_function != map_string_function.end()) {
      container.emplace_back(var_name, it_function->second, T());
    }
  }

  template<typename T, typename MAPSTRINGFUNCTION, typename CONTAINER>
  void create_structure(const std::set<std::string>& vars,
                        const MAPSTRINGFUNCTION& map_string_function,
                        CONTAINER& container)
  {
    // WARNING: pay attention to iterator/pointer invalidation:
    // first populate container, then access to pointers. Do not mix!

    for (const auto& var_name : vars) {
      search_var_and_add<float>(var_name, map_string_function, container);
    }

    for (auto& it: container) {
      ATH_MSG_DEBUG("creating branch " << std::get<0>(it) << " at " << &std::get<2>(it));
      const std::string root_type_name = detail::ROOT_type_name<T>();
      //if (not root_type_name.empty()) {
      //  Branch((std::get<0>(it)).c_str(), &std::get<2>(it), (std::get<0>(it) + "/" + root_type_name).c_str());
      //}
      //else {
      //  ATH_MSG_WARNING("type of " << std::get<0>(it) << " is not int/float/bool");
        Branch((std::get<0>(it)).c_str(), &std::get<2>(it));
      //}
    }
  }
protected:
  typedef std::tuple<std::string, std::function<float(const xAOD::CaloCluster&)>, float> FloatElement; //!
  std::vector<FloatElement> m_functions_float_from_calo_cluster; //!
  typedef std::tuple<std::string, std::function<float(const xAOD::Egamma&)>, float> FloatElementParticle; //!
  std::vector<FloatElementParticle> m_functions_float_from_particle; //!
public:
  template<typename T> const T& get_ref(const std::string& var_name) const
  {
    const T* ptr = get_ptr<T>(var_name);
    if (!ptr) { throw std::runtime_error("var " + var_name + " not present"); }
    return *ptr;
  }
//#ifdef ROOTCORE
  //ClassDef(egammaMVATreeEgamma, 0)
//#endif
};

template<> const float* egammaMVATreeEgamma::get_ptr<float>(const std::string&) const;
template<> const int* egammaMVATreeEgamma::get_ptr<int>(const std::string&) const;

class egammaMVATreePhoton : public egammaMVATreeEgamma
{
public:
  egammaMVATreePhoton(const std::string& name,
                      const std::set<std::string>& variables,
                      bool use_layer_corrected=true,
                      bool force_conversion_to_zero_when_null_photon=false);
  void update(const xAOD::Photon* photon);
  void update(const xAOD::Photon* photon, const xAOD::CaloCluster* cluster);

private:
  bool m_force_conversion_to_zero_when_null_photon;
  template<typename T> const T* get_ptr(const std::string& var_name) const {
     return egammaMVATreeEgamma::get_ptr<T>(var_name);
  }
  void init_variables(const std::set<std::string>& variables);
  typedef std::tuple<std::string, std::function<float(const xAOD::Photon&)>, float> FloatElement; //!
  typedef std::tuple<std::string, std::function<int(const xAOD::Photon&)>, int> IntElement; //!
  typedef std::tuple<std::string, std::function<float(const egammaMVAFunctions::ConversionHelper&)>, float> FloatElementConv; //!
  typedef std::tuple<std::string, std::function<int(const egammaMVAFunctions::ConversionHelper&)>, int> IntElementConv; //!
  std::vector<FloatElement> m_functions_float_from_photon; //!
  std::vector<IntElement> m_functions_int_from_photon; //!
  std::vector<FloatElementConv> m_functions_float_from_ConversionHelper; //!
  std::vector<IntElementConv> m_functions_int_from_ConversionHelper; //!
public:
  template<typename T> const T& get_ref(const std::string& var_name) const
  {
    const T* ptr = get_ptr<T>(var_name);
    if (!ptr) { throw std::runtime_error("var " + var_name + " not present"); }
    return *ptr;
  }
//#ifdef ROOTCORE
    //ClassDef(egammaMVATreePhoton, 0)
//#endif
};

template<> const float* egammaMVATreePhoton::get_ptr<float>(const std::string&) const;
template<> const int* egammaMVATreePhoton::get_ptr<int>(const std::string&) const;

class egammaMVATreeElectron : public egammaMVATreeEgamma
{
public:
  egammaMVATreeElectron(const std::string& name,
                        const std::set<std::string>& variables,
                        bool use_layer_corrected=true);
  void update(const xAOD::Electron* electron);
  void update(const xAOD::Electron* electron, const xAOD::CaloCluster* cluster);
private:
  template<typename T> const T* get_ptr(const std::string& var_name) const {
     return egammaMVATreeEgamma::get_ptr<T>(var_name);
  }
  void init_variables(const std::set<std::string>& variables);
  typedef std::tuple<std::string, std::function<float(const xAOD::Electron&)>, float> FloatElement; //!
  typedef std::tuple<std::string, std::function<int(const xAOD::Electron&)>, int> IntElement; //!
  std::vector<FloatElement> m_functions_float_from_electron; //!
  std::vector<IntElement> m_functions_int_from_electron; //!
public:
  template<typename T> const T& get_ref(const std::string& var_name) const
  {
    const T* ptr = get_ptr<T>(var_name);
    if (!ptr) { throw std::runtime_error("var " + var_name + " not present"); }
    return *ptr;
  }
//#ifdef ROOTCORE
  //ClassDef(egammaMVATreeElectron, 0)
//#endif
};

template<> const float* egammaMVATreePhoton::get_ptr<float>(const std::string&) const;
template<> const int* egammaMVATreePhoton::get_ptr<int>(const std::string&) const;


#endif
