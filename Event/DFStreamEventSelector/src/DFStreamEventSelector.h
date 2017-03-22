/*   --+- C++ -+-- */
/* A simple class to use DFDataSource implementations to read data
 *
 */
#ifndef __DFSTREAMEVENTSELECTOR_H
#define __DFSTREAMEVENTSELECTOR_H

#ifndef GAUDIKERNEL_IEVTSELECTOR_H
# include "GaudiKernel/IEvtSelector.h"
#endif

#ifndef GAUDIKERNEL_PROPERTY_H
# include "GaudiKernel/Property.h"
#endif 

#ifndef GAUDIKERNEL_SERVICEHANDLE_H
# include "GaudiKernel/ServiceHandle.h"
#endif 

#ifndef GAUDIKERNEL_TOOLHANDLE_H
#include "GaudiKernel/ToolHandle.h"
#endif
#ifndef EVENTINFO_EVENTID_H
# include "EventInfo/EventID.h"  /* number_type */
#endif

#include "AthenaBaseComps/AthService.h"
class DFStreamEventSelector:public AthService,
			     public IEvtSelector{
public:
  DFStreamEventSelector(const std::string &name, ISvcLocator* pSvcLocator);
  virtual ~DFStreamEventSelector();
  typedef IEvtSelector::Context   EvtContext;
  /**Create and return a context object that will
     keep track of the state of selection.

     @param c Reference of a pointer to a Context object.
  */
  virtual StatusCode createContext(EvtContext*& c) const override;

  /**Fetch the next event or the first event if it will be use soon
     after the creation of the context.
     It will return StatusCode::FAILURE if there have been problem in the fetching or it
     has been reached the end of the list of events.

     @param c Reference to the Context object.
  */
  virtual StatusCode next(EvtContext& c) const  override;

  /**Same of next(const Context&) plus the possibility to jump the next n-1 events.

     @param c Reference to the Context object.
     @param jump The event to jump to from the current event.
  */
  virtual StatusCode next(EvtContext& c,int jump) const  override;

  /**Fetch the previous event.
     It will return StatusCode::FAILURE if there have been problem in the fetching or it
     has been reached the begin of the list of events.

     @param c Reference to the Context object.
   */
  virtual StatusCode previous(EvtContext& c) const override;

  /**Same of previous(Context& c) the possibility to jump the previous n-1 events.

     @param c Reference to the Context object.
     @param jump The event to jump to from the current event.
   */
  virtual StatusCode previous(EvtContext& c,int jump) const override;

  /** Access last item in the iteration
    * @param refContext [IN/OUT] Reference to the Context object.
    */
  virtual StatusCode last(EvtContext& refContext) const override;

  /** Will set the state of the context in a way that the next event read
    * is the first of the list.
    *
    * @param c Reference to the Context object.
    */
  virtual StatusCode rewind(EvtContext& c) const override;

  /** Create an IOpaqueAddress object from the event fetched.
    *
    * @param c Reference to the Context object.
    * @param iop Refernce pointer to a IOpaqueAddress object
    *
    */
  virtual StatusCode createAddress(const EvtContext& c,IOpaqueAddress*& iop) const override;

  /** Release the Context object.
    *
    * @param c Reference pointer to the Context object.
    */
  virtual StatusCode releaseContext(EvtContext*&)const override;

  /** Will set a new criteria for the selection of the next list of events and will change
    * the state of the context in a way to point to the new list.
    *
    * @param cr The new criteria string.
    * @param c Reference pointer to the Context object.
    */
  virtual StatusCode resetCriteria(const std::string& cr,Context& c)const override;

};
#endif
