/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSurveyConstraintTool/SurveyConstraintTestAlg.h"
#include "InDetSurveyConstraintTool/ISurveyConstraint.h"
#include "GaudiKernel/MsgStream.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "GaudiKernel/IHistogramSvc.h" 
#include "AIDA/IHistogram1D.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include <cmath>

////namespace InDetSurveyConstraintTool {

/////////////////////////////////////////////////////////////////////////////

SurveyConstraintTestAlg::SurveyConstraintTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
  m_toolsvc{},            //!< Pointer to tool service
  m_SurvConstr{},
  m_pixid{},
  m_sctid{},
  m_h_PixEC_Align_Disk{},
  m_h_PixEC_Align_first{},
  m_h_PixEC_Align{},
  m_AlignResults_nModules{},
  m_NLoop(1){

// Part 2: Properties go here
  declareProperty("NLoop"                   ,     m_NLoop);


}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode SurveyConstraintTestAlg::initialize(){

  // Part 1: Get the messaging service, print where you are
  ATH_MSG_DEBUG( "initialize()" );
  
  // Get The ToolSvc
  ATH_CHECK( service("ToolSvc",m_toolsvc));
 
  
  // Get SurveyConstraint from ToolService
  ATH_CHECK( m_toolsvc->retrieveTool("SurveyConstraint",m_SurvConstr));
 
  // get ID helpers from detector store (relying on GeoModel to put them)
  ATH_CHECK(detStore()->retrieve(m_pixid));
  ATH_CHECK(detStore()->retrieve(m_sctid));
  ATH_MSG_DEBUG( "got ID helpers from detector store (relying on GeoModel to put them)" );

  // book histograms
  BookHist();
  CreateMisAlignNtuple();

return StatusCode::SUCCESS;
}

  void SurveyConstraintTestAlg::CreateMisAlignNtuple(){
    StatusCode sc;
    // initialize AlignResults NTuple
    
    NTupleFilePtr file1(ntupleSvc(), "/NTUPLES/FILE1");
    
    NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/InitialAlignment");
    if ( !nt )    {    // Check if already booked
      nt = ntupleSvc()->book("/NTUPLES/FILE1/InitialAlignment", CLID_ColumnWiseTuple, "InitialAlignment");
      if ( nt )    {
  ATH_MSG_INFO( "InitialAlignment ntuple booked." );
  
  sc = nt->addItem("x"         , m_AlignResults_x);
  sc = nt->addItem("y"         , m_AlignResults_y);
  sc = nt->addItem("z"         , m_AlignResults_z);
  sc = nt->addItem("alpha"     , m_AlignResults_alpha);
  sc = nt->addItem("beta"      , m_AlignResults_beta);
  sc = nt->addItem("gamma"     , m_AlignResults_gamma);
  sc = nt->addItem("ID"        , m_AlignResults_Identifier_ID);
  sc = nt->addItem("PixelSCT"  , m_AlignResults_Identifier_SCT);
  sc = nt->addItem("BarrelEC"  , m_AlignResults_Identifier_BarrelEC);
  sc = nt->addItem("LayerDisc" , m_AlignResults_Identifier_LayerDisc);
  sc = nt->addItem("Phi"       , m_AlignResults_Identifier_Phi);
  sc = nt->addItem("Eta"       , m_AlignResults_Identifier_Eta);
  
      } else {   // did not manage to book the N tuple....
  ATH_MSG_ERROR( "Failed to book InitialAlignment ntuple." );
      }
    }
    
    m_AlignResults_nModules = 0;
    int SCT,barrel_ec,layer_disk,phi,eta;
    std::map<Identifier, SurveyConstraintModule*, std::less<Identifier> > ModuleMap;
    m_SurvConstr -> MMap(ModuleMap);
    std::map<Identifier, SurveyConstraintModule*, std::less<Identifier> >::iterator it;
    for (it = ModuleMap.begin(); it != ModuleMap.end(); ++it) {
      const Identifier& ModuleID = (it->second)->moduleID();
      SurveyConstraintModule* mut = ModuleMap[ModuleID];
      if(mut->isPixel()){
  SCT = 1;
  barrel_ec = m_pixid->barrel_ec(ModuleID);
  layer_disk = m_pixid->layer_disk(ModuleID);
  phi = m_pixid->phi_module(ModuleID);
  eta = m_pixid->eta_module(ModuleID);  
      } else {
  if (m_sctid->side(ModuleID) != 0) continue;
  SCT = 2;
  barrel_ec = m_sctid->barrel_ec(ModuleID);
  layer_disk = m_sctid->layer_disk(ModuleID);
  phi = m_sctid->phi_module(ModuleID);
  eta = m_sctid->eta_module(ModuleID);
      }
      Amg::VectorX Misalign = mut->DOCA_Vector();
      m_AlignResults_x = Misalign[0];
      m_AlignResults_y = Misalign[1];
      m_AlignResults_z = Misalign[2];
      m_AlignResults_alpha = Misalign[3];
      m_AlignResults_beta = Misalign[4];
      m_AlignResults_gamma = Misalign[5];
      m_AlignResults_Identifier_ID = 2;
      m_AlignResults_Identifier_SCT = SCT;
      m_AlignResults_Identifier_BarrelEC = barrel_ec;
      m_AlignResults_Identifier_LayerDisc = layer_disk;
      m_AlignResults_Identifier_Phi = phi;
      m_AlignResults_Identifier_Eta = eta;
      
      m_AlignResults_nModules++;
    
      // Write out AlignResults ntuple
      sc = ntupleSvc()->writeRecord("NTUPLES/FILE1/InitialAlignment");
      if (sc.isFailure()) {
        ATH_MSG_ERROR( "Could not write InitialAlignment ntuple." );
      }
      
    } // end of module loop
  }
  
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode SurveyConstraintTestAlg::execute() {

// Part 1: Get the messaging service, print where you are
ATH_MSG_INFO( "execute()" );

 float multipl=1.E3;

 Amg::VectorX dparams(6);         // alignment parameters
 double deltachisq = 0;
 Amg::VectorX DOCA_Vector(6);
 Amg::MatrixX DOCA_Matrix(6,6);

 // Pix EC
 bool NewDisk = true, NewSector = true;
 int previous_disk = -1, previous_sector = -1;
 for(int i=0;i!=m_NLoop;++i){
   if(i!=0){m_SurvConstr -> finalize().ignore();m_SurvConstr -> setup_SurveyConstraintModules();}
   for (PixelID::const_id_iterator wafer_it=m_pixid->wafer_begin(); wafer_it!=m_pixid->wafer_end(); ++wafer_it) {
     const Identifier Pixel_ModuleID = *wafer_it;
     if(std::abs(m_pixid->barrel_ec(Pixel_ModuleID)) == 2){
       ATH_CHECK(m_SurvConstr -> computeConstraint(Pixel_ModuleID,
                                                   dparams,
                                                   deltachisq,
                                                   DOCA_Vector,
                                                   DOCA_Matrix));
       
       
       if(previous_disk == m_pixid->layer_disk(Pixel_ModuleID) && 
          previous_sector == m_SurvConstr->SectorNumber(m_pixid->phi_module(Pixel_ModuleID)))
         NewSector=false;
       else NewSector=true;
       if(previous_disk == m_pixid->layer_disk(Pixel_ModuleID))
         NewDisk=false;
       else NewDisk=true;
       if (NewDisk) for (unsigned int i=0;i!=6;++i) m_h_PixEC_Align_Disk[i] -> fill(multipl*dparams[i]);
       if (NewSector) for (unsigned int i=0;i!=6;++i) m_h_PixEC_Align_first[i] -> fill(multipl*dparams[i]);
       else for (unsigned int i=0;i!=6;++i) m_h_PixEC_Align[i] -> fill(multipl*dparams[i]);
       previous_disk = m_pixid->layer_disk(Pixel_ModuleID);
       previous_sector = m_SurvConstr->SectorNumber(m_pixid->phi_module(Pixel_ModuleID)); 

       ATH_MSG_DEBUG( "Pixel_ModuleID = " <<  Pixel_ModuleID);
       ATH_MSG_DEBUG( "alignment parameters = (" << dparams[0] << "," << dparams[1] << "," << dparams[2] << "," << dparams[3] << "," << dparams[4] << "," << dparams[5] << ")");
       ATH_MSG_DEBUG( "chi^2 = " << deltachisq);
       ATH_MSG_DEBUG( "DOCA_Vector = (" << DOCA_Vector[0] << "," << DOCA_Vector[1] << "," << DOCA_Vector[2] << "," << DOCA_Vector[3] << "," << DOCA_Vector[4] << "," << DOCA_Vector[5] << ")");
       ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,0) << "," << DOCA_Matrix(1,0) << "," << DOCA_Matrix(2,0) << "," << DOCA_Matrix(3,0) << "," << DOCA_Matrix(4,0) << "," << DOCA_Matrix(5,0) << ")");
       ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,1) << "," << DOCA_Matrix(1,1) << "," << DOCA_Matrix(2,1) << "," << DOCA_Matrix(3,1) << "," << DOCA_Matrix(4,1) << "," << DOCA_Matrix(5,1) << ")");
       ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,0) << "," << DOCA_Matrix(1,1) << "," << DOCA_Matrix(2,2) << "," << DOCA_Matrix(3,2) << "," << DOCA_Matrix(4,4) << "," << DOCA_Matrix(5,5) << ")");
       //break;
     }
   }
 }
 
 // Pix B
 for (PixelID::const_id_iterator wafer_it=m_pixid->wafer_begin(); wafer_it!=m_pixid->wafer_end(); ++wafer_it) {
   const Identifier Pixel_ModuleID = *wafer_it;
   if(m_pixid->barrel_ec(Pixel_ModuleID) == 0){
     ATH_CHECK(m_SurvConstr -> computeConstraint(Pixel_ModuleID,
                                                 dparams,
                                                 deltachisq,
                                                 DOCA_Vector,
                                                 DOCA_Matrix));
     ATH_MSG_DEBUG( "Pixel Barrel ModuleID = " <<  Pixel_ModuleID);
     ATH_MSG_DEBUG( "alignment parameters = (" << dparams[0] << "," << dparams[1] << "," << dparams[2] << "," << dparams[3] << "," << dparams[4] << "," << dparams[5] << ")");
     ATH_MSG_DEBUG( "chi^2 = " << deltachisq);
     ATH_MSG_DEBUG( "DOCA_Vector = (" << DOCA_Vector[0] << "," << DOCA_Vector[1] << "," << DOCA_Vector[2] << "," << DOCA_Vector[3] << "," << DOCA_Vector[4] << "," << DOCA_Vector[5] << ")");
     ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,0) << "," << DOCA_Matrix(1,0) << "," << DOCA_Matrix(2,0) << "," << DOCA_Matrix(3,0) << "," << DOCA_Matrix(4,0) << "," << DOCA_Matrix(5,0) << ")");
     ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,1) << "," << DOCA_Matrix(1,1) << "," << DOCA_Matrix(2,1) << "," << DOCA_Matrix(3,1) << "," << DOCA_Matrix(4,1) << "," << DOCA_Matrix(5,1) << ")");
     ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,0) << "," << DOCA_Matrix(1,1) << "," << DOCA_Matrix(2,2) << "," << DOCA_Matrix(3,2) << "," << DOCA_Matrix(4,4) << "," << DOCA_Matrix(5,5) << ")");
     break;
   }
 }

 // SCT EC
 
 for (SCT_ID::const_id_iterator wafer_it=m_sctid->wafer_begin(); wafer_it!=m_sctid->wafer_end(); ++wafer_it) {
   const Identifier SCT_ModuleID = *wafer_it; 
   if(std::abs(m_sctid->barrel_ec(SCT_ModuleID)) == 2){
     ATH_CHECK(m_SurvConstr -> computeConstraint(SCT_ModuleID,
                                                 dparams,
                                                 deltachisq,
                                                 DOCA_Vector,
                                                 DOCA_Matrix));
     ATH_MSG_DEBUG( "SCT_ModuleID = " <<  SCT_ModuleID);
     ATH_MSG_DEBUG( "alignment parameters = (" << dparams[0] << "," << dparams[1] << "," << dparams[2] << "," << dparams[3] << "," << dparams[4] << "," << dparams[5] << ")");
     ATH_MSG_DEBUG( "chi^2 = " << deltachisq);
     ATH_MSG_DEBUG( "DOCA_Vector = (" << DOCA_Vector[0] << "," << DOCA_Vector[1] << "," << DOCA_Vector[2] << "," << DOCA_Vector[3] << "," << DOCA_Vector[4] << "," << DOCA_Vector[5] << ")");
     ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,0) << "," << DOCA_Matrix(1,0) << "," << DOCA_Matrix(2,0) << "," << DOCA_Matrix(3,0) << "," << DOCA_Matrix(4,0) << "," << DOCA_Matrix(5,0) << ")");
     ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,1) << "," << DOCA_Matrix(1,1) << "," << DOCA_Matrix(2,1) << "," << DOCA_Matrix(3,1) << "," << DOCA_Matrix(4,1) << "," << DOCA_Matrix(5,1) << ")");
     ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,0) << "," << DOCA_Matrix(1,1) << "," << DOCA_Matrix(2,2) << "," << DOCA_Matrix(3,2) << "," << DOCA_Matrix(4,4) << "," << DOCA_Matrix(5,5) << ")");
     break;
   }
 }


 // SCT B
 for (SCT_ID::const_id_iterator wafer_it=m_sctid->wafer_begin(); wafer_it!=m_sctid->wafer_end(); ++wafer_it) {
   const Identifier SCT_ModuleID = *wafer_it;
   if(m_sctid->barrel_ec(SCT_ModuleID) == 0){
     ATH_CHECK(m_SurvConstr -> computeConstraint(SCT_ModuleID,
                                                 dparams,
                                                 deltachisq,
                                                 DOCA_Vector,
                                                 DOCA_Matrix));
     ATH_MSG_DEBUG( "SCT Barrel ModuleID = " <<  SCT_ModuleID);
     ATH_MSG_DEBUG( "alignment parameters = (" << dparams[0] << "," << dparams[1] << "," << dparams[2] << "," << dparams[3] << "," << dparams[4] << "," << dparams[5] << ")");
     ATH_MSG_DEBUG( "chi^2 = " << deltachisq);
     ATH_MSG_DEBUG( "DOCA_Vector = (" << DOCA_Vector[0] << "," << DOCA_Vector[1] << "," << DOCA_Vector[2] << "," << DOCA_Vector[3] << "," << DOCA_Vector[4] << "," << DOCA_Vector[5] << ")");
     ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,0) << "," << DOCA_Matrix(1,0) << "," << DOCA_Matrix(2,0) << "," << DOCA_Matrix(3,0) << "," << DOCA_Matrix(4,0) << "," << DOCA_Matrix(5,0) << ")");
     ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,1) << "," << DOCA_Matrix(1,1) << "," << DOCA_Matrix(2,1) << "," << DOCA_Matrix(3,1) << "," << DOCA_Matrix(4,1) << "," << DOCA_Matrix(5,1) << ")");
     ATH_MSG_DEBUG( "DOCA_Matrix = (" <<DOCA_Matrix(0,0) << "," << DOCA_Matrix(1,1) << "," << DOCA_Matrix(2,2) << "," << DOCA_Matrix(3,2) << "," << DOCA_Matrix(4,4) << "," << DOCA_Matrix(5,5) << ")");
     break;
   }
 }
 
 return StatusCode::SUCCESS;
 }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode SurveyConstraintTestAlg::finalize() {

  // Part 1: Get the messaging service, print where you are
  ATH_MSG_INFO( "finalize()" );

  ATH_MSG_INFO( "mm = " << CLHEP::mm 
     << ", mrad = " << CLHEP::mrad 
     << ", micrometer = " << CLHEP::micrometer 
     << ", deg = " << CLHEP::deg);

  return StatusCode::SUCCESS;
}

