/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCABLING_LARHVCABLINGTOOL_H
#define LARCABLING_LARHVCABLINGTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "Identifier/HWIdentifier.h"

#include <vector>
#include <map>

//class LArHVCellID;
//class LArHVDetectorID;
//class LArHVElectrodeID;
//class LArHVSpecialElectrodeID;
class LArElectrodeID;
class LArHVLineID;

class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class CaloCell_ID;

//class CondAttrListCollection;
class AthenaAttributeList;
static const InterfaceID IID_LArHVCablingTool("LArHVCablingTool", 1 , 0); 

/**  
   @brief Tool for mapping HV from HVdetectornal electrode

  date of creation : 07/09/2004 
  @author : lucotte@in2p3.fr 
*/


class LArHVCablingTool : public AthAlgTool {
  public:
  
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_LArHVCablingTool; }
  
  // constructor
  LArHVCablingTool( const std::string& type, const std::string& name, const IInterface* parent ) ;
  
  // Destructor
  virtual ~LArHVCablingTool(){} 
  
  StatusCode initialize( );
  
  StatusCode finalize( );
    
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%% NEW INTERFACE FOR HIGH VOLTAGE CODING %%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  /** Return the LArHVLineID corresponding to a given LArElectrodeId

   */ 
  const HWIdentifier getLArHVLineID(HWIdentifier& electrodeId);

  // Returns a vector<electrodeID> given an input offlineID
  //-------------------------------------------------------
  void getElectrodeInCell(const Identifier& offId, std::vector<HWIdentifier> &elecId );

  /** Returns a vector<HVLineID> given an input offlineID
   */
  void getHVLineInCell( const Identifier& offId, std::vector<HWIdentifier> &hvlineId);


  /** Return a vector of LArElectrodeID corresponding to a given LArHVLineID
  */
  const std::vector<HWIdentifier>& getLArElectrodeIDvec(HWIdentifier& hvlineId);


  /** returns the *Module* of a given offId
   */
  int getCellModule(const Identifier& offId );
  int getCellEtaSector(const Identifier& offId );
  int getCellPhiSector(const Identifier& offId );
  std::pair<int,int> getCellElectrodeMinMax(const Identifier& offId);


  /** initialize HV map
   */
  int fillHVMap();

  virtual StatusCode iovCallback(IOVSVC_CALLBACK_ARGS);


  /*
  // Get Module index as a char 
  virtual std::string getModChar( int mod );
  // CheckLArHVCellID values
  virtual std::string checkLArHVCellIDValues(int l_partition, int l_canline, 
					     int l_cannode, int l_hvline );

  // CheckLArHVDetectorID values
  virtual std::string checkLArHVDetectorIDValues(int l_det, int l_mod, int l_hvphi, 
  						 int l_hveta, int l_gaps );
  // CheckLArHVElectrodeID values
  virtual std::string checkLArHVElectrodeIDValues(int l_min, int l_max, int l_nele  );

  // CheckLArHVElectrodeID values
  virtual std::string checkLArHVSpecialElectrodeIDValues(int l_nspe, int l_elec1, int l_elec2  );
  */  
  private:
  /*
  // for LArHVCellID
  static const unsigned int m_partition;
  static const unsigned int m_canline;
  static const unsigned int m_cannode;
  static const unsigned int m_hvline;

  // for LArHVDetectorID
  static const unsigned int m_detector;
  static const unsigned int m_module;
  static const unsigned int m_hvphi;
  static const unsigned int m_hveta;
  static const unsigned int m_gaps;

  // for LArHVElectrodeID
  static const unsigned int m_elemin;
  static const unsigned int m_elemax;
  static const unsigned int m_nele;

  // for LArHVSpecialElectrodeID
  static const unsigned int m_nspecial;
  static const unsigned int m_elec1;
  static const unsigned int m_elec2;
  */
  // =========================
  // Data Members
  // =========================
  const CaloCell_ID*     m_caloHelper ;
  const LArEM_ID   *     m_emHelper ;
  const LArHEC_ID  *     m_hecHelper ;
  const LArFCAL_ID *     m_fcalHelper ;
  // AL--> added for new interface ..
  const LArHVLineID *    m_hvlineHelper ;
  const LArElectrodeID*  m_electrodeHelper ;

  //std::vector<HWIdentifier> m_hvline_electrode_vec;
  //std::vector<HWIdentifier> m_electrodeIDvec;
  std::map<HWIdentifier,HWIdentifier> m_electrode_hvline_map;
  std::map<HWIdentifier,std::vector<HWIdentifier> > m_hvline_electrode_map;
  bool m_initHVMap;
  //<-- AL

 private:
  std::string m_mappingFolder;
  const DataHandle<AthenaAttributeList> m_attrList;
};
  	  		  		 
#endif // LARCABLING_LARHVCABLINGTOOL_H
