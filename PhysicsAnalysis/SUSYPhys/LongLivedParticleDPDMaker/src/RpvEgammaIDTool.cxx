/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RpvEgammaIDTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Straight copy of DerivationFrameworkEGamma/EGammaPassSelectionWrapper.cxx
// because DerivationFrameworkEGamma is not in AtlasProduction
// From DerivationFrameworkEGamma/EGammaPassSelectionWrapper.cxx:
// Author: James Catmore (James.Catmore@cern.ch)
// Wrapper around the passSelection() method of xAOD egamma
// Writes result to SG for later selection by string parser

#include "LongLivedParticleDPDMaker/RpvEgammaIDTool.h"
#include "xAODEgamma/EgammaContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::RpvEgammaIDTool::RpvEgammaIDTool( const std::string& t,
						       const std::string& n,
						       const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_sgPrefix("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("SelectionVariables",m_qualFlags);
    declareProperty("SGPrefix", m_sgPrefix);
  }
 
// Destructor
DerivationFramework::RpvEgammaIDTool::~RpvEgammaIDTool() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::RpvEgammaIDTool::initialize()
{
     if (m_qualFlags.size()==0) {
        ATH_MSG_ERROR("No selection variables for the egamma passSelection wrapper tool!");
        return StatusCode::FAILURE;
     }
     ATH_MSG_VERBOSE("initialize() ...");
     ATH_CHECK(m_collName.initialize());
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::RpvEgammaIDTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     return StatusCode::SUCCESS;
}

// Augmentation
StatusCode DerivationFramework::RpvEgammaIDTool::addBranches() const
{

     // Retrieve data
     SG::ReadHandle<xAOD::EgammaContainer> egammas(m_collName);
     if( !egammas.isValid() ) {
        ATH_MSG_ERROR("Couldn't retrieve e-gamma container with key: " << m_collName);
        return StatusCode::FAILURE;
     }
       
     // Make vectors for the cut results
     std::vector<SG::WriteHandle<std::vector<int> > > allSelectionResults;
     for (auto str : m_qualFlags) {
        std::string sgKey("");
        if (m_sgPrefix=="") {
                sgKey = str;
        } else {
                sgKey = m_sgPrefix+str;
        }
        SG::WriteHandle< std::vector<int> > tmp(sgKey);
        allSelectionResults.push_back(tmp);
        ATH_CHECK(allSelectionResults.back().record(std::make_unique< std::vector<int> >()));
     } 
     // Loop over egammas, set decisions   
     for (xAOD::EgammaContainer::const_iterator eIt = egammas->begin(); eIt!=egammas->end(); ++eIt) {
        unsigned int itr(0);
        for (std::vector<std::string>::const_iterator strItr = m_qualFlags.begin(); strItr!=m_qualFlags.end(); ++strItr, ++itr) {
                bool val(0);
                if ( (*eIt)->passSelection(val,*strItr) ) {
                        if (val) {allSelectionResults[itr]->push_back(1);}
                        else {allSelectionResults[itr]->push_back(0);}
                } else {
                        ATH_MSG_WARNING("Evident problem with quality flag " << *strItr << " so setting to false!");
                        allSelectionResults[itr]->push_back(0);
                }
        }
     }     

     return StatusCode::SUCCESS;

}
