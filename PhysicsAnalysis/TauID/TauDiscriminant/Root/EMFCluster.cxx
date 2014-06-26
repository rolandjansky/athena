/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*******************************************************//
// Name: EMFCluster.cxx                                  //
// Author: Michel Trottier-McDonald <mtm@cern.ch>        //
// Description: A simple class to house cluster          //
// TLorentzVectors and their associated fractions of     //
// energy in different calorimeter layers                //
//*******************************************************//

#include "TauDiscriminant/EMFCluster.h"

//---------------------------------------------------------
// Default Constructor
//---------------------------------------------------------
EMFCluster::EMFCluster()
{}


//---------------------------------------------------------
// Initiate TLorentzVector Contructor
//---------------------------------------------------------
EMFCluster::EMFCluster(double pt)
{
    m_cluster = TLorentzVector(pt, 0.0, 0.0, 0.0);
    m_PSSF = 0.0;
    m_EM2F = 0.0;
    m_EM3F = 0.0;
}



//---------------------------------------------------------
// Main constructor
//---------------------------------------------------------
EMFCluster::EMFCluster(const TLorentzVector& inCluster,
		       double inPSSF,
		       double inEM2F,
		       double inEM3F)
{
    m_cluster = inCluster;

    m_PSSF = inPSSF;
    m_EM2F = inEM2F;
    m_EM3F = inEM3F;

    update();
}


//---------------------------------------------------------
// Destructor
//---------------------------------------------------------
EMFCluster::~EMFCluster()
{}


//-----------------------------------------------------------
// Update method
//-----------------------------------------------------------
void EMFCluster::update()
{
    m_HADF = 1 - m_PSSF - m_EM2F - m_EM3F;
    m_pseudoHADF = 1 - m_PSSF - m_EM2F;

    double clE = m_cluster.E();

    m_PSSE = m_PSSF*clE;
    m_EM2E = m_EM2F*clE;
    m_EM3E = m_EM3F*clE;
    m_HADE = m_HADF*clE;
    m_pseudoHADE = m_pseudoHADF*clE;
}
