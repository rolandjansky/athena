/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONTAGTOOL_H 
#define PHOTONTAGTOOL_H 

/*****************************************************************************
Name    : PhotonTagTool.h
Package : offline/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the Photon Tag objects - PhotonTagCollection.h. 
	  The Photon Tag fragment is built here
*****************************************************************************/

#include "GaudiKernel/AlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

class StoreGateSvc;
class PhotonTagCollection;

/** Interface ID for PhotonTagTool */  
static const InterfaceID IID_PhotonTagTool("PhotonTagTool", 1, 0);

class PhotonTagTool : public AlgTool {

public:
  
  /** Standard Constructor */
  PhotonTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_PhotonTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int& max);
  virtual StatusCode execute(TagFragmentCollection& photonTagCol, const int& max); 
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~PhotonTagTool( );

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
  std::vector<std::string> m_isoStr;

  /** Event Store */
  StoreGateSvc* m_storeGate;

 };

#endif // PHOTONTAGTOOL_H


