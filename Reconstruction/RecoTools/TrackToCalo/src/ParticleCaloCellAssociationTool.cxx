/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleCaloCellAssociationTool.h"
// forward declares
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

#include "ParticleCaloExtension/ParticleCellAssociationCollection.h"

#include "TrackToCalo/CaloCellHelpers.h"

#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"

#include "CaloUtils/CaloCellList.h"

#include "xAODTracking/TrackingPrimitives.h"
#include <math.h>
#include <memory>

namespace Rec {

  ParticleCaloCellAssociationTool::ParticleCaloCellAssociationTool(const std::string& t, const std::string& n, const IInterface*  p )
    : AthAlgTool(t,n,p),
      m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
      m_defaultSelector(0.4)
  {

    declareInterface<IParticleCaloCellAssociationTool>(this);
    declareProperty("ParticleCaloExtensionTool",   m_caloExtensionTool );

    //Default data source for the calocells
    declareProperty("CaloCellContainer", m_cellContainerName="AllCalo");

    //coneSize for including calo cells around track below 0.2 we will loose cells energy in e.g. Tile HEC etc.
    declareProperty("ConeSize", m_coneSize = 0.2);

  }

  ParticleCaloCellAssociationTool::~ParticleCaloCellAssociationTool() {}

  StatusCode ParticleCaloCellAssociationTool::initialize() {
    /* Retrieve track extrapolator from ToolService */
    ATH_CHECK( m_caloExtensionTool.retrieve() );

    m_defaultSelector.setConeSize(m_coneSize);

    if(!m_cellContainerName.key().empty()) {
      ATH_CHECK(m_cellContainerName.initialize());
    }

    return StatusCode::SUCCESS;
  }

  StatusCode ParticleCaloCellAssociationTool::finalize() {
    return StatusCode::SUCCESS;
  }


