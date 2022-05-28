/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_TOOL_INFORMATIONSTORE_H
#define PANTAUALGS_TOOL_INFORMATIONSTORE_H

//! C++
#include <map>
#include <string>

//! ASG
#include "AsgTools/AsgTool.h"

//! PanTau
#include "PanTauAlgs/ITool_InformationStore.h"


namespace PanTau {

  /** @class Tool_InformationStore
      Tool to store information needed in PanTau Algorithms
      @author Christian Limbach (limbach@physik.uni-bonn.de)
  */
  class Tool_InformationStore : public asg::AsgTool, virtual public PanTau::ITool_InformationStore {
    
    ASG_TOOL_CLASS1(Tool_InformationStore, PanTau::ITool_InformationStore)
    
      public:

#ifdef XAOD_ANALYSIS
    inline void setMapString( MapString&v ){ m_Infos_String = v; }
    inline void setMapVecString( MapVecString&v ){ m_Infos_VecString  = v; }
    inline void setMapInt( MapInt &v ){ m_Infos_Int = v; }
    inline void setMapDouble( MapDouble &v ){  m_Infos_Double = v; }
    inline void setMapVecDouble( MapVecDouble &v ){  m_Infos_VecDouble = v; }
#endif

    Tool_InformationStore(const std::string &name);
    virtual ~Tool_InformationStore ();
        
    virtual void ABRDefaultInit();
    virtual StatusCode initialize();
        
    virtual StatusCode getInfo_Int(const std::string& varName, int& value) const;
    virtual StatusCode getInfo_Double(const std::string& varName, double& value) const;
    virtual StatusCode getInfo_VecDouble(const std::string& varName, std::vector<double>& value) const;
    virtual StatusCode getInfo_String(const std::string& varName, std::string& value) const;
    virtual StatusCode getInfo_VecString(const std::string& varName, std::vector<std::string>& value) const;
        
       
  private:
        
    //! named strings, ints etc... for configuration
    MapString       m_Infos_String;
    MapVecString    m_Infos_VecString;
    MapInt          m_Infos_Int;
    MapDouble       m_Infos_Double;
    MapVecDouble    m_Infos_VecDouble;
        
    //!other information
    // input containers
    std::string  m_Name_Container_eflowRec;
    std::string  m_Name_Container_TauRec;
    std::string  m_Name_Container_Tracks;

    bool m_init=false;

  public:
    inline bool isInitialized(){return m_init;}        
       
  };


} // end of namespace PanTau
#endif // PANTAUALGS_TOOL_INFORMATIONSTORE_H
