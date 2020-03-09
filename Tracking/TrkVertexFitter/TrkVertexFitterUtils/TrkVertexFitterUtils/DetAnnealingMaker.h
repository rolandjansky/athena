/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXADAPTIVEFITTERUTILS_DETANNEALINGMAKER_H
#define TRKVERTEXADAPTIVEFITTERUTILS_DETANNEALINGMAKER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexAnnealingMaker.h"

/**
 * @class Trk::DetAnnealingMaker
 * This class implements an annealing maker.
 * The weight implemented is phi(chi2)=exp[-1/2.*chi2/Temp]
 * @author N. Giacinto Piacquadio (for the Freiburg Group)
 */


namespace Trk
{
  class DetAnnealingMaker : public extends<AthAlgTool, IVertexAnnealingMaker>
  {
  public:
   
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

   /**
    * Default constructor due to Athena interface
    */
    DetAnnealingMaker(const std::string& t, const std::string& n, const IInterface*  p);
    
   /**
    * Resets the annealing process to its beginning
    */
    virtual void reset(AnnealingState& state) const override;
  
   /**
    * One more annealing step
    */
    virtual void anneal(AnnealingState& state) const override;

   /**
    * Weight access method 
    */
    virtual double getWeight(const AnnealingState& state,
                             double chisq,
                             const std::vector<double>& allchisq) const override;

   /**
    * Weight access method 
    */
    virtual double getWeight(const AnnealingState& state,
                             double chisq) const override;

   /**
    * Equilibrium is reached (last scheduled temperature) 
    */
    virtual bool isEquilibrium(const AnnealingState& state) const  override
    {
      return state >= m_SetOfTemperatures.size();
    }
   
   /**
    * Access to the actual temperature value 
    */
    virtual double actualTemp(const AnnealingState& state) const override
    {
      if (state >= m_SetOfTemperatures.size()) {
        return m_SetOfTemperatures.back();
      }
      return m_SetOfTemperatures[state];
    }
         
    private:
   
   /**
    * A vector of temperatures, you will start from SetOfTemperature[0] and Anneal towards SetOfTemperature[last]
    */
     std::vector<double> m_SetOfTemperatures;
   
   /**
    * Weight will be insensitive to chi2 at order of magnitude of the cutoff...
    */  
     double m_cutoff;
  };
}
#endif
