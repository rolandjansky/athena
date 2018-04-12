#ifndef TrigT1Muctpi_IMuctpiSimTool
#define TrigT1Muctpi_IMuctpiSimTool

//#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IAlgTool.h"


class MuCTPI_RDO;

namespace LVL1 {
  class MuCTPIL1Topo;
}

namespace LVL1MUCTPI {

   /**
   * \class IMuctpiSimTool
   *
   * \brief Interface for tools to run MuCTPI sim
   */
  class IMuctpiSimTool : virtual public extend_interfaces1<IAlgTool> {
  public:
     DeclareInterfaceID(IMuctpiSimTool, 0, 1);

     /** \brief runs MuctpiSim to provide outputs for further use by HLT
      *  \param inputEvent reference to the inputEvent
      */

     virtual StatusCode fillMuCTPIL1Topo(LVL1::MuCTPIL1Topo &, int bcidOffset=0) const = 0;

  };

}


#endif
