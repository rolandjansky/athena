/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelMapTestAlg.h"
#include "PixelConditionsServices/ISpecialPixelMapSvc.h"
#include "PixelConditionsSummarySvc.h"

// Athena includes
#include "AthenaKernel/IOVSvcDefs.h" 

// Gaudi includes
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/GaudiException.h" 

// geometry
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

// CoralDB
#include "CoralDB/CoralDB.h"
#include "CoralDB/Alias.h"
#include "CoralDB/Connection.h"

// STL
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ios>
#include <sstream>

static bool isIBL(false); 
static bool isITK(false);

PixelMapTestAlg::PixelMapTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_specialPixelMapSvc("SpecialPixelMapSvc", name),
  m_pixelConditionsSummarySvc("PixelConditionsSummarySvc", name),
  m_useSummarySvc(true),
  m_writeTextfile(false),
  m_dummy(false),
  m_overlay(false),
  m_pixelID(0),
  m_pixman(0)
{
  declareProperty("UseSummarySvc", m_useSummarySvc, "switch for use of PixelConditionsSummarySvc"); 
  declareProperty("SpecialPixelMapSvc", m_specialPixelMapSvc); 
  declareProperty("WriteTextFile", m_writeTextfile);
  declareProperty("MakeDummy",m_dummy);
  declareProperty("MakeOverlay", m_overlay);
}

PixelMapTestAlg::~PixelMapTestAlg(){}


StatusCode PixelMapTestAlg::initialize(){
  
  ATH_MSG_INFO( "Initializing PixelMapTestAlg" );
   
  StatusCode sc = m_specialPixelMapSvc.retrieve();
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve SpecialPixelMapSvc" );
    return StatusCode::FAILURE;
  } 

  if(m_useSummarySvc){
    sc = m_pixelConditionsSummarySvc.retrieve();
    if( !sc.isSuccess() ){
      ATH_MSG_FATAL( "Unable to retrieve PixelConditionsSummarySvc" );
      return StatusCode::FAILURE;
    } 
  }
  sc = detStore()->retrieve( m_pixman, "Pixel" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel manager" );
    return StatusCode::FAILURE;
  }


  sc = detStore()->retrieve( m_pixelID, "PixelID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }
  if(m_pixelID->wafer_hash_max()>1744)isIBL = true;
  if(m_pixelID->wafer_hash_max()>3000)isITK = true;
  InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax;
  itermin = m_pixman->getDetectorElementBegin();
  itermax = m_pixman->getDetectorElementEnd();
  //
  if(m_dummy&&isITK){
    for( iter=itermin; iter !=itermax; ++iter){
      const InDetDD::SiDetectorElement* element = *iter;
      if(element !=0){
	const Identifier ident = element->identify();
	if(m_pixelID->is_pixel(ident)){  // OK this Element is included
	  const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
	  if(!design)continue;
          unsigned int mchips = design->numberOfCircuits();
	  int mrow =design->rows();
          if(mchips==8||abs(m_pixelID->barrel_ec(ident))==2||(m_pixelID->barrel_ec(ident)==0&&m_pixelID->layer_disk(ident)>0)){
	    mchips *=2; // guess numberOfCircuits()
	    mrow /=2; //number of chips in row
	  }
	  unsigned int i(0);
	  if(mrow==336)i = 1;
	  else if(mrow==672) i = 2;
	  else if(mrow==339) i = 3;
	  else if(mrow==678) i = 4;
	  std::ostringstream s;
	  s<<"["<<2<<","<<1<<","<<m_pixelID->barrel_ec(ident)<<","<<m_pixelID->layer_disk(ident)
	   <<","<<m_pixelID->phi_module(ident)<<","<<m_pixelID->eta_module(ident)<<","<<0<<"]";
	  std::string rfile ="SpecialMapSLHC/"+s.str();
	  std::ofstream* outfile = new std::ofstream(rfile.c_str());
	  for(int kv = 0; kv<(int)mchips; ++kv){
	    *outfile<<kv<<" "<<kv+i*10<<" "<<mrow/2<<" "<<3<<std::endl;
	  }
	outfile->close();
	}
      }
    }
  }
  //
  return StatusCode::SUCCESS;
}