void SurveyConstraintTestAlg::BookHist() {
  m_h_PixEC_Align_Disk[0] = histoSvc()->book("/stat/MisAlign", "Misalignment Disk X [mu]", "Misalignment Disk X [mu]",100, -50.,50.);
  m_h_PixEC_Align_Disk[1] = histoSvc()->book("/stat/MisAlign", "Misalignment Disk Y [mu]", "Misalignment Disk Y [mu]",100, -100.,100.);
  m_h_PixEC_Align_Disk[2] = histoSvc()->book("/stat/MisAlign", "Misalignment Disk Z [mu]", "Misalignment Disk Z [mu]",100, -200.,200.);
  m_h_PixEC_Align_Disk[3] = histoSvc()->book("/stat/MisAlign", "Misalignment Disk PhiX [mrad]", "Misalignment Disk PhiX [mrad]",100, -2.,2.);
  m_h_PixEC_Align_Disk[4] = histoSvc()->book("/stat/MisAlign", "Misalignment Disk PhiY [mrad]", "Misalignment Disk PhiY [mrad]",100, -5.,5.);
  m_h_PixEC_Align_Disk[5] = histoSvc()->book("/stat/MisAlign", "Misalignment Disk PhiZ [mrad]", "Misalignment Disk PhiZ [mrad]",100, -2.,2.);

  m_h_PixEC_Align_first[0] = histoSvc()->book("/stat/MisAlign", "Misalignment first X [mu]", "Misalignment first X [mu]",100, -50.,50.);
  m_h_PixEC_Align_first[1] = histoSvc()->book("/stat/MisAlign", "Misalignment first Y [mu]", "Misalignment first Y [mu]",100, -100.,100.);
  m_h_PixEC_Align_first[2] = histoSvc()->book("/stat/MisAlign", "Misalignment first Z [mu]", "Misalignment first Z [mu]",100, -200.,200.);
  m_h_PixEC_Align_first[3] = histoSvc()->book("/stat/MisAlign", "Misalignment first PhiX [mrad]", "Misalignment first PhiX [mrad]",100, -2.,2.);
  m_h_PixEC_Align_first[4] = histoSvc()->book("/stat/MisAlign", "Misalignment first PhiY [mrad]", "Misalignment first PhiY [mrad]",100, -5.,5.);
  m_h_PixEC_Align_first[5] = histoSvc()->book("/stat/MisAlign", "Misalignment first PhiZ [mrad]", "Misalignment first PhiZ [mrad]",100, -2.,2.);

  m_h_PixEC_Align[0] = histoSvc()->book("/stat/MisAlign", "Misalignment X [mu]", "Misalignment X [mu]",100, -50.,50.);
  m_h_PixEC_Align[1] = histoSvc()->book("/stat/MisAlign", "Misalignment Y [mu]", "Misalignment Y [mu]",100, -100.,100.);
  m_h_PixEC_Align[2] = histoSvc()->book("/stat/MisAlign", "Misalignment Z [mu]", "Misalignment Z [mu]",100, -200.,200.);
  m_h_PixEC_Align[3] = histoSvc()->book("/stat/MisAlign", "Misalignment PhiX [mrad]", "Misalignment PhiX [mrad]",100, -2.,2.);
  m_h_PixEC_Align[4] = histoSvc()->book("/stat/MisAlign", "Misalignment PhiY [mrad]", "Misalignment PhiY [mrad]",100, -5.,5.);
  m_h_PixEC_Align[5] = histoSvc()->book("/stat/MisAlign", "Misalignment PhiZ [mrad]", "Misalignment PhiZ [mrad]",100, -2.,2.);
}
  //__________________________________________________________________________
////} // end of namespace bracket

