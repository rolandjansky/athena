/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_TOOL_HELPERFUNCTIONS
#define PANTAUALGS_TOOL_HELPERFUNCTIONS

#include "AthenaBaseComps/AthAlgTool.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "PanTauInterfaces/ITool_HelperFunctions.h"

#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFO.h"

namespace CLHEP {
    class HepLorentzVector;
    class Hep3Vector;
}

namespace PanTau {
    class TauConstituent;
}

// namespace xAOD {
//     class PFO;
//     class TauJet;
// }

class TVector3;
class eflowObject;

namespace PanTau {

class Tool_HelperFunctions : public AthAlgTool, virtual public PanTau::ITool_HelperFunctions  {
        
    public:
        
        Tool_HelperFunctions(const std::string&,const std::string&,const IInterface*);
        virtual ~Tool_HelperFunctions ();
        
        
        virtual void dumpFourMomentum(CLHEP::HepLorentzVector FourMom) const;
        virtual void dumpTauConstituent(PanTau::TauConstituent* tauConstituent) const;
        
        TauConstituent* getNeutralConstWithLargestAngle(CLHEP::HepLorentzVector                 charged, 
                                                        std::vector<PanTau::TauConstituent*>    neutral);
        
        virtual std::string convertNumberToString(double x) const;
        
//         virtual void vertexCorrection_eflowObjects(TVector3 vertexToCorrectTo, eflowObject* efo);
        virtual void vertexCorrection_PFOs(const xAOD::TauJet* tauJet, xAOD::PFO* efo);
        
        virtual int getBinIndex(std::vector<double> binEdges, double value) const;
        
        virtual double stddev(double sumOfSquares, double sumOfValues, int numConsts) const;
        virtual double deltaRprime(const CLHEP::Hep3Vector& vec1, const CLHEP::Hep3Vector& vec2) const;
        
        virtual int                 iPow(int man, int exp);
        virtual double              ulAngle(double x, double y);
        virtual double              sign(double a, double b);
        virtual std::vector<double> calcThrust(std::vector<TauConstituent*>* tauConstituents, bool& calcIsValid);
        virtual void                ludbrb(TMatrix* mom, double the, double phi, double bx, double by, double bz);
        virtual std::vector<double> calcFWMoments(std::vector<TauConstituent*>* tauConstituents, bool& calcIsValid);
        virtual std::vector<double> calcSphericity(std::vector<TauConstituent*>* tauConstituents, bool& calcIsValid);
        
    }; //end class Tool_HelperFunctions


}//end namespace PanTau




#endif // PANTAUALGS_TOOL_HELPERFUNCTIONS