/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EGAMMATOOLS_EMTRACKISOLATIONTOOL_H
#define EGAMMATOOLS_EMTRACKISOLATIONTOOL_H

/** 
    @class EMTrackIsolationTool
    Wrapper tool that calls the TrackIsolationTool from 
    Reconstruction/RecoTools/TrackIsolationTools to compute
    the ptcone and regisers it to the EMShower.

    @author Jochen Hartert
    @author Christos Anastopoulos

*/
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "egammaInterfaces/IEMTrackIsolationTool.h"
#include "IsolationTool/ITrackIsolationTool.h"


// forward declarations
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODTracking/VertexContainer.h" 

class EMTrackIsolationTool : public AthAlgTool, virtual public IEMTrackIsolationTool
{
 public:
  /** @brief Default constructor */
  EMTrackIsolationTool(const std::string& type,
		       const std::string& name,
		       const IInterface* parent);

  /** @brief Default destructor */
  ~EMTrackIsolationTool();

  /** @brief initialize method */
  virtual StatusCode initialize();
  

  /** @brief execute method */
  virtual StatusCode execute(xAOD::Egamma* eg,const xAOD::TrackParticleContainer* aTrackParticleContainer = 0);

  /** @brief method to retrieve the value of ptcone in a cone 0.2 */
  double ptcone20() const;
  /** @brief method to retrieve the value of ptcone in a cone 0.3 */
  double ptcone30() const;
  /** @brief method to retrieve the value of ptcone in a cone 0.4 */
  double ptcone40() const;

  /** @brief method to retrieve the value of nucone in a cone 0.2 */
  double nucone20() const;
  /** @brief method to retrieve the value of nucone in a cone 0.3 */
  double nucone30() const;
  /** @brief method to retrieve the value of nucone in a cone 0.4 */
  double nucone40() const;

 private:

  /** @brief the TrackIsolationTool */
  ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool;
 
  /** @brief vertex container name */
  std::string m_vertexContainerName;

  /** @brief variable to contain TrackIsolation value */
  double m_ptcone20;
  double m_ptcone30;
  double m_ptcone40;
  double m_nucone20;
  double m_nucone30;
  double m_nucone40;

  /** @brief To be used for brem assoc */
  bool m_useBremAssoc;


};

inline double EMTrackIsolationTool::ptcone20() const { return m_ptcone20;}
inline double EMTrackIsolationTool::ptcone30() const { return m_ptcone30;}
inline double EMTrackIsolationTool::ptcone40() const { return m_ptcone40;}
inline double EMTrackIsolationTool::nucone20() const { return m_nucone20;}
inline double EMTrackIsolationTool::nucone30() const { return m_nucone30;}
inline double EMTrackIsolationTool::nucone40() const { return m_nucone40;}

#endif // EGAMMATOOLS_EMTRACKISOLATIONTOOL_H
