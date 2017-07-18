/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ShowerShapeRegression_h
#define ShowerShapeRegression_h

class ShowerShapeRegression
{
public:

   ShowerShapeRegression();
   ~ShowerShapeRegression();

   void Run(std::vector<string> );

   void TMVARegressionTraining(std::string);

   std::vector<float> TMVARegressionApplication(float, float, string);



   void set_calolayer(int calolayer_) { m_calolayer = calolayer_; }
   void set_PCAbin(int PCAbin_) { m_PCAbin = PCAbin_; }
   void set_nbinsR(int nbinsR_) { m_nbinsR = nbinsR_; }
   void set_particle(string particle_) { m_particle = particle_; }
   void set_energy(float energy_) { m_particleEnergy = energy_; }
   void set_eta(float etamin_, float etamax_) { m_etamin = etamin_; m_etamax = etamax_; }


   void set_topDir(string topDir_) { m_topDir = topDir_; }
   void set_fileName(string fileName_) { m_fileName = fileName_; }
   void set_outputDirName(string outputDirName_) { m_outputDirName = outputDirName_; }
   void set_NNinputName(string NNinputName_) { m_NNinputName = NNinputName_; }
   void set_NNoutputName(string NNoutputName_) { m_NNoutputName = NNoutputName_; }

   void set_neurons(int neurons_) { m_neurons = neurons_; }
   void set_hiddenLayers(int hiddenLayers_) { m_hiddenLayers = hiddenLayers_; }



private:

   int m_calolayer;
   int m_PCAbin;
   int m_nbinsR;
   string m_particle;
   float m_particleEnergy; // in Gev
   float m_etamin;
   float m_etamax;

   string m_topDir;
   string m_fileName;
   string m_outputDirName;
   string m_NNinputName;
   string m_NNoutputName;

   // * TMVA parameters

   int m_neurons;
   int m_hiddenLayers;



   ClassDef(ShowerShapeRegression, 1);
};

#if defined(__MAKECINT__)
#pragma link C++ class ShowerShapeRegression+;
#endif


#endif
