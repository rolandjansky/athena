package atlantis.event.oncrpc;
import org.acplt.oncrpc.*;
import java.io.IOException;

/**
 * This structure declares the event as returned from a call
 * to the ONCRPC_GETEVENT_PROC.
 * @author Sebastian Boeser
 */
public class Event implements XdrAble {


    /** False if no event exists for the requested stream at the server*/
    public boolean isAvailable;
    /** True if no new event exists at the server */
    public boolean isIdentical;
    /** True if the event data is compressed */
    public boolean isCompressed;
    /** The name fo the stream this event belongs to */
    public String StreamName;
    /** The number of the event */
    public long EventNumber;
    /** The number of the run */
    public long RunNumber;
    /** The event data as byte array */
    public byte [] EventData;

    /**
     * Constructor
     */
    public Event() {
        //Initialize variables
        isAvailable = false ;
        isIdentical = false ;
        isCompressed = false ;
        StreamName = "";
        EventNumber = -1;
        RunNumber = -1;
        EventData = null;
    }

    /**
     * Constructor that can be used with RPC client call
     * @param xdr the stream that holds the encoded event
     * @throws OncRpcException if decoding fails
     * @throws IOException
     */
    public Event(XdrDecodingStream xdr) throws OncRpcException, IOException {
        xdrDecode(xdr);
    }

    /**
     * Encode the information in XDR representation
     * @param xdr the stream to encode into
     * @throws OncRpcException if encoding fails
     * @throws IOException
     */
    public void xdrEncode(XdrEncodingStream xdr) throws OncRpcException, IOException {
      //Encode all fields in proper order into the stream
      xdr.xdrEncodeBoolean(isAvailable);
      xdr.xdrEncodeBoolean(isIdentical);
      xdr.xdrEncodeBoolean(isCompressed);
      xdr.xdrEncodeString(StreamName);
      xdr.xdrEncodeLong(EventNumber);
      xdr.xdrEncodeLong(RunNumber);
      xdr.xdrEncodeByteVector(EventData);
    }

    /**
     * Decode the information from XDR representation
     * @param xdr the stream to decode from
     * @throws OncRpcException if decoding fails
     * @throws IOException
     */
    public void xdrDecode(XdrDecodingStream xdr) throws OncRpcException, IOException {
      //Decode all fields in proper order from stream
      isAvailable = xdr.xdrDecodeBoolean();
      isIdentical = xdr.xdrDecodeBoolean();
      isCompressed = xdr.xdrDecodeBoolean();
      StreamName = xdr.xdrDecodeString();
      EventNumber = xdr.xdrDecodeLong();
      RunNumber = xdr.xdrDecodeLong();
      EventData = xdr.xdrDecodeDynamicOpaque();
    }

}