/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_TOOL_HELPERFUNCTIONS
#define PANTAUALGS_TOOL_HELPERFUNCTIONS

//#include "NavFourMom/INavigable4MomentumCollection.h"


#include <string>
#include <iostream>

#include "AsgMessaging/AsgMessaging.h"
#include "AsgTools/ToolHandle.h"

#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFO.h"

class TLorentzVector;
class TVector3;

namespace PanTau {
    class TauConstituent2;
}

class TVector3;

namespace PanTau {

  class HelperFunctions : public asg::AsgMessaging {
    
    public:

        // from PhysicsAnalysis/TauID/TauDiscriminant/TauDiscriminant/MethodBase.h        
        HelperFunctions(const std::string& name = ""):
            AsgMessaging(name),
	    m_name(name)
	{}

	virtual ~HelperFunctions () {};
        
        
        virtual void dumpFourMomentum(TLorentzVector FourMom) const;
        virtual void dumpTauConstituent2(PanTau::TauConstituent2* tauConstituent) const;
        
        TauConstituent2* getNeutralConstWithLargestAngle(TLorentzVector                 charged, 
                                                        std::vector<PanTau::TauConstituent2*>    neutral);
        
        virtual std::string convertNumberToString(double x) const;
        
        // Will: moved to TauPi0ClusterScaler
        //void vertexCorrection_PFOs(const xAOD::TauJet* tauJet, xAOD::PFO* efo) const;
        
        virtual int getBinIndex(std::vector<double> binEdges, double value) const;
        
        virtual double stddev(double sumOfSquares, double sumOfValues, int numConsts) const;
        virtual double deltaRprime(const TVector3& vec1, const TVector3& vec2) const;
        
        virtual int                 iPow(int man, int exp) const;
        virtual double              ulAngle(double x, double y) const;
        virtual double              sign(double a, double b) const;
        virtual std::vector<double> calcThrust(std::vector<TauConstituent2*>* tauConstituents, bool& calcIsValid) const;
        virtual void                ludbrb(TMatrix* mom, double the, double phi, double bx, double by, double bz) const;
        virtual std::vector<double> calcFWMoments(std::vector<TauConstituent2*>* tauConstituents, bool& calcIsValid) const;
        virtual std::vector<double> calcSphericity(std::vector<TauConstituent2*>* tauConstituents, bool& calcIsValid) const;

#ifdef XAOD_ANALYSIS
	template<class T>
        static StatusCode bindToolHandle( ToolHandle<T>& handle, std::string tool_name){
	  T* tool = dynamic_cast<T*> (asg::ToolStore::get(tool_name));
	  if(!tool->isInitialized())
	    if (tool->initialize().isFailure())
	      return StatusCode::FAILURE;
	  handle = ToolHandle<T>(tool);
	  return StatusCode::SUCCESS;
	}
#else
	template<class T>
	static StatusCode bindToolHandle( ToolHandle<T>& , std::string){return StatusCode::SUCCESS;}
#endif

	std::string getName() const
	{
	  return m_name;
	}
        

    private:
	
	std::string m_name;


    }; //end class Tool_HelperFunctions


}//end namespace PanTau




#endif // PANTAUALGS_TOOL_HELPERFUNCTIONS
