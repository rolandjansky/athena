/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEDATA_EXAMPLETRACK_H
#define ATHENAPOOLEXAMPLEDATA_EXAMPLETRACK_H

/** @file ExampleTrack.h
 *  @brief This file contains the class definition for the ExampleTrack class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleTrack.h,v 1.9 2009-02-23 16:10:30 gemmeren Exp $
 **/

#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "Navigation/Navigable.h"
#include "AthenaPoolExampleData/ExampleHitContainer.h"

#include <string>

/** @class ExampleTrack
 *  @brief This class provides a dummy track data object for AthenaPool.
 **/
class ExampleTrack {

public: // Constructor and Destructor
   /// Default Constructor
   ExampleTrack() : m_pt(0.0), m_eta(0.0), m_phi(0.0), m_detector(""), m_elementlink1(), m_elementlink2(), m_elementlinkvector(), m_navigable(), m_weightednavigable() {}
   /// Destructor
   virtual ~ExampleTrack() {}

public: // Non-static members
   /// @return the transverse momentum value.
   double getPT() const { return(m_pt); }

   /// @return the eta value.
   double getEta() const { return(m_eta); }

   /// @return the phi value.
   double getPhi() const { return(m_phi); }

   /// @return the detector string.
   const std::string& getDetector() const { return(m_detector); }

   /// Set the transverse momentum value.
   /// @param pt [IN] transverse momentum value.
   void setPT(double pt) { m_pt = pt; }

   /// Set the eta value.
   /// @param eta [IN] eta value.
   void setEta(double eta) { m_eta = eta; }

   /// Set the phi value.
   /// @param phi [IN] phi value.
   void setPhi(double phi) { m_phi = phi; }

   /// Set the detector string.
   /// @param detector [IN] detector string.
   void setDetector(std::string detector) { m_detector = detector; }

   /// @return a pointer to the first element link.
   ElementLink<ExampleHitContainer>* getElementLink1() { return(&m_elementlink1); }
   const ElementLink<ExampleHitContainer>* getElementLink1() const { return(&m_elementlink1); }
   /// @return a constant pointer to the first linked object.
   const ExampleHit* getElement1() const { return(*m_elementlink1); }
   /// @return a pointer to the second element link.
   ElementLink<ExampleHitContainer>* getElementLink2() { return(&m_elementlink2); }
   const ElementLink<ExampleHitContainer>* getElementLink2() const { return(&m_elementlink2); }
   /// @return a constant pointer to the second linked object.
   const ExampleHit* getElement2() const { return(*m_elementlink2); }

   /// @return a pointer to the element link vector.
   ElementLinkVector<ExampleHitContainer>* getElementLinkVector() { return(&m_elementlinkvector); }
   /// @return a constant pointer to the element link vector.
   const ElementLinkVector<ExampleHitContainer>* getElementLinkVector() const { return(&m_elementlinkvector); }

   /// @return a pointer to the navigable.
   Navigable<ExampleHitContainer>* getNavigable() { return(&m_navigable); }
   /// @return a const pointer to the navigable.
   const Navigable<ExampleHitContainer>* getNavigable() const { return(&m_navigable); }

   /// @return a pointer to the weighted navigable.
   Navigable<ExampleHitContainer, double>* getWeightedNavigable() { return(&m_weightednavigable); }
   /// @return a const pointer to the weighted navigable.
   const Navigable<ExampleHitContainer, double>* getWeightedNavigable() const { return(&m_weightednavigable); }

private:
   double m_pt;
   double m_eta;
   double m_phi;
   std::string m_detector;

   ElementLink<ExampleHitContainer> m_elementlink1, m_elementlink2;
   ElementLinkVector<ExampleHitContainer> m_elementlinkvector;
   Navigable<ExampleHitContainer> m_navigable;
   Navigable<ExampleHitContainer, double> m_weightednavigable;
};
#endif
