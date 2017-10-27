/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <iostream>

#include "LArCondUtils/LArHVToolMC.h" 

#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArElectrodeID.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <iostream> 
#include <cmath> 

// constructor 
LArHVToolMC::LArHVToolMC(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_readASCII(false),
    m_larem_id(nullptr)
{
 declareInterface< ILArHVTool >( this );
 declareProperty("readASCII",m_readASCII);
}

// destructor 
LArHVToolMC::~LArHVToolMC()
{ }

// intialize 
StatusCode LArHVToolMC::initialize()
{
  ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );

  m_larem_id   = m_caloIdMgr->getEM_ID();

  const LArElectrodeID* electrodeID = nullptr;
  ATH_CHECK( detStore()->retrieve(electrodeID) );

  //Dummy implementation for MC. We assume that all electrodes had been update
  m_updatedElectrodes.reserve(electrodeID->electrodeHashMax());
  for (unsigned i=0;i<electrodeID->electrodeHashMax();++i) {
    m_updatedElectrodes.push_back(electrodeID->ElectrodeId(IdentifierHash(i)));
  }

  InitHV();
  return StatusCode::SUCCESS;
}



StatusCode LArHVToolMC::getHV(const Identifier& id,
         std::vector< HV_t > & v  ) 
{


  v.clear();

// check identifier in LAR
  if (m_larem_id->is_lar_em(id)) {
// check identifier in barrel
     if (abs(m_larem_id->barrel_ec(id))==1) {
       int sampling=m_larem_id->sampling(id);
       if (sampling>0) {
         int region=m_larem_id->region(id);
         int ieta=m_larem_id->eta(id);
         int iphi=m_larem_id->phi(id);
         int ietahv,iphi1,iphi2,ipm;
// ipm =0 for eta>0   1 for eta<0
         if (m_larem_id->barrel_ec(id)==1) ipm=0;
         else                              ipm=1;
         if (sampling==1 && region==0) {
           iphi1=iphi*16;
           iphi2=iphi1+16;
         }
         else {
           iphi1=iphi*4;
           iphi2=iphi1+4;
         }
         if (region==0) {
          if (sampling==1) ietahv=ieta/64;
          else if (sampling==2) ietahv=ieta/8;
          else  ietahv=ieta/4;
         }
         else ietahv=6;
         const double ngaps = 2.*(iphi2-iphi1);
         const double inv_ngaps = 1. / ngaps;
      
         for (int iphihv=iphi1;iphihv<iphi2;iphihv++) {
           for (int iside=0;iside<2;iside++) {
             double hv=m_hv[ipm][iphihv][ietahv][iside];
             bool found=false;
             for (unsigned int i=0;i<v.size();i++) {
                if (std::fabs(hv-v[i].hv) < 0.1) {
                   found=true; 
                   v[i].weight += inv_ngaps;
                 }
             }
             if (!found) {
               HV_t hh;
               hh.hv = hv;
               hh.weight = inv_ngaps;
               v.push_back(hh);
             }     // not already in the list
           }      // loop over gap size
         }   // loop over electrodes
       }       // in  accordion
     }      // in  barrel
  }        // in LAr

// hack to return something for other calos
  if (v.size()==0) {
     HV_t hh;
     hh.hv = 2000.;
     hh.weight = 1.;
     v.push_back(hh);
  }

  return StatusCode::SUCCESS; 
}

StatusCode LArHVToolMC::getCurrent(const Identifier& /* id */,
         std::vector< CURRENT_t > & v  ) 
{
     ATH_MSG_WARNING ( " LArHVToolMC: getCurrent not implemented " );
     CURRENT_t cu;
     cu.current=0;
     cu.weight=1.;
     v.push_back(cu);
     return StatusCode::SUCCESS; 
}

