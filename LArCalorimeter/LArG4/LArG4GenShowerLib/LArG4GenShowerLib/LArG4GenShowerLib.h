/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4GENSHOWERLIB_LARG4GENSHLIB_H
#define LARG4GENSHOWERLIB_LARG4GENSHLIB_H

// STL includes
#include <string>
#include <list>
#include <map>

#include "AthenaBaseComps/AthAlgorithm.h"

// CLHEP include(s)
#include "CLHEP/Vector/ThreeVector.h"

// forward includes in namespaces
namespace ShowerLib {
  class IShowerLib;
  class StepInfoCollection;
  class StepInfo;
  typedef std::list<StepInfo*> StepInfoList;
}
#include "AtlasHepMC/GenParticle_fwd.h"


  /**
   *
   *   @short Class for shower library generation algorithm
   *
   *      Create shower library using geant hits
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Sasha Glazov, DESY Hamburg, Germany
   *
   * @version $Id: LArG4GenShowerLib.h 711210 2015-11-27 15:56:00Z jchapman $
   *
   */
class LArG4GenShowerLib : public AthAlgorithm {

 public:

  LArG4GenShowerLib(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:

  void clusterize(ShowerLib::StepInfoList* stepinfo);

  const ShowerLib::StepInfoCollection* getStepInfo();

  ShowerLib::StepInfoList* copyStepInfo(const ShowerLib::StepInfoCollection* stepinfo);

  ShowerLib::StepInfoList* copyStepInfoZeroCleanup(const ShowerLib::StepInfoCollection* stepinfo);

  void truncate(ShowerLib::StepInfoList* stepinfo);

  //! return first MC truth particle for event
  const HepMC::GenParticle* getParticleFromMC();
  //! calculate moments from StepInfoCollection
  void calculateMoments(const ShowerLib::StepInfoCollection&  eventSteps,
			double& weights, double& xavfra, double& yavfra, double& ravfra);
  //! adding tag information (release, detector description, ...) to library comment
  void addingTagsToLibrary();

  /* data members */

  DoubleProperty            m_maxDistance;          //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
  DoubleProperty            m_maxRadius;            //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
  DoubleProperty            m_minEnergy;            //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
  DoubleProperty            m_containmentEnergy;    //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
  DoubleProperty            m_energyFraction;       //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink

  StringArrayProperty		m_lib_struct_files;

  typedef std::map<std::string, ShowerLib::IShowerLib*> libMap;
  libMap m_libraries;                  //!< pointer to shower library
  libMap m_libraries_by_filename;

  int m_stat_numshowers;
  int m_stat_valid;
  int m_stat_invalid;
  int m_stat_nolib;

  std::map<ShowerLib::IShowerLib*, int> m_stat_lib_saved;
  std::map<ShowerLib::IShowerLib*, int> m_stat_lib_notsaved;

  std::string m_physicslist_name;


}; // class LArG4GenShowerLib

#endif // LARG4GENSHOWERLIB_LARG4GENSHLIB_H