  void ParticleCaloCellAssociationTool::getCellIntersections( const Trk::CaloExtension& extension,
                                                              const std::vector<const CaloCell*>& cells,
                                                              ParticleCellAssociation::CellIntersections& cellIntersections ) const { 
    
// use 3D pathlength in cells

    bool use3D = true;

    cellIntersections.reserve(extension.caloLayerIntersections().size()*1.3);


    CaloExtensionHelpers::EntryExitLayerMap  entryExitLayerMap;
    CaloExtensionHelpers::entryExitLayerMap( extension, entryExitLayerMap );
    ATH_MSG_DEBUG("EntryExitLayerMap " << entryExitLayerMap.size() );

    CaloExtensionHelpers::ScalarLayerMap  eLossLayerMap, pathLenLayerMap;
    CaloExtensionHelpers::eLossLayerMap( extension, eLossLayerMap );
    CaloExtensionHelpers::pathLenLayerMap( extension, pathLenLayerMap );

    ATH_MSG_DEBUG("Getting cells intersections using cells " << cells.size() );
    for( auto cell : cells ){
      // get sampling and look up entry/exit points
      CaloSampling::CaloSample sample = cell->caloDDE()->getSampling();

      auto pos = entryExitLayerMap.find(sample);
      if( pos == entryExitLayerMap.end() ) continue;
//
// pos2 and weight2 are introduced because the PreSamplerB has sometimes a very small size
//         PresamplerB and EMB1 are merged 
//
      auto pos2 = pos;
      if(sample== CaloSampling::PreSamplerB) {
        pos2 = entryExitLayerMap.find(CaloSampling::EMB1);
	if (pos2==entryExitLayerMap.end()) {
	    pos2=pos;
	  }
      } 
      //// calculate 3D path length
      double path = 0.;

      double drFix = cell->caloDDE()->dr();
      double dzFix = cell->caloDDE()->dz();
//      double dphi = cell->caloDDE()->dphi();

      int isample = cell->caloDDE()->getSampling();
      bool barrel = false;
      if(cell->caloDDE()->getSubCalo() == CaloCell_ID::TILE) barrel = true;
      if(sample== CaloSampling::PreSamplerB ||  sample== CaloSampling::EMB1 ||  sample== CaloSampling::EMB2 || sample== CaloSampling::EMB3) barrel = true;

    
     double drTG =  fabs((pos->second.first-pos2->second.second).perp());
     double dzTG =  fabs((pos->second.first-pos2->second.second).z());

     if(barrel) ATH_MSG_VERBOSE(" barrel cell sampling " << cell->caloDDE()->getSampling() <<  " dr " << cell->caloDDE()->dr() << " drTG " << drTG ); 
     if(!barrel) ATH_MSG_VERBOSE(" endcap cell sampling " << cell->caloDDE()->getSampling() <<  " dz " << cell->caloDDE()->dz() << " dzTG " << dzTG ); 

      if(drFix==0.) { 
// recalculate the r values from the other cells 
// BUG/FEATURE: extract dr from cell container for sampling 4 5 6 7 needed EME 
// BUG/FEATURE: extract dr from cell container for sampling 8 9 10 11 needed HEC 
        if(cell->caloDDE()->deta()>0) {
          double theta = atan2(cell->caloDDE()->r(),cell->z());
          double dtheta = 2*cell->caloDDE()->deta()*sin(theta/2.)*cos(theta/2);
          if( theta+dtheta < M_PI ) {
            double dr = fabs(cell->z()*tan(theta+dtheta) - cell->z()*tan(theta));
            drFix = fabs(dr); 
            double detaCheck = -log(tan((theta+dtheta)/2.)) + log(tan((theta)/2.));
            ATH_MSG_VERBOSE(" FIX cell sampling " << cell->caloDDE()->getSampling() << " deta " << cell->caloDDE()->deta() << " detaCheck " << detaCheck << " drFix " << drFix);
          }else{
            ATH_MSG_WARNING(" FIXR cell sampling failed: theta " << theta << " dtheta " << dtheta << " sum/pi " << (theta+dtheta)*M_1_PI 
                            << " deta " << cell->caloDDE()->deta());
          }
//          ATH_MSG_VERBOSE(" FIX cell sampling deta " << deta << " dtheta " << dtheta  << "  scale " <<  scale << " theta " << theta );
        } else {
          double drMin = 100000.;
          int dscut = 1;
          if(!barrel) dscut = 0;
          const CaloCell* cellFound = 0;
          for( auto celln : cells ){
            if(cell==celln) continue;
            if(cell->caloDDE()->getSubCalo() == celln->caloDDE()->getSubCalo()) {
              int dsample = isample-celln->caloDDE()->getSampling();
              if(abs(dsample)==dscut) {
                double drNew = fabs(cell->caloDDE()->r()-celln->caloDDE()->r()); 
                if(drNew<1) continue;
                if(drNew<drMin) { 
                  drMin = drNew;
                  cellFound = celln;   
                }
              }
            }
          }
          drFix = drMin;
          ATH_MSG_VERBOSE(" Problem cell sampling " << cell->caloDDE()->getSampling() << " x " << cell->caloDDE()->x() << " y " << cell->caloDDE()->y() << " z " << cell->caloDDE()->z() << " dr " << cell->caloDDE()->dr() << " drFix "  << drFix << " drTG " << drTG ); 
        if(cellFound) ATH_MSG_VERBOSE(" cellFound sampling " << cellFound->caloDDE()->getSampling() << " x " << cellFound->caloDDE()->x() << " y " << cellFound->caloDDE()->y() << " z " << cellFound->caloDDE()->z() << " dr " << cellFound->caloDDE()->dr() << " dscut " << dscut  << " drFix "  << drFix );
        }
      }

      if(dzFix==0.) { 
// recalculate z values from the other cells 
// BUG/FEATURE: extract dz from cell container for sampling 0 1 2 3 needed EMB  
        if(cell->caloDDE()->deta()>0) {
          double theta = atan2(cell->caloDDE()->r(),cell->z());
          double dtheta = 2*cell->caloDDE()->deta()*sin(theta/2.)*cos(theta/2);
          if( theta+dtheta < M_PI ) {
            double dz = fabs(cell->caloDDE()->r()/tan(theta+dtheta) - cell->caloDDE()->r()/tan(theta));
            dzFix = dz; 
          }else{
            ATH_MSG_WARNING(" FIXZ cell sampling failed: theta " << theta << " dtheta " << dtheta << " sum/pi " << (theta+dtheta)*M_1_PI 
                            << " deta " << cell->caloDDE()->deta());
          }
          double detaCheck = -log(tan((theta+dtheta)/2.)) + log(tan((theta)/2.));
          ATH_MSG_VERBOSE(" Fix cell sampling " << cell->caloDDE()->getSampling() << " deta " << cell->caloDDE()->deta() << " detaCheck  " << detaCheck  << " dtheta " << dtheta << " dzFix " << dzFix);
          //          ATH_MSG_VERBOSE(" FIX cell sampling deta " << deta << " dtheta " << dtheta  << "  scale " <<  scale << " theta " << theta );
        } else {
          double dzMin = 100000.;
          int dscut = 1;
          if(barrel) dscut = 0;
          const CaloCell* cellFound = 0;
          for( auto celln : cells ){
            if(cell==celln) continue;
            if(cell->caloDDE()->getSubCalo() == celln->caloDDE()->getSubCalo()) {
              int isample2 = celln->caloDDE()->getSampling();
              if(abs(isample-isample2)==dscut) {
                double dzNew = fabs(cell->caloDDE()->z()-celln->caloDDE()->z()); 
                if(dzNew<1) continue;
                if(dzNew<dzMin) { 
                  dzMin = dzNew;
                  cellFound = celln;   
                }
              }
            }
          }
          dzFix = dzMin;
          ATH_MSG_VERBOSE(" Problem cell sampling " << cell->caloDDE()->getSampling() << " x " << cell->caloDDE()->x() << " y " << cell->caloDDE()->y() << " z " << cell->caloDDE()->z() << " dz " << cell->caloDDE()->dz() << " dzFix "  << dzFix << " dzTG " << dzTG  ); 
          if(cellFound) ATH_MSG_VERBOSE(" cellFound sampling " << cellFound->caloDDE()->getSampling() << " x " << cellFound->caloDDE()->x() << " y " << cellFound->caloDDE()->y() << " z " << cellFound->caloDDE()->z() << " dz " << cellFound->caloDDE()->dz()  << " dscut " << dscut << " dzFix "  << dzFix );
        }    
      }
// 
// always use fixed values that correspond to the Calorimeter Tracking Geometry
// these are different from the CaloCell values
//
      double ratioCheck = 1;
      if(barrel)  ratioCheck = drFix/drTG;
      if(!barrel) ratioCheck = dzFix/dzTG;
      if(cell->energy()>50.) ATH_MSG_DEBUG(" cell sampling and size "  << cell->caloDDE()->getSampling() << " cell energy " << cell->energy() << " dzFix " << dzFix << " dzTG " << dzTG << " drFix " << drFix << " drTG " << drTG << " barrel " << barrel << " ratioCheck " << ratioCheck );
     
      if(!barrel) dzFix = dzTG;
      if(barrel)  drFix = drTG;

      if(use3D) {
      //m_pathLenUtil.pathInsideCell( *cell, entryExitLayerMap);
        double pathInMM = m_pathLenUtil.get3DPathLength(*cell, pos->second.first, pos2->second.second,drFix,dzFix);
        double totpath =  (pos->second.first-pos2->second.second).mag();
        path = totpath!=0 ? pathInMM / totpath : 0.; 
        if(path>0||cell->energy()>50.) { 
          ATH_MSG_DEBUG(" cell sampling and size "  << cell->caloDDE()->getSampling() << " cell energy " << cell->energy() << " drFix " << drFix << " dzFix " << dzFix << " path " << path << " length TG " << totpath);
          ATH_MSG_DEBUG(" cell dr "  << cell->caloDDE()->dr() << " cell dz " << cell->caloDDE()->dz() << " deta " << cell->caloDDE()->deta()); 
        }
      }

      //// calculate 2D path length (method2)
      double path2 = 0.;

      if(!use3D) path2 = pathInsideCell(*cell, pos->second.first, pos2->second.second );

      if( path2 <= 0. && path <= 0. ) continue;

      // auto entrancePair = entryExitLayerMap.find(entranceID);
      auto eLossPair = eLossLayerMap.find(sample);
      double eLoss = 0.;
//
// Just store total expected eloss
//
      if( eLossPair != eLossLayerMap.end() ){
        eLoss = eLossPair->second;
        if(sample == CaloSampling::PreSamplerB) {
          auto eLossPair2 = eLossLayerMap.find(CaloSampling::EMB1);
          if( eLossPair2 != eLossLayerMap.end() ){
            eLoss = 0.5*(eLossPair->second) +  0.5*(eLossPair2->second); 
          }
        } else if(sample == CaloSampling::EMB1) {
          auto eLossPair2 = eLossLayerMap.find(CaloSampling::PreSamplerB);
          if( eLossPair2 != eLossLayerMap.end() ){
            eLoss = 0.5*(eLossPair->second) +  0.5*(eLossPair2->second); 
          }
        } 
      } // IF

      ATH_MSG_DEBUG(" PATH3D = " << path << " PATH2D = " << path2 << " eLoss " << eLoss << " cell energy " << (cell)->energy() << " radius " << cell->caloDDE()->r() << " phi " << cell->caloDDE()->phi() << " dr " << cell->caloDDE()->dr() << " dphi " << cell->caloDDE()->dphi()  << " x " << cell->caloDDE()->x() << " y " << cell->caloDDE()->y() << " z " << cell->caloDDE()->z() << " dx " << cell->caloDDE()->dx() << " dy " << cell->caloDDE()->dy() << " dz " << cell->caloDDE()->dz() << " volume " << cell->caloDDE()->volume() <<  " ratioCheck " << ratioCheck  );

      cellIntersections.push_back( std::make_pair(cell, new ParticleCellIntersection( *cell, eLoss, use3D?path:path2) ) );
      //cellIntersections.push_back( std::make_pair(cell, new ParticleCellIntersection( *cell, eLoss, path/pathLenLayerMap[sample]) ) );
      //cellIntersections.push_back( std::make_pair(cell, new ParticleCellIntersection( *cell, path2, path/pathLenLayerMap[sample]) ) ); //tmp hack
    } 
    ATH_MSG_DEBUG(" added cell intersections  " << cellIntersections.size() );
  }


