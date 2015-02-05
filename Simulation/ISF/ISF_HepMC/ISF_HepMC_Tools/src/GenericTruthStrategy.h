/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericTruthStrategy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_GENERICTRUTHSTRATEGY_H
#define ISF_TOOLS_GENERICTRUTHSTRATEGY_H 1

// stl includes
#include <set>
#include <vector>

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// ISF includes
#include "ISF_HepMC_Interfaces/ITruthStrategy.h"

namespace ISF {
  typedef std::vector<int>              VertexTypesVector;
  typedef std::set<int>                 VertexTypesSet;
  typedef std::vector<int>              PDGCodesVector;
  typedef std::set<int>                 PDGCodesSet;

  /** @class GenericTruthStrategy
  
      TODO
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class GenericTruthStrategy : public AthAlgTool, public ITruthStrategy {
      
    public: 
     /** Constructor with parameters */
     GenericTruthStrategy( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~GenericTruthStrategy();

     // Athena algtool's Hooks
     StatusCode  initialize();
     StatusCode  finalize();

     /** true if the ITruthStrategy implementation applies to the given ITruthIncident */
     bool pass( const ITruthIncident& incident) const;

	  private:
     /** primary kinetic energy / transverse momentum cuts
         (pT is stored as pT^2 which allows for faster comparisons) */
     bool                                   m_usePrimaryPt;        //!< use pT or Ekin cuts?
     double                                 m_primaryPt2;          //!< primary particle
     double                                 m_primaryEkin;         //!< primary particle

     /** primary kinetic energy / transverse momentum cuts
         (pT is stored as pT^2 which allows for faster comparisons) */
     bool                                   m_useSecondaryPt;      //!< use pT or Ekin cuts?
     double                                 m_secondaryPt2;        //!< pT momentum cut
     double                                 m_secondaryEkin;       //!< Ekin cut
     bool                                   m_allowSecondaryOrPrimaryPass; //!< pass cuts if primary did not

     /** vertex type (physics code) checks */
     VertexTypesVector                      m_vertexTypesVector;  //!< Python property
     VertexTypesSet                         m_vertexTypes;        //!< optimized for search
     bool                                   m_doVertexRangeCheck;
     int                                    m_vertexTypeRangeLow;
     int                                    m_vertexTypeRangeHigh;
     unsigned                               m_vertexTypeRangeLength;

     /** PDG code checks */
     PDGCodesVector                         m_primPdgCodesVector;  //!< Python property
     PDGCodesSet                            m_primPdgCodes;        //!< optimized for search
   }; 
  
}


#endif //> !ISF_TOOLS_GENERICTRUTHSTRATEGY_H
