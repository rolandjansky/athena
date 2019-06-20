/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/*
 *      Fast LVL2 version by D.Emeliyanov@rl.ac.uk
 *
 *      based on original code by 
 *	Kondo Gnanvo (QMUL London)
 *	Kondo.Gnanvo@cern.ch
 *	
 *
 */ 
 
#ifndef TRIGONLINESPACEPOINTTOOL_FASTSCTRODDECODER_H
#define TRIGONLINESPACEPOINTTOOL_FASTSCTRODDECODER_H

#include <stdint.h>
#include <vector>
#include <string>

#include "AthenaBaseComps/AthAlgTool.h"

#include "eformat/Version.h"

#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/IdContext.h" 
#include "ByteStreamData/RawEvent.h" 

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <deque>

class FastSCT_Clusterization;
class ISCT_CablingTool;

class FastSCT_RodDecoder : public AthAlgTool 
{
public: 

  // constructor
   FastSCT_RodDecoder(const std::string& type, const std::string& name, const IInterface* parent ) ;

  static const InterfaceID& interfaceID( ) ;

  // destructor 
  virtual ~FastSCT_RodDecoder(); 

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  bool fillCollections(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* rob,
                       uint32_t robid, ISCT_CablingTool* cablingTool, 
		       std::vector<bool>&, FastSCT_Clusterization* );

  int addNewStrip(int strip, int, uint32_t onlineId, int ERRORS, float errorHit[20],
		  std::vector<bool>&, const InDetDD::SiDetectorElementCollection* elements);

 private:
  const SCT_ID* m_sct_id; 
  IdContext m_cntx_sct; 
  FastSCT_Clusterization* m_pClusterization;
  ISCT_CablingTool* m_cablingTool;
  ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retreive Lorentz angle"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
};

#endif
