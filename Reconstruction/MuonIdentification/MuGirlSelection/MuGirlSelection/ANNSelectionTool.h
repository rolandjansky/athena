/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ANNSelectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUGIRLNSANNSELECTIONTOOL_H
#define MUGIRLNSANNSELECTIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuGirlInterfaces/IANNSelectionTool.h"
#include "MuGirlInterfaces/ICandidateTool.h"
#include "MuGirlInterfaces/CandidateSummary.h"
#include "MuGirlSelection/AnnResults_tgcfix_barrel_bad.h"
#include "MuGirlSelection/AnnResults_tgcfix_barrel_good.h"
#include "MuGirlSelection/AnnResults_tgcfix_barrel_endcap.h"
#include "MuGirlSelection/AnnResults_tgcfix_magnet.h"

#include "MuGirlSelection/AnnResults_barrelg_gt5.h"
#include "MuGirlSelection/AnnResults_barrelb_gt5.h"
#include "MuGirlSelection/AnnResults_overlapreg_gt5.h"
#include "MuGirlSelection/AnnResults_overlapmag_gt5.h"
#include "MuGirlSelection/AnnResults_endcap_gt5.h"
#include "MuGirlSelection/AnnResults_endcap_lt5.h"
#include "MuGirlSelection/AnnResults_forward_gt5.h"
#include "MuGirlSelection/AnnResults_forward_lt5.h"


#include "MuGirlSelection/test_barrel.h"
#include "MuGirlSelection/test_endcap.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/MuonFeature.h"
	
class MuonFeature;
class TrigMuonEFTrack;

class AtlasDetectorID;
class Identifier;

namespace Rec
{
class TrackParticle;
}
namespace Trk
{
class MeasuredPerigee;
}
namespace MuGirlNS
{
class ICandidateTool;
class AnnResults_tgcfix_barrel_good;
class AnnResults_tgcfix_barrel_bad;
class AnnResults_tgcfix_barrel_endcap;
class AnnResults_tgcfix_magnet;
class AnnResults_tgcfix_forward_gt5;
class AnnResults_tgcfix_forward_lt5;
class AnnResults_tgcfix_endcap_gt5;
class AnnResults_tgcfix_endcap_lt5;
class AnnResults_tgcfix_barrelg_gt5;
class AnnResults_tgcfix_barrelb_gt5;
class AnnResults_tgcfix_overlapreg_gt5;
class AnnResults_tgcfix_overlapmag_gt5;

/** @class ANNSelectionTool

    This is for the Doxygen-Documentation.
    Please delete these lines and fill in information about
    the Algorithm!
    Please precede every member function declaration with a
    short Doxygen comment stating the purpose of this function.

    @author  Sofia Vallecorsa <sofia.vallecorsa@cern.ch>
*/
class CandidateSummary;


class ANNSelectionTool : virtual public IANNSelectionTool, public AthAlgTool
{
public:
    ANNSelectionTool(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~ANNSelectionTool ();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  ();
    typedef double (MuGirlNS::ANNSelectionTool::*ANNFunction)(CandidateSummary* info);

    virtual void evaluateANN(CandidateSummary* info,double v[8], double NN[2]);

private:
    /** member variables for algorithm properties: */

    template <class T>
    StatusCode retrieve(ToolHandle<T>& pHandle, bool bError = false)
    {
        StatusCode sc = pHandle.retrieve();
        if (sc.isFailure())
        {
            if (bError)
                msg(MSG::ERROR) << "Cannot retrieve " << pHandle << endmsg;
            else
                msg(MSG::WARNING) << "Cannot retrieve " << pHandle << endmsg;
        }
        else
            msg(MSG::INFO) << "Retrieved " << pHandle << endmsg;
        return sc;
    }

    /** member variables for algorithm properties: */

    ToolHandle<MuGirlNS::ICandidateTool>  m_pMuGirlCandidateTool;

    int calculateRegion(double pt, double eta);
    double evaluateAnnBarrelGoodLowPt(CandidateSummary* info);
    double evaluateAnnBarrelBadLowPt(CandidateSummary* info);
    double evaluateAnnEndCapLowPt(CandidateSummary* info);
    double evaluateAnnForwardLowPt(CandidateSummary* info);
    double evaluateAnnRegionOverlapLowPt(CandidateSummary* info);
    double evaluateAnnMagnetOverlapLowPt(CandidateSummary* info);

    double evaluateAnnBarrelGoodHighPt(CandidateSummary* info);
    double evaluateAnnBarrelBadHighPt(CandidateSummary* info);
    double evaluateAnnEndCapHighPt(CandidateSummary* info);
    double evaluateAnnForwardHighPt(CandidateSummary* info);
    double evaluateAnnRegionOverlapHighPt(CandidateSummary* info);
    double evaluateAnnMagnetOverlapHighPt(CandidateSummary* info);

    double evaluateAnnTest_barrel(CandidateSummary* info);
    double evaluateAnnTest_endcap(CandidateSummary* info);
};
} // end of namespace

#endif
