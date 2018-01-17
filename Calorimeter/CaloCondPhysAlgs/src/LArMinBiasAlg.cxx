/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondPhysAlgs/LArMinBiasAlg.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/PileUpEventInfo.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "CaloUtils/CaloCellList.h"
#include "CaloUtils/CaloLayerCalculator.h"

#include "CaloIdentifier/LArID.h"

#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"



  //Constructor
  LArMinBiasAlg:: LArMinBiasAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_larmcsym("LArMCSymTool"),
    m_datasetID_lowPt(119995),
    m_datasetID_highPt(119996),
    m_weight_lowPt(39.8606),
    m_weight_highPt(0.138128),
    m_cablingService("LArCablingService")
  {
     declareProperty("datasetID_lowPt",m_datasetID_lowPt);
     declareProperty("datasetID_highPt",m_datasetID_highPt);
     declareProperty("weight_highPt",m_weight_highPt);
     declareProperty("weight_lowPt",m_weight_lowPt);
     m_first=true;
  }
  
  //__________________________________________________________________________
  //Destructor
  LArMinBiasAlg::~LArMinBiasAlg()
  {
  }
  //__________________________________________________________________________
  StatusCode LArMinBiasAlg::initialize()
  {
    
    ATH_MSG_INFO ( " LArMinBiasAlg initialize() " );

    ATH_CHECK( service("THistSvc",m_thistSvc) );


    m_tree = new TTree("m_tree","Offset ntuple");
    m_tree->Branch("ncell",&m_nsymcell,"ncell/I");
    m_tree->Branch("nevt_total",&m_nevt_total,"nevt_total/I");
    m_tree->Branch("identifier",m_identifier,"identifier[ncell]/I");
    m_tree->Branch("layer",m_layer,"layer[ncell]/I");
    m_tree->Branch("region",m_region,"region[ncell]/I");
    m_tree->Branch("ieta",m_ieta,"ieta[ncell]/I");
    m_tree->Branch("eta",m_eta,"eta[ncell]/F");
    m_tree->Branch("phi",m_phi,"phi[ncell]/F");
    m_tree->Branch("nevt",m_nevt,"nevt[ncell]/D");
    m_tree->Branch("average",m_average,"average[ncell]/D");
    m_tree->Branch("rms",m_rms,"rms[ncell]/D");
    m_tree->Branch("reference",m_offset,"offset[ncell]/D");
   
    if( m_thistSvc->regTree("/file1/offset",m_tree).isFailure()) {
       ATH_MSG_WARNING(" cannot register ntuple " );
       return StatusCode::SUCCESS;
     }

    ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );
    m_larem_id   = m_caloIdMgr->getEM_ID();
    m_calo_id      = m_caloIdMgr->getCaloCell_ID();

