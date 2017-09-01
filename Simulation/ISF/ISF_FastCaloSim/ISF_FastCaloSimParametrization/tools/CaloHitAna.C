/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define CaloHitAna_cxx
#include "CaloHitAna.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "FCS_Cell.h"
#include <iostream>
#include <map>
#include "TSystem.h"

using namespace std;

void CaloHitAna::Loop()
{
  //This Loop is supposed to read the output ntuple/tree
  //(vector of cells, vector of G4 hits, vector of FCS detailed hits, ...)
  //from the athena ISF_HitAnalysis algorithm (==ESD dumper)
  //and match them together + save structured output
  //NB: E = Ehit * SamplingFraction for Tile or E = Ehit / SamplingFraction (LAr) conversion
  //is done here

  if (fChain == 0) return;
  ProcInfo_t procinfo;
  Long64_t nentries = fChain->GetEntriesFast();

  if (m_max_nentries >= 0 && m_max_nentries < nentries) nentries = m_max_nentries;

  std::map<Long64_t, FCS_cell> cells; //read all objects and collect them by identifier (Long64_t)
  std::map<Long64_t, std::vector<FCS_g4hit> > g4hits;
  std::map<Long64_t, std::vector<FCS_hit> > hits;

  FCS_cell   one_cell; //note that this is not extra safe if I don't have a clear method!
  FCS_g4hit  one_g4hit;
  FCS_hit    one_hit;
  FCS_matchedcell one_matchedcell;
  //FCS_truth  one_truth;

  //From here: Loop over events:
  Long64_t nbytes = 0, nb = 0;
  std::cout << "before event loop" << std::endl;
  for (Long64_t jentry = 0; jentry < nentries; jentry++)
  {
    if (jentry % 100 == 0)
      std::cout << "jentry " << jentry << endl;

    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    if (jentry % m_PrintOutFrequency == 0)
    {
      std::cout << "Processing entry " << jentry << std::endl;
      gSystem->GetProcInfo(&procinfo);
      std::cout << "Memory usage: " << procinfo.fMemResident << " " << procinfo.fMemVirtual << std::endl;
    }

    //Copy truth
    new_truthE->clear();
    new_truthPx->clear();
    new_truthPy->clear();
    new_truthPz->clear();
    new_truthPDG->clear();
    new_truthBarcode->clear();
    new_truthVtxBarcode->clear();
    //truthcollection->clear();
    m_newTTC_entrance_eta->clear();
    m_newTTC_entrance_phi->clear();
    m_newTTC_entrance_r->clear();
    m_newTTC_entrance_z->clear();
    m_newTTC_back_eta->clear();
    m_newTTC_back_phi->clear();
    m_newTTC_back_r->clear();
    m_newTTC_back_z->clear();
    m_newTTC_IDCaloBoundary_eta->clear();
    m_newTTC_IDCaloBoundary_phi->clear();
    m_newTTC_IDCaloBoundary_r->clear();
    m_newTTC_IDCaloBoundary_z->clear();
    m_newTTC_Angle3D->clear();
    m_newTTC_AngleEta->clear();

    oneeventcells->m_vector.clear();

    if (TruthE->size() != 1)
      std::cout << "Strange! TruthE->size() is " << TruthE->size() << ", but it should be 1. Please check input." << std::endl;

    for (unsigned int truth_i = 0; truth_i < TruthE->size(); truth_i++)
    {
      //std::cout <<truth_i<<" "<<TruthE->size()<<" "<<TruthPDG->at(truth_i)<<std::endl;
      //std::cout <<"TTC2: "<<(*TTC_entrance_eta)[truth_i].size()<<std::endl;//this one has 24 layers ->ok

      new_truthE->push_back(TruthE->at(truth_i));
      new_truthPx->push_back(TruthPx->at(truth_i));
      new_truthPy->push_back(TruthPy->at(truth_i));
      new_truthPz->push_back(TruthPz->at(truth_i));
      new_truthPDG->push_back(TruthPDG->at(truth_i));
      new_truthBarcode->push_back(TruthBarcode->at(truth_i));
      //new_truthVtxBarcode->push_back(TruthVtxBarcode->at(truth_i));

      m_newTTC_IDCaloBoundary_eta->push_back(newTTC_IDCaloBoundary_eta->at(truth_i));
      m_newTTC_IDCaloBoundary_phi->push_back(newTTC_IDCaloBoundary_phi->at(truth_i));
      m_newTTC_IDCaloBoundary_r  ->push_back(newTTC_IDCaloBoundary_r->at(truth_i));
      m_newTTC_IDCaloBoundary_z  ->push_back(newTTC_IDCaloBoundary_z->at(truth_i));
      m_newTTC_Angle3D           ->push_back(newTTC_Angle3D->at(truth_i));
      m_newTTC_AngleEta          ->push_back(newTTC_AngleEta->at(truth_i));

      //create temporary vectors
      vector<float> new_entrance_eta;
      vector<float> new_entrance_phi;
      vector<float> new_entrance_r;
      vector<float> new_entrance_z;
      vector<float> new_back_eta;
      vector<float> new_back_phi;
      vector<float> new_back_r;
      vector<float> new_back_z;

      for (unsigned int s = 0; s < 24; s++)
      {
        new_entrance_eta.push_back((newTTC_entrance_eta->at(truth_i))[s]);
        new_entrance_phi.push_back((newTTC_entrance_phi->at(truth_i))[s]);
        new_entrance_r.push_back((newTTC_entrance_r->at(truth_i))[s]);
        new_entrance_z.push_back((newTTC_entrance_z->at(truth_i))[s]);
        new_back_eta.push_back((newTTC_back_eta->at(truth_i))[s]);
        new_back_phi.push_back((newTTC_back_phi->at(truth_i))[s]);
        new_back_r.push_back((newTTC_back_r->at(truth_i))[s]);
        new_back_z.push_back((newTTC_back_z->at(truth_i))[s]);
      }

      //push back temporary vectors
      m_newTTC_entrance_eta->push_back(new_entrance_eta);
      m_newTTC_entrance_phi->push_back(new_entrance_phi);
      m_newTTC_entrance_r->push_back(new_entrance_r);
      m_newTTC_entrance_z->push_back(new_entrance_z);
      m_newTTC_back_eta->push_back(new_back_eta);
      m_newTTC_back_phi->push_back(new_back_phi);
      m_newTTC_back_r->push_back(new_back_r);
      m_newTTC_back_z->push_back(new_back_z);

      /*
      one_truth.SetPxPyPzE((*TruthPx)[truth_i], (*TruthPy)[truth_i], (*TruthPz)[truth_i], (*TruthE)[truth_i]);
      one_truth.pdgid   = TruthPDG->at(truth_i);
      one_truth.barcode = TruthBarcode->at(truth_i);
      one_truth.vtxbarcode = TruthVtxBarcode->at(truth_i);
      truthcollection->push_back(one_truth);
      */

    }

    //std::cout<<"check. after truth block"<<std::endl;

    //Now matching between cells, G4hits and G4detailed hits
    //sort cells by identifier:
    //clear first the containers for this event:
    cells.clear();
    g4hits.clear();
    hits.clear();
    //std::cout <<"Check Original size: Cells: "<<CellIdentifier->size()<<" G4Hits: "<<G4HitCellIdentifier->size()<<" FCSHits: "<<HitCellIdentifier->size()<<std::endl;
    if (m_Debug > 1) std::cout << "Reading cells...";

    for (unsigned int cell_i = 0; cell_i < CellIdentifier->size(); cell_i++)
    {
      if (cells.find((*CellIdentifier)[cell_i]) == cells.end()) //doesn't exist
      {
        one_cell.cell_identifier = (*CellIdentifier)[cell_i];
        one_cell.sampling = (*CellSampling)[cell_i];
        one_cell.energy = (*CellE)[cell_i];
        one_cell.center_x = 0.0; //for now
        one_cell.center_y = 0.0;
        one_cell.center_z = 0.0;
        cells.insert(std::pair<Long64_t, FCS_cell>(one_cell.cell_identifier, one_cell));
      }
      else
      {
        //there shouldn't be a cell with the same identifier in this event
        std::cout << "ISF_HitAnalysis: Same cell???? ERROR" << std::endl;
      }
    }

    if (m_Debug > 1) std::cout << " Done" << std::endl;

    if (m_Debug > 1) std::cout << "Reading G4hits";

    if (m_do_g4_hits)
    {
      for (unsigned int g4hit_i = 0; g4hit_i < G4HitIdentifier->size(); g4hit_i++)
      {
        if ((*G4HitSampling)[g4hit_i] >= 0 && (*G4HitSampling)[g4hit_i] <= 25 && (*G4HitT)[g4hit_i] > m_TimingCut)
        {
          if (m_Debug > 1) std::cout << "Ignoring G4hit, time too large: " << g4hit_i << " time: " << (*G4HitT)[g4hit_i] << std::endl;
          continue;
        }

        if (g4hits.find((*G4HitCellIdentifier)[g4hit_i]) == g4hits.end())
        {
          //this G4 hit doesn't exist yet
          one_g4hit.identifier = (*G4HitIdentifier)[g4hit_i];
          one_g4hit.cell_identifier = (*G4HitCellIdentifier)[g4hit_i];
          one_g4hit.sampling = (*G4HitSampling)[g4hit_i];
          one_g4hit.hit_time = (*G4HitT)[g4hit_i];
          //one_g4hit.hit_energy = (*G4HitE)[g4hit_i];
          //scale the hit energy with the sampling fraction
          if (one_g4hit.sampling >= 12 && one_g4hit.sampling <= 20)
          { //tile
            //std::cout <<"Tile: "<<(*G4HitE)[g4hit_i]<<" "<<(*G4HitSamplingFraction)[g4hit_i]<<std::endl;
            if ((*G4HitSamplingFraction)[g4hit_i])
            {
              one_g4hit.hit_energy = (*G4HitE)[g4hit_i] * (*G4HitSamplingFraction)[g4hit_i];
              //std::cout <<"TileE: "<<one_g4hit.hit_energy<<std::endl;
            }
            else one_g4hit.hit_energy = 0.;
          }
          else
          {
            //std::cout <<"LAr: "<<(*G4HitE)[g4hit_i]<<" "<<(*G4HitSamplingFraction)[g4hit_i]<<std::endl;
            one_g4hit.hit_energy = (*G4HitE)[g4hit_i] / (*G4HitSamplingFraction)[g4hit_i];
          }
          //one_g4hit.hit_sampfrac = (*G4HitSamplingFraction)[g4hit_i];
          //new g4hit -> insert vector with 1 element
          g4hits.insert(std::pair<Long64_t, std::vector<FCS_g4hit> >(one_g4hit.cell_identifier, std::vector<FCS_g4hit>(1, one_g4hit)));
        }
        else
        {
          //G4 hit exists in this identifier -> push_back new to the vector                                                                                       //FCS_g4hit one_g4hit;
          one_g4hit.identifier = (*G4HitIdentifier)[g4hit_i];
          one_g4hit.cell_identifier = (*G4HitCellIdentifier)[g4hit_i];
          one_g4hit.sampling = (*G4HitSampling)[g4hit_i];
          one_g4hit.hit_time = (*G4HitT)[g4hit_i];
          if (one_g4hit.sampling >= 12 && one_g4hit.sampling <= 20)
          { //tile
            //std::cout <<"Tile2: "<<(*G4HitE)[g4hit_i]<<" "<<(*G4HitSamplingFraction)[g4hit_i]<<std::endl;
            if ((*G4HitSamplingFraction)[g4hit_i])
            {
              one_g4hit.hit_energy = (*G4HitE)[g4hit_i] * (*G4HitSamplingFraction)[g4hit_i];
              //std::cout <<"TileE2: "<<one_g4hit.hit_energy<<std::endl;
            }
            else one_g4hit.hit_energy = 0.;
          }
          else
          {
            //std::cout <<"LAr2: "<<(*G4HitE)[g4hit_i]<<" "<<(*G4HitSamplingFraction)[g4hit_i]<<std::endl;
            one_g4hit.hit_energy = (*G4HitE)[g4hit_i] / (*G4HitSamplingFraction)[g4hit_i];
          }
          //one_g4hit.hit_sampfrac = (*G4HitSamplingFraction)[g4hit_i];
          g4hits[(*G4HitCellIdentifier)[g4hit_i]].push_back(one_g4hit);
        }
      }
    }

    if (m_Debug > 1) std::cout << " Done" << std::endl;

    if (m_Debug > 1) std::cout << "Reading detailed FCS hits " << HitIdentifier->size() << std::endl;

    for (unsigned int hit_i = 0; hit_i < HitIdentifier->size(); hit_i++)
    {
      if ((*HitSampling)[hit_i] >= 0 && (*HitSampling)[hit_i] <= 25 && (*HitT)[hit_i] > m_TimingCut)
      {
        if (m_Debug > 1)
          std::cout << "Ignoring FCS hit, time too large: " << hit_i << " time: " << (*HitT)[hit_i] << std::endl;
        continue;
      }
      if (hits.find((*HitCellIdentifier)[hit_i]) == hits.end())
      {
        //Detailed hit doesn't exist yet
        one_hit.identifier = (*HitIdentifier)[hit_i];
        one_hit.cell_identifier = (*HitCellIdentifier)[hit_i];
        one_hit.sampling = (*HitSampling)[hit_i];

        if (one_hit.sampling >= 12 && one_hit.sampling <= 20)
        { //tile
          if ((*HitSamplingFraction)[hit_i])
          {
            one_hit.hit_energy = (*HitE)[hit_i] * (*HitSamplingFraction)[hit_i];
          }
          else one_hit.hit_energy = 0.;
        }
        else
        {
          one_hit.hit_energy = (*HitE)[hit_i] / (*HitSamplingFraction)[hit_i];
        }
        //one_hit.hit_sampfrac = (*HitSamplingFraction)[hit_i];
        one_hit.hit_time = (*HitT)[hit_i];
        one_hit.hit_x = (*HitX)[hit_i];
        one_hit.hit_y = (*HitY)[hit_i];
        one_hit.hit_z = (*HitZ)[hit_i];
        hits.insert(std::pair<Long64_t, std::vector<FCS_hit> >(one_hit.cell_identifier, std::vector<FCS_hit>(1, one_hit)));
      }
      else
      {
        //Detailed hit exists in this identifier -> push_back new to the vector
        one_hit.identifier = (*HitIdentifier)[hit_i];
        one_hit.cell_identifier = (*HitCellIdentifier)[hit_i];
        one_hit.sampling = (*HitSampling)[hit_i];
        //one_hit.hit_energy = (*HitE)[hit_i];
        if (one_hit.sampling >= 12 && one_hit.sampling <= 20)
        { //tile
          if ((*HitSamplingFraction)[hit_i])
          {
            one_hit.hit_energy = (*HitE)[hit_i] * (*HitSamplingFraction)[hit_i];
          }
          else one_hit.hit_energy = 0.;
        }
        else
        {
          one_hit.hit_energy = (*HitE)[hit_i] / (*HitSamplingFraction)[hit_i];
        }
        //one_hit.hit_sampfrac = (*HitSamplingFraction)[hit_i];
        one_hit.hit_time = (*HitT)[hit_i];
        one_hit.hit_x = (*HitX)[hit_i];
        one_hit.hit_y = (*HitY)[hit_i];
        one_hit.hit_z = (*HitZ)[hit_i];
        hits[(*HitCellIdentifier)[hit_i]].push_back(one_hit);
      }
    }

    if (m_Debug > 1) std::cout << " Done" << std::endl;

    if (m_Debug > 1) std::cout << "ISF_HitAnalysis Check: Cells: " << cells.size() << " G4hits: " << g4hits.size() << " FCS detailed hits: " << hits.size() << std::endl;

    //Start matching:
    Int_t iindex = 0;
    for (std::map<Long64_t, FCS_cell>::iterator it = cells.begin(); it != cells.end(); )
    {
      iindex++;
      // std::cout <<iindex<<std::endl;
      one_matchedcell.clear(); //maybe not completely necessery, as we're not pushing_back into vectors
      //set the cell part
      one_matchedcell.cell = it->second;
      //now look for FCS detailed hits in this cell
      std::map<Long64_t, std::vector<FCS_hit> >::iterator it2 = hits.find(it->first);
      if (it2 != hits.end())
      {
        //std::cout <<"FCS hits found in this cell"<<std::endl;
        one_matchedcell.hit = it2->second;
        hits.erase(it2); //remove it
      }
      else
      {
        //no hit found for this cell
        one_matchedcell.hit.clear(); //important!
      }
      //now look for G4hits in this cell
      std::map<Long64_t, std::vector<FCS_g4hit> >::iterator it3 = g4hits.find(it->first);
      if (it3 != g4hits.end())
      {
        //std::cout <<"G4 hits found in this cell"<<std::endl;
        one_matchedcell.g4hit = it3->second;
        g4hits.erase(it3);
      }
      else
      {
        //no g4hit found for this cell
        one_matchedcell.g4hit.clear();//important!
      }
      //std::cout <<"Erase cell"<<std::endl;
      cells.erase(it++);
      //std::cout <<"Insert matched object"<<std::endl;
      //push_back matched cell for event jentry
      oneeventcells->push_back(one_matchedcell);
      //std::cout <<"Go next"<<std::endl;

    }

    //ok, cells should be empty, what about hits and g4hits?
    //There could be G4hits/FCS hits for which we don't have a cell ->create a dummy empty cell with 0 energy, take the cell identifier from the hit
    if (m_Debug > 1) std::cout << "ISF_HitAnalysis Check after cells: " << cells.size() << " " << g4hits.size() << " " << hits.size() << std::endl;

    for (std::map<Long64_t, std::vector<FCS_hit> >::iterator it = hits.begin(); it != hits.end();)
    {
      one_matchedcell.clear();
      one_matchedcell.cell.cell_identifier = it->first;
      //std::cout <<"This hit didn't exist in cell: "<<it->first<<std::endl;
      if (it->second.size())
      {
        one_matchedcell.cell.sampling = (it->second)[0].sampling;
      }
      else
      {
        one_matchedcell.cell.sampling = -1; //
        //ok, but you really shouldn't be here
        std::cout << "ERROR: You shouldn't really be here" << std::endl;
      }
      one_matchedcell.cell.energy = 0.;
      one_matchedcell.cell.center_x = 0.0;
      one_matchedcell.cell.center_y = 0.0;
      one_matchedcell.cell.center_z = 0.0;
      one_matchedcell.hit = it->second;
      std::map<Long64_t, std::vector<FCS_g4hit> >::iterator it3 = g4hits.find(it->first);
      if (it3 != g4hits.end())
      {
        one_matchedcell.g4hit = it3->second;
        g4hits.erase(it3);
      }
      else
      {
        //no g4hit found for this cell
        one_matchedcell.g4hit.clear(); //important!
      }
      hits.erase(it++);
      oneeventcells->push_back(one_matchedcell);

    }

    //ok, hits should be empty, what about g4hits?
    if (m_Debug > 1 )std::cout << "ISF_HitAnalysis Check after hits: " << cells.size() << " " << g4hits.size() << " " << hits.size() << std::endl;
    for (std::map<Long64_t, std::vector<FCS_g4hit> >::iterator it = g4hits.begin(); it != g4hits.end();)
    {
      one_matchedcell.clear(); //maybe not so important
      one_matchedcell.cell.cell_identifier = it->first;
      if (it->second.size())
      {
        one_matchedcell.cell.sampling = (it->second)[0].sampling;
      }
      else
      {
        one_matchedcell.cell.sampling = -1; //
        //not really
        std::cout << "ERROR: You shouldn't really be here" << std::endl;
      }
      one_matchedcell.cell.energy = 0.;
      one_matchedcell.cell.center_x = 0.0;
      one_matchedcell.cell.center_y = 0.0;
      one_matchedcell.cell.center_z = 0.0;
      one_matchedcell.g4hit = it->second;
      one_matchedcell.hit.clear(); //important!!
      g4hits.erase(it++);
      oneeventcells->push_back(one_matchedcell);
    }

    if (m_Debug > 1) std::cout << "ISF_HitAnalysis Check after g4hits: " << cells.size() << " " << g4hits.size() << " " << hits.size() << std::endl;
    //Final size for this event
    if (m_Debug > 1) std::cout << "ISF_HitAnalysis Matched cells size: " << oneeventcells->size() << std::endl;

    //Can fill the output tree already here:
    total_cell_e  = 0;
    total_hit_e   = 0;
    total_g4hit_e = 0;

    for (int j = 0; j < MAX_LAYER - 1; j++)
    {
      layercells[j]->m_vector = oneeventcells->GetLayer(j);
    }

    //this is for invalid cells
    layercells[MAX_LAYER - 1]->m_vector = oneeventcells->GetLayer(-1);

    for (int i = 0; i < MAX_LAYER; i++)
    {
      cell_energy->at(i)  = 0.0; //zero for each event!
      hit_energy->at(i)   = 0.0;
      g4hit_energy->at(i) = 0.0;

      for (unsigned int cellindex = 0; cellindex < layercells[i]->size(); cellindex++)
      {
        if (i != MAX_LAYER - 1)
        {
          cell_energy->at(i) += layercells[i]->m_vector.at(cellindex).cell.energy;
          total_cell_e += layercells[i]->m_vector.at(cellindex).cell.energy;
        }
        else
        {
          //don't add the energy in the invalid layer to the total energy (if there is any (shouldn't)
          cell_energy->at(i) += layercells[i]->m_vector.at(cellindex).cell.energy; //this should be here anyway
        }

        //sum energy of all FCS detailed hits in this layer/cell
        for (unsigned int j = 0; j < layercells[i]->m_vector.at(cellindex).hit.size(); j++)
        {
          if (i != MAX_LAYER - 1)
          {
            total_hit_e += layercells[i]->m_vector.at(cellindex).hit[j].hit_energy;
            hit_energy->at(i) += layercells[i]->m_vector.at(cellindex).hit[j].hit_energy;
          }
          else
          {
            //again, don't add invalid layer energy to the sum
            hit_energy->at(i) += layercells[i]->m_vector.at(cellindex).hit[j].hit_energy;
          }
        }

        //sum energy of all G4 hits in this layer/cell
        for (unsigned int j = 0; j < layercells[i]->m_vector.at(cellindex).g4hit.size(); j++)
        {
          if (i != MAX_LAYER - 1)
          {
            total_g4hit_e += layercells[i]->m_vector.at(cellindex).g4hit[j].hit_energy;
            g4hit_energy->at(i) += layercells[i]->m_vector.at(cellindex).g4hit[j].hit_energy;
          }
          else
          {
            //don't add invalied layer energy to the sum
            g4hit_energy->at(i) += layercells[i]->m_vector.at(cellindex).g4hit[j].hit_energy;
          }
        }
      }
    }

    cell_energy->at(MAX_LAYER)  = total_cell_e;
    hit_energy->at(MAX_LAYER)   = total_hit_e;
    g4hit_energy->at(MAX_LAYER) = total_g4hit_e;

    //Uncomment this to get energy cross check
    if (jentry % m_PrintOutFrequency == 0) std::cout << "Energy check: event: " << jentry << " cell: " << total_cell_e << " g4hits: " << total_g4hit_e << " hits: " << total_hit_e << std::endl;
    //tree gets filled
    m_OutputTree->Fill();

  }//loop over entries

  m_OutputTree->Write();
  m_Output->Close
};
