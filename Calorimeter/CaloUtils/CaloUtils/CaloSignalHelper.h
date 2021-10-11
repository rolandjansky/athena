/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOSIGNALHELPER_H
#define CALOUTILS_CALOSIGNALHELPER_H

#include "CaloInterface/ICaloCellSelector.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCellContainer.h"

//#include <iostream>

struct CaloSignalHelper
{
  public:
  static double CaloESum(const CaloCellContainer* cellContainer)
    {
      double eTot = 0.;
      for (const CaloCell* cell : *cellContainer)
	{
	  eTot += cell->e();
	}
      return eTot;
    };

  static double CaloESum(const CaloCellContainer* cellContainer,
			 const ICaloCellSelector& aSelector)
    {
      double eTot = 0.;
      for (const CaloCell* cell : *cellContainer)
	{
	  if ( aSelector.accept(cell) ) eTot += cell->e();
	}
      return eTot;
    }


  static double CaloESum(const CaloCellContainer* cellContainer,
			 const CaloCell_ID::SUBCALO& idCalo)
    {
      double eTot = 0.;
      if ( cellContainer->nCellsCalo(idCalo) == 0 )
	{
	  return eTot;
	}

      for ( CaloCellContainer::const_iterator firstCell = 
	      cellContainer->beginConstCalo(idCalo);
	    firstCell != cellContainer->endConstCalo(idCalo);
	    ++firstCell
	    )
	{
	  eTot += (*firstCell)->e();
	}
      return eTot;
    };

  static double CaloESum(const CaloCellContainer* cellContainer,
			 const CaloCell_ID::SUBCALO& idCalo,
			 const ICaloCellSelector& aSelector)
    {
      double eTot = 0.;
      if ( cellContainer->nCellsCalo(idCalo) == 0 )
	{
	  return eTot;
	}

      for ( CaloCellContainer::const_iterator firstCell = 
	      cellContainer->beginConstCalo(idCalo);
	    firstCell != cellContainer->endConstCalo(idCalo);
	    ++firstCell
	    )
	{
	  if ( aSelector.accept(*firstCell)) eTot += (*firstCell)->e();
	}
      return eTot;
    };

  static double CaloESum(const CaloCellContainer* cellContainer,
			 const CaloSampling::CaloSample& idSample)
    {
      double eTot = 0.;

      // find out which calorimeter module
      CaloCell_ID::SUBCALO idCalo;
      switch ( idSample )
	{
	case CaloSampling::PreSamplerB:
	case CaloSampling::EMB1:
	case CaloSampling::EMB2:
	case CaloSampling::EMB3:
	case CaloSampling::PreSamplerE:
	case CaloSampling::EME1:
	case CaloSampling::EME2:
	case CaloSampling::EME3:
	  idCalo = CaloCell_ID::LAREM;
	  break;
	case CaloSampling::HEC0:
	case CaloSampling::HEC1:
	case CaloSampling::HEC2:
	case CaloSampling::HEC3:
	  idCalo = CaloCell_ID::LARHEC;
	  break;
	case CaloSampling::TileBar0:
	case CaloSampling::TileBar1:
	case CaloSampling::TileBar2:
	case CaloSampling::TileGap1:
	case CaloSampling::TileGap2:
	case CaloSampling::TileGap3:
	case CaloSampling::TileExt0:
	case CaloSampling::TileExt1:
	case CaloSampling::TileExt2:
	  idCalo = CaloCell_ID::TILE;
	  break;
	case CaloSampling::FCAL0:
	case CaloSampling::FCAL1:
	case CaloSampling::FCAL2:
	  idCalo = CaloCell_ID::LARFCAL;
	  break;
	default:
	  return eTot;
	}
      // std::cout << "[CaloSignalHelper::CaloESum] - sum energies for "
		// << "sampling " << idSample
		// << " in calo " << idCalo << std::endl;

      // extract sampling energy sums
      for ( CaloCellContainer::const_iterator firstCell = 
	      cellContainer->beginConstCalo(idCalo);
	    firstCell != cellContainer->endConstCalo(idCalo);
	    ++firstCell )
	{
	  CaloSampling::CaloSample theSample = (*firstCell)->caloDDE()->getSampling();
	  //	  std::cout << "this sample/requested sample: "
	  //		    << theSample << "/" << idSample
	  //		    << std::endl;
	  if ( theSample == idSample ) eTot += (*firstCell)->e(); 
	}
      // std::cout << "ESampling = " << eTot << std::endl;

      return eTot;
    };

  static double CaloESum(const CaloCellContainer* cellContainer,
			 const CaloSampling::CaloSample& idSample,
			 const ICaloCellSelector& aSelector)
    {
      double eTot = 0.;

      // find out which calorimeter module
      CaloCell_ID::SUBCALO idCalo;
      switch ( idSample )
	{
	case CaloSampling::PreSamplerB:
	case CaloSampling::EMB1:
	case CaloSampling::EMB2:
	case CaloSampling::EMB3:
	case CaloSampling::PreSamplerE:
	case CaloSampling::EME1:
	case CaloSampling::EME2:
	case CaloSampling::EME3:
	  idCalo = CaloCell_ID::LAREM;
	  break;
	case CaloSampling::HEC0:
	case CaloSampling::HEC1:
	case CaloSampling::HEC2:
	case CaloSampling::HEC3:
	  idCalo = CaloCell_ID::LARHEC;
	  break;
	case CaloSampling::TileBar0:
	case CaloSampling::TileBar1:
	case CaloSampling::TileBar2:
	case CaloSampling::TileGap1:
	case CaloSampling::TileGap2:
	case CaloSampling::TileGap3:
	case CaloSampling::TileExt0:
	case CaloSampling::TileExt1:
	case CaloSampling::TileExt2:
	  idCalo = CaloCell_ID::TILE;
	  break;
	case CaloSampling::FCAL0:
	case CaloSampling::FCAL1:
	case CaloSampling::FCAL2:
	  idCalo = CaloCell_ID::LARFCAL;
	  break;
	default:
	  return eTot;
	}
      // std::cout << "[CaloSignalHelper::CaloESum] - sum energies for "
		// << "sampling " << idSample
		// << " in calo " << idCalo << std::endl;

      // extract sampling energy sums
      for ( CaloCellContainer::const_iterator firstCell = 
	      cellContainer->beginConstCalo(idCalo);
	    firstCell != cellContainer->endConstCalo(idCalo);
	    ++firstCell )
	{
	  CaloSampling::CaloSample theSample = (*firstCell)->caloDDE()->getSampling();
	  //	  std::cout << "this sample/requested sample: "
	  //		    << theSample << "/" << idSample
	  //		    << std::endl;
	  if ( theSample == idSample && aSelector.accept(*firstCell) ) 
	    eTot += (*firstCell)->e(); 
	}
      // std::cout << "ESampling = " << eTot << std::endl;
      return eTot;
    };
};
#endif
