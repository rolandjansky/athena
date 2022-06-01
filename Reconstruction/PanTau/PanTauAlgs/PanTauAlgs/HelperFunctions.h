/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_TOOL_HELPERFUNCTIONS
#define PANTAUALGS_TOOL_HELPERFUNCTIONS

#include <string>
#include <iostream>
#include <vector>

#include "AsgMessaging/AsgMessaging.h"
#include "AsgTools/ToolHandle.h"

class TLorentzVector;
class TVector3;

namespace PanTau {
    class TauConstituent;
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
        
        TauConstituent* getNeutralConstWithLargestAngle(const TLorentzVector& charged, 
                                                        const std::vector<PanTau::TauConstituent*>& neutral) const;
        
        virtual std::string convertNumberToString(double x) const;
        
        virtual int getBinIndex(const std::vector<double>& binEdges, double value) const;
        
        virtual double stddev(double sumOfSquares, double sumOfValues, int numConsts) const;
        virtual double deltaRprime(const TVector3& vec1, const TVector3& vec2) const;
        
        virtual int                 iPow(int man, int exp) const;
        virtual double              ulAngle(double x, double y) const;
        virtual double              sign(double a, double b) const;

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
