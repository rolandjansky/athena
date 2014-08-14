package atlantis.event.oncrpc;
import org.acplt.oncrpc.*;
import java.io.IOException;

/**
 * Data structure as passed to the ONCRPC_GETEVENT_PROC
 * When sending an event request to the server, we have to specify the stream name,
 * along with informatian about events we already have. Negative event and run number
 * indicates we don't have any events yet.
 * @author Sebastian Boeser
 */
public class EventRequest implements XdrAble {

  /** The event number of the event we already have */
  public long EventNumber;
  /** The run number of the event we already have */
  public long RunNumber;
  /** The name of the stream we are asking events from */
  public String StreamName;  

  /**
   * Constructor
   */
  public EventRequest() {
      //Initialize values
      EventNumber = -1;
      RunNumber = -1;
      StreamName = "";
  }

  /**
   * Fully qualified constructor
   * @param eventNumber the event number of the event we already have
   * @param runNumber   the run number of the event we already have
   * @param streamName  the stream we are asking events from
   */
  public EventRequest(long eventNumber, long runNumber, String streamName){
    EventNumber = eventNumber;
    RunNumber = runNumber;
    StreamName = streamName;
  }

  /**
   * Construct an event request returned from an rpc call
   * @param xdr the stream to construct the request from
   * @throws OncRpcException if decoding fails
   * @throws IOException
   */
  public EventRequest(XdrDecodingStream xdr) throws OncRpcException, IOException {
      xdrDecode(xdr);
  }

  /**
   * Encode the information in XDR representation
   * @param xdr the stream to encode into
   * @throws OncRpcException if encoding fails
   * @throws IOException
   */
  public void xdrEncode(XdrEncodingStream xdr) throws OncRpcException, IOException {
    //Encode into stream in proper order
    xdr.xdrEncodeLong(EventNumber);
    xdr.xdrEncodeLong(RunNumber);
    xdr.xdrEncodeString(StreamName);
  }

  /**
   * Decode the information in XDR representation
   * @param xdr the stream to decode from
   * @throws OncRpcException if decoding fails
   * @throws IOException
   */
  public void xdrDecode(XdrDecodingStream xdr) throws OncRpcException, IOException {
    //Decode from stream in proper order
    EventNumber = xdr.xdrDecodeLong();
    RunNumber = xdr.xdrDecodeLong();
    StreamName = xdr.xdrDecodeString();
  }

}