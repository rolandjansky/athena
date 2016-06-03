/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexWeightCalculator_NNVertexWeightCalculator_H
#define TrkVertexWeightCalculator_NNVertexWeightCalculator_H
 
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexWeightCalculator.h"
#include <vector>
#include <TString.h>

//xAOD include
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleFwd.h"

 /**
  * @class Trk::NNVertexWeightCalculator 
  *
  * @author Johanna Bronner, April 2009
  *
  * ---------------------------------------------------
  * Changes:
  *
  * David Shope <david.richard.shope@cern.ch> (2016-06-01)
  *
  *   EDM Migration to xAOD - remove method using VxCandidate
  *
  */

class TTrainedNetwork;
class TH1;
class ITHistSvc;

 namespace Trk
 {
   class NeuralNetworkToHistoTool;
   
   class NNVertexWeightCalculator : public AthAlgTool, virtual public IVertexWeightCalculator
   {
   public:
     StatusCode initialize();
     StatusCode finalize();
     
     /**
      * constructor
      */
     NNVertexWeightCalculator(const std::string& t, const std::string& n, const IInterface*  p);
     
     /**
      * destructor
      */
     virtual ~NNVertexWeightCalculator();
     
    /**
     * WeightCalculator
     */
    virtual double estimateSignalCompatibility(const xAOD::Vertex& vertex);
         
   private:

    double mEstimateSignalCompatibility(const std::vector<const Trk::TrackParameters*>& tpList);
     
     ToolHandle<Trk::NeuralNetworkToHistoTool> m_networkToHistoTool;
     TTrainedNetwork *m_NN;
     int m_NLayers;
     ITHistSvc* m_iTHistSvc;
     std::string m_directoryName;
     std::vector <TH1*> m_NNhistos;
     double sphericityFunc(std::vector <std::vector<double> > v_P);
     double LogFunc(double value); 

   }; //end of class description
 }//end of namespace definition

 #endif