StatusCode PixelMapTestAlg::execute(){
  
  ATH_MSG_DEBUG( "Creating conditions objects " );
  
  StatusCode sc = m_specialPixelMapSvc->create();
  
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Could not create conditions objects" );
    return(StatusCode::FAILURE);
  }

  sc = m_specialPixelMapSvc->print();

  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Could not print conditions objects" );
    return(StatusCode::FAILURE);
  }

  ATH_MSG_INFO("");
  
  if(m_useSummarySvc){
    for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i ++){
    
      Identifier moduleID = m_pixelID->wafer_id(IdentifierHash(i));
      
      unsigned int hashID = isIBL ? i:( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
	( m_pixelID->layer_disk(moduleID) << 23) + ( m_pixelID->phi_module(moduleID) << 17) +
	( (m_pixelID->eta_module(moduleID) + 6) << 13); 	
      
      ATH_MSG_INFO( "Hash " << i << " ID " << hashID << " b/ec " 
		    << m_pixelID->barrel_ec(moduleID) << " l/d " 
		    << m_pixelID->layer_disk(moduleID) << " phi " 
		    << m_pixelID->phi_module(moduleID) << " eta " 
		    << m_pixelID->eta_module(moduleID) << " isActive: " 
		    << m_pixelConditionsSummarySvc->isActive(IdentifierHash(i)) << " isGood: "
		    << m_pixelConditionsSummarySvc->isGood(IdentifierHash(i)) );
    }    
  }
   
  if(m_writeTextfile){
    const DetectorSpecialPixelMap* spm = m_specialPixelMapSvc->getPixelMap(0);
    if(spm==0){
      ATH_MSG_FATAL( "Could not find SpecialMap objects" );
      return(StatusCode::FAILURE);
    }
 
    //
    if(!isIBL){ 
      CoralDB::CoralDB* db = new CoralDB::CoralDB("oracle://ATLAS_COOLPROD/ATLAS_COOLONL_PIXEL", coral::ReadOnly, coral::Info, true);
      db->setObjectDictionaryTag("PIXEL");
      db->setConnectivityTag("PIT-ALL-V39");
      db->setAliasTag("PIT-ALL-V39");

      CoralDB::ObjectDictionaryMap objectDictionary;
      db->getObjectDictionary(objectDictionary, db->compoundTag());

      std::map<unsigned int, std::string> moduleIDMap;

      for(CoralDB::ObjectDictionaryMap::const_iterator object = objectDictionary.begin();
	  object != objectDictionary.end(); ++object){

	if((*object).second == "MODULE"){

	  std::string prodID = db->findAlias((*object).first, "PRODID");
	  //        unsigned int serialNumber = std::atoi(prodID.substr(1,6).c_str());
	  unsigned int moduleIDNumber = std::atoi(db->findAlias((*object).first, "HASHID").c_str());
	  moduleIDMap[moduleIDNumber] = prodID;
	}
      }  
      bool lfst = true;
      bool lfstA = true;
      bool lfstC = true;
      for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i ++){

	Identifier moduleID = m_pixelID->wafer_id(IdentifierHash(i));
	unsigned int hashID;
	hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
	  ( m_pixelID->layer_disk(moduleID) << 23) +
	  ( m_pixelID->phi_module(moduleID) << 17) +
	  ( (m_pixelID->eta_module(moduleID) + 6) << 13);
	std::ostringstream s;
	std::string mydir ="";
	if(m_dummy){
	  int dl = 0;
	  if(m_pixelID->barrel_ec(moduleID)==0)dl =1;
	  s<<"["<<2<<","<<1<<","<<m_pixelID->barrel_ec(moduleID)<<","<<(m_pixelID->layer_disk(moduleID)+dl)
	   <<","<<m_pixelID->phi_module(moduleID)<<","<<m_pixelID->eta_module(moduleID)<<","<<0<<"]";
	  mydir = "SpecialMapIBL3D/";
          if(m_overlay){
            mydir = "SpecialMapIBL3D_overlay/";
            lfst = false;
          }
          else{
            if(m_pixelID->barrel_ec(moduleID)==-2&&lfstA){
              lfstA = false;
              for(int iv=0; iv<3; ++iv){
                for(int jv=0; jv<4; ++jv){
		  std::ostringstream sx;
                  sx<<"["<<2<<","<<1<<","<<-4<<","<<iv<<","<<jv<<","<<0<<","<<0<<"]";
		  std::string rfilex =mydir+sx.str();
		  std::ofstream* outfilex = new std::ofstream(rfilex.c_str());
                  outfilex->close();
                }
              }
            }
            if(m_pixelID->barrel_ec(moduleID)==2&&lfstC){
              lfstC = false;
              for(int iv=0; iv<3; ++iv){
                for(int jv=0; jv<4; ++jv){
		  std::ostringstream sx;
                  sx<<"["<<2<<","<<1<<","<<4<<","<<iv<<","<<jv<<","<<0<<","<<0<<"]";
		  std::string rfilex =mydir+sx.str();
		  std::ofstream* outfilex = new std::ofstream(rfilex.c_str());
                  outfilex->close();
                }
              }
            }
	    if(m_pixelID->barrel_ec(moduleID)==0&&m_pixelID->layer_disk(moduleID)==0&&lfst){
	      lfst = false;
	      for(int iv=0; iv<14; ++iv){
		for(int jv=-10; jv<10; ++jv){
		  // int mx = 2;
		  // if(jv<-6||jv>5) mx = 1;
		  std::ostringstream sx;
		  sx<<"["<<2<<","<<1<<","<<0<<","<<0<<","<<iv<<","<<jv<<","<<0<<"]";
		  std::string rfilex =mydir+sx.str();
		  std::ofstream* outfilex = new std::ofstream(rfilex.c_str());
		  //for(int kv = 0; kv<mx; ++kv){
		  //  *outfilex<<kv<<" "<<kv*40+iv<<" "<<iv*10<<" "<<3<<std::endl;
		  //}
		  outfilex->close();
		}
	      }
	    }
	  }
	}
	else{
	  s<<moduleIDMap[hashID];
	  mydir = "SpecialMap/";
	}
	std::string rfile =mydir+s.str();
        if(m_overlay){
          if((*spm)[i]->moduleStatus()){
	    std::ofstream* outfile = new std::ofstream(rfile.c_str());
            *outfile<<(*spm)[i]->moduleStatus()<<std::endl;
            outfile->close();
          }
        }
        else{
	  std::ofstream* outfile = new std::ofstream(rfile.c_str());
	  for(ModuleSpecialPixelMap::const_iterator pixel = (*spm)[i]->begin();
	      pixel != (*spm)[i]->end(); ++pixel){
	    unsigned int pixelID = pixel->first;
	    int chip = pixelID%16;
	    pixelID /=16;
	    int column = pixelID%32;
	    pixelID /=32;
	    int stax =pixel->second;
	    if(stax&(1<<16))stax -=(1<<16); //reset bit 16 "unknown dead"
	    *outfile<<chip<<" "<<column<<" "<<pixelID<<" "<<stax<<std::endl;
	  }
	  outfile->close();
	}
      }
      delete db;
    }
    else{
      for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i ++){

        Identifier moduleID = m_pixelID->wafer_id(IdentifierHash(i));
	std::ostringstream s;
	s<<"["<<2<<","<<1<<","<<m_pixelID->barrel_ec(moduleID)<<","<<m_pixelID->layer_disk(moduleID)
	 <<","<<m_pixelID->phi_module(moduleID)<<","<<m_pixelID->eta_module(moduleID)<<","<<0<<"]";
	std::string rfile =isITK? "SpecialMapSLHC/" : "SpecialMapIBL3D/";
        if(m_overlay)rfile =isITK? "SpecialMapSLHC_overlay/" : "SpecialMapIBL3D_overlay/";
	rfile +=s.str();

        if(m_overlay){
          if((*spm)[i]->moduleStatus()){
	    std::ofstream* outfile = new std::ofstream(rfile.c_str());
            *outfile<<(*spm)[i]->moduleStatus()<<std::endl;
            outfile->close();
          }
        }
        else{
	  std::ofstream* outfile = new std::ofstream(rfile.c_str());
	  unsigned int ityp =(*spm)[i]->chipType();
	  for(ModuleSpecialPixelMap::const_iterator pixel = (*spm)[i]->begin();
	      pixel != (*spm)[i]->end(); ++pixel){  
	    unsigned int pixelID = pixel->first;
	    unsigned int chip = pixelID % 16;
	    unsigned int column = ityp==0 ? ((pixelID /16) % 32) : ((pixelID / 16) % 256);
	    unsigned int row = ityp==0 ? (pixelID / 512) : (pixelID / 4096);
	    int stax =pixel->second;
	    if(stax&(1<<16))stax -=(1<<16); // reset bit=16 "unknown dead"
	    *outfile<<chip<<" "<<column<<" "<<row<<" "<<stax<<std::endl;
	  }
	  outfile->close();
	}
      }
    }
    //
  }

  return StatusCode::SUCCESS;
}


StatusCode PixelMapTestAlg::finalize(){

  return StatusCode::SUCCESS;
}