void LArHVToolMC::InitHV()
{
  bool setHandcoded=false;
  double defaultHvVal = 2000.;

  if (!m_readASCII) {
    IRDBAccessSvc *pAccessSvc;
    StatusCode status = service("RDBAccessSvc",pAccessSvc);
    if(status.isFailure())
    {
     std::cout << "LArHVToolMC::InitHV() unable to get RDBAccess Service! Use default HV values\n";
     setHandcoded = true;
    }
    const AthenaAttributeList* keys = 0;
    if (!setHandcoded) {
      if (detStore()->contains<AthenaAttributeList>("/LAR/HVBARREL/MAPINLINE")) {
       status = detStore()->retrieve(keys, "/LAR/HVBARREL/MAPINLINE");
      if(status.isFailure())
      {
        std::cout << "LArHVToolMC::InitHV() unable to retrieve AthenaAttributeList! Use default HV values\n";
        setHandcoded = true;
       }
     }
     else {
       std::cout << "LArHVToolMC::InitHV() no HV map in DetStore. Use Dedault HV values\n";
       setHandcoded = true;
     }
    }  

    std::string nodeName, tagName;
    if (!setHandcoded) {
      try
      {
       nodeName = (*keys)["HvBarrelNode"].data<std::string>();
       tagName  = (*keys)["HvBarrelTag"].data<std::string>();
      }
      catch(std::exception& e)
      {
       std::cout << e.what() << "\n";
       std::cout << "   Use default HV values " << std::endl;
       setHandcoded = true;
      }
    }

    if(!setHandcoded)
    {
     // retrieve the payload data
     pAccessSvc->connect("LARHV");
     const IRDBRecordset* hvGlob;
     hvGlob = pAccessSvc->getRecordset("HVBarrelGlob",tagName,nodeName,"LARHV");

     if(hvGlob->size()>0)
      defaultHvVal = (*hvGlob)[0]->getDouble("DEFAULTVAL");

     std::cout << " read defaultHvVal from database " << defaultHvVal << std::endl;
    }

    // set default values
    std::cout << " defaultHvVal " << defaultHvVal <<std::endl;
    for (int ipm=0;ipm<2;ipm++) {
      for (int ielec=0;ielec<1024;ielec++) {
        for (int ieta=0;ieta<7;ieta++) {
          for (int iside=0;iside<2;iside++) {
            m_hv[ipm][ielec][ieta][iside] = defaultHvVal;
          }
        }
      }
    }

    // apply imperfections
    if(!setHandcoded)
    {
      const IRDBRecordset* hvVal;
      hvVal  = pAccessSvc->getRecordset("HVBarrelVal", tagName,nodeName,"LARHV");

      for(unsigned int indx=0; indx<hvVal->size(); indx++)
      {
        const IRDBRecord* record = (*hvVal)[indx];
        int nZSide = record->getInt("ZSIDE");
        int nEtaReg = record->getInt("ETAREGION");
        int nGapSide = record->getInt("GAPSIDE");
        int nFirstElec = record->getInt("FIRSTELEC");
        int nLastElec = record->getInt("LASTELEC");
        double hvVal = record->getDouble("HVVALUE");

        std::cout << " found pathological region " << nZSide << " " << nEtaReg << " " << nGapSide
          << " " << nFirstElec << " " << nLastElec << " " << hvVal << std::endl;

        if(nFirstElec<=nLastElec)
          for(int iel=nFirstElec; iel<=nLastElec; iel++)
            m_hv[nZSide][iel][nEtaReg][nGapSide] = hvVal;
        else
          std::cerr << "LArBarrelCalculator::InitHV() nLastElec<nFirstElec for:\n"
                    << "Z Side   = " << nZSide << "\n"
                    << "Eta Reg  = " << nEtaReg << "\n"
                    << "Gap Side = " << nGapSide << "\n";
      }
      pAccessSvc->disconnect("LARHV");
    }

  }

// read from ASCII
  else  {

    // set default values
    std::cout << " defaultHvVal " << defaultHvVal <<std::endl;
    for (int ipm=0;ipm<2;ipm++) {
      for (int ielec=0;ielec<1024;ielec++) {
        for (int ieta=0;ieta<7;ieta++) {
          for (int iside=0;iside<2;iside++) {
            m_hv[ipm][ielec][ieta][iside] = defaultHvVal;
          }
        }
      }
    }

    std::string filename = "hv.txt";

    FILE * fp = fopen(filename.c_str(),"r");
    if (!fp) {
      std::cerr << "LArBarrelCalculator::InitHV() ERROR Can't open file " 
                << filename << "\n";
      return;
    }
    char line[81];
    while (fgets(line,80,fp)) {
        int nZSide,nEtaReg,nGapSide,nFirstElec,nLastElec;
        float hvVal;
        sscanf(&line[0],"%80d%80d%80d%80d%80d%80f",&nZSide,&nEtaReg,&nGapSide,&nFirstElec,&nLastElec,&hvVal);

        std::cout << " found pathological region " << nZSide << " " << nEtaReg << " " << nGapSide
          << " " << nFirstElec << " " << nLastElec << " " << hvVal << std::endl;

        if(nFirstElec>=0 && nFirstElec<1024 &&
           nLastElec>=0 && nLastElec<1024 &&
           nZSide >= 0 && nZSide < 2 &&
           nEtaReg >= 0 && nEtaReg < 7 &&
           nGapSide >= 0 && nGapSide < 2 &&
           nFirstElec<=nLastElec)
        {
          for(int iel=nFirstElec; iel<=nLastElec; iel++)
            m_hv[nZSide][iel][nEtaReg][nGapSide] = hvVal;
        }
        else
          std::cerr << "LArBarrelCalculator::InitHV() bad input for:\n"
                    << "Z Side   = " << nZSide << "\n"
                    << "Eta Reg  = " << nEtaReg << "\n"
                    << "Gap Side = " << nGapSide << "\n";

    }
    fclose (fp);

  }

}

StatusCode LArHVToolMC::LoadCalibration(IOVSVC_CALLBACK_ARGS) 
{
  return StatusCode::SUCCESS;
}