//  retrieve CaloDetDescrMgr 
    ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );

    ATH_CHECK(m_larmcsym.retrieve());

    ATH_CHECK(m_cablingService.retrieve());

    m_n1=0;
    m_n2=0;

    m_nevt_total=0;

    return StatusCode::SUCCESS; 

  }
  //__________________________________________________________________________
  StatusCode LArMinBiasAlg::stop()
  {
    std::cout << "number of events in the two samples  " << m_n1 << " " << m_n2 << std::endl;
    this->fillNtuple();
    std::cout <<" stop after fill ntuple " << std::endl;
    return StatusCode::SUCCESS;
  }
  //__________________________________________________________________________
  StatusCode LArMinBiasAlg::finalize()
  {
    std::cout << " finalize() " << std::endl;
    return StatusCode::SUCCESS; 
  }
  
  //__________________________________________________________________________
  StatusCode LArMinBiasAlg::execute()
  {
    //.............................................
    
    ATH_MSG_DEBUG(" LArMinBiasAlg execute()");

    if (m_first) {
      m_ncell = m_calo_id->calo_cell_hash_max();

      std::cout << " --- first event " << m_ncell << std::endl;
      m_symCellIndex.resize(m_ncell,-1);
      std::vector<int> doneCell;
      doneCell.resize(m_ncell,-1);

      //m_readCell.resize(m_ncell,0);

      m_eCell.resize(m_ncell,0.);

      m_CellList.reserve(MAX_SYM_CELLS);
      int nsym=0; 
      // loop over cells
      // and find symmetry cells
      for (unsigned int i=0;i<((unsigned int)(m_ncell));i++) {
        IdentifierHash idHash=i;
        Identifier id=m_calo_id->cell_id(idHash);
        if (m_calo_id->is_tile(id)) continue;
        // convert cell id to symetric identifier
        HWIdentifier hwid2=m_larmcsym->symOnline(id);
        Identifier id2 = m_cablingService->cnvToIdentifier(hwid2);
        int i2 = (int) (m_calo_id->calo_cell_hash(id2));
        if(i2>=m_ncell) {
           ATH_MSG_WARNING("problem: i2: "<<i2<<" for id: "<<m_calo_id->print_to_string(id)<<" symmetrized: "<<m_calo_id->print_to_string(id2));
        }
        // we have already processed this hash => just need to associate cell i to the same symetric cell
        if (doneCell[i2]>=0) {
           m_symCellIndex[i]=doneCell[i2];
        }
        // we have not already processed this hash, add an entry for this new symmetric cell
        else {  
           doneCell[i2]=nsym;
           m_symCellIndex[i] = nsym; 
           CellInfo cell;
           const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(id);
           cell.eta =  calodde->eta();
           cell.phi = calodde->phi();
           cell.layer = m_calo_id->calo_sample(id);
           cell.region = m_calo_id->region(id);
           cell.ieta = m_calo_id->eta(id);
           //cell.identifier = id2.get_identifier32().get_compact();
           cell.identifier = id2;
           cell.average=0.;
           cell.offset=0.;
           cell.rms=0.;
           cell.nevt=0.;
           m_CellList.push_back(cell);
           nsym++;
        }
      }
      std::cout << " --- number of symmetric cells found " << nsym << " " << m_CellList.size() << std::endl;
      if (nsym>=MAX_SYM_CELLS) ATH_MSG_ERROR(" More than allowed number of symmetric cells... Fix array size for ntuple writing !!!! ");
      m_nsymcell=nsym;
      m_first=false;
    }


  const EventInfo* eventInfo;
  ATH_CHECK (evtStore()->retrieve(eventInfo));
  int channelNumber = eventInfo->event_type()->mc_channel_number() ;

  m_nevt_total++;

  double weight=1.;

// Dataset ID for lowPt MinBias
  if (channelNumber==m_datasetID_lowPt) {
     weight = m_weight_lowPt;
     m_n1+=1;
  }
//  Dataset ID for highPt MinBias
  else if (channelNumber==m_datasetID_highPt) {
     weight = m_weight_highPt;
     m_n2+=1;
  }
  else {
     ATH_MSG_WARNING(" Neither low nor high Pt MinBias sample " << channelNumber << "  set weight to 1.0 ");
     weight=1.;
  } 


  if ((m_nevt_total%100)==1) std::cout << " ---- process event number " << m_nevt_total <<  " " << channelNumber << "  weight " << weight << std::endl;

   for (int i=0;i<m_ncell;i++) m_eCell[i]=0.;

    std::vector <std::string> HitContainer;
    HitContainer.push_back("LArHitEMB");
    HitContainer.push_back("LArHitEMEC");
    HitContainer.push_back("LArHitHEC");
    HitContainer.push_back("LArHitFCAL");
    for (unsigned int iHitContainer=0;iHitContainer<HitContainer.size();iHitContainer++)
    {
      const LArHitContainer* hit_container ;
      ATH_CHECK(evtStore()->retrieve(hit_container,HitContainer[iHitContainer]));
      int ihit = 0;
      LArHitContainer::const_iterator hititer;
      for(hititer=hit_container->begin();
           hititer != hit_container->end();hititer++)
      {       
          ihit++; 
          LArHit* hit = (*hititer);
          Identifier cellID=hit->cellID();
          double energy = hit->energy(); 
          double time =hit->time();
          int index = (int) (m_calo_id->calo_cell_hash(cellID));
          if (index < m_ncell && index>=0 && fabs(time)<25.) {
             m_eCell[index] += energy;
        }

      }  // loop over hits in container
    }  // loop over containers

    for (int i=0;i<m_ncell;i++) {
          addCell(i,m_eCell[i],0.,weight);
    }


  return StatusCode::SUCCESS;
 }

 void LArMinBiasAlg::addCell(int index, double  energy, double eshift, double weight) 
 {
          if (index < m_ncell && index>=0) {
            int index2= m_symCellIndex[index];
            if (index2<0) return;
            if (index2 >= ((int)(m_CellList.size())) ) {
                std::cout << " problem " << index << " " << index2 << " " << m_CellList.size() << std::endl;
            }
            double oldN =  m_CellList[index2].nevt;
            double oldAverage = m_CellList[index2].average;
            double oldRMS     = m_CellList[index2].rms;
            double oldAverage2 = m_CellList[index2].offset;

            double frac = oldN/(weight+oldN);
            double Anew = weight+oldN;
            double newAverage = frac*oldAverage + weight*energy/Anew;
            double deltaE = energy-newAverage;
            double newRMS     = frac*(oldRMS + (newAverage-oldAverage)*(newAverage-oldAverage)) + weight*deltaE*deltaE/Anew;

            double newAverage2 = frac*oldAverage2 + weight*eshift/Anew;

            m_CellList[index2].nevt = Anew;
            m_CellList[index2].average = newAverage;
            m_CellList[index2].rms = newRMS; 
            m_CellList[index2].offset = newAverage2; 

          }
 }

 void LArMinBiasAlg::fillNtuple()
 {

   std::cout << " in fillNtuple " << m_nsymcell << std::endl;
   for (int i=0;i<m_nsymcell;i++) {
     m_identifier[i] = m_CellList[i].identifier.get_identifier32().get_compact();
     m_layer[i] = m_CellList[i].layer;
     m_region[i] = m_CellList[i].region;
     m_ieta[i] = m_CellList[i].ieta;
     m_eta[i] = m_CellList[i].eta;
     m_phi[i] = m_CellList[i].phi;
     m_nevt[i] = m_CellList[i].nevt;
     m_offset[i] = (float) (m_CellList[i].offset);
     m_average[i] = (float) (m_CellList[i].average);
     m_rms[i] = (float) (sqrt(m_CellList[i].rms));
   }
   m_tree->Fill();
   std::cout << " after tree fill " << std::endl;

    for (int i=0;i<m_nsymcell;i++) {
     m_CellList[i].nevt=0;
     m_CellList[i].offset=0.;
     m_CellList[i].average=0;
     m_CellList[i].rms=0;
   }
   std::cout << " end of fillNtuple " << std::endl;
 
 }
