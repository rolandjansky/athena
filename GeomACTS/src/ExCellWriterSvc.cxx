#include "GeomACTS/ExCellWriterSvc.h"

#include <vector>
#include <deque>
#include <mutex>
#include <thread>


StatusCode
Acts::ExCellWriterSvc::initialize()
{
  RootExCellWriter<Acts::TrackParameters>::Config reccWriterConfig;
  reccWriterConfig.filePath       = "excells_charged.root";
  reccWriterConfig.treeName       = "extrapolation_charged";
  reccWriterConfig.writeBoundary  = true;
  reccWriterConfig.writeMaterial  = true;
  reccWriterConfig.writeSensitive = true;
  reccWriterConfig.writePassive   = true;
  m_rootEccWriter
      = std::make_shared<RootExCellWriter<Acts::TrackParameters>>(
          reccWriterConfig);

  ATH_MSG_INFO("Starting writer thread");
  m_doEnd = false;
  m_writeThread = std::thread(&ExCellWriterSvc::doWrite, this);

  return StatusCode::SUCCESS;
}
  
StatusCode 
Acts::ExCellWriterSvc::finalize()
{
  ATH_MSG_INFO("Waiting for writer thread to finish.");
  m_doEnd = true;
  m_writeThread.join();

  return StatusCode::SUCCESS;
}

void 
Acts::ExCellWriterSvc::store(std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>& ecells)
{
  std::lock_guard<std::mutex> lock(m_chargedMutex);

  for(size_t i=0;i<ecells.size();++i) {
    m_exCells.push_back(std::move(ecells[i]));
  }
}

void
Acts::ExCellWriterSvc::doWrite()
{
  using namespace std::chrono_literals;
  // wait until we have events
  while(m_exCells.size() == 0) {
    std::this_thread::sleep_for(2s);
  }

  while(true) {
    std::unique_lock<std::mutex> lock(m_chargedMutex);
    
    if (m_exCells.empty()) {
      lock.unlock();
      if (!m_doEnd) {
        std::this_thread::sleep_for(0.5s);
        continue;
      } else {
        ATH_MSG_INFO("Writer thread caught termination signal. Shutting down.");
        m_rootEccWriter->endRun();
        return;
      }
    }

    Acts::ExtrapolationCell<Acts::TrackParameters> ecell = std::move(m_exCells.front());
    m_exCells.pop_front();

    lock.unlock();

    m_rootEccWriter->write(ecell);
  }
    
  //std::vector<ExCellCharged> writeBuffer;
  
  //while(true) {
    //std::unique_lock<std::mutex> lock(m_chargedMutex);
    //if (m_exCells.size() < 1000 && !m_doEnd) {
      //lock.unlock();
      //std::this_thread::sleep_for(0.2s);
      //continue;
    //}

    //// have more some events
    //writeBuffer.reserve(m_exCells.size());
    //while(!m_exCells.empty()) {
      //writeBuffer.push_back(std::move(m_exCells.front()));
      //m_exCells.pop_front();
    //}
  
    //// emptied, can unlock
    //lock.unlock();

    //// now do the write
    //for(auto &ecell : writeBuffer) {
      //m_rootEccWriter->write(std::move(ecell));
    //}
    //writeBuffer.clear();

    //if (m_doEnd) {
      //lock.lock();
      //while(!m_exCells.empty()) {
        //m_rootEccWriter->write(std::move(m_exCells.front()));
        //m_exCells.pop_front();
      //}
      //lock.unlock();
      //break;
    //}

  //}

}
