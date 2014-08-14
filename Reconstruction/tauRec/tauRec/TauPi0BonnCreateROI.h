/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0BONNCREATEROI_H
#define	TAUREC_TAUPI0BONNCREATEROI_H

#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include <map>

#include "GaudiKernel/ToolHandle.h"
#include "tauRec/TauToolBase.h"

#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTau/TauJet.h"



class IHadronicCalibrationTool;
class IExtrapolateToCaloTool;
class ICaloCellMakerTool;
class TauOriginCorrectionTool;
class TauPi0BonnParser;
/**
 * @brief Create ROIs for the "Bonn" Pi0 Finder.
 * 
 * @author Veit Scharf
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 */

class TauPi0BonnCreateROI : public TauToolBase {
public:
    TauPi0BonnCreateROI(const std::string& type,
            const std::string& name,
            const IInterface *parent);
    virtual ~TauPi0BonnCreateROI();

    virtual StatusCode initialize();
    virtual StatusCode eventInitialize(TauCandidateData *data);
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode eventFinalize(TauCandidateData *data);
    virtual StatusCode finalize();

private:

    /** @brief get extrapolated track position at each layer */
    void getExtrapolatedPositions( std::vector<const xAOD::TrackParticle*>, 
                                   int sampling);


    /** @brief get hadronic energy associated to the tracks */
    std::vector<double> getEstEcalEnergy( std::vector<const xAOD::TrackParticle*>,
                                          const xAOD::CaloClusterContainer*, 
                                          const xAOD::TauJet*); 
  
    /** @brief get cell weight from lateral shape */
    double getLatWeight( int   /* samp     */,
                        double /* deltaEta */,
                        double /* deltaPhi */,
                        double /* cellWidthEta */,
                        double /* cellWidthPhi */,
                        unsigned   /* trackNumber  */,
                        const xAOD::TrackParticle* /* track */
                        );

    /** @brief store cell in output container */ 
    void storeCell(const CaloCell* /* cell*/, 
                   double          /* subtractedEnergy */);


    /** @brief tool handles */
    ToolHandle<IHadronicCalibrationTool> m_caloWeightTool;
    ToolHandle<IExtrapolateToCaloTool> m_trackToCaloTool;
    ToolHandle<ICaloCellMakerTool> m_cellMakerTool;

    /** @brief calo cell navigation */
    const CaloDetDescrManager* m_calo_dd_man;
    const CaloCell_ID* m_calo_id;
   
    /** @brief lateral shower parameteristaion parser */
    boost::scoped_ptr<TauPi0BonnParser> m_latParser;
    std::string m_latParFile;    

 
    /** @brief all calo cell container name */
    std::string m_caloCellContainerName; // TODO: replace with tau clusters? 
  
    /** @brief all cluster container name */
    std::string m_clusterContainerName;  // TODO: replace with tau clusters?
 
    /** @brief output cell container and name*/ 
    CaloCellContainer *m_pPi0CellContainer;
    std::string m_pi0CellContainerName;

    /** @brief new charged PFO container and name */
    xAOD::PFOContainer* m_chargedPFOContainer;
    std::string m_chargedPFOContainerName;
    xAOD::PFOAuxContainer* m_chargedPFOAuxStore;

    /** @brief map of tracks to normalization factors*/
    std::map<int, TH1*> m_trackHistMapBarrel;
    std::map<int, TH1*> m_trackHistMapEndcap;

    /** @brief variables used for shower parameteristaion */
    double m_pt;
    double m_abseta;
    double m_hadf;
    double m_sampling; // its really an int but just use double
                      // should function correctly in parser

    /** @brief hash map in order to keep track, which cells have been added to output cell container*/
    std::vector<CaloCell*> m_addedCellsMap;

    /** @brief extrapolated position of tracks and vector of bools to keep track for which sampleings this has already been done */
    std::vector<std::vector<double> > m_tracksEtaAtSampling;
    std::vector<std::vector<double> > m_tracksPhiAtSampling;
    std::vector<bool> m_extrapolatedSamplings;
    std::vector<double> m_defaultValues;
    std::vector<double> m_defaultValuesZero;
};

#endif	/* TAUPI0BONNCREATEROI_H */

