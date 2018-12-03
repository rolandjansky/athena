//  @file HLTTauMonTool_efficiencyRatioPlots.cxx
//  created by Eleni Myrto Asimakopoulou <eleni.myrto.asimakopoulou@cern.ch>
#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TEfficiency.h"
#include "TProfile.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>

#include "HLTTauMonTool.h"
#include "AthenaKernel/Units.h"
using namespace std;
using Athena::Units::GeV;
StatusCode HLTTauMonTool::efficiencyRatioPlots(const std::string & trigItem){
	
	//StatusCode sc;
   
    ATH_MSG_DEBUG ("HLTTauMonTool::doEffRatioPlots");
    setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
   
    std::string trig_item_EF = "HLT_"+trigItem;
    std::string trig_item_L1(LowerChain( trig_item_EF ) );

    // Loop over selected offline taus  
    for(unsigned int t=0;t<m_taus.size();t++)
    {
   	    TLorentzVector TauTLV = m_taus.at(t)->p4();
	    float eta = (float)TauTLV.Eta();
	    float pt = (float)TauTLV.Pt();
	    int ntracks = m_taus.at(t)->nTracks();
		float mu(Pileup());

		// Eff comp TProfiles for FTK vs Non-FTK chains
		//setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");

        // idperf sel. step
        if (LST_tracktwo_PassHLTsel.at(0) == 1)
        {
			if (effRatioChains_Active)
			{
		        if (LST_FTK_PassHLTsel.at(0) == 1)
		        {
					// both passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTK_chains.at(0), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_idperf")->Fill(pt/GeV,0);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_idperf")->Fill(eta,0);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_idperf")->Fill(mu,0);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_idperf")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_idperf")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_idperf")->Fill(mu,0);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_idperf")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_idperf")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_idperf")->Fill(mu,0);
						}
					}	
		        }
		        else
		        {
		            // only tracktwo chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTK_chains.at(0), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_idperf")->Fill(pt/GeV,-1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_idperf")->Fill(eta,-1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_idperf")->Fill(mu,-1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_idperf")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_idperf")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_idperf")->Fill(mu,-1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_idperf")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_idperf")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_idperf")->Fill(mu,-1);
						}
					}	
		        }
			}// end if (effRatioChains_Active)
        }
        else
		{
			if (effRatioChains_Active)
			{
		        if (LST_FTK_PassHLTsel.at(0) == 1)
		        {
		            // only FTK chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTK_chains.at(0), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_idperf")->Fill(pt/GeV,1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_idperf")->Fill(eta,1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_idperf")->Fill(mu,1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_idperf")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_idperf")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_idperf")->Fill(mu,1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_idperf")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_idperf")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_idperf")->Fill(mu,1);
						}
					}	
		        }// else --> both chains failled. No filling of the TProfiles
			}// end if (effRatioChains_Active)
        }

        // perf sel. step
        if (LST_tracktwo_PassHLTsel.at(1) == 1)
        {
			if (effRatioChains_Active)
			{
		        if (LST_FTK_PassHLTsel.at(1) == 1)
		        {
		            // both passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTK_chains.at(1), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_perf")->Fill(pt/GeV,0);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_perf")->Fill(eta,0);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_perf")->Fill(mu,0);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_perf")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_perf")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_perf")->Fill(mu,0);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_perf")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_perf")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_perf")->Fill(mu,0);
						}
					}	
		        }
		        else
		        {
		            // only tracktwo chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTK_chains.at(1), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_perf")->Fill(pt/GeV,-1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_perf")->Fill(eta,-1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_perf")->Fill(mu,-1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_perf")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_perf")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_perf")->Fill(mu,-1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_perf")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_perf")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_perf")->Fill(mu,-1);
						}
					}	
	            }
			} // end if (effRatioChains_Active)
			if (effRatio0ProngChains_Active)
			{
		        if (LST_FTK0Prong_PassHLTsel.at(1) == 1)
		        {
					// both passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel0Prong_FTK_chains.at(1), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_perf0")->Fill(pt/GeV,0);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_perf0")->Fill(eta,0);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_perf0")->Fill(mu,0);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_perf0")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_perf0")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_perf0")->Fill(mu,0);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_perf0")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_perf0")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_perf0")->Fill(mu,0);
						}
					}	
		        }
		        else
		        {
		            // only tracktwo chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel0Prong_FTK_chains.at(1), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_perf0")->Fill(pt/GeV,-1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_perf0")->Fill(eta,-1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_perf0")->Fill(mu,-1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_perf0")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_perf0")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_perf0")->Fill(mu,-1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_perf0")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_perf0")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_perf0")->Fill(mu,-1);
						}
					}	
		        }
			}// end if (effRatio0ProngChains_Active)
        }
        else
		{
			if (effRatioChains_Active)
			{
		        if (LST_FTK_PassHLTsel.at(1) == 1)
		        {
		            // only FTK chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTK_chains.at(1), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_perf")->Fill(pt/GeV,1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_perf")->Fill(eta,1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_perf")->Fill(mu,1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_perf")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_perf")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_perf")->Fill(mu,1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_perf")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_perf")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_perf")->Fill(mu,1);
						}
					}	
		        }// else --> both chains failled. No filling of the TProfiles
			} // end if (effRatioChains_Active)
			if (effRatio0ProngChains_Active)
			{
		        if (LST_FTK0Prong_PassHLTsel.at(1) == 1)
		        {
		            // only FTK chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel0Prong_FTK_chains.at(1), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_perf0")->Fill(pt/GeV,1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_perf0")->Fill(eta,1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_perf0")->Fill(mu,1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_perf0")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_perf0")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_perf0")->Fill(mu,1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_perf0")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_perf0")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_perf0")->Fill(mu,1);
						}
					}	
		        }// else --> both chains failled. No filling of the TProfiles
			} // end if (effRatio0ProngChains_Active)
        }

        // medium1 sel. step
        if (LST_tracktwo_PassHLTsel.at(2) == 1)
        {
			if (effRatioChains_Active)
			{
		        if (LST_FTK_PassHLTsel.at(2) == 1)
		        {
		            // both passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTK_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_medium1")->Fill(pt/GeV,0);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_medium1")->Fill(eta,0);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_medium1")->Fill(mu,0);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_medium1")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_medium1")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_medium1")->Fill(mu,0);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_medium1")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_medium1")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_medium1")->Fill(mu,0);
						}
					}	
		        }
		        else
		        {
		            // only tracktwo chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTK_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_medium1")->Fill(pt/GeV,-1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_medium1")->Fill(eta,-1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_medium1")->Fill(mu,-1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_medium1")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_medium1")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_medium1")->Fill(mu,-1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_medium1")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_medium1")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_medium1")->Fill(mu,-1);
						}
					}	
		        }
			} // end if(effRatioChains_Active)
			if (effRatio0ProngChains_Active)
			{
		        if (LST_FTK_PassHLTsel.at(2) == 1)
		        {
		            // both passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel0Prong_FTK_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_medium0")->Fill(pt/GeV,0);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_medium0")->Fill(eta,0);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_medium0")->Fill(mu,0);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_medium0")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_medium0")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_medium0")->Fill(mu,0);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_medium0")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_medium0")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_medium0")->Fill(mu,0);
						}
					}	
		        }
		        else
		        {
		            // only tracktwo chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel0Prong_FTK_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_medium0")->Fill(pt/GeV,-1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_medium0")->Fill(eta,-1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_medium0")->Fill(mu,-1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_medium0")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_medium0")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_medium0")->Fill(mu,-1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_medium0")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_medium0")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_medium0")->Fill(mu,-1);
						}
					}	
		        }
			} // end if(effRatio0ProngChains_Active)
			if (effRatioBDTChains_Active)
			{
		        if (LST_FTKBDT_PassHLTsel.at(2) == 1)
		        {
		            // both passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTKNoPrec_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(pt/GeV,0);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(eta,0);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(mu,0);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(mu,0);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(mu,0);
						}
					}	
		        }
		        else
		        {
		            // only tracktwo chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTKNoPrec_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(pt/GeV,-1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(eta,-1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(mu,-1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(mu,-1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(mu,-1);
						}
					}	
		        }
			} // end if(effRatioBDTChains_Active)
			if (effRatio0ProngBDTChains_Active)
			{
		        if (LST_FTK0ProngBDT_PassHLTsel.at(2) == 1)
		        {
		            // both passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel0Prong_FTKNoPrec_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(pt/GeV,0);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(eta,0);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(mu,0);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(mu,0);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(pt/GeV,0);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(eta,0);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(mu,0);
						}
					}	
		        }
		        else
		        {
		            // only tracktwo chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel0Prong_FTKNoPrec_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(pt/GeV,-1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(eta,-1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(mu,-1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(mu,-1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(pt/GeV,-1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(eta,-1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(mu,-1);
						}
					}	
		        }
			} // end if(effRatio0ProngBDTChains_Active)
        }
        else
		{
			if (effRatioChains_Active)
			{
		        if (LST_FTK_PassHLTsel.at(2) == 1)
		        {
		            // only FTK chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTK_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_medium1")->Fill(pt/GeV,1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_medium1")->Fill(eta,1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_medium1")->Fill(mu,1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_medium1")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_medium1")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_medium1")->Fill(mu,1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_medium1")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_medium1")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_medium1")->Fill(mu,1);
						}
					}	
		        }// else --> both chains failled. No filling of the TProfiles
			} // end if(effRatioChains_Active)
			if (effRatio0ProngChains_Active)
			{
		        if (LST_FTK0Prong_PassHLTsel.at(2) == 1)
		        {
		            // only FTK chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel0Prong_FTK_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKvsNonFTK_medium0")->Fill(pt/GeV,1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKvsNonFTK_medium0")->Fill(eta,1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKvsNonFTK_medium0")->Fill(mu,1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKvsNonFTK_medium0")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKvsNonFTK_medium0")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKvsNonFTK_medium0")->Fill(mu,1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKvsNonFTK_medium0")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKvsNonFTK_medium0")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKvsNonFTK_medium0")->Fill(mu,1);
						}
					}	
		        }// else --> both chains failled. No filling of the TProfiles
			} // end if(effRatio0ProngChains_Active)
			if (effRatioBDTChains_Active)
			{
		        if (LST_FTKBDT_PassHLTsel.at(2) == 1)
		        {
		            // only FTK chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel_FTKNoPrec_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(pt/GeV,1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(eta,1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(mu,1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(mu,1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKNoPrecvsNonFTK_medium1")->Fill(mu,1);
						}
					}	
		        }// else --> both chains failled. No filling of the TProfiles
			} // end if(effRatioBDTChains_Active)
			if (effRatio0ProngBDTChains_Active)
			{
		        if (LST_FTK0ProngBDT_PassHLTsel.at(2) == 1)
		        {
		            // only FTK chain passed
					setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency/EffRatios_FTKvsNonFTK");
					if ( HLTTauMatching(m_LST_HLTsel0Prong_FTKNoPrec_chains.at(2), TauTLV, 0.2)  )
					{
						profile("TProfRecoHLTLSTPtEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(pt/GeV,1);
						profile("TProfRecoHLTLSTEtaEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(eta,1);
						profile("TProfRecoHLTLSTMuEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(mu,1);
						if (ntracks==1) 
						{
							profile("TProfRecoHLTLSTPt1PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta1PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu1PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(mu,1);
						}
						if (ntracks>1) 
						{
							profile("TProfRecoHLTLSTPt3PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(pt/GeV,1);
							profile("TProfRecoHLTLSTEta3PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(eta,1);
							profile("TProfRecoHLTLSTMu3PEfficiency_CompFTKNoPrecvsNonFTK_medium0")->Fill(mu,1);
						}
					}	
		        }// else --> both chains failled. No filling of the TProfiles
			} // end if(effRatio0ProngBDTChains_Active)
        }
    } //end of the reco loop
   
    return StatusCode::SUCCESS;
}
