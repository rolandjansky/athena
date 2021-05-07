/*
*   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FFexEMFinder.h"
#include "FwdCell.h"
#include "FwdCellReader.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_Base_ID.h"
#include <map>

//function to build up the location of all seeds
StatusCode FFexEMFinder::SeedGrid(std::vector<CaloCell*> forwardCells, std::shared_ptr<FFexEMFinder::Seed> seeds,std::shared_ptr<FFexEMFinder::SeedXY> seedsXY){

	// Find first layer cells of FCAL to define grid
	std::vector<CaloCell*> fcalLayer1Cells;
	std::vector<CaloDetDescrElement*> fcalLayer1CellsDDE;
	for(unsigned i=0; i<forwardCells.size();i++){
		const CaloCell*cell = forwardCells.at(i);
		if(cell->caloDDE()->getSubCalo()==(CaloCell_Base_ID::SUBCALO)2){
			if(cell->caloDDE()->getLayer()==1){
				fcalLayer1Cells.push_back((CaloCell*)cell);
				fcalLayer1CellsDDE.push_back((CaloDetDescrElement*)cell->caloDDE());
			}
		}

	}//cell loop

	// eta and phi coordinates for EMEC seeding
	std::vector<float> phiSeeds;
	std::vector<float> etaSeeds;

	for(int i=-33;i<33;i++){//-31,31
		if(fabs(i)<25) continue;// forwardCells, 2.5
		etaSeeds.push_back(i*0.1);
	}
	for(int i=-32;i<32;i++){
		phiSeeds.push_back((TMath::Pi()/32.) *i);
	}

	// Create FCAL xy seeding from layer 1 cell positions
	//std::cout<<"Size fcal layer 1 cells: "<<fcalLayer1Cells.size()<<std::endl;
	for(unsigned i=0; i<fcalLayer1Cells.size();i++){
		const CaloCell*cell = fcalLayer1Cells.at(i);
		// cell size is 30mm in x and 25.9mm in y in FCAL1 for large cells
		 //std::cout<<"dx, dy, dz: "<<cell->caloDDE()->dx()<<", "<<cell->caloDDE()->dy()<<", "<<cell->caloDDE()->dz()<<std::endl;
		
		// use bottom left of cells
		(seedsXY->x).push_back(cell->caloDDE()->x()-cell->caloDDE()->dx()/2.);
		(seedsXY->y).push_back(cell->caloDDE()->y()-cell->caloDDE()->dy()/2.);
		if(cell->caloDDE()->z()>0) (seedsXY->z).push_back(cell->caloDDE()->z()-cell->caloDDE()->dz());// dz = half width, dx/y = full width...
		if(cell->caloDDE()->z()<0) (seedsXY->z).push_back(cell->caloDDE()->z()+cell->caloDDE()->dz());// dz = half width, dx/y = full width...

		 //std::cout<<"XYZ seed: "<<cell->caloDDE()->x()-cell->caloDDE()->dx()/2.<<", "<<cell->caloDDE()->y()-cell->caloDDE()->dy()/2.<<", "<<cell->caloDDE()->z()<<"+-"<<cell->caloDDE()->dz()<<std::endl;
	}

	sort(etaSeeds.begin(),etaSeeds.end());
	sort(phiSeeds.begin(),phiSeeds.end());

	seeds->eta = etaSeeds;

	for(unsigned int i=0;i<etaSeeds.size();i++){
		(seeds->phi).push_back(phiSeeds);
	}

	return StatusCode::SUCCESS;
}

//To find the seeds as local maxima
StatusCode FFexEMFinder::SeedFinding(std::vector<CaloCell*> forwardCells, std::shared_ptr<FFexEMFinder::Seed> seeds, std::shared_ptr<FFexEMFinder::SeedXY> seedsXY,float range){

	//split by hemisphere for speedup
	std::vector<CaloCell*> forwardCells_posEta;
	std::vector<CaloCell*> forwardCells_negEta;

	for(unsigned t=0; t<forwardCells.size(); t++){
		const CaloCell*cell=forwardCells.at(t);
		if(cell->eta()>0)forwardCells_posEta.push_back((CaloCell*)cell);
		else forwardCells_negEta.push_back((CaloCell*)cell);
	}



	 //std::cout<<"///////////////////////////////////////////////////////////"<<std::endl;
	 //std::cout<<"/////////////////////// EMEC //////////////////////////////"<<std::endl;
	 //std::cout<<"///////////////////////////////////////////////////////////"<<std::endl;
	for(unsigned i=0; i<seeds->eta.size(); i++){

		std::vector<float> vec_emCore_et;
		std::vector<float> vec_emCore_pairEt;
		std::vector<float> vec_had_et;
		std::vector<float> vec_emIsolaion_et;
		std::vector<int> vec_emCore_nCells;
		std::vector<int> vec_had_nCells;
		std::vector<int> vec_emIsolaion_nCells;
		std::vector<float> vec_showerDepthZ;
		std::vector<float> vec_showerVarianceR;
		std::vector<float> vec_showerVarianceZ;
		std::vector<float> vec_showerCentreFirstLayer_eta;
		std::vector<float> vec_showerCentreFirstLayer_phi;
		std::vector<float> vec_fractionMaxEnergyCell;
		std::vector<float> vec_centreEta;
		std::vector<float> vec_centrePhi;
		std::vector<float> vec_emCore_centreEta;
		std::vector<float> vec_emCore_centrePhi;
		std::vector<std::vector<float>> vec_emecCoreEt;

		for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){
			 //std::cout<<"--------------Start loop seeds-----------------"<<std::endl;

			std::vector<CaloCell*> cellsInBox;
			std::vector<CaloCell*> emCoreCells_emec;
			std::vector<CaloCell*> emIsolationCells_emec;
			std::vector<CaloCell*> hadCells_hec;


			cellsInBox.clear();
			emCoreCells_emec.clear();
			emIsolationCells_emec.clear();
			hadCells_hec.clear();

			float eta = seeds->eta.at(i);
			float phi = seeds->phi.at(i).at(ii);
			float emecEt = 0;

			std::vector<CaloCell*> hemisphereCells;

			if(eta>0) hemisphereCells = forwardCells_posEta;
			else hemisphereCells = forwardCells_negEta;

			for(unsigned t=0; t<hemisphereCells.size(); t++){
				const CaloCell*cell = hemisphereCells.at(t);
				if(!isInBox(cell->eta(),eta,0.2,cell->phi(),phi,(TMath::Pi()/32.)*2)) continue;

				cellsInBox.push_back((CaloCell*)cell);

				// Distinguish 0.2 x 0.2 and 0.4 x 0.4 window for algorithm
				if(isInBox(cell->eta(),eta,0.1,cell->phi(),phi,(TMath::Pi()/32.)*1)){
					// HEC
					if(cell->caloDDE()->getSubCalo()==(CaloCell_Base_ID::SUBCALO)1){
						hadCells_hec.push_back((CaloCell*)cell);
					}
					// EMEC
					if(cell->caloDDE()->getSubCalo()==(CaloCell_Base_ID::SUBCALO)0){
						emCoreCells_emec.push_back((CaloCell*)cell);
						emecEt += cell->et();
					}
					// FCAl Overlap currently enforced
					if(cell->caloDDE()->getSubCalo()==(CaloCell_Base_ID::SUBCALO)2){
						if(cell->caloDDE()->getLayer()==1){
							emCoreCells_emec.push_back((CaloCell*)cell);
							emecEt += cell->et();
						}
					}
				}
				else{
					// HEC
					if(cell->caloDDE()->getSubCalo()==(CaloCell_Base_ID::SUBCALO)1){
						hadCells_hec.push_back((CaloCell*)cell);
					}
					// EMEC
					if(cell->caloDDE()->getSubCalo()==(CaloCell_Base_ID::SUBCALO)0){
						emIsolationCells_emec.push_back((CaloCell*)cell);
					}
				}
			}
			// Optional: use z face, not centre!

			// Calculate electron ID-inspired shower shape variables using cells in window
			std::vector<float> centreCoordinates = firstlayerShowerCentre(cellsInBox);
			float centreEta = centreCoordinates.at(3);
			float centrePhi = centreCoordinates.at(4);

			std::vector<float> varFromCentre = varianceFromCentre(cellsInBox);

			float fracMaxEnergyCell = fractionMaxEnergyCell(cellsInBox);
			float showerDZ = showerDepthZ(cellsInBox);

			vec_showerDepthZ.push_back(showerDZ);
			vec_fractionMaxEnergyCell.push_back(fracMaxEnergyCell);
			vec_showerVarianceR.push_back(varFromCentre.at(0));
			vec_showerVarianceZ.push_back(varFromCentre.at(1));
			vec_showerCentreFirstLayer_eta.push_back(centreEta);
			vec_showerCentreFirstLayer_phi.push_back(centrePhi);
			vec_centreEta.push_back(centreEta);
			vec_centrePhi.push_back(centrePhi);

			// Calculate standard SLW energy variables
			// Note the difference in Pair Et and full Et
			//std::cout<<"EM Core size EMEC: "<<emCoreCells_emec.size()<<std::endl;
			float et = 0;
			float pair1_et = 0;
			float pair2_et = 0;
			float pair3_et = 0;
			float pair4_et = 0;
			float emCore_centreEta = 0;
			float emCore_centrePhi = 0;
			std::vector<float> emecCoreEt;
			if(emCoreCells_emec.size()>0){
				for(unsigned m=0;m<emCoreCells_emec.size();m++){
					CaloCell* cell = emCoreCells_emec.at(m);

					et += cell->et();
					emecCoreEt.push_back(cell->et());
					emCore_centreEta += cell->eta();
					emCore_centrePhi += cell->phi();
					float etCell = cell->et();
					float etaCell = cell->eta();
					float phiCell = cell->phi();
					float deta = eta-etaCell;
					float dphi = deltaPhiAtlas(phi,phiCell);
					if(deta<0) pair1_et += etCell;
					if(deta<0) pair2_et += etCell;
					if(dphi<0) pair3_et += etCell;
					if(dphi>0) pair4_et += etCell;
					//if(dphi==0 || deta ==0) std::cout<<"deta==0? "<<(deta==0)<<" dphi=0? "<<(dphi==0)<<std::endl;
				}
				//std::cout<<"Pair1 ET: "<<pair1_et<<std::endl;
				//std::cout<<"Pair2 ET: "<<pair2_et<<std::endl;
				//std::cout<<"Pair3 ET: "<<pair3_et<<std::endl;
				//std::cout<<"Pair4 ET: "<<pair4_et<<std::endl;

				std::vector<float>pairEnergies;
				pairEnergies.clear();
				pairEnergies.push_back(pair1_et);
				pairEnergies.push_back(pair2_et);
				pairEnergies.push_back(pair3_et);
				pairEnergies.push_back(pair4_et);
				float max = *std::max_element(pairEnergies.begin(),pairEnergies.end());
				//std::cout<<"Max Pair ET: "<<max<<std::endl;
				//std::cout<<"Full ET: "<<et<<std::endl;
				//if(emecEt!=et) std::cout<<"Et Calculation not working: "<<"List: "<<et<<", on-the-fly: "<<emecEt <<std::endl;
				if(fabs(et)<=2000) et=0;
				vec_emCore_et.push_back(et);
				vec_emCore_centreEta.push_back(emCore_centreEta);
				vec_emCore_centrePhi.push_back(emCore_centrePhi);
				vec_emCore_pairEt.push_back(max);
				vec_emecCoreEt.push_back(emecCoreEt);
			}else{
				vec_emCore_et.push_back(-999999);
				vec_emCore_pairEt.push_back(-999999);
				vec_emCore_centreEta.push_back(-999999);
				vec_emCore_centrePhi.push_back(-999999);
				vec_emecCoreEt.push_back({-9999999});
			}
			vec_emCore_nCells.push_back(emCoreCells_emec.size());

			//std::cout<<"EM Isolation size EMEC: "<<emIsolationCells_emec.size()<<std::endl;
			if(emIsolationCells_emec.size()>0){
				float et=sumOfEnergy(emIsolationCells_emec);
				vec_emIsolaion_et.push_back(et);
				//std::cout<<"Isolation energy: "<<et<<std::endl;
			}else{
				vec_emIsolaion_et.push_back(-999999);
			}
			vec_emIsolaion_nCells.push_back(emIsolationCells_emec.size());

			//std::cout<<"Had size HEC: "<<hadCells_hec.size()<<std::endl;

			// One HEC cell contains up to four EMEC cells. Need to account for this in energy calculation (overlap etc.)
			if(hadCells_hec.size()>0){
				float hecEnergy = -999999;
				// if two hec cells behind 0.2x0.2 window use half of each energy
				// Some of the following cases exist if the search window is mostly in FCAL and only the outermost HEC cells are contained.
				if(hadCells_hec.size()==4){
					hecEnergy = sumOfEnergy(hadCells_hec)/1.;
				}
				if(hadCells_hec.size()==6){
					hecEnergy = sumOfEnergy(hadCells_hec)/1.5;
				}
				if(hadCells_hec.size()==8){
					hecEnergy = sumOfEnergy(hadCells_hec)/2.;
				}
				if(hadCells_hec.size()==12){
					hecEnergy = sumOfEnergy(hadCells_hec)/3.;
				}
				if(hadCells_hec.size()==16){
					hecEnergy = sumOfEnergy(hadCells_hec)/4.;
				}
				if(hadCells_hec.size()==18){
					hecEnergy = sumOfEnergy(hadCells_hec)/4.5;
				}
				if(hadCells_hec.size()==24){
					hecEnergy = sumOfEnergy(hadCells_hec)/6.;
				}
				vec_had_et.push_back(hecEnergy);
				//std::cout<<"HEC energy: "<<hecEnergy<<std::endl;
			}else{
				vec_had_et.push_back(-999999);
			}
			vec_had_nCells.push_back(hadCells_hec.size());
		}

		seeds->showerDepthZ.push_back(vec_showerDepthZ);
		seeds->fractionMaxEnergyCell.push_back(vec_fractionMaxEnergyCell);
		seeds->showerVarianceR.push_back(vec_showerVarianceR);
		seeds->showerVarianceZ.push_back(vec_showerVarianceZ);
		seeds->showerCentreFirstLayer_eta.push_back(vec_centreEta);
		seeds->showerCentreFirstLayer_phi.push_back(vec_centrePhi);

		seeds->et.push_back(vec_emCore_pairEt);
		seeds->et_full.push_back(vec_emCore_et);
		seeds->isolationEt.push_back(vec_emIsolaion_et);
		seeds->hadEt.push_back(vec_had_et);
		seeds->core_nCells.push_back(vec_emCore_nCells);
		seeds->centreEta.push_back(vec_emCore_centreEta);
		seeds->centrePhi.push_back(vec_emCore_centrePhi);
		seeds->emecCoreEt.push_back(vec_emecCoreEt);

		seeds->isolation_nCells.push_back(vec_emIsolaion_nCells);
		seeds->had_nCells.push_back(vec_had_nCells);

		//std::cout<<"--------------End loop seeds-----------------"<<std::endl;

	}//seed eta loop

	//std::cout<<"--------------Finished loop seeds-----------------"<<std::endl;

	// Heavily inspired by jet algirithm
	// Determine local maximum to avoid multiple candidates from single hotspot
	//std::cout<<"--------------Start local max. calc-----------------"<<std::endl;
	for(unsigned iseed_eta=0; iseed_eta<seeds->eta.size(); iseed_eta++){

		std::vector<bool> tmp_max;
		for(unsigned iseed_phi=0; iseed_phi<seeds->phi.at(iseed_eta).size(); iseed_phi++){
			float et = seeds->et_full.at(iseed_eta).at(iseed_phi);
			// only seeds with Et > 2 GeV 
			if(et<2000) {// This value could be adjusted
				tmp_max.push_back(0);
				continue;
			}
			bool eta_n=1, eta_p=1, eta_0=1;


			for(unsigned i=iseed_eta+1; ;i++){
				if(i>=seeds->eta.size()) break;
				if(fabs(seeds->eta.at(i)-seeds->eta.at(iseed_eta))>range) break;

				for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){

					float dphi = deltaPhiAtlas(seeds->phi.at(iseed_eta).at(iseed_phi),seeds->phi.at(i).at(ii));
					if(fabs(dphi)>range) continue;

					if(seeds->et_full.at(iseed_eta).at(iseed_phi)<=seeds->et_full.at(i).at(ii)){
						eta_p = false;
						break;
					}
				}
			}

			for(int i=iseed_eta-1; ;i--){
				if(i<0) break;
				if(fabs(seeds->eta.at(iseed_eta)-seeds->eta.at(i))>range) break;

				for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){
					float dphi = deltaPhiAtlas(seeds->phi.at(iseed_eta).at(iseed_phi),seeds->phi.at(i).at(ii));
					if(fabs(dphi)>range) continue;
					if(seeds->et_full.at(iseed_eta).at(iseed_phi)<seeds->et_full.at(i).at(ii)){
						eta_n = false;
						break;
					}
				}
			}

			for(unsigned ii=0; ii<seeds->phi.at(iseed_eta).size(); ii++){
				if(ii==iseed_phi) continue;

				float dphi = seeds->phi.at(iseed_eta).at(iseed_phi)-seeds->phi.at(iseed_eta).at(ii);
				if(dphi>TMath::Pi()) dphi = TMath::Pi()*2 - dphi;
				if(dphi<-TMath::Pi()) dphi = -TMath::Pi()*2 - dphi;

				if(fabs(dphi)>range) continue;
				
				if(dphi>0){
					if(seeds->et_full.at(iseed_eta).at(iseed_phi)<=seeds->et_full.at(iseed_eta).at(ii)){
						eta_0 = false;
						break;
					}
				}
				if(dphi<0){
					if(seeds->et_full.at(iseed_eta).at(iseed_phi)<seeds->et_full.at(iseed_eta).at(ii)){
						eta_0 = false;
						break;
					}
				}
			}

			 //std::cout<<"Local max?: "<<(eta_n&&eta_p&&eta_0)<<std::endl;
			tmp_max.push_back(eta_n&&eta_p&&eta_0);
		}
		seeds->local_max.push_back(tmp_max);
	}//local max loop


	// For the FCAL wee need a separate algorithm since the geometry is in x-y
	//std::cout<<"///////////////////////////////////////////////////////////"<<std::endl;
	//std::cout<<"/////////////////////// FCAL //////////////////////////////"<<std::endl;
	//std::cout<<"///////////////////////////////////////////////////////////"<<std::endl;


	// FCAL Layer 1 large cells: dx= 30 mm, dy= 25.98mm
	float cellSize_x = 30;
	float cellSize_y = 25.9808;
	float dx_outer=cellSize_x*2;
	float dy_outer=cellSize_y*2;
	float dx_inner=cellSize_x;
	float dy_inner=cellSize_y;
	float dx_localmax = cellSize_x*2;
	float dy_localmax = cellSize_y*2;

	//std::cout<<"Seed size FCAL xy: "<<seedsXY->x.size()<<std::endl;

	for(unsigned i=0;i<seedsXY->x.size();i++){
		//std::cout<<"--------------Start loop XY seeds-----------------"<<std::endl;
		//std::cout<<"Use seed: "<<seedsXY->x.at(i)<<", "<<seedsXY->y.at(i)<<", "<<seedsXY->z.at(i)<<std::endl;
		float seed_x = seedsXY->x.at(i);
		float seed_y = seedsXY->y.at(i);
		float seed_z = seedsXY->z.at(i);

		// | >= | > | > |
		// | >= | R | > |
		// | >= | >=| > |
		// pos. x to the left, pos. y upwards
		std::vector<int> locMaxSeedIndices_right;
		std::vector<int> locMaxSeedIndices_left;
		std::vector<int> locMaxSeedIndices_top;
		std::vector<int> locMaxSeedIndices_bottom;

		locMaxSeedIndices_top.clear();
		locMaxSeedIndices_left.clear();
		locMaxSeedIndices_right.clear();
		locMaxSeedIndices_bottom.clear();

		// Get indices for local max calculation
		for(unsigned s =0;s<seedsXY->x.size();s++){
			if(i==s) continue;
			float challengeSeed_x = seedsXY->x.at(s);
			float challengeSeed_y = seedsXY->y.at(s);
			float challengeSeed_z = seedsXY->z.at(s);
			if(isInBoxXY(seed_x,challengeSeed_x,dx_localmax,seed_y,challengeSeed_y,dy_localmax, seed_z, challengeSeed_z)){
				if(challengeSeed_x>seed_x) locMaxSeedIndices_right.push_back(s);
				if(challengeSeed_x<seed_x) locMaxSeedIndices_left.push_back(s);
				if(challengeSeed_x==seed_x) {
					if(challengeSeed_y<seed_y) locMaxSeedIndices_bottom.push_back(s);
					if(challengeSeed_y>seed_y) locMaxSeedIndices_top.push_back(s);
				}
				 //std::cout<<"Challenge seed: "<<seedsXY->x.at(s)<<", "<<seedsXY->y.at(s)<<", "<<seedsXY->z.at(s)<<std::endl;
			}
		}

		seedsXY->locMaxSeedIndices_right.push_back(locMaxSeedIndices_right);
		seedsXY->locMaxSeedIndices_left.push_back(locMaxSeedIndices_left);
		seedsXY->locMaxSeedIndices_top.push_back(locMaxSeedIndices_top);
		seedsXY->locMaxSeedIndices_bottom.push_back(locMaxSeedIndices_bottom);

		std::vector<CaloCell*> hadCells_fcal;
		std::vector<CaloCell*> emCoreCells_fcal;
		std::vector<CaloCell*> emIsolationCells_fcal;
		std::vector<CaloCell*> cellsInBox_fcal;

		hadCells_fcal.clear();
		emCoreCells_fcal.clear();
		emIsolationCells_fcal.clear();
		cellsInBox_fcal.clear();

		// Split by A-C sides to reduce loop time
		std::vector<CaloCell*> hemisphereCells_fcal;
		if(seed_z>0)hemisphereCells_fcal=forwardCells_posEta;
		else hemisphereCells_fcal = forwardCells_negEta;

		for(unsigned c =0; c<hemisphereCells_fcal.size();c++){
			CaloCell* cell = hemisphereCells_fcal.at(c);
			float eta = cell->eta();
			float phi = cell->phi();
			float x = cell->x();
			float y = cell->y();
			float z = cell->z();
			std::vector<float> projected = projectXYCoordinates(x,y,z,eta,phi);
			float x_proj = projected.at(0);
			float y_proj = projected.at(1);

			if(!isInBoxXY(seed_x,x_proj,dx_outer,seed_y,y_proj,dy_outer, seed_z, cell->z())) continue;
			if(!(cell->caloDDE()->getSubCalo()==(CaloCell_Base_ID::SUBCALO)2)) continue;

			cellsInBox_fcal.push_back((CaloCell*)cell);

			// Determine Core, Iso and Had energies
			if(isInBoxXY(seed_x,x_proj,dx_inner,seed_y,y_proj,dy_inner, seed_z, cell->z())){
				// had
				if(!(cell->caloDDE()->getLayer()==1)){
					hadCells_fcal.push_back((CaloCell*)cell);
				}
				// EMcore
				if(cell->caloDDE()->getLayer()==1){
					emCoreCells_fcal.push_back((CaloCell*)cell);
					//if(cell->x()!=x_proj || cell->y()!=y_proj) std::cout<<"Cell x: "<<cell->x()<<", y: "<<cell->y()<<", proj x: "<<x_proj<<", y: "<<y_proj<<std::endl;
				}
			}else{
				if(!(cell->caloDDE()->getLayer()==1)){
					hadCells_fcal.push_back((CaloCell*)cell);
				}
				if(cell->caloDDE()->getLayer()==1){
					emIsolationCells_fcal.push_back((CaloCell*)cell);
				}
			}
		}

		float emCoreEt_fcal = -999999;
		float emPairEt_fcal = -999999;
		float emCore_nCells_fcal = -999999;
		float hadEt_fcal = -999999;
		float had_nCells_fcal = -999999;
		float emIsolationEt_fcal = -999999;
		float emIsolation_nCells_fcal = -999999;


		//std::cout<<"Em Core size FCAL: "<<emCoreCells_fcal.size()<<std::endl;
		float et = 0;
		float pair1_et = 0;
		float pair2_et = 0;
		float pair3_et = 0;
		float pair4_et = 0;
		std::vector<float>vec_fcalCoreEt;
		
		// Determine standard SLW variables
		if(emCoreCells_fcal.size()>0){

			for(unsigned m=0;m<emCoreCells_fcal.size();m++){
				CaloCell* cell = emCoreCells_fcal.at(m);
				et += cell->et();
				float etCell = cell->et();
				vec_fcalCoreEt.push_back(cell->et());
				float x = cell->x();
				float y = cell->y();

				float dx = seed_x-x;
				float dy = seed_y-y;
				if(dx<0) pair1_et += etCell;
				if(dx<0) pair2_et += etCell;
				if(dy<0) pair3_et += etCell;
				if(dy>0) pair4_et += etCell;
			}


			std::vector<float>pairEnergies;
			pairEnergies.clear();
			pairEnergies.push_back(pair1_et);
			pairEnergies.push_back(pair2_et);
			pairEnergies.push_back(pair3_et);
			pairEnergies.push_back(pair4_et);
			float max = *std::max_element(pairEnergies.begin(),pairEnergies.end());
			//std::cout<<"Pair1 ET: "<<pair1_et<<std::endl;
			//std::cout<<"Pair2 ET: "<<pair2_et<<std::endl;
			//std::cout<<"Pair3 ET: "<<pair3_et<<std::endl;
			//std::cout<<"Pair4 ET: "<<pair4_et<<std::endl;
			//std::cout<<"Max Pair ET: "<<max<<std::endl;
			//std::cout<<"Full ET: "<<et<<std::endl;

			emCoreEt_fcal =et;

			emPairEt_fcal = max;
		}
		emCore_nCells_fcal = emCoreCells_fcal.size();

		//std::cout<<"Em Isolation size FCAL: "<<emIsolationCells_fcal.size()<<std::endl;
		if(emIsolationCells_fcal.size()>0){
			float et=sumOfEnergy(emIsolationCells_fcal);
			emIsolationEt_fcal = et;
		}
		emIsolation_nCells_fcal = emIsolationCells_fcal.size();

		//std::cout<<"Had size FCAL: "<<hadCells_fcal.size()<<std::endl;

		// Determine overlap between cell areas of different layers since they don't align
		if(hadCells_fcal.size()>0){
			float et =0;
			for(unsigned mm=0;mm<hadCells_fcal.size();mm++){
				CaloCell* cell = hadCells_fcal.at(mm);
				float overlapPercentage = rectangeOverlapSeedCell(seed_x,seed_y,dx_inner,dy_inner,cell);
				et += cell->et()*overlapPercentage;
				 //std::cout<<"FCAL overlap percentage for cell "<<mm<<": "<<overlapPercentage<<std::endl;
			}
			hadEt_fcal = et;
		}
		had_nCells_fcal = hadCells_fcal.size();

		// Get shower shape like variables
		std::vector<float> centreCoordinates = firstlayerShowerCentre(cellsInBox_fcal);

		float centreEta = centreCoordinates.at(3);
		float centrePhi = centreCoordinates.at(4);

		std::vector<float> seed_etaPhi = fromXYtoEtaPhi(seed_x,seed_y,seed_z);

		std::vector<float> varFromCentre = varianceFromCentre(cellsInBox_fcal);

		float fracMaxEnergyCell = fractionMaxEnergyCell(cellsInBox_fcal);
		float showerDZ = showerDepthZ(cellsInBox_fcal);

		seedsXY->showerDepthZ.push_back(showerDZ);
		seedsXY->fractionMaxEnergyCell.push_back(fracMaxEnergyCell);
		seedsXY->showerVarianceR.push_back(varFromCentre.at(0));
		seedsXY->showerVarianceZ.push_back(varFromCentre.at(1));
		seedsXY->showerCentreFirstLayer_eta.push_back(centreEta);
		seedsXY->showerCentreFirstLayer_phi.push_back(centrePhi);

		seedsXY->et.push_back(emPairEt_fcal);
		seedsXY->et_full.push_back(emCoreEt_fcal);
		seedsXY->isolationEt.push_back(emIsolationEt_fcal);
		seedsXY->hadEt.push_back(hadEt_fcal);
		seedsXY->core_nCells.push_back(emCore_nCells_fcal);
		seedsXY->isolation_nCells.push_back(emIsolation_nCells_fcal);
		seedsXY->had_nCells.push_back(had_nCells_fcal);
		seedsXY->fcalCoreEt.push_back(vec_fcalCoreEt);
		//std::cout<<"--------------End loop XY seeds-----------------"<<std::endl;
	}

	// and now determine the local maximum in FCAL
	
	// Only used for debugging
	//bool localMaxTop = true;
	//bool localMaxLeft = true;
	//bool localMaxRight = true;
	//bool localMaxBottom = true;
	for(unsigned iseed=0; iseed<seedsXY->x.size(); iseed++){
		//std::cout<<"--------------Start loop local max XY seeds-----------------"<<std::endl;
		// Using full em Core here!
		float seedEt = seedsXY->et_full.at(iseed);

		// noise in FCAL is approx 0.5 GeV per large cell in FCAL1
		if(seedEt<2000) {
			seedsXY->local_max.push_back(0);
			continue;
		}

		bool isLocMax = true;
		for(unsigned jseed=0;jseed<seedsXY->x.size();jseed++){
			float x = seedsXY->x.at(iseed);
			float y = seedsXY->y.at(iseed);
			float z = seedsXY->z.at(iseed);
			float x_challenge = seedsXY->x.at(jseed);
			float y_challenge = seedsXY->y.at(jseed);
			float z_challenge = seedsXY->z.at(jseed);
			float etChallenge = seedsXY->et_full.at(jseed);
			if(isInBoxXY(x,x_challenge,dx_localmax ,y,y_challenge,dy_localmax, z, z_challenge)){
				if(seedEt<etChallenge){
					isLocMax=false;
					break;
				}
				if(seedEt==etChallenge){
					if(x<x_challenge){
						isLocMax=false;
						break;
					}
					if(x==x_challenge && y<y_challenge){
						isLocMax=false;
						break;
					}
				}
			}
			// eta phi

		}

		//std::cout<<"Local max FCAL?: "<<(localMaxTop&&localMaxBottom&&localMaxLeft&&localMaxRight)<<std::endl;
		seedsXY->local_max.push_back(isLocMax);


		//std::cout<<"--------------End loop local max XY seeds-----------------"<<std::endl;
	}//local max loop

	// Overlap between EMEC and FCAL
	// Seeds in EMEC reach into FCAL. Boundary FCAL cells are considered in EMEC Core energy currently, but not in isolation
	// EMEC is preferred if energy is equal or larger than close FCAL energy
	// If this is disabled it might lead to double counting in transition region FCAL-EMEC
	// Long-term have to keep in mind that FCAL1 acts as Had layer for edge (eta=3.2) EMEC objects
	for(unsigned iseed_eta=0; iseed_eta<seeds->eta.size(); iseed_eta++){
		float seedEta = seeds->eta.at(iseed_eta);
		if(fabs(seedEta)<3.2) continue;
		for(unsigned iseed_phi=0; iseed_phi<seeds->phi.at(iseed_eta).size(); iseed_phi++){
			float seedPhi = seeds->phi.at(iseed_eta).at(iseed_phi);
			float seedEt = seeds->et.at(iseed_eta).at(iseed_phi);
			for(unsigned iseedXY=0; iseedXY<seedsXY->x.size(); iseedXY++){
				std::vector<float> seedXY_etaPhi = fromXYtoEtaPhi(seedsXY->x.at(iseedXY),seedsXY->y.at(iseedXY),seedsXY->z.at(iseedXY));
				float seedXYEta = seedXY_etaPhi.at(0);
				if(fabs(seedXYEta)>3.4) continue;
				float seedXYPhi = seedXY_etaPhi.at(1);
				float seedXYEt = seedsXY->et_full.at(iseedXY);
				if(deltaR(seedPhi,seedXYPhi,seedEta,seedXYEta)<0.1){
					if(seedEt>=seedXYEt) seedsXY->local_max.at(iseedXY)=false;
					else seeds->local_max.at(iseed_eta).at(iseed_phi)=false;
				}
			}
		}
	}


	//std::cout<<"--------------Finished seed finding-----------------"<<std::endl;

	return StatusCode::SUCCESS;
}

// Construction of electron objects
StatusCode FFexEMFinder::BuildElectron( std::shared_ptr<FFexEMFinder::Seed>seeds, std::shared_ptr<FFexEMFinder::SeedXY> seedsXY, std::vector<std::shared_ptr<FFexEMFinder::electron>> &electrons){


	//std::cout<<"--------------Filling EMEC electrons-----------------"<<std::endl;
	for(unsigned eta_ind=0; eta_ind<seeds->eta.size(); eta_ind++){

		for(unsigned phi_ind=0; phi_ind<seeds->phi.at(eta_ind).size(); phi_ind++){

			if(!seeds->local_max.at(eta_ind).at(phi_ind)) continue;
			float eta = seeds->eta.at(eta_ind);
			float phi = seeds->phi.at(eta_ind).at(phi_ind);
			float et = seeds->et.at(eta_ind).at(phi_ind);
			float et_full = seeds->et_full.at(eta_ind).at(phi_ind);
			float isolationEt = seeds->isolationEt.at(eta_ind).at(phi_ind);
			float hadEt = seeds->hadEt.at(eta_ind).at(phi_ind);
			int core_nCells = seeds->core_nCells.at(eta_ind).at(phi_ind);
			int isolation_nCells = seeds->isolation_nCells.at(eta_ind).at(phi_ind);
			int had_nCells = seeds->had_nCells.at(eta_ind).at(phi_ind);
			float showerDepthZ = seeds->showerDepthZ.at(eta_ind).at(phi_ind);
			float showerVarianceR = seeds->showerVarianceR.at(eta_ind).at(phi_ind);
			float showerVarianceZ = seeds->showerVarianceZ.at(eta_ind).at(phi_ind);
			float showerCentreFirstLayer_eta = seeds->showerCentreFirstLayer_eta.at(eta_ind).at(phi_ind);
			float showerCentreFirstLayer_phi = seeds->showerCentreFirstLayer_phi.at(eta_ind).at(phi_ind);
			float fractionMaxEnergyCell = seeds->fractionMaxEnergyCell.at(eta_ind).at(phi_ind);
			float centreEta = seeds->centreEta.at(eta_ind).at(phi_ind);
			float centrePhi = seeds->centrePhi.at(eta_ind).at(phi_ind);
			std::vector<float> emecCoreEt = seeds->emecCoreEt.at(eta_ind).at(phi_ind);




			if(et_full<5000) continue; // Only keep candidates > 5 GeV
			std::shared_ptr<FFexEMFinder::electron> el = std::make_shared<FFexEMFinder::electron>();
			el->eta = eta;
			el->phi = phi;
			el->et = et;
			el->et_full = et_full;
			el->isolationEt = isolationEt;
			el->hadEt = hadEt;
			el->core_nCells = core_nCells;
			el->isolation_nCells = isolation_nCells;
			el->had_nCells = had_nCells;
			el->isFCAL = false;
			el->showerDepthZ=showerDepthZ;
			el->showerVarianceR=showerVarianceR;
			el->showerVarianceZ=showerVarianceZ;
			el->showerCentreFirstLayer_eta=showerCentreFirstLayer_eta;
			el->showerCentreFirstLayer_phi=showerCentreFirstLayer_phi;
			el->fractionMaxEnergyCell=fractionMaxEnergyCell;
			el->centreEta=centreEta;
			el->centrePhi=centrePhi;
			el->emecCoreEt=emecCoreEt;

			electrons.push_back(el);

		}
	}

	//std::cout<<"--------------Filling FCAL electrons-----------------"<<std::endl;

	for(unsigned s=0; s<seedsXY->x.size(); s++){
		if(!seedsXY->local_max.at(s)) continue;

		float x = seedsXY->x.at(s);
		float y = seedsXY->y.at(s);
		float z = seedsXY->z.at(s);
		std::vector<float> etaPhi =  fromXYtoEtaPhi(x,y,z);
		float eta = etaPhi.at(0);
		float phi = etaPhi.at(1);
		//std::cout<<"Electron FCAL z:"<<z<<", eta: "<<eta<<" (x,y): "<<x<<", "<<y<<std::endl;

		float et = seedsXY->et.at(s);
		float et_full = seedsXY->et_full.at(s);
		float isolationEt = seedsXY->isolationEt.at(s);
		float hadEt = seedsXY->hadEt.at(s);
		int core_nCells = seedsXY->core_nCells.at(s);
		int isolation_nCells = seedsXY->isolation_nCells.at(s);
		int had_nCells = seedsXY->had_nCells.at(s);
		float showerDepthZ = seedsXY->showerDepthZ.at(s);
		float showerVarianceR = seedsXY->showerVarianceR.at(s);
		float showerVarianceZ = seedsXY->showerVarianceZ.at(s);
		float showerCentreFirstLayer_eta = seedsXY->showerCentreFirstLayer_eta.at(s);
		float showerCentreFirstLayer_phi = seedsXY->showerCentreFirstLayer_phi.at(s);
		float fractionMaxEnergyCell = seedsXY->fractionMaxEnergyCell.at(s);
		std::vector<float> fcalCoreEt = seedsXY->fcalCoreEt.at(s);

		if(et_full<5000) continue;
		std::shared_ptr<FFexEMFinder::electron> el = std::make_shared<FFexEMFinder::electron>();
		el->eta = eta;
		el->phi = phi;
		el->et = et;
		el->et_full = et_full;
		el->isolationEt = isolationEt;
		el->hadEt = hadEt;
		el->core_nCells = core_nCells;
		el->isolation_nCells = isolation_nCells;
		el->had_nCells = had_nCells;
		el->isFCAL = true;
		el->showerDepthZ=showerDepthZ;
		el->showerVarianceR=showerVarianceR;
		el->showerVarianceZ=showerVarianceZ;
		el->showerCentreFirstLayer_eta=showerCentreFirstLayer_eta;
		el->showerCentreFirstLayer_phi=showerCentreFirstLayer_phi;
		el->fractionMaxEnergyCell=fractionMaxEnergyCell;
		el->centreEta=-999999;
		el->centrePhi=-999999;
		el->fcalCoreEt=fcalCoreEt;


		electrons.push_back(el);
	}

	std::sort(electrons.rbegin(),electrons.rend());

	//std::cout<<"Found "<<electrons.size()<<" ffex electrons"<<std::endl;

	return StatusCode::SUCCESS;
}
