/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************

NAME:     LArCellEmMiscalib
PACKAGE:  offline/LArCalorimeter/LArRecUtils

AUTHORS:  G.Unal                      
CREATED:  25 October 2006 

PURPOSE:  applies miscalibration in EM calorimeter

********************************************************************/
#include "LArCellEmMiscalib.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "AthenaKernel/RNGWrapper.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include <CLHEP/Random/Randomize.h>


using CLHEP::RandGauss;


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArCellEmMiscalib::LArCellEmMiscalib(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  : CaloCellCorrection(type, name, parent),
    m_larem_id(nullptr),
    m_seed(1234),
    m_sigmaPerRegion(0.005),
    m_sigmaPerCell(0.007),
    m_undo(false),
    m_ncellem(0)
{
  declareInterface<CaloCellCorrection>(this); 
  declareProperty("Seed",   m_seed, "seed : should always be the same");
  declareProperty("SigmaPerRegion", m_sigmaPerRegion,"smearing per region");
  declareProperty("SigmaPerCell", m_sigmaPerCell, "smearing per cell ");
  declareProperty("UndoMisCalib",m_undo,"Apply opposite sign to undo miscalibration");
}


/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode LArCellEmMiscalib::initialize()
{
  ATH_MSG_INFO( " in LArCellEmMiscalib::initialize()"  );
  ATH_CHECK(m_caloMgrKey.initialize());
  ATH_CHECK(m_rngSvc.retrieve());
  return StatusCode::SUCCESS;
}

void LArCellEmMiscalib::initOnce (const EventContext& ctx) {
  StatusCode sc;
  sc=detStore()->retrieve( m_caloIdMgr );
  sc.ignore();
  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey, ctx};
  m_calodetdescrmgr = *caloMgrHandle;

  ATHRNG::RNGWrapper* wrapper = m_rngSvc->getEngine (this);
  wrapper->setSeed (ctx.slot(), m_seed);
  CLHEP::HepRandomEngine* engine = wrapper->getEngine (ctx);


  // computes the smearing per region
  this->smearingPerRegion (engine);


  m_larem_id   = m_caloIdMgr->getEM_ID();

  m_ncellem=m_larem_id->channel_hash_max();
  m_calib.resize(m_ncellem,1.);

  ATH_MSG_DEBUG(" Number of EM cells " << m_ncellem);

  for (unsigned int i=0;i<m_ncellem;i++){
    IdentifierHash idHash = i;
    Identifier id = m_larem_id->channel_id(idHash);
    int barrel_ec = m_larem_id->barrel_ec(id);
    const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(id);
    double eta = fabs(calodde->eta_raw());
    double phi = calodde->phi_raw();
    

    int iregion= this->region(barrel_ec,eta,phi);

    ATH_MSG_VERBOSE("barrel_ec,eta,phi,region " << barrel_ec << " " << eta << " " << phi << " " 
		    << iregion << " " << m_spread1[iregion]);


    if (iregion>=0) {
         double spread2=m_sigmaPerCell*RandGauss::shoot(engine,0.,1.);
         m_calib[idHash] = m_spread1[iregion] + spread2;
         if (m_undo ) {
              if (m_calib[idHash] >0.) m_calib[idHash]=1./m_calib[idHash];
              else m_calib[idHash]=1.;
         }
         ATH_MSG_VERBOSE("  m_calib " << m_calib[idHash]);
    } 
    else {
      ATH_MSG_WARNING( " Cannot find region for cell " << eta << " " << phi  );
      m_calib[idHash]=1.;
    }
  }


  return;

}
////////////////////////////////////////////////////////////////////////////////////////

