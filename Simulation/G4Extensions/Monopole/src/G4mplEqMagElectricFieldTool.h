#ifndef MONOPOLE_G4mplEqMagElectricFieldTool_H
#define MONOPOLE_G4mplEqMagElectricFieldTool_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IEquationOfMotionTool.h"

/** @class G4mplEqMagElectricFieldTool G4mplEqMagElectricFieldTool.h "Monopole/G4mplEqMagElectricFieldTool.h"
 *
 *
 *
 */
class G4mplEqMagElectricFieldTool:  public AthAlgTool, virtual public IEquationOfMotionTool  {
public:
  /// Standard constructor
  G4mplEqMagElectricFieldTool( const std::string& type , const std::string& name,
                       const IInterface* parent ) ;

  /** Implements
   */
  virtual G4Mag_EqRhs* makeEquationOfMotion(G4MagneticField *emField) const override final;


protected:





};
#endif //MONOPOLE_G4mplEqMagElectricFieldTool_H
