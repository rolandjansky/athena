//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETATTRIBUTEHISTOS_H
#define JETMONITORING_JETATTRIBUTEHISTOS_H


#include "GaudiKernel/ToolHandle.h"

#include "JetMonitoring/JetHistoBase.h"
#include "JetMonitoring/HistoDefinitionTool.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

namespace jet {
  class HistoFiller;
}
class JetAttributeHisto : public JetHistoBase {
  ASG_TOOL_CLASS0(JetAttributeHisto);
  
public:
  //typedef SG::AuxElement::Accessor< T > accessor_t;
  
  JetAttributeHisto(const std::string &t);
  virtual ~JetAttributeHisto();

  virtual StatusCode initialize() ;
  virtual int buildHistos();
  virtual int fillHistosFromJet(const xAOD::Jet &j);
  
protected:
  /// Rename an histo according to the tool's path. redefine here to take m_selectedIndex into account.
  virtual void renameAndRegister(TH1* h, const std::string & subD="", Interval_t i=all);

  ToolHandle<HistoDefinitionTool> m_histoDef;
  std::vector<std::string> m_attTypes;
  std::vector<std::string> m_attNames;
  std::vector<bool> m_attGeV;

  int m_selectedIndex;
  bool m_doTProfile;
  jet::HistoFiller* m_histoFiller;
};


///////////////////////////////////////////
///
/// \class JetAttributeHisto
///
///  Templated base class to draw simple histos from generic jet attributes.
////////////////////////////////////////////
// template<typename T>
// class JetAttributeHistoBase : public JetHistoBase {
//   ASG_TOOL_CLASS0(JetAttributeHistoBase);
  
// public:
//   typedef SG::AuxElement::Accessor< T > accessor_t;
  
//   JetAttributeHistoBase(const std::string &t);
//   ~JetAttributeHistoBase(){}

//   virtual StatusCode initialize() ;
  
// protected:

//   ToolHandle<HistoDefinitionTool> m_histoDef;
//   accessor_t *m_accessor1;
//   accessor_t *m_accessor2; // used only for 2D hist filling.
  
// };

// ///////////////////////////////////////////
// ///
// /// \class JetAttributeHisto
// ///
// ///  Concrete class to draw simple 1D histos
// ////////////////////////////////////////////
// template<typename T>
// class JetAttributeHisto : public JetAttributeHistoBase<T> {
//   ASG_TOOL_CLASS0(JetAttributeHisto);
// public:
//   JetAttributeHisto(const std::string &t) : JetAttributeHistoBase<T>(t){}

//   virtual int buildHistos();
//   virtual int fillHistosFromJet(const xAOD::Jet &j);
// };

// //////////////////////////////////////////////////////
// ///
// /// \class JetVecAttributeHisto
// ///
// /// Derived JetAttributeHisto for vector attributes.
// /// It provides an option to plot only one selected index
// ///  -> This allows to draw JVF[0] 
// ///////////////////////////////////////////////////
// template<typename T>
// class JetVecAttributeHisto : public JetAttributeHistoBase<std::vector<T> > {
//   ASG_TOOL_CLASS0(JetVecAttributeHisto);
// public:
//   JetVecAttributeHisto(const std::string &t);

//   // redefine the filling method
//   virtual int buildHistos();
//   virtual int fillHistosFromJet(const xAOD::Jet &j);
// protected:

//   /// Rename an histo according to the tool's path. redefine here to take m_selectedInd into account.
//   virtual void renameAndRegister(TH1* h, const std::string & subD="");
  
//   int m_selectedInd; // if >-1 only index m_selectedInd will be histogrammed.
// };



// ///////////////////////////////////////////
// ///
// /// \class JetAttributeHisto2D
// ///
// ///  Templated class to draw 2D histos from generic  attributes of a same jet.
// ///  (could be moved to an separate header)
// ////////////////////////////////////////////
// template<typename T>
// class JetAttributeHisto2D : public JetAttributeHistoBase<T > {
//   ASG_TOOL_CLASS0(JetAttributeHisto2D);
// public:
//   typedef typename JetAttributeHisto<T>::accessor_t accessor_t;

//   JetAttributeHisto2D(const std::string &t);
  
//   virtual int buildHistos();
//   virtual int fillHistosFromJet(const xAOD::Jet &j);

// protected:
//   bool m_doTProfile;
// };



// #include "JetAttributeHisto.icc"

// // explicit instantiation for dictionnary generation.
// template class JetAttributeHisto<int> ;
// template class JetAttributeHisto<float> ;
// template class JetAttributeHisto2D<float> ;

// template class JetVecAttributeHisto<float> ;

// typedef JetAttributeHisto<int> JetIntAttributeHisto;
// typedef JetAttributeHisto<float> JetFloatAttributeHisto;
// typedef JetAttributeHisto2D<float> JetFloatAttributeHisto2D;
// typedef JetVecAttributeHisto<float> JetVecFloatAttributeHisto;





#endif
