/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//Author: Lianyou Shan <lianyou.shan@cern.ch>
// -*- c++ -*-
// InDetSecVtxTrackCut.h
// Declarations of objects to perform cuts for InDetTrackSelectionTool

#ifndef INDETSECVTXTRACKSELECTIONTOOL_INDETTRACKCUT_H
#define INDETSECVTXTRACKSELECTIONTOOL_INDETTRACKCUT_H

#include "InDetSecVtxTrackAccessor.h"
//#include "InDetSecVtxTrackSelectionTool/InDetSecVtxTrackSelectionTool.h"

#include <map>
#include <array>

namespace InDet {

//  class InDetTrackSelectionTool;
  class InDetSecVtxTrackSelectionTool;

  // ---------------- SecVtxTrackCut ----------------

  class SecVtxTrackCut : public asg::AsgMessaging {
    // abstract interface class for all cuts.
  public:
    SecVtxTrackCut(InDetSecVtxTrackSelectionTool*);
    virtual ~SecVtxTrackCut() = 0;
    // initialize should find the accessor(s) or create them if they don't exist
    virtual StatusCode initialize();
    virtual bool result() const = 0; // whether the cut passes
    operator bool() const {return this->result();}
  protected:
    // a function that gets the accessor labelled by "name", adding it if it needs to.
    template <class AccessorType>
    StatusCode getAccessor(const std::string& name, std::shared_ptr<AccessorType>& accessor);
  private:
    // a pointer to the tool's accessors so we can modify it in initialize()
    std::unordered_map< std::string, std::shared_ptr<SecVtxTrackAccessor> >* m_trackAccessors;
    // a pointer to the tool so that we can create the accessors when need be, as they inherit from AsgMessaging and need a pointer to the tool
    InDetSecVtxTrackSelectionTool* m_selectionTool;
  }; // class SecVtxTrackCut

class D0minCut : public virtual SecVtxTrackCut {
  public:
    D0minCut(InDetSecVtxTrackSelectionTool*, Double_t min=-99.);
    void setMinValue(Double_t min) { m_minValue = min;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    Double_t m_minValue;
    std::shared_ptr< svParamAccessor<0> > m_paramAccessor;
  }; // class D0Cut

  template <size_t N>
  class FuncSummaryValueCut : public virtual SecVtxTrackCut {
  public:
    FuncSummaryValueCut( InDetSecVtxTrackSelectionTool*, const std::array<xAOD::SummaryType,N>&&);
    void setFunction(std::function<bool(const std::array<uint8_t,N>&)> func) {m_func = func;}
    virtual StatusCode initialize();
    virtual bool result() const;
  private:
    std::function<bool(const std::array<uint8_t,N>&)> m_func;
    std::array< xAOD::SummaryType, N> m_summaryTypes;
    std::array< std::shared_ptr<SummaryAccessor>,N > m_summaryAccessors;
    mutable std::array<uint8_t,N> m_results; // member variable to store the results
  }; // class FuncSummaryValueCut

} // namespace InDet

template <class AccessorType>
StatusCode InDet::SecVtxTrackCut::getAccessor(const std::string& accessorName,
					std::shared_ptr<AccessorType>& accessor)
{
  // this function looks for the accessor indicated by "name" in the tool's
  // track accessors and assigns "accessor" to it if it exists, creating
  // the accessor if it does not.
  std::unordered_map< std::string, std::shared_ptr<SecVtxTrackAccessor> >::const_iterator it =
    std::find_if(m_trackAccessors->begin(), m_trackAccessors->end(),
		 [&] (std::pair< std::string, std::shared_ptr<SecVtxTrackAccessor> > acc)
		 {return acc.first == accessorName;} );
  if (it == m_trackAccessors->end()) {
    // if we can't find the accessor, add one to the tool
    if (!m_selectionTool) {
      ATH_MSG_ERROR( "Must initialize SecVtxTrackCut with pointer to the selection tool before adding accessors." );
      return StatusCode::FAILURE;
    }
    accessor = std::make_shared<AccessorType>(m_selectionTool);
    (*m_trackAccessors)[accessorName] = accessor;
    ATH_MSG_DEBUG( "Adding accessor " << accessorName );
  } else {
    accessor = std::dynamic_pointer_cast<AccessorType>(it->second);
    if (accessor==nullptr) {
      ATH_MSG_ERROR( "Logic error: could not cast accessor to type " << typeid(AccessorType).name() );
      return StatusCode::FAILURE;
    }
  }

  if (!accessor) {
    // this could happen if the accessor indicated by "name" is not of type "AccessorType"
    ATH_MSG_ERROR( "Could not instantiate "<< accessorName <<
		   " accessor. Pointer to accessor is to type " <<
		   typeid(AccessorType).name() << ". Is this correct?" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


#endif // INDETTRACKSELECTIONTOOL_INDETTRACKCUT_H
