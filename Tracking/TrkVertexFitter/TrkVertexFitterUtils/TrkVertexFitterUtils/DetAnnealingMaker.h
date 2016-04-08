/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  class DetAnnealingMaker : public AthAlgTool, virtual public IVertexAnnealingMaker
  {
  public:
   
    StatusCode initialize();
   
    StatusCode finalize();

   /**
    * Default constructor due to Athena interface
    */
    DetAnnealingMaker(const std::string& t, const std::string& n, const IInterface*  p);
    
   /**
    * Destructor
    */
    virtual ~DetAnnealingMaker();

   /**
    * Rscets the annealing process to its beginning
    */
    void reset();
  
   /**
    * One more annealing step
    */
    void anneal();

   /**
    * Weight access method 
    */
    double getWeight(double chisq,const std::vector<double>& allchisq) const;

   /**
    * Weight access method 
    */
    double getWeight(double chisq) const;

   /**
    * Equilibrium is reached (last scheduled temperature) 
    */
    bool isEquilibrium() const
    {
      return m_isequilibrium;
    }
   
   /**
    * Access to the actual temperature value 
    */
    double actualTemp() const 
    {
      return m_SetOfTemperatures[m_PtrToActualTemperature];
    }
         
    private:
   
   /**
    * A vector of temperatures, you will start from SetOfTemperature[0] and Anneal towards SetOfTemperiture[last]
    */
     std::vector<double> m_SetOfTemperatures;
   
   /**
    * If actualtemperature == SetOfTemperature[last] then annealing is finished and m_isequilibrium=true
    */
     bool m_isequilibrium;
     
   /**
    * Weight will be insensitive to chi2 at order of magnitude of the cutoff...
    */  
     double m_cutoff;
     
     //std::vector<double>::const_iterator m_PtrToActualTemperature;
     unsigned int m_PtrToActualTemperature;
  };
}
#endif
