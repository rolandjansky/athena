/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCALIBCLUSTERMOMENTSMAKER_H
#define CALOCALIBCLUSTERMOMENTSMAKER_H
/**
 * @class CaloCalibClusterMomentsMaker
 * @version \$Id: CaloCalibClusterMomentsMaker.h,v 1.8 2009-05-18 16:16:48 pospelov Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 17-June-2008
 * @brief Calculate calibration hit based moments for CaloCluster objects
 *
 * This is a CaloClusterCollectionProcessor which can be plugged into
 * a CaloClusterMaker for calculating calibration hit based moments
 * for each CaloCluster. */
class CaloDetDescrManager; 
class CaloDetDescrElement;
class CaloCell_ID;
class CaloDM_ID;
class CaloDmDescrManager;
class CaloDmEnergy;

#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include <string>
#include <vector>
#include <set>
#include <atomic>

class ClusWeight {
 public: 
  int iClus;
  double weight;
  ClusWeight * next;
};

class CalibHitIPhiIEtaRange {
 public:
  char iPhi,iEtaMin,iEtaMax;
};
  
class CaloCalibClusterMomentsMaker: public AthAlgTool, virtual public CaloClusterCollectionProcessor
{
 public:    
  
  /** @brief typedef for a pair to index the enums defined in
   *  CaloClusterMoment with a string. */
  typedef std::pair<std::string,xAOD::CaloCluster::MomentType> 
    moment_name_pair;

  /** @brief vector of pairs defined above.*/
  typedef std::vector<moment_name_pair> moment_name_vector;

  /** @brief set of pairs defined above.*/
  typedef std::set<moment_name_pair> moment_name_set;

  CaloCalibClusterMomentsMaker(const std::string& type, const std::string& name,
			       const IInterface* parent);

  virtual ~CaloCalibClusterMomentsMaker() override;

  using CaloClusterCollectionProcessor::execute;
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloClusterContainer* theClusColl) const override;
  virtual StatusCode initialize() override;

 private:

  /** 
   * @brief vector holding the input list of names of moments to
   * calculate.
   *
   * This is the list of desired names of moments given in the
   * jobOptions.*/
  std::vector<std::string>  m_momentsNames; 

  /** 
   * @brief vector holding the names of valid moments which can be
   * calculated.
   *
   * Each name has to correspond to one of the enums in
   * CaloClusterMoment. This list is defined in the constructor of
   * CaloCalibClusterMomentsMaker and holds the names and enums of
   * moments defined in CaloClusterMoment. The name is used as the key
   * to the enum. */
  moment_name_vector m_validNames;

  /** 
   * @brief set of moments which will be calculated.
   *
   * This set will hold each valid enum indexed with the name if it
   * was found on the input list (m_momentsNames) and in the list of
   * valid moment names (m_validNames). */
  moment_name_set m_validMoments;

  /** 
   * @brief vector holding the list of moment names which go in the first store
   * i.e. they are available on the AOD.
   *
   * Only moments listed in this property can later be directly retrieved from
   * AOD - the others are available in the ESD ... */
  std::vector<std::string>  m_momentsNamesAOD; 

  /** 
   * @brief set holding the list of moment enums which go in the first store
   * i.e. they are available on the AOD.
   *
   * Only moments listed in this property can later be directly retrieved from
   * AOD - the others are available in the ESD ... */
  std::set<xAOD::CaloCluster::MomentType>  m_momentsAOD; 

  /** 
   * @brief vector of calibration hit container names to use. 
   *
   * The containers specified in this property should hold calibration
   * hits inside the calorimeter systems. */
  SG::ReadHandleKeyArray<CaloCalibrationHitContainer> m_CalibrationHitContainerNames;

  /** 
   * @brief vector of dead material calibration hit container names to use. 
   *
   * The containers specified in this property should hold calibration
   * hits outside the calorimeter systems - i.e. dead material hits ... */
  SG::ReadHandleKeyArray<CaloCalibrationHitContainer> m_DMCalibrationHitContainerNames;

  const CaloCell_ID* m_calo_id;

  const CaloDM_ID*    m_caloDM_ID;

  const CaloDmDescrManager* m_caloDmDescrManager; 

  int m_n_phi_out;
  int m_n_eta_out;
  double m_out_phi_max;
  double m_out_eta_max;

  double m_rmaxOut[3];

  std::vector<CalibHitIPhiIEtaRange> *m_i_phi_eta[3];

  mutable std::atomic<bool> m_foundAllContainers;

  enum keys_dm_energy_sharing {kMatchDmOff, kMatchDmLoose, kMatchDmMedium, kMatchDmTight};

  bool m_doDeadEnergySharing;
  bool m_doOutOfClusterL;
  bool m_doOutOfClusterM;
  bool m_doOutOfClusterT;
  bool m_doDeadL;
  bool m_doDeadM;
  bool m_doDeadT;
  float m_energyMin;
  float m_energyMinCalib;
  float m_apars_alpha;
  float m_apars_r0;
  //double m_showerScale;
  int m_MatchDmType;

  double angle_mollier_factor(double x) const;
};

#endif // CALOCALIBCLUSTERMOMENTSMAKER_H






