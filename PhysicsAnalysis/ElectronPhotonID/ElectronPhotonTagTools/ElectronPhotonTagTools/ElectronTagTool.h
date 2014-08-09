/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONTAGTOOL_H 
#define ELECTRONTAGTOOL_H 

/*****************************************************************************
Name    : ElectronTagTool.h
Package : offline/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the Electron Tag objects - ElectronTagCollection.h. 
	  The Electron Tag fragment is built here
*****************************************************************************/

#include "GaudiKernel/AlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

class StoreGateSvc;
class ElectronTagCollection;

/** Interface ID for ElectronTagTool*/  
static const InterfaceID IID_ElectronTagTool("ElectronTagTool", 1, 0);

class ElectronTagTool : public AlgTool {

public:
  
  /** Standard Constructor */
  ElectronTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_ElectronTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int& max);
  virtual StatusCode execute(TagFragmentCollection& electronTagCol, const int& max);
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~ElectronTagTool( );

private:

  /** Properties */
  std::string m_containerName;
  double m_cut_Et;
  std::vector<float> m_caloisocutvalues;
  std::vector<float> m_trackisocutvalues;

  /** the attribute names */
  std::vector<std::string> m_ptStr;
  std::vector<std::string> m_etaStr;
  std::vector<std::string> m_phiStr;
  std::vector<std::string> m_tightStr;
  std::vector<std::string> m_fwdStr;
  std::vector<std::string> m_isoStr;

  /** Event Store */
  StoreGateSvc* m_storeGate;

 };

#endif // ELECTRONTAGTOOL_H


