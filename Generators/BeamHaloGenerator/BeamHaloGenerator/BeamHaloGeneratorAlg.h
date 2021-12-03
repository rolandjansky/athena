/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BEAMHALOGENERATORALG_H
#define BEAMHALOGENERATORALG_H

#include "GaudiKernel/ServiceHandle.h"
#include "GeneratorModules/GenModule.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"

#include <string>

class BeamHaloGenerator;


/** @class BeamHaloGeneratorAlg

    @author W. H. Bell <W.Bell@cern.ch>
    
    @brief A GenModule algorithm to produce beam halo HepMC records
    from input ASCII files produced with MARS or FLUKA beam background
    simulations.
*/
class BeamHaloGeneratorAlg : public GenModule {
public:
  BeamHaloGeneratorAlg(const std::string& name, ISvcLocator *svcLocator);
  virtual ~BeamHaloGeneratorAlg();
  virtual StatusCode genInitialize();
  virtual StatusCode genFinalize();

  /** Read one event from the selected input and convert it into
      GenEvent format.  If the end of file is reached the status code
      returned will not be success. */
  virtual StatusCode callGenerator();

  /** Fill the GenEvent pointer with the contents of the GenEvent
      cache.  The GenEvent cache is filled by calling
      callGenerator() */
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);

private:

  /** Input file type and therefore associated beam halo generator
      that should be used. */
  std::string m_inputTypeStr;
  
  /** Input file name */
  std::string m_inputFile;
  
  /** The position of the interface plane in mm. */
  double m_interfacePlane;
  
  /** Flag for flipping event */
  bool m_enableFlip;
  
  /** Flip probability */
  float m_flipProbability;
  
  /** Flag to enable or disable sampling. */
  bool m_enableSampling;
  
  /** The name of the binary buffer file, needed for sampling from a
      converted file. */
  std::string m_bufferFileName;
  
  /** A vector of strings defining generator settings. */
  std::vector<std::string> m_generatorSettings;
  
  /** A flag to allow monitoring plots to be turned on or off. */
  bool m_doMonitoringPlots;
  
  /** A pointer to the THist service for validation plots. */
  ITHistSvc *m_tHistSvc;
  
  /** Name of the random number stream */
  std::string m_randomStream;
  
  /** A pointer to the beam halo generator */
  BeamHaloGenerator *m_beamHaloGenerator;

  /** An empty GenEvent to cache the generate output between
      callGenerator and fillEvt. */
  HepMC::GenEvent m_evt;

  enum validationPlotsEnum {
    PRI_R,
    PRI_Z,
    PRI_Z_TCT, 
    SP_R_ALL,
    SP_E_ALL,
    SP_PZ_ALL,
    SP_PT_ALL,
    SP_R_PROTONS,
    SP_E_PROTONS,
    SP_PZ_PROTONS,
    SP_PT_PROTONS,
    SP_R_MUONS,
    SP_E_MUONS,
    SP_PZ_MUONS,
    SP_PT_MUONS,
    NPLOTS};

  /** An array of TH1F pointers for validation plots */
  TH1F *m_validationPlots[NPLOTS];

};

#endif
