// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#ifndef JETMOMENTTOOLS_JETBADCHANCORRTOOL_H
#define JETMOMENTTOOLS_JETBADCHANCORRTOOL_H

/**  @class JetBadChanCorrTool.
     Calculates correction for dead cells

     It produces and stores three jet moment : BchCorrCell, BchCorrDotx, BchCorrJet
     after computing the energy fraction of dead cells.

     IMPORTANT : this implementation is less complete than before.
      -> it can not make use of the older IHadronicCalibrationTool (wasn't used by default anyway)
      and therefore doesn't take the cryostat correction into account.

      Also it now requires the existence of a MissingCaloCellsMap in SG.

     @author Yousuke Kataoka
     @date (first implementation) December , 2009
     @author P.A. Delsart
     @date (faster implementation) March , 2011
     @date (Run 2 re-implemantation) March , 2013
     @author Bill Balunas
     @date (Run 3 re-implementation) February , 2020

*/

#include "AsgTools/AsgTool.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "JetInterface/IJetDecorator.h"

#include "JetRecCalo/MissingCellListTool.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "TH1D.h"


class Identifier;

class ITileBadChanTool;
class ITHistSvc;

class JetBadChanCorrTool: public asg::AsgTool,
                          virtual public IJetDecorator
{
  ASG_TOOL_CLASS0(JetBadChanCorrTool)
public:
  JetBadChanCorrTool(const std::string& name);

  virtual ~JetBadChanCorrTool();

  virtual StatusCode initialize() override;

  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

  virtual StatusCode setupEvent();


protected:
  // These two apply the moments they compute as decorations to the jet collection
  StatusCode correctionFromClustersBadCells(const xAOD::JetContainer& jets) const;
  StatusCode correctionFromCellsInJet(const xAOD::JetContainer& jets, const jet::CaloCellFastMap * badCellMap) const;
  // This one computes the moment without applying it
  float correctionFromCellsInCone(const xAOD::Jet* jet, const jet::CaloCellFastMap * badCellMap) const;

 private:

  ServiceHandle<ITHistSvc> m_thistSvc;

  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for the input jet container"};
  Gaudi::Property<int> m_nBadCellLimit{this, "NBadCellLimit", 10000, "Limit to calculate moments"};

  // for jet-level correction
  Gaudi::Property<std::string> m_streamName{this, "StreamName", "/JetBadChanCorrTool/", "Stream name"};
  Gaudi::Property<std::string> m_profileName{this, "ProfileName", "JetBadChanCorrTool.root", "Profile name"};
  Gaudi::Property<std::string> m_profileTag{this, "ProfileTag", "", "Profile tag"};

  Gaudi::Property<bool> m_useCone{this, "UseCone", true, "Use cone?"};
  Gaudi::Property<bool> m_useClusters{this, "UseClusters", false, "Use clusters?"};

  SG::ReadHandleKey<jet::CaloCellFastMap> m_badCellMap_key{this, "MissingCellMap", "MissingCaloCellsMap", "SG key for missing cell map"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_corrCellKey{this, "CorrCellDecorKey", "BchCorrCell", "SG key for cell level decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_corrDotxKey{this, "CorrDotxDecorKey", "BchCorrDotx", "SG key for DOTX decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_corrJetKey{this, "CorrJetDecorKey", "BchCorrJet", "SG key for jet level decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_corrJetForCellKey{this, "CorrJetForCellDecorKey", "BchCorrJetForCell", "SG key for JetForCell decoration"};

  // jet profiles
  class ProfileData {
  public:
    ProfileData(TH1D* th, int sample,
    double ptMin=0, double ptMax=9999,
    double etaMin=0, double etaMax=5.0,
    double phiMin=-M_PI, double phiMax=M_PI):
      m_th(th), m_sample(sample), 
      m_ptMin(ptMin), m_ptMax(ptMax), 
      m_etaMin(etaMin), m_etaMax(etaMax),
      m_phiMin(phiMin), m_phiMax(phiMax) {}

    virtual ~ProfileData(){}

    bool match(double pt, int sample, double eta, double phi) const {
      return ( pt>=m_ptMin && pt<m_ptMax 
         && sample==m_sample
         && fabs(eta)>=m_etaMin && fabs(eta)<m_etaMax 
         && phi>=m_phiMin && phi<m_phiMax);
    }

    double frac(double dr) const {
      int idr = m_th->FindBin(dr);
      return m_th->GetBinContent(idr);
    }
  private:
    TH1D* m_th;
    int m_sample;
    double m_ptMin;
    double m_ptMax;
    double m_etaMin;
    double m_etaMax;
    double m_phiMin;
    double m_phiMax;

  };
  std::vector<ProfileData> m_profileDatas[CaloCell_ID::Unknown];//24

  double getProfile(double pt, double dr, int sample, double eta, double phi) const;
};
#endif

#endif

// DoxygenDocumentation
/*! @class JetBadChanCorrTool
 @brief JetBadChanCorrTool

The energy fraction of dead cells are estimated by jet-level estimator using jet profile.
Some cells are already corrected in cell-level using neighboring cells, so this fraction is calculated.
The estimation by jet-level for those cells corrected in cell-level is also stored to see the difference or combined the corrections.

BCH_CORR_JET= jet level estimation of the energy fraction of dead cells
BCH_CORR_CELL= cell level correction, which is already corrected
BCH_CORR_JET_FORCELL= jet level estimation for the corrected cells in cell level

The relation and usage of the moment is
jet                                               = jet already corrected in cell level
jet x ( 1 - BCH_CORR_CELL )                       = jet without cell level correction
jet x ( 1 - BCH_CORR_CELL + BCH_CORR_JET )        = jet corrected by jet level estimation
jet x ( 1 + BCH_CORR_JET - BCH_CORR_JET_FORCELL ) = jet with cell level correction and corrected in jet level for not covered cells


<b>Properties:</b>
<table style="text-align: left; width: 80%;" border="1">
<tr><td> <b>Name</b>             </td> <td><b>Type</b>    </td> <td><b>Default</b> </td> <td><b>Description</b></td></tr>

<tr><td>ProcessNJetMax           </td> <td>               </td> <td>999999         </td> <td></td></tr>
<tr><td>NBadCellLimit            </td> <td>               </td> <td>10000          </td> <td>limit to calculate the moments, if exceed, giving up the calculation ie) when Calorimeter off</td></tr>
<tr><td>AttachCorrCell           </td> <td>               </td> <td>True           </td> <td>switch to attach BCH_CORR_CELL</td></tr>
<tr><td>AttachCorrDOTX           </td> <td>               </td> <td>True           </td> <td>switch to attach BCH_CORR_DOTX</td></tr>
<tr><td>AttachCorrJet            </td> <td>               </td> <td>True           </td> <td>switch to attach BCH_CORR_JET</td></tr>
<tr><td>AttachCorrJetForCell     </td> <td>               </td> <td>True           </td> <td>switch to attach BCH_CORR_JET_FORCELL</td></tr>

<tr><td>StreamName               </td> <td>               </td> <td>"/JetBadChanCorrTool/" </td> <td>name of stream name</td></tr>
<tr><td>ProfileName              </td> <td>               </td> <td>"JetBadChanCorrTool.root" </td> <td>name of profile data</td></tr>
<tr><td>ProfileTag               </td> <td>               </td> <td>""             </td> <td>tag to select profile,"" for all in file</td></tr>
<tr><td>UseCone                  </td> <td>               </td> <td>True           </td> <td>boundary of correction, cone or only associated cells</td></tr>
<tr><td>ConeDr                   </td> <td>               </td> <td>0.4            </td> <td>boundary of correction in case UseCone=True</td></tr>
<tr><td>UseCalibScale            </td> <td>               </td> <td>False          </td> <td>scale for calculation of the contribution of bad cells</td></tr>
<tr><td>CellCalibrator           </td> <td>               </td> <td>               </td> <td>calibration tool for cell-level correction</td></tr>

</table>
*/
