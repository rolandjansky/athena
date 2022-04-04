/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsExCellWriterSvc.h"
#include "GaudiKernel/IInterface.h"

#include <vector>
#include <deque>
#include <mutex>
#include <thread>

#include "util/RootExCellWriter.h"

ActsExCellWriterSvc::ActsExCellWriterSvc( const std::string& name, ISvcLocator* svc )
: base_class(name, svc) {
}

StatusCode
ActsExCellWriterSvc::initialize()
{
  RootExCellWriter<Acts::TrackParameters>::Config reccWriterConfig;
  reccWriterConfig.filePath       = m_filePath;
  reccWriterConfig.treeName       = m_treeName;
  reccWriterConfig.writeBoundary  = m_writeBoundary;
  reccWriterConfig.writeMaterial  = m_writeMaterial;
  reccWriterConfig.writeSensitive = m_writeSensitive;
  reccWriterConfig.writePassive   = m_writePassive;
  m_rootEccWriter
      = std::make_shared<RootExCellWriter<Acts::TrackParameters>>(
          reccWriterConfig);

  ATH_MSG_INFO("Starting writer thread");
  m_doEnd = false;
  m_writeThread = std::thread(&ActsExCellWriterSvc::doWrite, this);

  return StatusCode::SUCCESS;
}
  
StatusCode 
ActsExCellWriterSvc::finalize()
{
  ATH_MSG_INFO("Waiting for writer thread to finish.");
  m_doEnd = true;
  m_writeThread.join();
  ATH_MSG_INFO("Writer thread has terminated.");

  return StatusCode::SUCCESS;
}

void 
ActsExCellWriterSvc::store(std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>& ecells)
{
  
  const auto& ctx = Gaudi::Hive::currentContext();

  std::lock_guard<std::mutex> lock(m_chargedMutex);

  for(size_t i=0;i<ecells.size();++i) {
    m_queue.emplace_back(ctx.eventID().event_number(), std::move(ecells[i]));
  }
}

void
ActsExCellWriterSvc::doWrite()
{
  using namespace std::chrono_literals;
  // wait until we have events
  while(m_queue.empty()) {
    std::this_thread::sleep_for(2s);
    if (m_doEnd) return;
  }

  while(true) {
    std::unique_lock<std::mutex> lock(m_chargedMutex);
    
    if (m_queue.empty()) {
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

    queue_item_t queue_item = std::move(m_queue.front());
    m_queue.pop_front();

    lock.unlock();
    
    size_t eventNum = queue_item.first;
    ExCellCharged ecell = std::move(queue_item.second);

    m_rootEccWriter->write(ecell, eventNum);
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