  bool ParticleCaloCellAssociationTool::particleCellAssociation( const xAOD::IParticle& particle,  
                                                                 const ParticleCellAssociation*& association, float dr, 
                                                                 const CaloCellContainer* container, bool useCaching ) const {


    ATH_MSG_DEBUG(" particleCellAssociation: ptr " << &particle << " dr " << dr << " useCaching " << useCaching);

    // reset pointer
    association = 0;
    // check if link is already there
    if( useCaching ){
      if( particle.isAvailable< ParticleCellAssociation* >("cellAssociation") ){
        ParticleCellAssociation* theAssociation = particle.auxdata< ParticleCellAssociation* >("cellAssociation");
        if( theAssociation ){
          // check whether the cached association is from the same container
          if( container && theAssociation->container() != container ){
            ATH_MSG_WARNING("Calling cached version with different container pointer");
            return false;
          }
          // check if we need to resize the cone
          if( dr > theAssociation->associationConeSize() ){
            std::vector<const CaloCell*> cells;
            ATH_MSG_DEBUG(" dr larger then cached dr: " << dr << " cached dr " << theAssociation->associationConeSize());
            associateCells(*theAssociation->container(),theAssociation->caloExtension(),dr,cells);
            theAssociation->updateData(std::move(cells),dr);
          }
          association = theAssociation;
          ATH_MSG_DEBUG("Found existing calo extension");
          return true;
        }
      }
    }

    // get the extrapolation into the calo
    std::unique_ptr<Trk::CaloExtension> caloExtension = m_caloExtensionTool->caloExtension(particle);
    if( !caloExtension ) {
      ATH_MSG_DEBUG("Failed to get calo extension");      
      return false;
    }
    if( caloExtension->caloLayerIntersections().empty()){
      ATH_MSG_DEBUG( "Received a caloExtension object without track extrapolation");
      return false;
    }
    
    //retrieve the cell container if not provided, return false it retrieval failed
    if (!container) {
      if (m_cellContainerName.key().empty()) {
	ATH_MSG_DEBUG("Failed to get calo cell container");
	return false;
      }
      SG::ReadHandle<CaloCellContainer> cccHdl(m_cellContainerName);
      container=&(*cccHdl);
    }
    std::vector<const CaloCell*> cells;
    // update cone size in case it is smaller than the default
    if( dr < m_coneSize ) dr = m_coneSize;
    associateCells(*container,*caloExtension,dr,cells);    
    
    // get cell intersections
    ParticleCellAssociation::CellIntersections cellIntersections;
    getCellIntersections(*caloExtension,cells,cellIntersections);
    
    //From here on the ParticleCellAssociation has ownership of the caloExtension
    //We can not assume the CaloExtension stays alive in Storegate etc etc
    ParticleCellAssociation* theAssocation = new ParticleCellAssociation( caloExtension.release(), std::move(cells), dr, 
                                                                          std::move(cellIntersections), container );

    // now add the extension to the output collection so we are not causing any leaks
    ParticleCellAssociationCollection* collection = 0;
    if( !evtStore()->contains<ParticleCellAssociationCollection>(m_cellContainerName.key()) ){
      collection = new ParticleCellAssociationCollection();
      if( evtStore()->record( collection, m_cellContainerName.key()).isFailure() ) {
        ATH_MSG_WARNING( "Failed to record output collection, will leak the ParticleCaloExtension");
        delete collection;
        collection = 0;
      }
    }else{
      //ATH_MSG_WARNING("Updating existing ParticleCaloExtension container");
      if(evtStore()->retrieve(collection,m_cellContainerName.key()).isFailure()) {
        ATH_MSG_WARNING( "Unable to retrieve " << m_cellContainerName.key() << " will leak the ParticleCaloExtension" );
      }
    }
    if( collection ) collection->push_back(theAssocation);
    else{
      ATH_MSG_WARNING( "No ParticleCaloCellAssociationCollection, failing extension to avoid memory leak");
      delete theAssocation;
      theAssocation = 0;
    }

    association = theAssocation;
    if( useCaching ) particle.auxdecor< ParticleCellAssociation* >("cellAssociation") = theAssocation;


    return true;

  }

  void ParticleCaloCellAssociationTool::associateCells( const CaloCellContainer& container, 
                                                        const Trk::CaloExtension& caloExtension,
                                                        float dr,
                                                        std::vector<const CaloCell*>& cells ) const {    

    const Trk::TrackParameters*  pars = caloExtension.caloEntryLayerIntersection();
    if(!pars) {
      ATH_MSG_WARNING( " NO TrackParameters caloExtension.caloEntryLayerIntersection() ");
      return;
    } 

    double eta = pars->position().eta();
    double phi = pars->position().phi();

    // Use Calorimeter list for CPU reasons
    CaloCellList myList(&container);
    myList.select(eta,phi,dr);
    cells.reserve(myList.ncells());
    cells.insert(cells.end(),myList.begin(),myList.end());
 
    ATH_MSG_DEBUG("associated cells " << cells.size() << " using cone " << dr );
    
  }

} // end of namespace Trk
