/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundJetAssociationTool.h 589851 2014-03-27 20:01:30Z ssnyder $
#ifndef BACKGROUNDD3PDMAKER_BEAMBACKGROUNDJETASSOCIATIONTOOL_H
#define BACKGROUNDD3PDMAKER_BEAMBACKGROUNDJETASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "RecBackgroundEvent/BeamBackgroundData.h"
#include "xAODJet/Jet.h"


namespace D3PD {

/**
 * @short Tool "associating" Jet objects to BeamBackgroundData
 *
 *        BeamBackgroundData object contains a list of fake jet candidates
 *        which can be filled into D3PD using this associatoion tool.
 *
 * @author David Salek <David.Salek@cern.ch>
 *
 * $Revision: 589851 $
 * $Date: 2014-03-27 21:01:30 +0100 (Thu, 27 Mar 2014) $
 */
class BeamBackgroundJetAssociationTool :
  public MultiAssociationTool< BeamBackgroundData, xAOD::Jet >
{
public:
  typedef MultiAssociationTool< BeamBackgroundData, xAOD::Jet> Base;

  /// Regular AlgTool constructor
  BeamBackgroundJetAssociationTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);

  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const BeamBackgroundData& p);

  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const xAOD::Jet* next();


private:
  int m_jetCounter;
  const BeamBackgroundData* m_beamBackgroundData;
};


} // namespace D3PD


#endif 
