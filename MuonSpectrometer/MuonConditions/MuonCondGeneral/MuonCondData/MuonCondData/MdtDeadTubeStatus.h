/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Monica Verducci monica.verducci@cern.ch author
// class to read the conditiond data object stored with COOL 
// need to understand where to put it (Detector Descriptions??)
#ifndef MUONCONDDATA_MDTDEADTUBESTATUS_H
#define MUONCONDDATA_MDTDEADTUBESTATUS_H

#include <vector>
#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
//#include "MuonCondCool/MdtDeadTubeStatusContainer.h"

class MdtDeadTubeStatus{

 public:
  MdtDeadTubeStatus();

  ~MdtDeadTubeStatus();

  std::string name() const; // test method
  unsigned int mlayer() const;
  unsigned int layer() const;
  unsigned int tubeid() const;
  unsigned int status() const;
  std::vector< unsigned int >  mlayerArray();
  
  void set(std::string name, int mlayer, int layer, int tubeid); // default to set deadtube directly from db
  void setstatus(std::string name, int mlayer, int layer, int tubeid, int status); // when in the db will be include all the tubes with problems...
 
  void setChamberStatus(std::string name, std::vector< unsigned int >  mlayerArray); // info from  the multilayer_list

 private:
   std::string m_name;
   unsigned int m_mlayer;
   unsigned int m_layer;
   unsigned int m_tubeid;
   unsigned int m_status; 
   std::vector<unsigned int >  m_mlayerArray;
};

#endif //MUONCONDCOOL_MDTDEADTUBESTATUS.H
