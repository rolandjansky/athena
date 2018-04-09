#ifndef GEOMACTS_EXCELLWRITER_SERVICE
#define GEOMACTS_EXCELLWRITER_SERVICE

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"

#include "GeomACTS/IExCellWriterSvc.h"
#include "GeomACTS/Extrapolation/RootExCellWriter.hpp"

#include "ACTS/EventData/TrackParameters.hpp"

#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <atomic>


namespace Acts {

template <class>
class ExtrapolationCell;


class ExCellWriterSvc : public extends<AthService, IExCellWriterSvc> {
public:
    
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
    
  ExCellWriterSvc( const std::string& name, ISvcLocator* svc )
   : base_class(name, svc) {}

  void
  store(std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>& ecells) override;

private:
  using ExCellCharged = Acts::ExtrapolationCell<Acts::TrackParameters>;

  std::shared_ptr<RootExCellWriter<Acts::TrackParameters>> m_rootEccWriter;
  std::deque<Acts::ExtrapolationCell<Acts::TrackParameters>> m_exCells;
  std::mutex m_chargedMutex;
  std::thread m_writeThread;
  std::atomic<bool> m_doEnd;

  void doWrite();


};

}


#endif 
