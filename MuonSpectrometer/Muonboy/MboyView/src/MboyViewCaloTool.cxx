/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"


#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloID.h"
#include "CLHEP/Units/SystemOfUnits.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewCaloTool.h"

#include <sstream>

static const InterfaceID IID_IMboyViewCaloTool("MboyViewCaloTool", 1, 0);

const InterfaceID& MboyViewCaloTool::interfaceID()
{
  return IID_IMboyViewCaloTool;
}

MboyViewCaloTool::MboyViewCaloTool(
                           const std::string& type, 
                           const std::string& name, 
                           const IInterface* parent)
  :AthAlgTool(type, name, parent),
   m_cellsName("AllCalo")
{  

  declareInterface<MboyViewCaloTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

}

MboyViewCaloTool::~MboyViewCaloTool()
{ }

StatusCode MboyViewCaloTool::initialize()
{
  

  m_InitOK = false;
  
  ATH_MSG_INFO( " reading CaloCellContainer " << m_cellsName ) ;

  StatusCode sc ;
   

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  sc = detStore()->retrieve(m_emid);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Unable to retrieve LArEM_ID helper from DetectorStore" ) ;
    return sc;
  }
  
  sc = detStore()->retrieve(m_fcalid);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Unable to retrieve LArFCAL_ID helper from DetectorStore" ) ;
    return sc;
  }
  
  sc = detStore()->retrieve(m_hecid);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Unable to retrieve LArHEC_ID helper from DetectorStore" ) ;
    return sc;
  }
  
  sc = detStore()->retrieve(m_tileid);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Unable to retrieve TileID helper from DetectorStore" ) ;
    return sc;
  }
  
  m_InitOK = true;
  
  return StatusCode::SUCCESS;
  
  
}


StatusCode MboyViewCaloTool::finalize()
{

  return StatusCode::SUCCESS;
}

size_t MboyViewCaloTool::get_id_value(Identifier const & id) const
{
    std::istringstream iss(id.getString());
    size_t value = 0;
    if (! (iss >> std::hex >> value))
    {
        ATH_MSG_WARNING("Could not parse calorimeter cell ID");
    }
    return value;
}


StatusCode MboyViewCaloTool::ViewCalo(std::ostream* out, float& CaloMinimumE)
{

  // typedef ObjectVector<CaloCell> CONTAINER; 
  typedef CaloCellContainer CONTAINER; 

//Access
  if (!(evtStore()->contains<CONTAINER>(m_cellsName))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << m_cellsName ) ;
    }
    return( StatusCode::SUCCESS );
  }

  const CONTAINER* cellcoll ;
  StatusCode sc = evtStore()->retrieve(cellcoll, m_cellsName) ; 
  if (sc.isFailure()){
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO(  " Selection: cellcoll not found at " << m_cellsName ) ;
    }
    return( StatusCode::SUCCESS);
  }

    
    CONTAINER::const_iterator f_cell = cellcoll->begin();
    CONTAINER::const_iterator l_cell = cellcoll->end();

//  int index = 0 ;
      
    for ( ; f_cell!=l_cell; ++f_cell)
    {
      const CaloCell* cell = (*f_cell) ;
	  
      // Apply Energy Threshold
      float e  = cell->energy() ; 
      if ( e < CaloMinimumE ) continue;

      double eta = cell->eta() ;
      double phi = cell->phi() ;

      std::string regionName = " ";
      const CaloCell_ID::CaloSample  sampl = cell->caloDDE()->getSampling();

      double r = cell->caloDDE()->r();
      int Layer = cell->caloDDE()->getLayer();
      double dphi = cell->caloDDE()->dphi();
      double deta = cell->caloDDE()->deta();
      double dr = cell->caloDDE()->dr();
      double dx = cell->caloDDE()->dx();
      double dy = cell->caloDDE()->dy();
      double dz = cell->caloDDE()->dz();
      Identifier id=cell->caloDDE()->identify() ;
//       double x = cell->caloDDE()->x();
//       double y = cell->caloDDE()->y();
//       double z = cell->caloDDE()->z();
      
      // Define the region names for Dumps
      if ( cell->caloDDE()->is_lar_em_barrel () ) {
        regionName = "EMB";
      }
      if ( cell->caloDDE()->is_lar_em_endcap () ) {
        regionName = "EME";
      }
      // For the TILE it is the full cell size!
      if ( cell->caloDDE()->is_tile() ) {
        regionName = "TIL";
	dr = 0.5 * dr;
      }
      if ( cell->caloDDE()->is_lar_hec() ) {
        regionName = "HEC";
      }
      if ( cell->caloDDE()->is_lar_fcal() ) {
        regionName = "FCA";
      }
      if ( sampl == CaloCell_ID::PreSamplerE ) {
        regionName = "PSE";
      }
      if ( sampl == CaloCell_ID::PreSamplerB ) {
        regionName = "PSB";
      }
      
      // Writeout the Dump
      *out
        << "CAL " << std::setw(3) << regionName << " "  
	<< std::setw(10)<<std::setprecision(4) <<r  << " "
	<< std::setw(10)<<std::setprecision(4) <<eta  << " "
	<< std::setw(10)<<std::setprecision(4) <<phi  << " "
	<< std::setw(4)<<std::setprecision(0) <<Layer  << " "
	<< std::setw(12)<<std::setprecision(1) <<e  << " "
	<< std::setw(10)<<std::setprecision(4) <<deta  << " "
	<< std::setw(10)<<std::setprecision(4) <<dphi  << " "
	<< std::setw(10)<<std::setprecision(4) <<dr  << " "
	<< std::setw(10)<<std::setprecision(4) <<dx  << " "
	<< std::setw(10)<<std::setprecision(4) <<dy  << " "
	<< std::setw(10)<<std::setprecision(4) <<dz  << " "
	<< std::setw(10)<< get_id_value(id)  << " "
        << std::endl;
    }


  return StatusCode::SUCCESS;
}



