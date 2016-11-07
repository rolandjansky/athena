/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/IdContext.h" 
#include "ByteStreamData/RawEvent.h" 

#include "InDetReadoutGeometry/SCT_DetectorManager.h"  
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include <deque>

class FastSCT_Clusterization;

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
                       uint32_t robid, ISCT_CablingSvc* cablingSvc, 
		       std::vector<bool>&, FastSCT_Clusterization* );

  int addNewStrip(int strip, int, uint32_t onlineId, int ERRORS, float errorHit[20],
		  std::vector<bool>&);

 private:
  const InDetDD::SCT_DetectorManager *m_indet_mgr; 
  const SCT_ID* m_sct_id; 
  IdContext m_cntx_sct; 
  FastSCT_Clusterization* m_pClusterization;
  ISCT_CablingSvc* m_cablingSvc;
};

#endif
