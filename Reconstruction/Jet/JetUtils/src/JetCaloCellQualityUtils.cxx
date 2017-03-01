/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "JetUtils/JetCaloCellQualityUtils.h"

#include "TileEvent/TileCell.h"
#include "JetUtils/JetCellAccessor.h"

namespace jet {


  double JetCaloCellCalculator::operator()(const xAOD::Jet* jet, xAOD::JetConstitScale ){
    if (! setupJet( jet ) ) return 0;
    size_t nConstit = jet->numConstituents();
    if( nConstit == 0) return true;
    
    jet::JetCellAccessor::const_iterator it = jet::JetCellAccessor::begin(jet);
    jet::JetCellAccessor::const_iterator itE = jet::JetCellAccessor::end(jet);

    for( ; it != itE ; it++) {
      processCell(*it, it.weight());
    }    
    return jetCalculation();
  }





  void JetCaloCellCalculations::addCellCalculator(JetCaloCellCalculator* c){
    m_cellcalculators.push_back(c);
    JetCaloCalculations::addCalculator( c );
  }

  std::vector<double> JetCaloCellCalculations::process(const xAOD::Jet* jet) const {

    size_t nConstit = jet->numConstituents();
    std::vector<double> results;
    results.reserve(m_calculators.size());

    if( nConstit == 0) {
      results.resize(m_calculators.size(),0);
      return results;
    }

    // Clone calculators
    std::vector<JetCaloCellCalculator*> clonedCalc;
    clonedCalc.reserve(m_calculators.size());
    for( const JetCaloCellCalculator *calc : m_cellcalculators) clonedCalc.push_back( calc->clone() );  



    // navigate from cell
    jet::JetCellAccessor::const_iterator it = jet::JetCellAccessor::begin(jet);
    jet::JetCellAccessor::const_iterator itE = jet::JetCellAccessor::end(jet);
    for( ; it != itE ; it++) {
      double w= it.weight();
      const CaloCell* cell = *it;
      for( JetCaloCellCalculator *calc : clonedCalc) {
        calc->processCell(cell, w);
      }
    }

    // copy results & clear the cloned calc
    for(JetCaloCalculator* calc: clonedCalc) {
      results.push_back( calc->jetCalculation() );
      delete calc;
    }
    
    return results;
  }









  bool timeAndQualityDefined(const CaloCell* theCell){
    // ensure timing information is defined
    bool timeIsDefined(false);
    if(theCell->caloDDE()->is_tile())	  
      {
        if ( (theCell->provenance()) & 0x8080)
          timeIsDefined = true;
      }
    else //is_lar
      {
        if ( (theCell->provenance() & 0x2000) &&
             !(theCell->provenance()  & 0x0800 ) // do not take masked-patched cells int
             )         timeIsDefined = true;
      }
    return timeIsDefined;
  }



  bool JetCalcnLeadingCells_fromCells::processCell(const CaloCell *thisCell, weight_t )
  {

    m_cell_energies.push_back(thisCell->e());
    m_sumE_cells+=thisCell->e();
    return true;
  }


  bool JetCalcOutOfTimeEnergyFraction_fromCells::processCell(const CaloCell *thisCell, weight_t ) {
    m_sumE+=thisCell->e();
  
    // ensure timing information is defined
    bool timeIsDefined = timeAndQualityDefined(thisCell);  
    if ( timeIsDefined ) {
      double time = thisCell->time();
      if(fabs(time)>timecut)m_sumE_OOT+=thisCell->e();
    }
    return true;
  }


  bool JetCalcTimeCells_fromCells::processCell(const CaloCell *theCell, weight_t weight)
  {
    // ensure timing information is defined
    bool timeIsDefined = timeAndQualityDefined(theCell);
  
    if ( timeIsDefined )
      {
        double thisNorm = weight * fabs(weight) * theCell->e() * theCell->e();
        double thisTime = thisNorm * theCell->time();
      
        m_time += thisTime;
        m_norm += thisNorm;
      }
  
    return true;
  }


  bool JetCalcAverageLArQualityF_fromCells::processCell(const CaloCell *theCell, weight_t weight)
  {
    // ensure timing information is defined ...
    bool timeIsDefined = timeAndQualityDefined(theCell);
    // ... and restrict to LAr cells
    if ( timeIsDefined 	&& ( theCell->caloDDE()->is_tile() == m_useTile ) )
      {
        double thisNorm = weight * fabs(weight) * theCell->e() * theCell->e();
        double thisQf = thisNorm * theCell->quality();

        m_qf += thisQf;
        m_norm += thisNorm;
      }
  
    return true;
  }


  bool JetCalcQuality_fromCells::processCell(const CaloCell *theCell, weight_t)
  {
    // Calculate the fraction of energy in cells flagged with a 'bad' quality
    // The cuts used are fairly approximative, and are used to flag obviously problematic cells
    // The quality factor might have an energy/gain dependence, so choosing a single cut
    // is surely a coarse approximation...

    if(theCell->caloDDE()->is_tile() && includeTile )
      {
        const TileCell* theTileCell = dynamic_cast<const TileCell*>(theCell);
        if(theTileCell)
          {
            if((theCell->provenance()) & 0x8080)
              {
                m_totE += theCell->e();
                int tileQuality = std::max(theTileCell->qual1(), theTileCell->qual2());
                if(tileQuality > TileQualityCut)
                  m_badE += theCell->e();
              }
          }
      }
    else
      {
        if( includeLAr && 
            (theCell->provenance() & 0x2000) &&
            !(theCell->provenance()  & 0x0800 ) // do not take masked-patched cells into account
            )
          {
            m_totE += theCell->e();
            if(theCell->quality() > LArQualityCut)
              m_badE += theCell->e();
          }
      }
  
    return true;
  }


  bool JetCalcQualityHEC_fromCells::processCell(const CaloCell *theCell, weight_t)
  {
    if(theCell->caloDDE()->is_lar_hec())
      {
        if( ((theCell->provenance()) & 0x2000) &&
            !(theCell->provenance()  & 0x0800 ) )// do not take masked-patched cells into account
          {
            m_totE += theCell->e();
            if(theCell->quality() > LArQualityCut)
              m_badE += theCell->e();
          }
      }
  
    return true;
  }


  bool JetCalcNegativeEnergy_fromCells::processCell(const CaloCell *theCell, weight_t)
  {
    if(theCell->e()<-2500)
      m_totE += theCell->e();
    return true;
  }


  bool JetCalcCentroid_fromCells::processCell(const CaloCell */*theCell*/, weight_t)
  {  
    return true;
  }

  // // Not implemented yet.
  // bool JetCalcBadCellsFrac_fromCells::processCell(const CaloCell *, weight_t) { return true; }
    
}
#endif
