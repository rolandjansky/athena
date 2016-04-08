/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUINTERFACES_ITOOL_HELPERFUNCTIONS_H
#define PANTAUINTERFACES_ITOOL_HELPERFUNCTIONS_H
// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "TMatrix.h"
#include <vector>

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



    static const InterfaceID IID_ITool_HelperFunctions("PanTau::ITool_HelperFunctions", 1,0);

    /** @class ITool_HelperFunctions
        @brief Interface for Tool_HelperFunctions
        @author Christian Limbach (limbach@physik.uni-bonn.de)
    */


    class ITool_HelperFunctions : virtual public IAlgTool {
        public:
            static const InterfaceID& interfaceID();
            
            virtual void dumpFourMomentum(CLHEP::HepLorentzVector FourMom) const = 0;
            virtual void dumpTauConstituent(PanTau::TauConstituent* tauConstituent) const = 0;
            
            virtual TauConstituent* getNeutralConstWithLargestAngle(CLHEP::HepLorentzVector                 charged, 
                                                                    std::vector<PanTau::TauConstituent*>    neutral) = 0;
            
            virtual std::string convertNumberToString(double x) const = 0;
            
            virtual int getBinIndex(std::vector<double> binEdges, double value) const = 0;
            
//             virtual void vertexCorrection_eflowObjects(TVector3 vertexToCorrectTo, eflowObject* efo) = 0;
            virtual void vertexCorrection_PFOs(const xAOD::TauJet* tauJet, xAOD::PFO* efo) = 0;
            
            virtual double stddev(double sumOfSquares, double sumOfValues, int numConsts) const = 0;
            virtual double deltaRprime(const CLHEP::Hep3Vector& vec1, const CLHEP::Hep3Vector& vec2) const = 0;
        
            virtual int                 iPow(int man, int exp) = 0;
            virtual double              ulAngle(double x, double y) = 0;
            virtual double              sign(double a, double b) = 0;
            virtual std::vector<double> calcThrust(std::vector<TauConstituent*>* tauConstituents, bool& calcIsValid) = 0;
            virtual void                ludbrb(TMatrix* mom, double the, double phi, double bx, double by, double bz) = 0;
            virtual std::vector<double> calcFWMoments(std::vector<TauConstituent*>* tauConstituents, bool& calcIsValid) = 0;
            virtual std::vector<double> calcSphericity(std::vector<TauConstituent*>* tauConstituents, bool& calcIsValid) = 0;
    };
    

    inline const InterfaceID& PanTau::ITool_HelperFunctions::interfaceID() {
        return IID_ITool_HelperFunctions;
    }

}
#endif //PANTAUINTERFACES_ITOOL_HELPERFUNCTIONS_H

