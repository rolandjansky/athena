/* this file is -*- C++ -*- 
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETVARTOOL_H
#define JETMONITORING_JETVARTOOL_H

#include "xAODJet/JetContainer.h"

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "JetMonitoring/JetVariable.h"


static const InterfaceID IID_IJetHistoVarTool("IJetHistoVarTool", 1 , 0); 

///////////////////////////////////////////////////////////
/// \class JetHistoVarTool
///
/// This class is a wrapper around JetVar::Variable inheriting AthAlgTool.
/// Its purpose is to allow to define a jet variable in the configuration
/// and to use it from other tools.
/// Client tools include JetHistoAttributeFiller and JetHistoSelectSort
///

class IJetHistoVarTool : virtual public IAlgTool                         
{
public:
  
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IJetHistoVarTool; }

  virtual ~IJetHistoVarTool(){}
  
  /// the value of the variable for a given Jet
  virtual float value(const xAOD::Jet &) const = 0;
  /// The actual variable represented by this tool
  virtual const JetVar::Variable * variable() const = 0;
  /// a compact description of the variable.
  virtual std::string describe() const =0;
};


class JetHistoVarTool : public AthAlgTool, virtual public IJetHistoVarTool {
public:
  JetHistoVarTool( const std::string& type,  const std::string & name ,const IInterface* parent);
  virtual ~JetHistoVarTool(){}

  virtual StatusCode initialize() ;  

  virtual float value(const xAOD::Jet &) const ;  
  virtual const JetVar::Variable * variable() const {return m_v.get();};
  virtual std::string describe() const {
    return m_name+", "+m_type+","+std::to_string(m_index);
  }
  
  
private:

  std::unique_ptr<JetVar::Variable>  m_v;

  Gaudi::Property<std::string> m_name {this,"Name", ""};
  Gaudi::Property<std::string> m_type {this,"Type", "float"};
  Gaudi::Property<int> m_index {this,"Index", -1};
  Gaudi::Property<float> m_scale {this,"Scale", 1.};
  
};


#endif
