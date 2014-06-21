/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauCellsDetailsFillerTool.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date December, 2009
 * @brief Block filler tool for tau track details.
 */


#include "TauCellsDetailsFillerTool.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloEvent/CaloCellLink.h"
#include "CaloEvent/CaloCell.h"
#include "CaloUtils/CaloVertexedCell.h"
#include "AthenaKernel/errorcheck.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "VxVertex/VxContainer.h"
#include "CLHEP/Geometry/Vector3D.h"
	
namespace D3PD {
    
    
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
    TauCellsDetailsFillerTool::TauCellsDetailsFillerTool (const std::string& type,
							  const std::string& name,
							  const IInterface* parent)
	: BlockFillerTool<Analysis::TauJet> (type, name, parent),
	  m_cellenergy(0), m_celleta(0), m_cellphi(0), m_samplingID(0)
    {
      book().ignore(); // Avoid coverity warnings.
    }
    
    
/**
 * @brief Book variables for this block.
 */
    StatusCode TauCellsDetailsFillerTool::book()
    {
	CHECK( addVariable ("cell_E",  m_cellenergy )  );
	CHECK( addVariable ("cell_eta",  m_celleta )  );
	CHECK( addVariable ("cell_phi",  m_cellphi )  );
	CHECK( addVariable ("cell_eta_atTJVA",  m_celleta_atTJVA )  );
	CHECK( addVariable ("cell_phi_atTJVA",  m_cellphi_atTJVA )  );
	CHECK( addVariable ("cell_n",  m_numcells )  );
	CHECK( addVariable ("cell_nEffCells",  m_numEffCells )  );
	CHECK( addVariable ("cell_nEffStripCells", m_numEffStripCells )  );
	CHECK( addVariable ("cell_samplingID",  m_samplingID )  );
	
	return StatusCode::SUCCESS;
    }
    
    
/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
    StatusCode TauCellsDetailsFillerTool::fill (const Analysis::TauJet& p)
    {
	NavigationToken<CaloCell> nt;
	std::vector<const CaloCell*> Cells;
	// loop over all cells of the tau (placed there by the tauSeedBuilder)
	p.fillToken( nt ); 
	NavigationToken<CaloCell>::const_iterator firstcell = nt.begin();
	NavigationToken<CaloCell>::const_iterator lastcell = nt.end();
	unsigned int num_cells = 0;
	float energysum = 0.;
	float squareEsum = 0.;
	float energysum_strip = 0.;
	float squareEsum_strip = 0.;
	for ( ; firstcell != lastcell; firstcell++ )
	{
	    ++num_cells;
	    Cells.push_back(*firstcell);
	    energysum += firstcell->energy() ;
	    squareEsum += (firstcell->energy())*(firstcell->energy()) ;
	}

	if ( squareEsum > 0.00001 )
	    *m_numEffCells = energysum*energysum/squareEsum;

	else
	    *m_numEffCells = 0;

	*m_numcells = num_cells;    
	
	// now insert cells into event ordered by energy
	// this makes it much faster to recalculate cell-based
	// variables in macros later


	if(Cells.size() > 0)
	{
	    //Retrieve TJVA vertex
	    const Trk::RecVertex* myVtx = p.origin();
            Amg::Vector3D TauJetVertex;
	    if (myVtx!=0)
	    {
                const Amg::Vector3D& pos = myVtx->position();
		float vtx_x = pos.x();
		float vtx_y = pos.y();
		float vtx_z = pos.z();
		TauJetVertex = Amg::Vector3D(vtx_x, vtx_y, vtx_z);
	    }
	    
	    AnalysisUtils::Sort::e (&Cells);
	    
	    for (unsigned int i=0; i<num_cells; ++i )
	    {
		float cellenergy = Cells[i]->energy();
		const CaloDetDescrElement *dde = Cells[i]->caloDDE();
		int sampling = dde->getSampling();
		if (sampling == CaloCell_ID::EMB1 || sampling == CaloCell_ID::EME1)
		{
		    energysum_strip += cellenergy;
		    squareEsum_strip += cellenergy*cellenergy;

		}

		m_cellenergy->push_back(cellenergy);
		m_celleta->push_back(Cells[i]->eta());
		m_cellphi->push_back(Cells[i]->phi());
		m_samplingID->push_back(sampling);
		
		// Get corrected directions for the cells
                CaloVertexedCell vxCell (*Cells[i], TauJetVertex);
		m_celleta_atTJVA->push_back(vxCell.eta());
		m_cellphi_atTJVA->push_back(vxCell.phi());
	    }

	if ( squareEsum_strip > 0.00001 )
	    *m_numEffStripCells = energysum_strip*energysum_strip/squareEsum_strip;

	}
	return StatusCode::SUCCESS;
    } // end fill method
    
} // namespace D3PD