void LArCellEmMiscalib::smearingPerRegion (CLHEP::HepRandomEngine* engine)
{
  m_spread1.resize(544,1);
  for (int i=0;i<544;i++) {
    m_spread1[i]=1.+m_sigmaPerRegion*RandGauss::shoot(engine,0.,1.);
  }

  if (msgLvl(MSG::DEBUG)) {
    for (int i=0;i<544;i++) {
      int iregion=i;
      int ieta,iphi,barrelec,ii;
      if (iregion<128) {
          barrelec=-1;
          ii=iregion;
      }
      else if (iregion<256) {
          barrelec=1;
          ii=iregion-128;
      } 
      else if (iregion<(256+112)) {
          barrelec=-2;
          ii=iregion-256;
      }
      else if (iregion<(256+224)) {
          barrelec=2;
          ii=iregion-(256+112);
      }
      else if (iregion<(256+224+32)) {
          barrelec=-3;
          ii=iregion-(256+224);
      }
      else {
          barrelec=3;
          ii=iregion-(256+224+32);
      }
      iphi=ii%16;
      ieta=ii/16;
      ATH_MSG_DEBUG( "iregion,barrelec,ieta,iphi,spread " << iregion << " " 
                     << barrelec << " " << ieta << " " << iphi << " " << m_spread1[i]  );
    }
  }//end if msg_lvl(DEBUG)

}

////////////////////////////////////////////////////////////////////////////////////////

int LArCellEmMiscalib::region(int barrelec, double eta, double phi)
{
 int iregion=-1;
 if (phi<0.) phi=phi+2*M_PI;
 int iphi=(int)(phi*(16./(2*M_PI)));
 if (iphi>15) iphi=15;
 if (abs(barrelec)==1) {
    int ieta=(int)(eta*(1./0.2));
    if (ieta>7) ieta=7;
    iregion=16*ieta+iphi;
    if (barrelec==1) iregion=iregion+128;
 }
 if (abs(barrelec)==2) {
    int ieta;
    if (eta<1.5) ieta=0;
    else if(eta<1.6) ieta=1;
    else if(eta<1.8) ieta=2;
    else if(eta<2.0) ieta=3;
    else if(eta<2.1) ieta=4;
    else if(eta<2.3) ieta=5;
    else  ieta=6;
    iregion=16*ieta+iphi;
    if (barrelec==-2) iregion=iregion+256;
    if (barrelec==2) iregion=iregion+256+112;
 }
 if (abs(barrelec)==3) {
   //int ieta;
   //if (eta<2.8) ieta=0;
   //else ieta=1;
    if (barrelec==-3) iregion=iregion+256+224;
    if (barrelec==3) iregion=iregion+256+224+32;
 }
 return iregion;

}

/////////////////////////////////////////////////////////////////////
// PROCESS:
// 
/////////////////////////////////////////////////////////////////////


void  LArCellEmMiscalib::MakeCorrection (CaloCell * theCell,
                                         const EventContext& ctx) const
{
  LArCellEmMiscalib* thisNC ATLAS_THREAD_SAFE = const_cast<LArCellEmMiscalib*>(this);
  std::call_once(m_initOnce, &LArCellEmMiscalib::initOnce,thisNC, ctx);
  float energy = theCell->energy();

  float weight=1; 


  const CaloDetDescrElement * theDDE = theCell->caloDDE();
  // Identifier id = theCell->ID();

  //if (m_larem_id->is_lar_em(id)) {
    
  if (theDDE->getSubCalo()==CaloCell_ID::LAREM) {
    // IdentifierHash idHash = m_larem_id->channel_hash(id);
    IdentifierHash idHash = theDDE->calo_hash();
    if (idHash < m_ncellem) {
      weight = m_calib[idHash];
      } 
    else
      {
	ATH_MSG_WARNING( "Inconsistent hash value found " << idHash 
                         << " >= " << m_ncellem  );
      }
//    if (m_larem_id->phi(id)==0)
//      std::cout << "Barrel_ec,sampl.region,eta,wt " << m_larem_id->barrel_ec(id) <<
//       " " << m_larem_id->sampling(id) << " " << m_larem_id->region(id) << " " <<
//       m_larem_id->eta(id) << " " << weight << std::endl;

    setenergy(theCell,energy*weight);


  }
}
