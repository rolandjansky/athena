/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <inttypes.h>
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "TString.h"
#include "MuonCondSvc/TriggerCoolSvc.h"

namespace MuonCalib{

TriggerCoolSvc::TriggerCoolSvc(const std::string& name, ISvcLocator* svc) :
  AthService(name,svc),
  p_detstore(nullptr),
  m_etafolder("/RPC/TRIGGER/CM_THR_ETA"),
  m_phifolder("/RPC/TRIGGER/CM_THR_PHI")
{
  //declare properties
  declareProperty("EtaFolder",m_etafolder);
  declareProperty("PhiFolder",m_phifolder);
}

TriggerCoolSvc::~TriggerCoolSvc() {}

const InterfaceID& TriggerCoolSvc::type() const
{
  return ITriggerCoolSvc::interfaceID();
}
  
StatusCode TriggerCoolSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (ITriggerCoolSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(ITriggerCoolSvc*)this;
  } else {
    return AthService::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode TriggerCoolSvc::initialize()
{
  ATH_MSG_INFO ("Initializing TriggerCoolSvc");
  
  // get detector store, linked to cool database by other algorithms in your
  // jobOptions file.
  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    ATH_MSG_FATAL( "Detector store not found"  );
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO( "using folder phi ******************** " << m_phifolder );
  ATH_MSG_INFO( "using folder eta ********************" << m_etafolder );
  

  // StoreGateSvc* detStore= 0;
  //StatusCode sc = serviceLocator()->service("DetectorStore",detStore);
  
 
  
  return StatusCode::SUCCESS;
}
  
StatusCode TriggerCoolSvc::finalize()
{
  ATH_MSG_DEBUG( "in finalize()"  );
  return StatusCode::SUCCESS;
}




StatusCode TriggerCoolSvc::writeToDBEta(const std::string& etafolder, const std::string& filename_CM,const std::string& filename_Th0,
					const int chan, const std::string& sequence,const std::string& info) const{
  
  // this writes the contents of theEntries in the db
  ATH_MSG_INFO( "using folder eta ********************" << m_phifolder );

  // reading files to store in string
 
  std::string sdata_Th0_new;

  FILE* f = fopen (filename_Th0.c_str(),"rb");
  if (f != nullptr)   {
    fseek (f, 0L, SEEK_END);
    unsigned int size = ftell (f);
    fseek (f, 0L, SEEK_SET);
    ATH_MSG_INFO( "Input Th0 file size is " << size  );

    std::vector<char> sbuf(size);

    if (sbuf.size()>= size) {
       size_t readbytes = 0;
       readbytes = fread(&sbuf[0],size,1,f);
       if(readbytes == 0)
         ATH_MSG_ERROR( "Empty file read!" << size  );
    }
    
    fclose (f);
    
    std::string sdata_Th0(sbuf.begin(),sbuf.begin()+size);
    
    sdata_Th0_new=sdata_Th0;
    
  }
  
  CondAttrListCollection* atrc=nullptr;
  if (!p_detstore->contains<CondAttrListCollection>(etafolder)) {
    ATH_MSG_DEBUG( "Creating new CondAttrListCollection for folder "
                   << etafolder  );
    CondAttrListCollection* atrc=new CondAttrListCollection(true);
    if (StatusCode::SUCCESS!=p_detstore->record(atrc,etafolder)) {
      ATH_MSG_ERROR( "Could not create CondAttrListCollection " <<
                     etafolder  );
      return StatusCode::FAILURE;
    }
  }
  
  // do const cast here so we can add to already exisiting collections
  const CondAttrListCollection* catrc=nullptr;
  ATH_MSG_DEBUG( "Attempting to retrieve collection (const)"  );
  if (StatusCode::SUCCESS!=p_detstore->retrieve(catrc,etafolder)) {
    ATH_MSG_ERROR( "Could not retrieve CondAttrListCollection " <<
                   etafolder  );
    return StatusCode::FAILURE;
  }
  atrc=const_cast<CondAttrListCollection*>(catrc);
  if (atrc==nullptr) {
    ATH_MSG_ERROR( "Could not retrieve non-const pointer to atrc"  );
    return StatusCode::FAILURE;
  }

  // check channel zero is not being requested
  if (chan==0) {
    ATH_MSG_ERROR( "Channel 0 cannot be used"  );
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_DEBUG( "About to create AttributeListSpecification"  );
  
  coral::AttributeListSpecification* aspec=nullptr;
  aspec=new coral::AttributeListSpecification();
 
  aspec->extend("CM_File","string");
  aspec->extend("Th0","string");
  aspec->extend("Sequence_Th","string");
  aspec->extend("Additional_Info","string");
 
  AthenaAttributeList alist(*aspec);
  alist["CM_File"].setValue(filename_CM);
  alist["Th0"].setValue(sdata_Th0_new);
  alist["Sequence_Th"].setValue(sequence);
  alist["Additional_Info"].setValue(info);
  CondAttrListCollection::ChanNum channum=chan;

  std::ostringstream atrstring;
  alist.print(atrstring);
  ATH_MSG_DEBUG( "About to add channel to: " << atrc  );
  atrc->add(channum,alist);
  ATH_MSG_DEBUG( " creation folder"  );
  return StatusCode::SUCCESS;

  
}

StatusCode TriggerCoolSvc::writeToDBPhi(const std::string& phifolder, const std::string& filename_CM,const std::string& filename_Th0,
				  const int chan,const std::string& info) const{
  
  std::string sdata_Th0_new;
    
  FILE* f = fopen (filename_Th0.c_str(),"rb");
  if (f != nullptr)   {
    fseek (f, 0L, SEEK_END);
    int size = ftell (f);
    fseek (f, 0L, SEEK_SET);
    ATH_MSG_INFO( "Input file size is " << size  );
    std::vector<char> sbuf(size);
    size_t readbytes = 0;
    readbytes = fread(&sbuf[0],size,1,f);
    if(readbytes == 0)
      ATH_MSG_ERROR( "Empty file read!" << size  );
    fclose (f);
    
    std::string sdata_Th0(sbuf.begin(),sbuf.begin()+size);
    
    sdata_Th0_new=sdata_Th0;
    
  }
 
  //ATH_MSG_DEBUG( " Data++++++" << sdata_Th0_new );
  //std::cout<< "dentro writeToDBPhi ************************88 " <<std::endl;
  //ATH_MSG_DEBUG( " folder" << phifolder );
  //ATH_MSG_DEBUG( " filename " << filename_CM );
  CondAttrListCollection* atrc=nullptr;
  if (!p_detstore->contains<CondAttrListCollection>(phifolder)) {
    ATH_MSG_DEBUG( "Creating new CondAttrListCollection for folder "
                   << phifolder  );
    CondAttrListCollection* atrc=new CondAttrListCollection(true);
    if (StatusCode::SUCCESS!=p_detstore->record(atrc,phifolder)) {
      ATH_MSG_ERROR( "Could not create CondAttrListCollection " <<
                     phifolder  );
      return StatusCode::FAILURE;
    }
  }
  
  // do const cast here so we can add to already exisiting collections
  const CondAttrListCollection* catrc=nullptr;
  ATH_MSG_DEBUG( "Attempting to retrieve collection (const)"  );
  if (StatusCode::SUCCESS!=p_detstore->retrieve(catrc,phifolder)) {
    ATH_MSG_ERROR( "Could not retrieve CondAttrListCollection " <<
                   phifolder  );
    return StatusCode::FAILURE;
  }
  atrc=const_cast<CondAttrListCollection*>(catrc);
  if (atrc==nullptr) {
    ATH_MSG_ERROR( "Could not retrieve non-const pointer to atrc"  );
    return StatusCode::FAILURE;
  }

  // check channel zero is not being requested
  if (chan==0) {
    ATH_MSG_ERROR( "Channel 0 cannot be used"  );
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_DEBUG( "About to create AttributeListSpecification"  );
  
  coral::AttributeListSpecification* aspec=nullptr;
  aspec=new coral::AttributeListSpecification();
 
  aspec->extend("CM_File","string");
  aspec->extend("Th0","string");
  aspec->extend("Additional_Info","string");
  
  //std::cout<< "file ************************ " <<sdata_Th0_new<<std::endl;
  AthenaAttributeList alist(*aspec);
  alist["CM_File"].setValue(filename_CM);
  alist["Th0"].setValue(sdata_Th0_new);
  alist["Additional_Info"].setValue(info);
  CondAttrListCollection::ChanNum channum=chan;

  std::ostringstream atrstring;
  alist.print(atrstring);
  ATH_MSG_DEBUG( "About to add channel to: " << atrc  );
  std::cout<< "channel ************************ " <<chan<<std::endl;
  atrc->add(channum,alist);
  ATH_MSG_DEBUG( " creation folder"  );
  return StatusCode::SUCCESS;

  
}

StatusCode TriggerCoolSvc::getData(const std::string& phifolder, const int chan) const {
  MsgStream log(msgSvc(),name());
  const CondAttrListCollection* atrc;
  std::string file="";
  std::string th0="";
  std::string info="";
  if (StatusCode::SUCCESS!=p_detstore->retrieve(atrc,phifolder)) {
    ATH_MSG_ERROR( "getData failed for folder " << phifolder << " channel "
                   << chan  );
    return StatusCode::FAILURE;
  }
  CondAttrListCollection::ChanNum channum=chan;
  CondAttrListCollection::const_iterator itr=atrc->chanAttrListPair(channum);
  if (itr!=atrc->end()) {
    const coral::AttributeList& atr=itr->second;
    file=atr["CM_File"].data<std::string>();
    th0=atr["Th0"].data<std::string>();
    info=atr["Additional_Info"].data<std::string>();
    
    if (log.level() < MSG::INFO) {
      std::ostringstream atrstring;
      atr.toOutputStream(atrstring);
      ATH_MSG_DEBUG( "read Channum " << channum << " atrlist: " << atrstring.str()  );
    } else {
      ATH_MSG_ERROR( "Invalid channel number"  );
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}



}
